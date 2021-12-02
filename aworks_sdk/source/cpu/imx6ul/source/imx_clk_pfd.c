#include "imx_clk.h"

#define to_clk_pfd(_hw) container_of(_hw, struct clk_pfd, hw)

#define SET 0x4
#define CLR 0x8
#define OTG 0xc

static void clk_pfd_do_hardware(struct clk_pfd *pfd, aw_bool_t enable)
{
    if (enable) {
        writel(1 << ((pfd->idx + 1) * 8 - 1), pfd->reg + CLR);
    }
    else {
        writel(1 << ((pfd->idx + 1) * 8 - 1), pfd->reg + SET);
    }
}

static void clk_pfd_do_shared_clks(struct clk *hw, aw_bool_t enable)
{
    struct clk_pfd *pfd = to_clk_pfd(hw);
    clk_pfd_do_hardware(pfd, enable);

}

static aw_err_t clk_pfd_enable(struct clk *hw)
{
    clk_pfd_do_shared_clks(hw, AW_TRUE);

    return 0;
}

static aw_err_t clk_pfd_disable(struct clk *hw)
{
    clk_pfd_do_shared_clks(hw, AW_FALSE);
    return 0;
}

static aw_clk_rate_t clk_pfd_recalc_rate(struct clk *hw,
                     aw_clk_rate_t parent_rate)
{
    struct clk_pfd     *pfd = to_clk_pfd(hw);
    uint64_t            tmp = parent_rate;
    uint8_t             frac = (readl(pfd->reg) >> (pfd->idx * 8)) & 0x3f;

    tmp *= 18;

    tmp /= frac;
    return tmp;
}

static aw_clk_rate_t clk_pfd_round_rate(struct clk *hw, aw_clk_rate_t rate,
                   aw_clk_rate_t *prate)
{
    uint64_t        tmp = *prate;
    uint8_t         frac;

    tmp = tmp * 18 + rate / 2;
    tmp /= rate;
    frac = tmp;
    if (frac < 12)
        frac = 12;
    else if (frac > 35)
        frac = 35;
    tmp = *prate;
    tmp *= 18;
    tmp /= frac;

    return tmp;
}

static int clk_pfd_set_rate(struct clk *hw, unsigned long rate,
        unsigned long parent_rate)
{
    struct clk_pfd *pfd = to_clk_pfd(hw);
    uint64_t        tmp = parent_rate;
    uint8_t         frac;

    tmp = tmp * 18 + rate / 2;
    tmp /= rate;
    frac = tmp;
    if (frac < 12)
        frac = 12;
    else if (frac > 35)
        frac = 35;

    writel(0x3f << (pfd->idx * 8), pfd->reg + CLR);
    writel(frac << (pfd->idx * 8), pfd->reg + SET);

    return 0;
}

static int clk_pfd_is_enabled(struct clk *hw)
{
    struct clk_pfd *pfd = to_clk_pfd(hw);

    if (readl(pfd->reg) & (1 << ((pfd->idx + 1) * 8 - 1)))
        return 0;

    return 1;
}

static const struct clk_ops clk_pfd_ops = {
    .enable     = clk_pfd_enable,
    .disable    = clk_pfd_disable,
    .recalc_rate    = clk_pfd_recalc_rate,
    .round_rate = clk_pfd_round_rate,
    .set_rate   = clk_pfd_set_rate,
    //.is_enabled     = clk_pfd_is_enabled,
};

struct clk *imx_clk_pfd(
        struct clk_pfd *pfd,
        aw_clk_id_t clk_id,
        const char *name,
        const char *parent_name,
        void *reg,
        uint8_t idx)
{
    struct clk             *clk;
    struct clk_init_data    init;

    pfd->reg = reg;
    pfd->idx = idx;

    init.name = name;
    init.ops = &clk_pfd_ops;
    init.flags = 0;
    init.clk_id = clk_id;
    init.parent_name = parent_name;

    clk =  clk_register_hw(&pfd->hw, &init);

    return clk;
}

