#ifndef         ___IMX_CLK_H__
#define         ___IMX_CLK_H__

#include "clk_provider.h"
#include "imx6ul_iomuxc_regs.h"

static inline struct clk *imx_clk_mux(
        struct clk_mux *p_clk_mux,
        aw_clk_id_t clk_id,
        const char *name,
        void *reg,
        uint8_t shift,
        uint8_t width,
       const char **parent_names, int num_parents)
{
    return clk_register_mux(p_clk_mux,clk_id, name, parent_names, num_parents,
            CLK_SET_RATE_NO_REPARENT | CLK_SET_PARENT_GATE,
            reg, shift, width, 0,NULL);
}

static inline struct clk *imx_clk_mux_flags(
        struct clk_mux *p_clk_mux,
        aw_clk_id_t clk_id,
        const char *name,
        void *reg,
        uint8_t shift,
        uint8_t width,
        const char **parent_names,
        int num_parents,
        unsigned long flags)
{
    return clk_register_mux(p_clk_mux,clk_id, name, parent_names, num_parents,
            flags |CLK_SET_RATE_NO_REPARENT | CLK_SET_PARENT_GATE,
            reg, shift, width, 0,NULL);
}

static inline struct clk *imx_clk_mux_glitchless(
        struct clk_mux *p_clk_mux,
        aw_clk_id_t clk_id,
        const char *name,
        void *reg,
        uint8_t shift,
        uint8_t width,
        const char **parents,
        int num_parents)
{
    return clk_register_mux(p_clk_mux,clk_id, name, parents, num_parents,
            CLK_SET_RATE_NO_REPARENT, reg, shift,
            width, 0,NULL);
}

static inline struct clk *imx_clk_mux_bus(
        struct clk_mux *p_clk_mux,
        aw_clk_id_t clk_id,
        const char *name,
        void *reg,
        uint8_t shift,
        uint8_t width, const char **parents, int num_parents)
{
    return clk_register_mux(p_clk_mux,clk_id, name, parents, num_parents,
            CLK_SET_RATE_NO_REPARENT,
            reg, shift, width, 0,NULL);
}

static inline struct clk *imx_clk_fixed_factor(
        struct clk_fixed_factor *p_clk_fixed_factor,
        aw_clk_id_t clk_id,
        const char *name,
        const char *parent_name,
        unsigned int mult, unsigned int div)
{
    return clk_register_fixed_factor(
            p_clk_fixed_factor,
            clk_id,
            name,
            parent_name,
            CLK_SET_RATE_PARENT, mult, div);
}

static inline struct clk *imx_clk_gate(
        struct clk_gate *gate,
        aw_clk_id_t clk_id,
        const char *name,
        const char *parent_name,
        void *reg,
        uint8_t shift)
{
    return clk_register_gate(
            gate,
            clk_id,
            name,
            parent_name,
            CLK_SET_RATE_PARENT,
            reg,
            shift,
            0,
            NULL);
}

static inline struct clk *imx_clk_gate_iomuxc_gpr(
        struct clk_gate *gate,
        aw_clk_id_t clk_id,
        const char *name,
        const char *parent_name,
        void *reg,
        uint8_t shift)
{
    return clk_register_gate(
            gate,
            clk_id,
            name,
            parent_name,
            CLK_SET_RATE_PARENT,
            reg,
            shift,
            0,
            &__spinlock_isr_iomuxc_gpr);
}

enum imx_pllv3_type {
    IMX_PLLV3_GENERIC,
    IMX_PLLV3_SYS,
    IMX_PLLV3_USB,
    IMX_PLLV3_AV,
    IMX_PLLV3_ENET,
    IMX_PLLV3_SYSV2,
};

/**
 * struct clk_pllv3 - IMX PLL clock version 3
 * @clk_hw:  clock source
 * @base:    base address of PLL registers
 * @powerup_set: set POWER bit to power up the PLL
 * @powerdown:   pll powerdown offset bit
 * @div_mask:    mask of divider bits
 *
 * IMX PLL clock version 3, found on i.MX6 series.  Divider for pllv3
 * is actually a multiplier, and always sits at bit 0.
 */
struct clk_pllv3 {
    struct clk          hw;
    void               *base;
    aw_bool_t              powerup_set;
    uint32_t            powerdown;
    uint32_t            div_mask;
    uint32_t            num_offset;
    uint32_t            denom_offset;
};

struct clk *imx_clk_pllv3(
        struct clk_pllv3 *p_clk_pllv3,
        aw_clk_id_t clk_id,
        enum imx_pllv3_type type,
        const char *name,
        const char *parent_name,
        void *base,
        uint32_t div_mask);

/**
 * struct clk_pfd - IMX PFD clock
 * @clk_hw: clock source
 * @reg:    PFD register address
 * @idx:    the index of PFD encoded in the register
 *
 * PFD clock found on i.MX6 series.  Each register for PFD has 4 clk_pfd
 * data encoded, and member idx is used to specify the one.  And each
 * register has SET, CLR and TOG registers at offset 0x4 0x8 and 0xc.
 */
struct clk_pfd {
    struct clk          hw;
    void               *reg;
    uint8_t             idx;
};

struct clk *imx_clk_pfd(
        struct clk_pfd *pfd,
        aw_clk_id_t clk_id,
        const char *name,
        const char *parent_name,
        void *reg,
        uint8_t idx);

struct clk_busy_divider {
    struct clk_divider      div;
    const struct clk_ops   *div_ops;
    void                   *reg;
    uint8_t                 shift;
};

struct clk *imx_clk_busy_divider(
        struct clk_busy_divider *p_busy_div,
        aw_clk_id_t clk_id,
        const char *name,
        const char *parent_name,
        void *reg,
        uint8_t shift,
        uint8_t width,
        void *busy_reg,
        uint8_t busy_shift);

struct clk_busy_mux {
    struct clk_mux          mux;
    const struct clk_ops   *mux_ops;
    void                   *reg;
    uint8_t                 shift;
};

struct clk *imx_clk_busy_mux(
        struct clk_busy_mux *p_busy_mux,
        aw_clk_id_t clk_id,
        const char *name,
        void *reg,
        uint8_t shift,
        uint8_t width,
        void *busy_reg,
        uint8_t busy_shift,
        const char **parent_names,
        int num_parents);

static inline struct clk *imx_clk_divider(
        struct clk_divider *p_clk_div,
        aw_clk_id_t clk_id,
        const char *name,
        const char *parent_name,
        void *reg,
        uint8_t shift,
        uint8_t width)
{
    return clk_register_divider(
            p_clk_div,
            clk_id,
            name,
            parent_name,
            CLK_SET_RATE_PARENT | CLK_SET_RATE_GATE,
            reg, shift, width, 0,NULL);
}

/**
 * DOC: basic gatable clock which can gate and ungate it's ouput
 *
 * Traits of this clock:
 * prepare - clk_(un)prepare only ensures parent is (un)prepared
 * enable - clk_enable and clk_disable are functional & control gating
 * rate - inherits rate from parent.  No clk_set_rate support
 * parent - fixed parent.  No clk_set_parent support
 */

struct clk_gate2 {
    struct clk      hw;
    void           *reg;
    uint8_t         bit_idx;
    uint8_t         flags;
    unsigned int   *share_count;
};

struct clk *clk_register_gate2(
        struct clk_gate2 *gate2,
        aw_clk_id_t clk_id,
        const char *name,
        const char *parent_name,
        unsigned long flags,
        void *reg,
        uint8_t bit_idx,
        uint8_t clk_gate2_flags,
        unsigned int *share_count);

static inline struct clk *imx_clk_gate2(
        struct clk_gate2 *gate2,
         aw_clk_id_t clk_id,
        const char *name,
        const char *parent,
        void *reg, uint8_t shift)
{
    return clk_register_gate2(gate2,clk_id, name, parent,
            CLK_SET_RATE_PARENT | CLK_SET_RATE_GATE,
            reg, shift, 0, NULL);
}

static inline struct clk *imx_clk_gate2_shared(
        struct clk_gate2 *gate2,
        aw_clk_id_t clk_id,
        const char *name,
        const char *parent,
        void *reg,
        uint8_t shift,
        unsigned int *share_count)
{
    return clk_register_gate2(gate2,clk_id, name, parent,
            CLK_SET_RATE_PARENT | CLK_SET_RATE_GATE,
            reg, shift, 0, share_count);
}

static inline struct clk *imx_clk_busy_gate(
        struct clk_gate2 *gate2,
        aw_clk_id_t clk_id,
        const char *name,
        const char *parent,
        void *reg,
        uint8_t shift)
{
    return clk_register_gate2(gate2,clk_id, name, parent,
            CLK_SET_RATE_PARENT | CLK_SET_RATE_GATE,
            reg, shift, 0, NULL);
}

struct clk_enet_tx_ref {
    struct clk      hw;
    struct clk     *parents[2];
    int             iomuxc_gpr1_reg_offset;
    uint8_t         sel_idx;
    uint8_t         output_idx;
};

struct clk *imx_clk_enet_tx_clk_ref(
        struct clk_enet_tx_ref *enet_tx_ref,
        aw_clk_id_t clk_id,
        const char *name,
        const char **parent_names,
        uint8_t num_parents,
        int  iomuxc_gpr1_reg_offset,
        uint8_t sel_idx,
        uint8_t output_idx);
#endif
