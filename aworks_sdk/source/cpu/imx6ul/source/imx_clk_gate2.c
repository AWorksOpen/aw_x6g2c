#include "imx_clk.h"

#define to_clk_gate2(_hw) container_of(_hw, struct clk_gate2, hw)
#define CCM_CCGR_FULL_ENABLE    0x3

static void clk_gate2_do_hardware(struct clk_gate2 *gate, aw_bool_t enable)
{
    uint32_t        reg;

    reg = readl(gate->reg);
    if (enable) {
        reg |= CCM_CCGR_FULL_ENABLE << gate->bit_idx;
    }
    else {
        reg &= ~(CCM_CCGR_FULL_ENABLE << gate->bit_idx);
    }
    writel(reg, gate->reg);
}

static void clk_gate2_do_shared_clks(struct clk *hw, aw_bool_t enable)
{
    struct clk_gate2 *gate = to_clk_gate2(hw);
    clk_gate2_do_hardware(gate, enable);
}

static aw_err_t clk_gate2_enable(struct clk *hw)
{
    struct clk_gate2       *gate = to_clk_gate2(hw);

    if (gate->share_count && (*gate->share_count)++ > 0) {
        goto out;
    }

    clk_gate2_do_shared_clks(hw, AW_TRUE);
out:

    return 0;
}

static aw_err_t clk_gate2_disable(struct clk *hw)
{
    struct clk_gate2 *gate = to_clk_gate2(hw);

    if (gate->share_count) {
        if (*gate->share_count > 0) {
            if (--(*gate->share_count) > 0) {
                goto out;
            }
        }
    }

    clk_gate2_do_shared_clks(hw, AW_FALSE);
out:
    return 0;
}


static struct clk_ops clk_gate2_ops = {
    .enable = clk_gate2_enable,
    .disable = clk_gate2_disable,
};

struct clk *clk_register_gate2(
        struct clk_gate2 *gate,
        aw_clk_id_t clk_id,
        const char *name,
        const char *parent_name,
        unsigned long flags,
        void *reg,
        uint8_t bit_idx,
        uint8_t clk_gate2_flags,
        unsigned int *share_count)
{
    struct clk             *clk;
    struct clk_init_data    init;

    /* struct clk_gate2 assignments */
    gate->reg = reg;
    gate->bit_idx = bit_idx;
    gate->flags = clk_gate2_flags;
    gate->share_count = share_count;

    init.name = name;
    init.ops = &clk_gate2_ops;
    init.flags = flags;
    init.parent_name = parent_name ;
    init.clk_id = clk_id;

    clk = clk_register_hw(&gate->hw,&init);
    return clk;
}
