#include "imx_clk.h"

static int clk_busy_wait(void *reg, uint8_t shift)
{
    unsigned long timeout = 1000000;

    while (readl(reg) & (1 << shift)) {
        timeout --;
        if (0 == timeout) {
            return -AW_ETIMEDOUT;
        }
    }
    return 0;
}

static inline struct clk_busy_divider *to_clk_busy_divider(struct clk *hw)
{
    struct clk_divider *div = container_of(hw, struct clk_divider, hw);

    return container_of(div, struct clk_busy_divider, div);
}


static aw_clk_rate_t clk_busy_divider_recalc_rate(struct clk *hw,
                          aw_clk_rate_t parent_rate)
{
    struct clk_busy_divider *busy = to_clk_busy_divider(hw);

    return busy->div_ops->recalc_rate(&busy->div.hw, parent_rate);
}

static aw_clk_rate_t clk_busy_divider_round_rate(
        struct clk *hw,
        aw_clk_rate_t rate,
        aw_clk_rate_t *prate)
{
    struct clk_busy_divider *busy = to_clk_busy_divider(hw);

    return busy->div_ops->round_rate(&busy->div.hw, rate, prate);
}

static aw_err_t clk_busy_divider_set_rate(
        struct clk *hw,
        aw_clk_rate_t rate,
        aw_clk_rate_t parent_rate)
{
    struct clk_busy_divider *busy = to_clk_busy_divider(hw);
    int ret;

    ret = busy->div_ops->set_rate(&busy->div.hw, rate, parent_rate);
    if (!ret)
        ret = clk_busy_wait(busy->reg, busy->shift);

    return ret;
}

static const struct clk_ops clk_busy_divider_ops = {
    .recalc_rate = clk_busy_divider_recalc_rate,
    .round_rate = clk_busy_divider_round_rate,
    .set_rate = clk_busy_divider_set_rate,
};

struct clk *imx_clk_busy_divider(
        struct clk_busy_divider *busy,
        aw_clk_id_t clk_id,
        const char *name,
        const char *parent_name,
        void *reg,
        uint8_t shift,
        uint8_t width,
        void *busy_reg,
        uint8_t busy_shift)
{
    struct clk *clk;
    struct clk_init_data init;

    busy->reg = busy_reg;
    busy->shift = busy_shift;

    busy->div.reg = reg;
    busy->div.shift = shift;
    busy->div.width = width;
    busy->div_ops = &clk_divider_ops;

    init.name = name;
    init.ops = &clk_busy_divider_ops;
    init.flags = CLK_SET_RATE_PARENT;
    init.parent_name = parent_name;
    init.clk_id = clk_id;



    clk = clk_register_hw(&busy->div.hw,&init);

    return clk;
}

static inline struct clk_busy_mux *to_clk_busy_mux(struct clk *hw)
{
    struct clk_mux *mux = container_of(hw, struct clk_mux, hw);

    return container_of(mux, struct clk_busy_mux, mux);
}

static aw_err_t clk_busy_mux_get_parent(struct clk *hw,aw_clk_id_t *parent)
{
    struct clk_busy_mux *busy = to_clk_busy_mux(hw);

    return busy->mux_ops->get_parent(&busy->mux.hw,parent);
}

static aw_err_t clk_busy_mux_set_parent(struct clk *hw, aw_clk_id_t parent)
{
    struct clk_busy_mux *busy = to_clk_busy_mux(hw);
    int ret;

    ret = busy->mux_ops->set_parent(&busy->mux.hw, parent);

    if (AW_OK == ret)
        ret = clk_busy_wait(busy->reg, busy->shift);

    return ret;
}

static struct clk_ops clk_busy_mux_ops = {
    .get_parent = clk_busy_mux_get_parent,
    .set_parent = clk_busy_mux_set_parent,
};

struct clk *imx_clk_busy_mux(
        struct clk_busy_mux *busy,
        aw_clk_id_t clk_id,
        const char *name,
        void *reg,
        uint8_t shift,
        uint8_t width,
        void *busy_reg,
        uint8_t busy_shift,
        const char **parent_names,
        int num_parents)
{
    struct clk                 *clk;
    struct clk                 *hw;
    struct clk_init_data        init;
    int                         i;

    busy->reg = busy_reg;
    busy->shift = busy_shift;
    clk_busy_mux_ops.init = clk_mux_ops.init;

    busy->mux.reg = reg;
    busy->mux.shift = shift;
    busy->mux.mask = AW_BIT(width) - 1;
    busy->mux_ops = &clk_mux_ops;

    busy->mux.num_parents = num_parents;
    for (i = 0;i< num_parents;i++) {
        hw = __clk_find_by_name(parent_names[i]);
        if (NULL == hw) {
            return NULL;
        }
        busy->mux.parents[i] = hw;
    }

    init.name = name;
    init.ops = &clk_busy_mux_ops;
    init.flags = CLK_SET_RATE_NO_REPARENT;
    init.parent_name = NULL;
    init.clk_id = clk_id;

    clk = clk_register_hw(&busy->mux.hw, &init);

    return clk;
}
