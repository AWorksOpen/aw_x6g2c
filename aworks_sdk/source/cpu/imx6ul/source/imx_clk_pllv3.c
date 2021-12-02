#include "imx_clk.h"

#define PLL_NUM_OFFSET      0x10
#define PLL_DENOM_OFFSET    0x20
#define PLL_AV_NUM_OFFSET   0x20
#define PLL_AV_DENOM_OFFSET 0x30

#define BM_PLL_POWER        (0x1 << 12)
#define BM_PLL_LOCK         (0x1 << 31)
#define BM_PLL_ENABLE       (0x1 << 13)
#define BM_PLL_BYPASS       (0x1 << 16)
#define ENET_PLL_POWER      (0x1 << 5)

#define to_clk_pllv3(_hw) container_of(_hw, struct clk_pllv3, hw)


static int clk_pllv3_wait_lock(struct clk_pllv3 *pll)
{
    unsigned long   timeout = 100000;
    uint32_t        val = readl(pll->base) & pll->powerdown;

    /* No need to wait for lock when pll is not powered up */
    if ((pll->powerup_set && !val) || (!pll->powerup_set && val))
        return 0;

    /* Wait for PLL to lock */
    do {
        if (readl(pll->base) & BM_PLL_LOCK)
            break;
        timeout --;
    } while (timeout);

    return readl(pll->base) & BM_PLL_LOCK ? 0 : -AW_ETIMEDOUT;
}

static int clk_pllv3_do_hardware(struct clk *hw, aw_bool_t enable)
{
    struct clk_pllv3   *pll = to_clk_pllv3(hw);
    uint32_t            val;
    int                 ret;

    val = readl(pll->base);
    if (enable) {
        if (pll->powerup_set) {
            val |= pll->powerdown;
        }
        else {
            val &= ~pll->powerdown;
        }
        writel(val, pll->base);

        ret = clk_pllv3_wait_lock(pll);
        if (ret) {
            return ret;
        }
    } else {
        if (pll->powerup_set) {
            val &= ~pll->powerdown;
        }
        else {
            val |= pll->powerdown;
        }
        writel(val, pll->base);
    }

    return 0;
}


static aw_err_t clk_pllv3_prepare(struct clk *hw)
{
    return clk_pllv3_do_hardware(hw, AW_TRUE);
}

static aw_err_t clk_pllv3_unprepare(struct clk *hw)
{
     return clk_pllv3_do_hardware(hw, AW_FALSE);
}


static aw_clk_rate_t clk_pllv3_recalc_rate(
        struct clk *hw,unsigned long parent_rate)
{
    struct clk_pllv3       *pll = to_clk_pllv3(hw);
    uint32_t                div = readl(pll->base)  & pll->div_mask;

    return (div == 1) ? parent_rate * 22 : parent_rate * 20;
}

static aw_clk_rate_t clk_pllv3_round_rate(struct clk *hw, aw_clk_rate_t rate,
                 aw_clk_rate_t *prate)
{
    unsigned long parent_rate = *prate;

    return (rate >= parent_rate * 22) ? parent_rate * 22 :
                        parent_rate * 20;
}

static aw_err_t clk_pllv3_set_rate(
        struct clk *hw,
        aw_clk_rate_t rate,
        aw_clk_rate_t parent_rate)
{
    struct clk_pllv3    *pll = to_clk_pllv3(hw);
    uint32_t            val, div;


    if (rate == parent_rate * 22)
        div = 1;
    else if (rate == parent_rate * 20)
        div = 0;
    else
        return -AW_EINVAL;

    val = readl(pll->base);
    val &= ~pll->div_mask;
    val |= div;
    writel(val, pll->base);

    return clk_pllv3_wait_lock(pll);
}

static const struct clk_ops clk_pllv3_ops = {
    .enable    = clk_pllv3_prepare,
    .disable  = clk_pllv3_unprepare,
    .recalc_rate    = clk_pllv3_recalc_rate,
    .round_rate = clk_pllv3_round_rate,
    .set_rate   = clk_pllv3_set_rate,
};

static aw_clk_rate_t clk_pllv3_sys_recalc_rate(struct clk *hw,
                           unsigned long parent_rate)
{
    struct clk_pllv3 *pll = to_clk_pllv3(hw);
    uint32_t        div = readl(pll->base) & pll->div_mask;

    return parent_rate * div / 2;
}

static aw_clk_rate_t clk_pllv3_sys_round_rate(struct clk *hw, aw_clk_rate_t rate,
                 aw_clk_rate_t *prate)
{
    unsigned long parent_rate = *prate;
    unsigned long min_rate = parent_rate * 54 / 2;
    unsigned long max_rate = parent_rate * 108 / 2;
    uint32_t div;

    if (rate > max_rate)
        rate = max_rate;
    else if (rate < min_rate)
        rate = min_rate;
    div = rate * 2 / parent_rate;

    return parent_rate * div / 2;
}

static aw_err_t clk_pllv3_sys_set_rate(
        struct clk *hw,
        aw_clk_rate_t rate,
        aw_clk_rate_t parent_rate)
{
    struct clk_pllv3       *pll = to_clk_pllv3(hw);
    unsigned long           min_rate;
    unsigned long           max_rate;
    uint32_t                val, div;

    max_rate = parent_rate * 108 / 2;
    min_rate = parent_rate * 54 / 2;
    if (rate < min_rate || rate > max_rate)
        return -AW_EINVAL;

    div = rate * 2 / parent_rate;
    val = readl(pll->base);
    val &= ~pll->div_mask;
    val |= div;
    writel(val, pll->base);

    return clk_pllv3_wait_lock(pll);
}

static const struct clk_ops clk_pllv3_sys_ops = {
    .enable    = clk_pllv3_prepare,
    .disable  = clk_pllv3_unprepare,
    .recalc_rate    = clk_pllv3_sys_recalc_rate,
    .round_rate = clk_pllv3_sys_round_rate,
    .set_rate   = clk_pllv3_sys_set_rate,
};

aw_clk_rate_t clk_pllv3_av_recalc_rate(struct clk *hw,
                           aw_clk_rate_t parent_rate)
{
    struct clk_pllv3       *pll = to_clk_pllv3(hw);
    uint32_t                mfn = readl(pll->base + pll->num_offset);
    uint32_t                mfd = readl(pll->base + pll->denom_offset);
    uint32_t                div = readl(pll->base) & pll->div_mask;
    uint64_t                temp64 = (uint64_t)parent_rate;

    temp64 *= mfn;
    temp64 /= mfd;

    return (parent_rate * div) + (uint32_t)temp64;
}

static aw_clk_rate_t clk_pllv3_av_round_rate(struct clk *hw, aw_clk_rate_t rate,
                 aw_clk_rate_t *prate)
{
    unsigned long       parent_rate = *prate;
    unsigned long       min_rate = parent_rate * 27;
    unsigned long       max_rate = parent_rate * 54;
    uint32_t            div;
    uint32_t            mfn, mfd = 1000000;
    int64_t             temp64;

    if (rate > max_rate)
        rate = max_rate;
    else if (rate < min_rate)
        rate = min_rate;

    div = rate / parent_rate;
    temp64 = (uint64_t) (rate - div * parent_rate);
    temp64 *= mfd;
    temp64 /= parent_rate;
    mfn = temp64;

    return parent_rate * div + parent_rate / mfd * mfn;
}

static aw_err_t clk_pllv3_av_set_rate(
        struct clk *hw,
        aw_clk_rate_t rate,
        aw_clk_rate_t parent_rate)
{
    struct clk_pllv3 *pll = to_clk_pllv3(hw);
    unsigned long min_rate = parent_rate * 27;
    unsigned long max_rate = parent_rate * 54;
    uint32_t val, div;
    uint32_t mfn, mfd = 1000000;
    int64_t temp64;

    if (rate < min_rate || rate > max_rate)
        return -AW_EINVAL;

    div = rate / parent_rate;
    temp64 = (uint64_t) (rate - div * parent_rate);
    temp64 *= mfd;
    temp64 /= parent_rate;
    mfn = temp64;

    val = readl(pll->base);
    val &= ~pll->div_mask;
    val |= div;
    writel(val, pll->base);
    writel(mfn, pll->base + pll->num_offset);
    writel(mfd, pll->base + pll->denom_offset);

    return clk_pllv3_wait_lock(pll);
}

static const struct clk_ops clk_pllv3_av_ops = {
    .enable    = clk_pllv3_prepare,
    .disable  = clk_pllv3_unprepare,
    .recalc_rate    = clk_pllv3_av_recalc_rate,
    .round_rate = clk_pllv3_av_round_rate,
    .set_rate   = clk_pllv3_av_set_rate,
};

aw_clk_rate_t clk_pllv3_enet_recalc_rate(struct clk *hw,
                           aw_clk_rate_t parent_rate)
{
    return 500000000;
}

static const struct clk_ops clk_pllv3_enet_ops = {
    .enable    = clk_pllv3_prepare,
    .disable  = clk_pllv3_unprepare,
    .recalc_rate    = clk_pllv3_enet_recalc_rate,
};

struct clk *imx_clk_pllv3(
        struct clk_pllv3 *pll,
        aw_clk_id_t clk_id,
        enum imx_pllv3_type type,
        const char *name,
        const char *parent_name,
        void *base,
        uint32_t div_mask)
{
    const struct clk_ops       *ops;
    struct clk                 *clk;
    struct clk_init_data        init;


    switch (type) {
    case IMX_PLLV3_SYS:
        ops = &clk_pllv3_sys_ops;
        break;
    case IMX_PLLV3_USB:
        ops = &clk_pllv3_ops;
        pll->powerup_set = AW_TRUE;
        break;
    case IMX_PLLV3_AV:
        ops = &clk_pllv3_av_ops;
        break;
    case IMX_PLLV3_ENET:
        ops = &clk_pllv3_enet_ops;
        break;
    case IMX_PLLV3_SYSV2:
        ops = &clk_pllv3_ops;
        break;
    default:
        ops = &clk_pllv3_ops;
    }

    pll->base = base;
    pll->div_mask = div_mask;
    pll->powerdown = BM_PLL_POWER;
    pll->num_offset = PLL_NUM_OFFSET;
    pll->denom_offset = PLL_DENOM_OFFSET;


    init.name = name;
    init.ops = ops;
    init.flags = CLK_SET_RATE_GATE | CLK_GET_RATE_NOCACHE;
    init.clk_id = clk_id;
    init.parent_name = parent_name;


    clk = clk_register_hw( &pll->hw,&init);

    return clk;
}
