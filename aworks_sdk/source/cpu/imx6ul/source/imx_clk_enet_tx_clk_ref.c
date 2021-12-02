#include "imx_clk.h"
#include <string.h>
#include "imx6ul_iomuxc_regs.h"

/*
 * DOC: basic adjustable multiplexer clock that cannot gate
 *
 * Traits of this clock:
 * prepare - clk_prepare only ensures that parents are prepared
 * enable - clk_enable only ensures that parents are enabled
 * rate - rate is only affected by parent switching.  No clk_set_rate support
 * parent - parent is adjustable through clk_set_parent
 */

#define to_clk_enet_tx_clk_ref(_hw) AW_CONTAINER_OF(_hw, struct clk_enet_tx_ref, hw)


static aw_err_t clk_enet_tx_clk_ref_get_parent_hw(
        struct clk *hw,
        struct clk **p_parent)
{
    struct clk_enet_tx_ref     *tx_ref = to_clk_enet_tx_clk_ref(hw);
    aw_clk_reg_t                val;
    aw_err_t                    err = 0;

    *p_parent  = NULL;
    /*
     * FIXME need a mux-specific flag to determine if val is bitwise or numeric
     * e.g. sys_clkin_ck's clksel field is 3 bits wide, but ranges from 0x1
     * to 0x7 (index starts at one)
     * OTOH, pmd_trace_clk_mux_ck uses a separate bit for each clock, so
     * val = 0x4 really means "bit 2, index starts at bit 0"
     */
    val = iomuxc_reg_bits_get(
            tx_ref->iomuxc_gpr1_reg_offset,
            tx_ref->sel_idx,
            1);
    *p_parent = tx_ref->parents[val];

    return err;
}

static aw_err_t clk_enet_tx_clk_ref_get_parent(
        struct clk *hw,
        aw_clk_id_t *p_parent_id)
{
    aw_err_t            err = 0;
    struct clk         *parent;


    err = clk_enet_tx_clk_ref_get_parent_hw(hw,&parent);
    if (AW_OK == err) {
        *p_parent_id = parent->clk_id;
    }

    return err;
}



static void clk_enet_tx_clk_ref_init(struct clk *hw)
{
    struct clk                 *parent;
    aw_err_t                    err;
    struct clk_enet_tx_ref     *tx_ref = to_clk_enet_tx_clk_ref(hw);

    err = clk_enet_tx_clk_ref_get_parent_hw(hw,&parent);
    if (AW_OK == err) {
        hw->parent = parent;
        // 默认禁用输出
        iomuxc_reg_bits_set(
                    tx_ref->iomuxc_gpr1_reg_offset,
                    tx_ref->output_idx,
                    1,
                    0);
    }
}

static aw_err_t clk_enet_tx_clk_ref_set_parent(struct clk *hw,aw_clk_id_t parent)
{
    struct clk_enet_tx_ref     *tx_ref = to_clk_enet_tx_clk_ref(hw);
    aw_err_t                    err = -AW_EINVAL;
    uint32_t                    index;

    // 首先查找看本mux clock是否支持指定的父时钟
    for (index = 0; index < 2;index ++) {
        if (parent == tx_ref->parents[index]->clk_id ) {
            err = 0;
            break;
        }
    }
    if (0 != err ) {
        return err;
    }

    iomuxc_reg_bits_set(
            tx_ref->iomuxc_gpr1_reg_offset,
            tx_ref->sel_idx,
            1,
            index);

    // 如果时钟已经被使能，且选择pll作为父时钟，则需要将时钟输出打开
    if (hw->ref_count > 0) {
        if ( 0 == index ) {
            iomuxc_reg_bits_set(
                    tx_ref->iomuxc_gpr1_reg_offset,
                    tx_ref->output_idx,
                    1,
                    1);
        }
        else {
            // 选择外部时钟输入，则需要禁用时钟输出
            iomuxc_reg_bits_set(
                    tx_ref->iomuxc_gpr1_reg_offset,
                    tx_ref->output_idx,
                    1,
                    0);
        }
    }

    return 0;
}

aw_err_t enet_tx_clk_ref_enable(struct clk *hw)
{
    struct clk_enet_tx_ref     *tx_ref = to_clk_enet_tx_clk_ref(hw);

    if (hw->parent == tx_ref->parents[0] ) {
        // 如果选择pll作父时钟，才打开时钟输出
        iomuxc_reg_bits_set(
                    tx_ref->iomuxc_gpr1_reg_offset,
                    tx_ref->output_idx,
                    1,
                    1);
    }

    return 0;
}

aw_err_t enet_tx_clk_ref_disable(struct clk *hw)
{
    struct clk_enet_tx_ref     *tx_ref = to_clk_enet_tx_clk_ref(hw);

    iomuxc_reg_bits_set(
                    tx_ref->iomuxc_gpr1_reg_offset,
                    tx_ref->output_idx,
                    1,
                    0);
    return 0;
}

static const struct clk_ops clk_enet_tx_clk_ref_ops = {
        .init = clk_enet_tx_clk_ref_init,
        .get_parent = clk_enet_tx_clk_ref_get_parent,
        .set_parent = clk_enet_tx_clk_ref_set_parent,
        .enable = enet_tx_clk_ref_enable,
        .disable = enet_tx_clk_ref_disable,
    //.determine_rate = __clk_mux_determine_rate,
};


struct clk *imx_clk_enet_tx_clk_ref(
        struct clk_enet_tx_ref *enet_tx_ref,
        aw_clk_id_t clk_id,
        const char *name,
        const char **parent_names,
        uint8_t num_parents,
        int  iomuxc_gpr1_reg_offset,
        uint8_t sel_idx,
        uint8_t output_idx)
{

    struct clk_init_data        init = {0};
    struct clk                 *hw = NULL;
    int                         i;

    //检查参数
    if (num_parents != 2|| 32 <= sel_idx || 32 <= output_idx ) {
        return NULL;
    }

    init.name = name;
    init.ops = &clk_enet_tx_clk_ref_ops;
    init.flags =  CLK_IS_BASIC | CLK_IS_BASIC_MUX;
    init.parent_name = NULL;
    init.clk_id = clk_id;

    /* struct clk_mux assignments */
    enet_tx_ref->iomuxc_gpr1_reg_offset = iomuxc_gpr1_reg_offset;
    enet_tx_ref->output_idx = output_idx;
    enet_tx_ref->sel_idx = sel_idx;

    for (i = 0;i< num_parents;i++) {
        hw = __clk_find_by_name(parent_names[i]);
        if (NULL == hw) {
            return NULL;
        }
        enet_tx_ref->parents[i] = hw;
    }

    hw = clk_register_hw(&enet_tx_ref->hw,&init);

    return hw;
}
