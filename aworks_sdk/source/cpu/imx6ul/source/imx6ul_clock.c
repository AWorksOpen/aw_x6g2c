#include "apollo.h"
#include "clk_provider.h"
#include "imx_clk.h"
#include "imx6ul_regs_base.h"

#define BM_CCM_CCDR_MMDC_CH0_MASK   (0x2 << 16)
#define CCDR    0x4

#ifndef ARRAY_SIZE
# define ARRAY_SIZE(ar) (sizeof(ar) / sizeof(ar[0]))
#endif

static struct clk      *__g_all_clk[imx6ul_clk_max_count] = {NULL};

static const char *pll_bypass_src_sels[] = { "osc", "dummy", };
static const char *pll1_bypass_sels[] = { "pll1", "pll1_bypass_src", };
static const char *pll2_bypass_sels[] = { "pll2", "pll2_bypass_src", };
static const char *pll3_bypass_sels[] = { "pll3", "pll3_bypass_src", };
static const char *pll4_bypass_sels[] = { "pll4", "pll4_bypass_src", };
static const char *pll5_bypass_sels[] = { "pll5", "pll5_bypass_src", };
static const char *pll6_bypass_sels[] = { "pll6", "pll6_bypass_src", };
static const char *pll7_bypass_sels[] = { "pll7", "pll7_bypass_src", };


static const struct clk_div_table clk_enet_ref_table[] = {
    { .val = 0, .div = 20, },
    { .val = 1, .div = 10, },
    { .val = 2, .div = 5, },
    { .val = 3, .div = 4, },
    { }
};

static const struct clk_div_table post_div_table[] = {
    { .val = 2, .div = 1, },
    { .val = 1, .div = 2, },
    { .val = 0, .div = 4, },
    { }
};

static const struct clk_div_table video_div_table[] = {
    { .val = 0, .div = 1, },
    { .val = 1, .div = 2, },
    { .val = 2, .div = 1, },
    { .val = 3, .div = 4, },
    { }
};


static const char *ca7_secondary_sels[] = { "pll2_pfd2_396m", "pll2_bus", };
static const char *step_sels[] = { "osc", "ca7_secondary_sel", };
static const char *pll1_sw_sels[] = { "pll1_sys", "step", };
static const char *axi_alt_sels[] = { "pll2_pfd2_396m", "pll3_pfd1_540m", };
static const char *axi_sels[] = {"periph", "axi_alt_sel", };
static const char *periph_pre_sels[] = { "pll2_bus", "pll2_pfd2_396m", "pll2_pfd0_352m", "pll2_198m", };
static const char *periph2_pre_sels[] = { "pll2_bus", "pll2_pfd2_396m", "pll2_pfd0_352m", "pll4_audio_div", };
static const char *periph_clk2_sels[] = { "pll3_usb_otg", "osc", "osc", };
static const char *periph2_clk2_sels[] = { "pll3_usb_otg", "osc", };
static const char *periph_sels[] = { "periph_pre", "periph_clk2", };
static const char *periph2_sels[] = { "periph2_pre", "periph2_clk2", };
static const char *usdhc_sels[] = { "pll2_pfd2_396m", "pll2_pfd0_352m", };
static const char *bch_sels[] = { "pll2_pfd2_396m", "pll2_pfd0_352m", };
static const char *gpmi_sels[] = { "pll2_pfd2_396m", "pll2_pfd0_352m", };
static const char *eim_slow_sels[] =  { "axi", "pll3_usb_otg", "pll2_pfd2_396m", "pll3_pfd0_720m", };
static const char *spdif_sels[] = { "pll4_audio_div", "pll3_pfd2_508m", "pll5_video_div", "pll3_usb_otg", };
static const char *sai_sels[] = { "pll3_pfd2_508m", "pll5_video_div", "pll4_audio_div", };
static const char *lcdif_pre_sels[] = { "pll2_bus", "pll3_pfd3_454m", "pll5_video_div", "pll2_pfd0_352m", "pll2_pfd1_594m", "pll3_pfd1_540m", };
static const char *sim_pre_sels[] = { "pll2_bus", "pll3_usb_otg", "pll5_video_div", "pll2_pfd0_352m", "pll2_pfd2_396m", "pll3_pfd2_508m", };
static const char *ldb_di0_sels[] = { "pll5_video_div", "pll2_pfd0_352m", "pll2_pfd2_396m", "pll2_pfd3_594m", "pll2_pfd1_594m", "pll3_pfd3_454m", };
static const char *ldb_di0_div_sels[] = { "ldb_di0_div_3_5", "ldb_di0_div_7", };
static const char *ldb_di1_div_sels[] = { "ldb_di1_div_3_5", "ldb_di1_div_7", };
static const char *qspi1_sels[] = { "pll3_usb_otg", "pll2_pfd0_352m", "pll2_pfd2_396m", "pll2_bus", "pll3_pfd3_454m", "pll3_pfd2_508m", };
static const char *enfc_sels[] = { "pll2_pfd0_352m", "pll2_bus", "pll3_usb_otg", "pll2_pfd2_396m", "pll3_pfd3_454m", "dummy", "dummy", "dummy", };
static const char *can_sels[] = { "pll3_60m", "osc", "pll3_80m", "dummy", };
static const char *ecspi_sels[] = { "pll3_60m", "osc", };
static const char *uart_sels[] = { "pll3_80m", "osc", };
static const char *perclk_sels[] = { "ipg", "osc", };
static const char *lcdif_sels[] = { "lcdif_podf", "ipp_di0", "ipp_di1", "ldb_di0", "ldb_di1", };
static const char *csi_sels[] = { "osc", "pll2_pfd2_396m", "pll3_120m", "pll3_pfd1_540m", };
static const char *sim_sels[] = { "sim_podf", "ipp_di0", "ipp_di1", "ldb_di0", "ldb_di1", };


#define CLK_BUG_ON(x) if((x) ) {while(1);};

void imx6ul_clock_init()
{
    struct clk         *clk;
    void               *base = (void *)0x020c8000;
    int                 i;


    aw_clk_init(__g_all_clk,imx6ul_clk_max_count);

    static struct clk_fixed_rate clk_dummy;
    clk = clk_register_fixed_rate(
            &clk_dummy,
            IMX6UL_CLK_DUMMY,
            "dummy",
            0,
            0);
    CLK_BUG_ON(NULL == clk);

    static struct clk_fixed_rate clk_hw_osc_32k;
    clk = clk_register_fixed_rate(
            &clk_hw_osc_32k,
            IMX6UL_CLK_CKIL,
            "ckil",
            0,
            32768);
    CLK_BUG_ON(NULL == clk);

    static struct clk_fixed_rate clk_hw_osc_24m;
    clk = clk_register_fixed_rate(
            &clk_hw_osc_24m,
            IMX6UL_CLK_OSC,
            "osc",
            0,
            24000000);
    CLK_BUG_ON(NULL == clk);

    static struct clk_user_fixed_rate clk_ipp_di0;
    clk = clk_register_user_fixed_rate(
            &clk_ipp_di0,
            IMX6UL_CLK_IPP_DI0,
            "ipp_di0",
            0);
    CLK_BUG_ON(NULL == clk);

    static struct clk_user_fixed_rate clk_ipp_di1;
    clk = clk_register_user_fixed_rate(
            &clk_ipp_di1,
            IMX6UL_CLK_IPP_DI1,
            "ipp_di1",
            0);
    CLK_BUG_ON(NULL == clk);

    static struct clk_mux pll1_bypass_src;
    clk = imx_clk_mux(
            &pll1_bypass_src,
            IMX6UL_PLL1_BYPASS_SRC,
            "pll1_bypass_src",
            base + 0x00, 14, 1,
            pll_bypass_src_sels,
            ARRAY_SIZE(pll_bypass_src_sels));
    CLK_BUG_ON(NULL == clk);

    static struct clk_mux pll2_bypass_src;
    clk = imx_clk_mux(
            &pll2_bypass_src,
            IMX6UL_PLL2_BYPASS_SRC,
            "pll2_bypass_src",
            base + 0x30, 14, 1,
            pll_bypass_src_sels,
            ARRAY_SIZE(pll_bypass_src_sels));
    CLK_BUG_ON(NULL == clk);

    static struct clk_mux pll3_bypass_src;
    clk = imx_clk_mux(
            &pll3_bypass_src,
            IMX6UL_PLL3_BYPASS_SRC,
            "pll3_bypass_src",
            base + 0x10, 14, 1,
            pll_bypass_src_sels,
            ARRAY_SIZE(pll_bypass_src_sels));
    CLK_BUG_ON(NULL == clk);

    static struct clk_mux pll4_bypass_src;
    clk = imx_clk_mux(
            &pll4_bypass_src,
            IMX6UL_PLL4_BYPASS_SRC,
            "pll4_bypass_src",
            base + 0x70, 14, 1,
            pll_bypass_src_sels,
            ARRAY_SIZE(pll_bypass_src_sels));
    CLK_BUG_ON(NULL == clk);

    static struct clk_mux pll5_bypass_src;
    clk = imx_clk_mux(
            &pll5_bypass_src,
            IMX6UL_PLL5_BYPASS_SRC,
            "pll5_bypass_src",
            base + 0xa0, 14, 1,
            pll_bypass_src_sels,
            ARRAY_SIZE(pll_bypass_src_sels));
    CLK_BUG_ON(NULL == clk);

    static struct clk_mux pll6_bypass_src;
    clk = imx_clk_mux(
            &pll6_bypass_src,
            IMX6UL_PLL6_BYPASS_SRC,
            "pll6_bypass_src",
            base + 0xe0, 14, 1,
            pll_bypass_src_sels,
            ARRAY_SIZE(pll_bypass_src_sels));
    CLK_BUG_ON(NULL == clk);

    static struct clk_mux pll7_bypass_src;
    clk = imx_clk_mux(
            &pll7_bypass_src,
            IMX6UL_PLL7_BYPASS_SRC,
            "pll7_bypass_src",
            base + 0x20, 14, 1,
            pll_bypass_src_sels,
            ARRAY_SIZE(pll_bypass_src_sels));
    CLK_BUG_ON(NULL == clk);

    static struct clk_pllv3 pll1;
    clk = imx_clk_pllv3(
            &pll1,
            IMX6UL_CLK_PLL1,
            IMX_PLLV3_SYS,
            "pll1",
            "pll1_bypass_src",
            base + 0x00,
            0x7f);
    CLK_BUG_ON(NULL == clk);

    static struct clk_pllv3 pll2;
    clk = imx_clk_pllv3(
            &pll2,
            IMX6UL_CLK_PLL2,
            IMX_PLLV3_GENERIC,
            "pll2",
            "pll2_bypass_src",
            base + 0x30,
            0x1);
    CLK_BUG_ON(NULL == clk);

    static struct clk_pllv3 pll3;
    clk = imx_clk_pllv3(
            &pll3,
            IMX6UL_CLK_PLL3,
            IMX_PLLV3_USB,
            "pll3",
            "pll3_bypass_src",
            base + 0x10,
            0x3);
    CLK_BUG_ON(NULL == clk);

    static struct clk_pllv3 pll4;
    clk = imx_clk_pllv3(
            &pll4,
            IMX6UL_CLK_PLL4,
            IMX_PLLV3_AV,
            "pll4",
            "pll4_bypass_src",
            base + 0x70,
            0x7f);
    CLK_BUG_ON(NULL == clk);

    static struct clk_pllv3 pll5;
    clk = imx_clk_pllv3(
            &pll5,
            IMX6UL_CLK_PLL5,
            IMX_PLLV3_AV,
            "pll5",
            "pll5_bypass_src",
            base + 0xa0,
            0x7f);
    CLK_BUG_ON(NULL == clk);

    static struct clk_pllv3 pll6;
    clk = imx_clk_pllv3(
            &pll6,
            IMX6UL_CLK_PLL6,
            IMX_PLLV3_ENET,
            "pll6",
            "pll6_bypass_src",
            base + 0xe0,
            0x3);
    CLK_BUG_ON(NULL == clk);

    static struct clk_pllv3 pll7;
    clk = imx_clk_pllv3(
            &pll7,
            IMX6UL_CLK_PLL7,
            IMX_PLLV3_USB,
            "pll7",
            "pll7_bypass_src",
            base + 0x20,
            0x3);
    CLK_BUG_ON(NULL == clk);

    static struct clk_mux pll1_bypass;
    clk = imx_clk_mux_flags(
            &pll1_bypass,
            IMX6UL_PLL1_BYPASS,
            "pll1_bypass",
            base + 0x00,
            16,
            1,
            pll1_bypass_sels,
            ARRAY_SIZE(pll1_bypass_sels),
            CLK_SET_RATE_PARENT);
    CLK_BUG_ON(NULL == clk);

    static struct clk_mux pll2_bypass;
    clk = imx_clk_mux_flags(
            &pll2_bypass,
            IMX6UL_PLL2_BYPASS,
            "pll2_bypass",
            base + 0x30,
            16,
            1,
            pll2_bypass_sels,
            ARRAY_SIZE(pll2_bypass_sels),
            CLK_SET_RATE_PARENT);
    CLK_BUG_ON(NULL == clk);

    static struct clk_mux pll3_bypass;
    clk = imx_clk_mux_flags(
            &pll3_bypass,
            IMX6UL_PLL3_BYPASS,
            "pll3_bypass",
            base + 0x10,
            16,
            1,
            pll3_bypass_sels,
            ARRAY_SIZE(pll3_bypass_sels),
            CLK_SET_RATE_PARENT);
    CLK_BUG_ON(NULL == clk);

    static struct clk_mux pll4_bypass;
    clk = imx_clk_mux_flags(
            &pll4_bypass,
            IMX6UL_PLL4_BYPASS,
            "pll4_bypass",
            base + 0x70,
            16,
            1,
            pll4_bypass_sels,
            ARRAY_SIZE(pll4_bypass_sels),
            CLK_SET_RATE_PARENT);
    CLK_BUG_ON(NULL == clk);

    static struct clk_mux pll5_bypass;
    clk = imx_clk_mux_flags(
            &pll5_bypass,
            IMX6UL_PLL5_BYPASS,
            "pll5_bypass",
            base + 0xa0,
            16,
            1,
            pll5_bypass_sels,
            ARRAY_SIZE(pll5_bypass_sels),
            CLK_SET_RATE_PARENT);
    CLK_BUG_ON(NULL == clk);

    static struct clk_mux pll6_bypass;
    clk = imx_clk_mux_flags(
            &pll6_bypass,
            IMX6UL_PLL6_BYPASS,
            "pll6_bypass",
            base + 0xe0,
            16,
            1,
            pll6_bypass_sels,
            ARRAY_SIZE(pll6_bypass_sels),
            CLK_SET_RATE_PARENT);
    CLK_BUG_ON(NULL == clk);

    static struct clk_mux pll7_bypass;
    clk = imx_clk_mux_flags(
            &pll7_bypass,
            IMX6UL_PLL7_BYPASS,
            "pll7_bypass",
            base + 0x20,
            16,
            1,
            pll7_bypass_sels,
            ARRAY_SIZE(pll7_bypass_sels),
            CLK_SET_RATE_PARENT);
    CLK_BUG_ON(NULL == clk);

    // Do not bypass PLLs initially
    aw_clk_parent_set(IMX6UL_PLL1_BYPASS,IMX6UL_CLK_PLL1);
    aw_clk_parent_set(IMX6UL_PLL2_BYPASS,IMX6UL_CLK_PLL2);
    aw_clk_parent_set(IMX6UL_PLL3_BYPASS,IMX6UL_CLK_PLL3);
    aw_clk_parent_set(IMX6UL_PLL4_BYPASS,IMX6UL_CLK_PLL4);
    aw_clk_parent_set(IMX6UL_PLL5_BYPASS,IMX6UL_CLK_PLL5);
    aw_clk_parent_set(IMX6UL_PLL6_BYPASS,IMX6UL_CLK_PLL6);
    aw_clk_parent_set(IMX6UL_PLL7_BYPASS,IMX6UL_CLK_PLL7);

    static struct clk_fixed_factor pll1_sys;
    clk = imx_clk_fixed_factor(
            &pll1_sys,
            IMX6UL_CLK_PLL1_SYS,
            "pll1_sys",
            "pll1_bypass",
            1,
            1);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate pll2_bus;
    clk = imx_clk_gate(
            &pll2_bus,
            IMX6UL_CLK_PLL2_BUS,
            "pll2_bus",
            "pll2_bypass",
            base + 0x30, 13);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate pll3_usb_otg;
    clk = imx_clk_gate(
            &pll3_usb_otg,
            IMX6UL_CLK_PLL3_USB_OTG,
            "pll3_usb_otg",
            "pll3_bypass",
            base + 0x10,
            13);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate pll4_audio;
    clk = imx_clk_gate(
            &pll4_audio,
            IMX6UL_CLK_PLL4_AUDIO,
            "pll4_audio",
            "pll4_bypass",
            base + 0x70,
            13);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate pll5_video;
    clk = imx_clk_gate(
            &pll5_video,
            IMX6UL_CLK_PLL5_VIDEO,
            "pll5_video",
            "pll5_bypass",
            base + 0xa0,
            13);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate pll6_enet;
    clk = imx_clk_gate(
            &pll6_enet,
            IMX6UL_CLK_PLL6_ENET,
            "pll6_enet",
            "pll6_bypass",
            base + 0xe0,
            19);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate pll7_usb_host;
    clk = imx_clk_gate(
            &pll7_usb_host,
            IMX6UL_CLK_PLL7_USB_HOST,
            "pll7_usb_host",
            "pll7_bypass",
            base + 0x20,
            13);
    CLK_BUG_ON(NULL == clk);

    /*
     * usbphy*_gate needs to be on after system boots up, and software
     * never needs to control it anymore.
     */
    static struct clk_gate usbphy1_gate;
    clk = imx_clk_gate(
            &usbphy1_gate,
            IMX6UL_CLK_USBPHY1_GATE,
            "usbphy1_gate",
            "pll3_usb_otg",
            base + 0x10,
            6);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate usbphy2_gate;
    clk = imx_clk_gate(
            &usbphy2_gate,
            IMX6UL_CLK_USBPHY2_GATE,
            "usbphy2_gate",
            "pll7_usb_host",
            base + 0x20,
            6);
    CLK_BUG_ON(NULL == clk);

    static struct clk_pfd pll2_pfd0_352m;
    clk = imx_clk_pfd(
            &pll2_pfd0_352m,
            IMX6UL_CLK_PLL2_PFD0,
            "pll2_pfd0_352m",
            "pll2_bus",
            base + 0x100,
            0);
    CLK_BUG_ON(NULL == clk);

    static struct clk_pfd pll2_pfd1_594m;
    clk = imx_clk_pfd(
            &pll2_pfd1_594m,
            IMX6UL_CLK_PLL2_PFD1,
            "pll2_pfd1_594m",
            "pll2_bus",
            base + 0x100,
            1);
    CLK_BUG_ON(NULL == clk);

    static struct clk_pfd pll2_pfd2_396m;
    clk = imx_clk_pfd(
            &pll2_pfd2_396m,
            IMX6UL_CLK_PLL2_PFD2,
            "pll2_pfd2_396m",
            "pll2_bus",
            base + 0x100,
            2);
    CLK_BUG_ON(NULL == clk);

    static struct clk_pfd pll2_pfd3_594m;
    clk = imx_clk_pfd(
            &pll2_pfd3_594m,
            IMX6UL_CLK_PLL2_PFD3,
            "pll2_pfd3_594m",
            "pll2_bus",
            base + 0x100,
            3);
    CLK_BUG_ON(NULL == clk);

    static struct clk_pfd pll3_pfd0_720m;
    clk = imx_clk_pfd(
            &pll3_pfd0_720m,
            IMX6UL_CLK_PLL3_PFD0,
            "pll3_pfd0_720m",
            "pll3_usb_otg",
            base + 0xf0,
            0);
    CLK_BUG_ON(NULL == clk);

    static struct clk_pfd pll3_pfd1_540m;
    clk = imx_clk_pfd(
            &pll3_pfd1_540m,
            IMX6UL_CLK_PLL3_PFD1,
            "pll3_pfd1_540m",
            "pll3_usb_otg",
            base + 0xf0,
            1);
    CLK_BUG_ON(NULL == clk);

    static struct clk_pfd pll3_pfd2_508m;
    clk = imx_clk_pfd(
            &pll3_pfd2_508m,
            IMX6UL_CLK_PLL3_PFD2,
            "pll3_pfd2_508m",
            "pll3_usb_otg",
            base + 0xf0,
            2);
    CLK_BUG_ON(NULL == clk);

    static struct clk_pfd pll3_pfd3_454m;
    clk = imx_clk_pfd(
            &pll3_pfd3_454m,
            IMX6UL_CLK_PLL3_PFD3,
            "pll3_pfd3_454m",
            "pll3_usb_otg",
            base + 0xf0,
            3);
    CLK_BUG_ON(NULL == clk);

    static struct clk_divider enet_ref;
    clk = clk_register_divider_table(
            &enet_ref,
            IMX6UL_CLK_ENET_REF,
            "enet_ref",
            "pll6_enet",
            0,
            base + 0xe0,
            0,
            2,
            0,
            clk_enet_ref_table,
            NULL);
    CLK_BUG_ON(NULL == clk);

    static struct clk_divider enet2_ref;
    clk  = clk_register_divider_table(
            &enet2_ref,
            IMX6UL_CLK_ENET2_REF,
            "enet2_ref",
            "pll6_enet",
            0,
            base + 0xe0,
            2,
            2,
            0,
            clk_enet_ref_table,
            NULL);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate enet1_ref_125m;
    clk = imx_clk_gate(
            &enet1_ref_125m,
            IMX6UL_CLK_ENET1_REF_125M,
            "enet1_ref_125m",
            "enet_ref",
            base + 0xe0,
            13);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate enet2_ref_125m;
    clk = imx_clk_gate(
            &enet2_ref_125m,
            IMX6UL_CLK_ENET2_REF_125M,
            "enet2_ref_125m",
            "enet2_ref",
            base + 0xe0,
            20);
    CLK_BUG_ON(NULL == clk);

    static struct clk_fixed_factor enet_ptp_ref;
    clk = imx_clk_fixed_factor(
            &enet_ptp_ref,
            IMX6UL_CLK_ENET_PTP_REF,
            "enet_ptp_ref",
            "pll6_enet",
            1,
            20);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate enet_ptp;
    clk = imx_clk_gate(
            &enet_ptp,
            IMX6UL_CLK_ENET_PTP,
            "enet_ptp",
            "enet_ptp_ref",
            base + 0xe0,
            21);
    CLK_BUG_ON(NULL == clk);

    static struct clk_divider pll4_post_div;
    clk = clk_register_divider_table(
            &pll4_post_div,
            IMX6UL_CLK_PLL4_POST_DIV,
            "pll4_post_div",
            "pll4_audio",
            CLK_SET_RATE_PARENT | CLK_SET_RATE_GATE,
            base + 0x70,
            19,
            2,
            0,
            post_div_table,
            NULL);
    CLK_BUG_ON(NULL == clk);

    static struct clk_divider pll4_audio_div;
    clk = clk_register_divider(
            &pll4_audio_div,
            IMX6UL_CLK_PLL4_AUDIO_DIV,
            "pll4_audio_div",
            "pll4_post_div",
            CLK_SET_RATE_PARENT | CLK_SET_RATE_GATE,
            base + 0x170,
            15,
            1,
            0,
            NULL);
    CLK_BUG_ON(NULL == clk);

    static struct clk_divider pll5_post_div;
    clk = clk_register_divider_table(
            &pll5_post_div,
            IMX6UL_CLK_PLL5_POST_DIV,
            "pll5_post_div",
            "pll5_video",
            CLK_SET_RATE_PARENT | CLK_SET_RATE_GATE,
            base + 0xa0,
            19,
            2,
            0,
            post_div_table,
            NULL);
    CLK_BUG_ON(NULL == clk);

    static struct clk_divider pll5_video_div;
    clk = clk_register_divider_table(
            &pll5_video_div,
            IMX6UL_CLK_PLL5_VIDEO_DIV,
            "pll5_video_div",
            "pll5_post_div",
            CLK_SET_RATE_PARENT | CLK_SET_RATE_GATE,
            base + 0x170,
            30,
            2,
            0,
            video_div_table,
            NULL);
    CLK_BUG_ON(NULL == clk);

    static struct clk_fixed_factor pll2_198m;
    clk = imx_clk_fixed_factor(
            &pll2_198m,
            IMX6UL_CLK_PLL2_198M,
            "pll2_198m",
            "pll2_pfd2_396m",
            1,
            2);
    CLK_BUG_ON(NULL == clk);

    static struct clk_fixed_factor pll3_80m;
    clk = imx_clk_fixed_factor(
            &pll3_80m,
            IMX6UL_CLK_PLL3_80M,
            "pll3_80m",
            "pll3_usb_otg",
            1,
            6);
    CLK_BUG_ON(NULL == clk);

    static struct clk_fixed_factor pll3_60m;
    clk = imx_clk_fixed_factor(
            &pll3_60m,
            IMX6UL_CLK_PLL3_60M,
            "pll3_60m",
            "pll3_usb_otg",
            1,
            8);
    CLK_BUG_ON(NULL == clk);

    static struct clk_fixed_factor pll3_120m;
    clk = imx_clk_fixed_factor(
            &pll3_120m,
            IMX6UL_CLK_PLL3_120M,
            "pll3_120m",
            "pll3_usb_otg",
            1,
            4);
    CLK_BUG_ON(NULL == clk);

    static struct clk_fixed_factor gpt_3m;
    clk = imx_clk_fixed_factor(
            &gpt_3m,
            IMX6UL_CLK_GPT_3M,
            "gpt_3m",
            "osc",
            1,
            8);
    CLK_BUG_ON(NULL == clk);

    base = (void *)0x020c4000;
    static struct clk_mux ca7_secondary_sel;
    clk = imx_clk_mux(
            &ca7_secondary_sel,
            IMX6UL_CA7_SECONDARY_SEL,
            "ca7_secondary_sel",
            base + 0xc,
            3,
            1,
            ca7_secondary_sels,
            ARRAY_SIZE(ca7_secondary_sels));
    CLK_BUG_ON(NULL == clk);

    static struct clk_mux step;
    clk = imx_clk_mux(
            &step,
            IMX6UL_CLK_STEP,
            "step",
            base + 0x0c,
            8,
            1,
            step_sels,
            ARRAY_SIZE(step_sels));
    CLK_BUG_ON(NULL == clk);

    static struct clk_mux pll1_sw;
    clk = imx_clk_mux_glitchless(
            &pll1_sw,
            IMX6UL_CLK_PLL1_SW,
            "pll1_sw",
            base + 0x0c,
            2,
            1,
            pll1_sw_sels,
            ARRAY_SIZE(pll1_sw_sels));
    CLK_BUG_ON(NULL == clk);

    static struct clk_mux axi_alt_sel;
    clk = imx_clk_mux(
            &axi_alt_sel,
            IMX6UL_CLK_AXI_ALT_SEL,
            "axi_alt_sel",
            base + 0x14,
            7,
            1,
            axi_alt_sels,
            ARRAY_SIZE(axi_alt_sels));
    CLK_BUG_ON(NULL == clk);

    static struct clk_mux periph_pre;
    clk = imx_clk_mux_bus(
            &periph_pre,
            IMX6UL_CLK_PERIPH_PRE,
            "periph_pre",
            base + 0x18,
            18,
            2,
            periph_pre_sels,
            ARRAY_SIZE(periph_pre_sels));
    CLK_BUG_ON(NULL == clk);

    static struct clk_mux periph2_pre;
    clk = imx_clk_mux_bus(&periph2_pre,
            IMX6UL_CLK_PERIPH2_PRE,
            "periph2_pre",
            base + 0x18,
            21,
            2,
            periph2_pre_sels,
            ARRAY_SIZE(periph2_pre_sels));
    CLK_BUG_ON(NULL == clk);

    static struct clk_mux periph_clk2_sel;
    clk = imx_clk_mux_bus(
            &periph_clk2_sel,
            IMX6UL_CLK_PERIPH_CLK2_SEL,
            "periph_clk2_sel",
            base + 0x18,
            12,
            2,
            periph_clk2_sels,
            ARRAY_SIZE(periph_clk2_sels));
    CLK_BUG_ON(NULL == clk);

    static struct clk_mux periph2_clk2_sel;
    clk = imx_clk_mux_bus(
            &periph2_clk2_sel,
            IMX6UL_CLK_PERIPH2_CLK2_SEL,
            "periph2_clk2_sel",
            base + 0x18,
            20,
            1,
            periph2_clk2_sels,
            ARRAY_SIZE(periph2_clk2_sels));
    CLK_BUG_ON(NULL == clk);

    static struct clk_mux gpmi_sel;
    clk = imx_clk_mux(
            &gpmi_sel,
            IMX6UL_CLK_GPMI_SEL,
            "gpmi_sel",
            base + 0x1c,
            19,
            1,
            gpmi_sels,
            ARRAY_SIZE(gpmi_sels));
    CLK_BUG_ON(NULL == clk);

    static struct clk_mux bch_sel;
    clk = imx_clk_mux(
            &bch_sel,
            IMX6UL_CLK_BCH_SEL,
            "bch_sel",
            base + 0x1c, 18, 1,
            bch_sels, ARRAY_SIZE(bch_sels));
    CLK_BUG_ON(NULL == clk);

    static struct clk_mux usdhc2_sel;
    clk = imx_clk_mux(
            &usdhc2_sel,
            IMX6UL_CLK_USDHC2_SEL,
            "usdhc2_sel",
            base + 0x1c,
            17,
            1,
            usdhc_sels,
            ARRAY_SIZE(usdhc_sels));
    CLK_BUG_ON(NULL == clk);

    static struct clk_mux usdhc1_sel;
    clk = imx_clk_mux(
            &usdhc1_sel,
            IMX6UL_CLK_USDHC1_SEL,
            "usdhc1_sel",
            base + 0x1c,
            16,
            1,
            usdhc_sels,
            ARRAY_SIZE(usdhc_sels));
    CLK_BUG_ON(NULL == clk);

    static struct clk_mux sai3_sel;
    clk = imx_clk_mux(
            &sai3_sel,
            IMX6UL_CLK_SAI3_SEL,
            "sai3_sel",
            base + 0x1c,
            14,
            2,
            sai_sels,
            ARRAY_SIZE(sai_sels));
    CLK_BUG_ON(NULL == clk);

    static struct clk_mux sai2_sel;
    clk = imx_clk_mux(
            &sai2_sel,
            IMX6UL_CLK_SAI2_SEL,
            "sai2_sel",
            base + 0x1c,
            12,
            2,
            sai_sels,
            ARRAY_SIZE(sai_sels));
    CLK_BUG_ON(NULL == clk);

    static struct clk_mux sai1_sel;
    clk = imx_clk_mux(
            &sai1_sel,
            IMX6UL_CLK_SAI1_SEL,
            "sai1_sel",
            base + 0x1c,
            10,
            2,
            sai_sels,
            ARRAY_SIZE(sai_sels));
    CLK_BUG_ON(NULL == clk);

    static struct clk_mux qspi1_sel;
    clk = imx_clk_mux(
            &qspi1_sel,
            IMX6UL_CLK_QSPI1_SEL,
            "qspi1_sel",
            base + 0x1c,
            7,
            3,
            qspi1_sels,
            ARRAY_SIZE(qspi1_sels));
    CLK_BUG_ON(NULL == clk);

    static struct clk_mux can_sel;
    clk = imx_clk_mux(
            &can_sel,
            IMX6UL_CLK_CAN_SEL,
            "can_sel",
            base + 0x20,
            8,
            2,
            can_sels,
            ARRAY_SIZE(can_sels));
    CLK_BUG_ON(NULL == clk);

    static struct clk_mux uart_sel;
    clk = imx_clk_mux(
            &uart_sel,
            IMX6UL_CLK_UART_SEL,
            "uart_sel",
            base + 0x24,
            6,
            1,
            uart_sels,
            ARRAY_SIZE(uart_sels));
    CLK_BUG_ON(NULL == clk);

    static struct clk_mux enfc_sel;
    clk = imx_clk_mux(
            &enfc_sel,
            IMX6UL_CLK_ENFC_SEL,
            "enfc_sel",
            base + 0x2c,
            15,
            3,
            enfc_sels,
            ARRAY_SIZE(enfc_sels));
    CLK_BUG_ON(NULL == clk);

    static struct clk_mux ldb_di0_sel;
    clk = imx_clk_mux(
            &ldb_di0_sel,
            IMX6UL_CLK_LDB_DI0_SEL,
            "ldb_di0_sel",
            base + 0x2c,
            9,
            3,
            ldb_di0_sels,
            ARRAY_SIZE(ldb_di0_sels));
    CLK_BUG_ON(NULL == clk);

    static struct clk_mux spdif_sel;
    clk = imx_clk_mux(
            &spdif_sel,
            IMX6UL_CLK_SPDIF_SEL,
            "spdif_sel",
            base + 0x30,
            20,
            2,
            spdif_sels,
            ARRAY_SIZE(spdif_sels));
    CLK_BUG_ON(NULL == clk);

    static struct clk_mux sim_pre_sel;
    clk = imx_clk_mux(
            &sim_pre_sel,
            IMX6UL_CLK_SIM_PRE_SEL,
            "sim_pre_sel",
            base + 0x34,
            15,
            3,
            sim_pre_sels,
            ARRAY_SIZE(sim_pre_sels));
    CLK_BUG_ON(NULL == clk);

    static struct clk_mux ecspi_sel;
    clk = imx_clk_mux(
            &ecspi_sel,
            IMX6UL_CLK_ECSPI_SEL,
            "ecspi_sel",
            base + 0x38,
            18,
            1,
            ecspi_sels,
            ARRAY_SIZE(ecspi_sels));
    CLK_BUG_ON(NULL == clk);

    static struct clk_mux lcdif_pre_sel;
    clk = imx_clk_mux(
            &lcdif_pre_sel,
            IMX6UL_CLK_LCDIF_PRE_SEL,
            "lcdif_pre_sel",
            base + 0x38,
            15,
            3,
            lcdif_pre_sels,
            ARRAY_SIZE(lcdif_pre_sels));
    CLK_BUG_ON(NULL == clk);

    static struct clk_fixed_factor ldb_di0_div_3_5;
    clk = imx_clk_fixed_factor(
            &ldb_di0_div_3_5,
            IMX6UL_CLK_LDB_DI0_DIV_3_5,
            "ldb_di0_div_3_5",
            "ldb_di0_sel",
            2,
            7);
    CLK_BUG_ON(NULL == clk);

    static struct clk_fixed_factor ldb_di0_div_7;
    clk = imx_clk_fixed_factor(
            &ldb_di0_div_7,
            IMX6UL_CLK_LDB_DI0_DIV_7,
            "ldb_di0_div_7",
            "ldb_di0_sel",
            1,
            7);
    CLK_BUG_ON(NULL == clk);

    static struct clk_fixed_factor ldb_di1_div_3_5;
    clk = imx_clk_fixed_factor(
            &ldb_di1_div_3_5,
            IMX6UL_CLK_LDB_DI1_DIV_3_5,
            "ldb_di1_div_3_5",
            "qspi1_sel",
            2,
            7);
    CLK_BUG_ON(NULL == clk);

    static struct clk_fixed_factor ldb_di1_div_7;
    clk = imx_clk_fixed_factor(
            &ldb_di1_div_7,
            IMX6UL_CLK_LDB_DI1_DIV_7,
            "ldb_di1_div_7",
            "qspi1_sel",
            1,
            7);
    CLK_BUG_ON(NULL == clk);

    static struct clk_divider periph2_clk2;
    clk = imx_clk_divider(
            &periph2_clk2,
            IMX6UL_CLK_PERIPH2_CLK2,
            "periph2_clk2",
            "periph2_clk2_sel",
            base + 0x14,
            0,
            3);
    CLK_BUG_ON(NULL == clk);


    static struct clk_divider periph_clk2;
    clk = imx_clk_divider(
            &periph_clk2,
            IMX6UL_CLK_PERIPH_CLK2,
            "periph_clk2",
            "periph_clk2_sel",
            base + 0x14,
            27,
            3);
    CLK_BUG_ON(NULL == clk);

    static struct clk_busy_mux periph;
    clk = imx_clk_busy_mux(
            &periph,
            IMX6UL_CLK_PERIPH,
            "periph",
            base + 0x14,
            25,
            1,
            base + 0x48,
            5,
            periph_sels,
            ARRAY_SIZE(periph_sels));
    CLK_BUG_ON(NULL == clk);

    static struct clk_busy_mux periph2;
    clk = imx_clk_busy_mux(
            &periph2,
            IMX6UL_CLK_PERIPH2,
            "periph2",
            base + 0x14,
            26,
            1,
            base + 0x48,
            3,
            periph2_sels,
            ARRAY_SIZE(periph2_sels));
    CLK_BUG_ON(NULL == clk);

    static struct clk_divider qspi1_podf;
    clk = imx_clk_divider(
            &qspi1_podf,
            IMX6UL_CLK_QSPI1_PDOF,
            "qspi1_podf",
            "qspi1_sel",
            base + 0x1c,
            26,
            3);
    CLK_BUG_ON(NULL == clk);

    static struct clk_divider can_podf;
    clk = imx_clk_divider(
            &can_podf,
            IMX6UL_CLK_CAN_PODF,
            "can_podf",
            "can_sel",
            base + 0x20,
            2,
            6);
    CLK_BUG_ON(NULL == clk);

    static struct clk_divider gpmi_podf;
    clk = imx_clk_divider(
            &gpmi_podf,
            IMX6UL_CLK_GPMI_PODF,
            "gpmi_podf",
            "gpmi_sel",
            base + 0x24,
            22,
            3);
    CLK_BUG_ON(NULL == clk);

    static struct clk_divider bch_podf;
    clk = imx_clk_divider(
            &bch_podf,
            IMX6UL_CLK_BCH_PODF,
            "bch_podf",
            "bch_sel",
            base + 0x24,
            19,
            3);
    CLK_BUG_ON(NULL == clk);

    static struct clk_divider usdhc2_podf;
    clk = imx_clk_divider(
            &usdhc2_podf,
            IMX6UL_CLK_USDHC2_PODF,
            "usdhc2_podf",
            "usdhc2_sel",
            base + 0x24,
            16,
            3);
    CLK_BUG_ON(NULL == clk);

    static struct clk_divider usdhc1_podf;
    clk = imx_clk_divider(
            &usdhc1_podf,
            IMX6UL_CLK_USDHC1_PODF,
            "usdhc1_podf",
            "usdhc1_sel",
            base + 0x24,
            11,
            3);
    CLK_BUG_ON(NULL == clk);

    static struct clk_divider uart_podf;
    clk = imx_clk_divider(
            &uart_podf,
            IMX6UL_CLK_UART_PODF,
            "uart_podf",
            "uart_sel",
            base + 0x24,
            0,
            6);
    CLK_BUG_ON(NULL == clk);

    static struct clk_divider sai3_pred;
    clk = imx_clk_divider(
            &sai3_pred,
            IMX6UL_CLK_SAI3_PRED,
            "sai3_pred",
            "sai3_sel",
            base + 0x28,
            22,
            3);
    CLK_BUG_ON(NULL == clk);

    static struct clk_divider sai3_podf;
    clk = imx_clk_divider(
            &sai3_podf,
            IMX6UL_CLK_SAI3_PODF,
            "sai3_podf",
            "sai3_pred",
            base + 0x28,
            16,
            6);
    CLK_BUG_ON(NULL == clk);

    static struct clk_divider sai1_pred;
    clk = imx_clk_divider(
            &sai1_pred,
            IMX6UL_CLK_SAI1_PRED,
            "sai1_pred",
            "sai1_sel",
            base + 0x28,
            6,
            3);
    CLK_BUG_ON(NULL == clk);

    static struct clk_divider sai1_podf;
    clk = imx_clk_divider(
            &sai1_podf,
            IMX6UL_CLK_SAI1_PODF,
            "sai1_podf",
            "sai1_pred",
            base + 0x28, 0,  6);
    CLK_BUG_ON(NULL == clk);

    static struct clk_divider enfc_pred;
    clk = imx_clk_divider(
            &enfc_pred,
            IMX6UL_CLK_ENFC_PRED,
            "enfc_pred",
            "enfc_sel",
            base + 0x2c,
            18,
            3);
    CLK_BUG_ON(NULL == clk);

    static struct clk_divider enfc_podf;
    clk = imx_clk_divider(
            &enfc_podf,
            IMX6UL_CLK_ENFC_PODF,
            "enfc_podf",
            "enfc_pred",
            base + 0x2c,
            21,
            6);
    CLK_BUG_ON(NULL == clk);

    static struct clk_divider sai2_pred;
    clk = imx_clk_divider(
            &sai2_pred,
            IMX6UL_CLK_SAI2_PRED,
            "sai2_pred",
            "sai2_sel",
            base + 0x2c,
            6,
            3);
    CLK_BUG_ON(NULL == clk);

    static struct clk_divider sai2_podf;
    clk = imx_clk_divider(
            &sai2_podf,
            IMX6UL_CLK_SAI2_PODF,
            "sai2_podf",
            "sai2_pred",
            base + 0x2c,
            0,
            6);
    CLK_BUG_ON(NULL == clk);

    static struct clk_divider spdif_pred;
    clk = imx_clk_divider(
            &spdif_pred,
            IMX6UL_CLK_SPDIF_PRED,
            "spdif_pred",
            "spdif_sel",
            base + 0x30,
            25,
            3);
    CLK_BUG_ON(NULL == clk);

    static struct clk_divider spdif_podf;
    clk = imx_clk_divider(
            &spdif_podf,
            IMX6UL_CLK_SPDIF_PODF,
            "spdif_podf",
            "spdif_pred",
            base + 0x30,
            22,
            3);
    CLK_BUG_ON(NULL == clk);

    static struct clk_divider sim_podf;
    clk = imx_clk_divider(
            &sim_podf,
            IMX6UL_CLK_SIM_PODF,
            "sim_podf",
            "sim_pre_sel",
            base + 0x34,
            12,
            3);
    CLK_BUG_ON(NULL == clk);

    static struct clk_divider ecspi_podf;
    clk = imx_clk_divider(
            &ecspi_podf,
            IMX6UL_CLK_ECSPI_PODF,
            "ecspi_podf",
            "ecspi_sel",
            base + 0x38,
            19,
            6);
    CLK_BUG_ON(NULL == clk);

    static struct clk_divider lcdif_pred;
    clk = imx_clk_divider(
            &lcdif_pred,
            IMX6UL_CLK_LCDIF_PRED,
            "lcdif_pred",
            "lcdif_pre_sel",
            base + 0x38,
            12,
            3);
    CLK_BUG_ON(NULL == clk);

    static struct clk_divider lcdif_podf;
    clk = imx_clk_divider(
            &lcdif_podf,
            IMX6UL_CLK_LCDIF_PODF,
            "lcdif_podf",
            "lcdif_pred",
            base + 0x18,
            23,
            3);
    CLK_BUG_ON(NULL == clk);

    static struct clk_busy_divider arm;
    clk = imx_clk_busy_divider(
            &arm,
            IMX6UL_CLK_ARM,
            "arm",
            "pll1_sw",
            base +  0x10,
            0,
            3,
            base + 0x48, 16);
    CLK_BUG_ON(NULL == clk);

    static struct clk_busy_divider mmdc_podf;
    clk = imx_clk_busy_divider(
            &mmdc_podf,
            IMX6UL_CLK_MMDC_PODF,
            "mmdc_podf",
            "periph2",
            base +  0x14,
            3,
            3,
            base + 0x48,
            2);
    CLK_BUG_ON(NULL == clk);

    static struct clk_mux axi_sel;
    clk = imx_clk_mux_glitchless(
            &axi_sel,
            IMX6UL_CLK_AXI_SEL,
            "axi_sel",
            base + 0x14,
            6,
            1,
            axi_sels,
            ARRAY_SIZE(axi_sels));
    CLK_BUG_ON(NULL == clk);

    static struct clk_busy_divider axi_podf;
    clk = imx_clk_busy_divider(
            &axi_podf,
            IMX6UL_CLK_AXI_PODF,
            "axi_podf",
            "axi_sel",
            base +  0x14,
            16,
            3,
            base + 0x48,
            0);
    CLK_BUG_ON(NULL == clk);

    static struct clk_busy_divider ahb;
    clk = imx_clk_busy_divider(
            &ahb,
            IMX6UL_CLK_AHB,
            "ahb",
            "periph",
            base +  0x14,
            10,
            3,
            base + 0x48, 1);
    CLK_BUG_ON(NULL == clk);

    static struct clk_divider ipg;
    clk = imx_clk_divider(
            &ipg,
            IMX6UL_CLK_IPG,
            "ipg",
            "ahb",
            base + 0x14,
            8,
            2);
    CLK_BUG_ON(NULL == clk);

    static struct clk_mux ldb_di0;
    clk = imx_clk_mux(
            &ldb_di0,
            IMX6UL_CLK_LDB_DI0_DIV_SEL,
            "ldb_di0",
            base + 0x20,
            10,
            1,
            ldb_di0_div_sels,
            ARRAY_SIZE(ldb_di0_div_sels));
    CLK_BUG_ON(NULL == clk);

    static struct clk_mux ldb_di1;
    clk = imx_clk_mux(
            &ldb_di1,
            IMX6UL_CLK_LDB_DI1_DIV_SEL,
            "ldb_di1",
            base + 0x20,
            11,
            1,
            ldb_di1_div_sels, ARRAY_SIZE(ldb_di1_div_sels));
    CLK_BUG_ON(NULL == clk);

    static struct clk_mux sim_sel;
    clk = imx_clk_mux(
            &sim_sel,
            IMX6UL_CLK_SIM_SEL,
            "sim_sel",
            base + 0x34,
            9,
            3,
            sim_sels,
            ARRAY_SIZE(sim_sels));
    CLK_BUG_ON(NULL == clk);

    static struct clk_mux csi_sel;
    clk = imx_clk_mux_flags(
            &csi_sel,
            IMX6UL_CLK_CSI_SEL,
            "csi_sel",
            base + 0x3c,
            9,
            2,
            csi_sels,
            ARRAY_SIZE(csi_sels),
            CLK_SET_RATE_PARENT);
    CLK_BUG_ON(NULL == clk);

    static struct clk_divider csi_podf;
    clk = imx_clk_divider(
            &csi_podf,
            IMX6UL_CLK_CSI_PODF,
            "csi_podf",
            "csi_sel",
            base + 0x3c, 11, 3);
    CLK_BUG_ON(NULL == clk);

    static struct clk_mux lcdif_sel;
    clk = imx_clk_mux(
            &lcdif_sel,
            IMX6UL_CLK_LCDIF_SEL,
            "lcdif_sel",
            base + 0x38,
            9,
            3,
            lcdif_sels,
            ARRAY_SIZE(lcdif_sels));
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate2 axi;
    clk = imx_clk_busy_gate(
            &axi,
            IMX6UL_CLK_AXI,
            "axi",
            "axi_podf",
            base + 0x74,
            28);
    CLK_BUG_ON(NULL == clk);

    static struct clk_mux eim_slow_sel;
    clk = imx_clk_mux(
            &eim_slow_sel,
            IMX6UL_CLK_EIM_SLOW_SEL,
            "eim_slow_sel",
            base + 0x1c,
            29,
            2,
            eim_slow_sels,
            ARRAY_SIZE(eim_slow_sels));
    CLK_BUG_ON(NULL == clk);

    static struct clk_divider eim_slow_podf;
    clk = imx_clk_divider(
            &eim_slow_podf,
            IMX6UL_CLK_EIM_SLOW_PODF,
            "eim_slow_podf",
            "eim_slow_sel",
            base + 0x1c,
            23,
            3);
    CLK_BUG_ON(NULL == clk);

    static struct clk_mux perclk_sel;
    clk = imx_clk_mux(
            &perclk_sel,
            IMX6UL_CLK_PERCLK_SEL,
            "perclk_sel",
            base + 0x1c,
            6,
            1,
            perclk_sels,
            ARRAY_SIZE(perclk_sels));
    CLK_BUG_ON(NULL == clk);

    static struct clk_divider perclk;
    clk = imx_clk_divider(
            &perclk,
            IMX6UL_CLK_PERCLK,
            "perclk",
            "perclk_sel",
            base + 0x1c,
            0,
            6);
    CLK_BUG_ON(NULL == clk);

    /* CCGR0 */
    static struct clk_gate2 aips_tz1;
    clk = imx_clk_gate2(
            &aips_tz1,
            IMX6UL_CLK_AIPSTZ1,
            "aips_tz1",
            "ahb",
            base + 0x68,
            0);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate2 aips_tz2;
    clk = imx_clk_gate2(
            &aips_tz2,
            IMX6UL_CLK_AIPSTZ2,
            "aips_tz2",
            "ahb",
            base + 0x68,
            2);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate2 apbh_dma;
    clk = imx_clk_gate2(
            &apbh_dma,
            IMX6UL_CLK_APBHDMA,
            "apbh_dma",
            "bch_podf",
            base + 0x68,
            4);
    CLK_BUG_ON(NULL == clk);

    static unsigned int share_count_asrc;
    static struct clk_gate2 asrc_ipg;
    clk = imx_clk_gate2_shared(
            &asrc_ipg,
            IMX6UL_CLK_ASRC_IPG,
            "asrc_ipg",
            "ahb",
            base + 0x68,
            6,
            &share_count_asrc);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate2 asrc_mem;
    clk = imx_clk_gate2_shared(
            &asrc_mem,
            IMX6UL_CLK_ASRC_MEM,
            "asrc_mem",
            "ahb",
            base + 0x68,
            6,
            &share_count_asrc);
    CLK_BUG_ON(NULL == clk);

     static struct clk_gate2 caam_mem;
    clk = imx_clk_gate2(
            &caam_mem,
            IMX6UL_CLK_CAAM_MEM,
            "caam_mem",
            "ahb",
            base + 0x68,
            8);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate2 caam_aclk;
    clk = imx_clk_gate2(
            &caam_aclk,
            IMX6UL_CLK_CAAM_ACLK,
            "caam_aclk",
            "ahb",
            base + 0x68,
            10);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate2 caam_ipg;
    clk = imx_clk_gate2(
            &caam_ipg,
            IMX6UL_CLK_CAAM_IPG,
            "caam_ipg",
            "ipg",
            base + 0x68,
            12);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate2 can1_ipg;
    clk = imx_clk_gate2(
            &can1_ipg,
            IMX6UL_CLK_CAN1_IPG,
            "can1_ipg",
            "ipg",
            base + 0x68,
            14);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate2 can1_serial;
    clk = imx_clk_gate2(
            &can1_serial,
            IMX6UL_CLK_CAN1_SERIAL,
            "can1_serial",
            "can_podf",
            base + 0x68,
            16);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate2 can2_ipg;
    clk = imx_clk_gate2(
            &can2_ipg,
            IMX6UL_CLK_CAN2_IPG,
            "can2_ipg",
            "ipg",
            base + 0x68,
            18);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate2 can2_serial;
    clk = imx_clk_gate2(
            &can2_serial,
            IMX6UL_CLK_CAN2_SERIAL,
            "can2_serial",
            "can_podf",
            base + 0x68,
            20);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate2 arm_dbg;
    clk = imx_clk_gate2(
            &arm_dbg,
            IMX6UL_CLK_ARM_DBG,
            "arm_dbg",
            "ahb",
            base + 0x68,
            22);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate2 gpt2_bus;
    clk = imx_clk_gate2(
            &gpt2_bus,
            IMX6UL_CLK_GPT2_BUS,
            "gpt2_bus",
            "perclk",
            base + 0x68,
            24);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate2 gpt2_serial;
    clk = imx_clk_gate2(
            &gpt2_serial,
            IMX6UL_CLK_GPT2_SERIAL,
            "gpt2_serial",
            "perclk",
            base + 0x68,
            26);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate2 uart2_ipg;
    static unsigned int     uart2_shared_count = 0;
    clk = imx_clk_gate2_shared(
            &uart2_ipg,
            IMX6UL_CLK_UART2_IPG,
            "uart2_ipg",
            "ipg",
            base + 0x68,
            28,
            &uart2_shared_count);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate2 uart2_serial;
    clk = imx_clk_gate2_shared(
            &uart2_serial,
            IMX6UL_CLK_UART2_SERIAL,
            "uart2_serial",
            "uart_podf",
            base + 0x68,
            28,
            &uart2_shared_count);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate2 gpio2_clk;
    clk = imx_clk_gate2(
            &gpio2_clk,
            IMX6UL_CLK_GPIO2,
            "gpio2_clk",
            "ipg",
            base + 0x68,
            30);
    CLK_BUG_ON(NULL == clk);

    /* CCGR1 */
    static struct clk_gate2 ecspi1;
    clk = imx_clk_gate2(
            &ecspi1,
            IMX6UL_CLK_ECSPI1,
            "ecspi1",
            "ecspi_podf",
            base + 0x6c,
            0);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate2 ecspi2;
    clk = imx_clk_gate2(
            &ecspi2,
            IMX6UL_CLK_ECSPI2,
            "ecspi2",
            "ecspi_podf",
            base + 0x6c,
            2);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate2 ecspi3;
    clk = imx_clk_gate2(
            &ecspi3,
            IMX6UL_CLK_ECSPI3,
            "ecspi3",
            "ecspi_podf",
            base + 0x6c,
            4);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate2 ecspi4;
    clk = imx_clk_gate2(
            &ecspi4,
            IMX6UL_CLK_ECSPI4,
            "ecspi4",
            "ecspi_podf",
            base + 0x6c,
            6);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate2 adc2;
    clk = imx_clk_gate2(
            &adc2,
            IMX6UL_CLK_ADC2,
            "adc2",
            "ipg",
            base + 0x6c,
            8);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate2 uart3_ipg;
    static unsigned int uart3_shared_count = 0;
    clk = imx_clk_gate2_shared(
            &uart3_ipg,
            IMX6UL_CLK_UART3_IPG,
            "uart3_ipg",
            "ipg",
            base + 0x6c,
            10,
            &uart3_shared_count);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate2 uart3_serial;
    clk = imx_clk_gate2_shared(
            &uart3_serial,
            IMX6UL_CLK_UART3_SERIAL,
            "uart3_serial",
            "uart_podf",
            base + 0x6c,
            10,
            &uart3_shared_count);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate2 epit1;
    clk = imx_clk_gate2(
            &epit1,
            IMX6UL_CLK_EPIT1,
            "epit1",
            "perclk",
            base + 0x6c,
            12);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate2 epit2;
    clk = imx_clk_gate2(
            &epit2,
            IMX6UL_CLK_EPIT2,
            "epit2",
            "perclk",
            base + 0x6c,
            14);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate2 adc1;
    clk = imx_clk_gate2(
            &adc1,
            IMX6UL_CLK_ADC1,
            "adc1",
            "ipg",
            base + 0x6c,
            16);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate2 sim_s;
    clk = imx_clk_gate2(
            &sim_s,
            IMX6UL_CLK_SIM_S,
            "sim_s",
            "ipg",
            base + 0x6c,
            18);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate2 gpt1_bus;
    clk = imx_clk_gate2(
            &gpt1_bus,
            IMX6UL_CLK_GPT1_BUS,
            "gpt1_bus",
            "perclk",
            base + 0x6c,
            20);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate2 gpt1_serial;
    clk = imx_clk_gate2(
            &gpt1_serial,
            IMX6UL_CLK_GPT1_SERIAL,
            "gpt1_serial",
            "perclk",
            base + 0x6c,
            22);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate2 uart4_ipg;
    static unsigned int uart4_shared_count = 0;
    clk = imx_clk_gate2_shared(
            &uart4_ipg,
            IMX6UL_CLK_UART4_IPG,
            "uart4_ipg",
            "ipg",
            base + 0x6c,
            24,
            &uart4_shared_count);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate2 uart4_serail;
    clk = imx_clk_gate2_shared(
            &uart4_serail,
            IMX6UL_CLK_UART4_SERIAL,
            "uart4_serail",
            "uart_podf",
            base + 0x6c,
            24,
            &uart4_shared_count);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate2 gpio1_clk;
    clk = imx_clk_gate2(
            &gpio1_clk,
            IMX6UL_CLK_GPIO1,
            "gpio1_clk",
            "ipg",
            base + 0x6c,
            26);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate2 csu_clk;
    clk = imx_clk_gate2(
            &csu_clk,
            IMX6UL_CLK_CSU,
            "csu_clk",
            "ipg",
            base + 0x6c,
            28);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate2 gpio5_clk;
    clk = imx_clk_gate2(
            &gpio5_clk,
            IMX6UL_CLK_GPIO5,
            "gpio5_clk",
            "ipg",
            base + 0x6c,
            30);
    CLK_BUG_ON(NULL == clk);

    /* CCGR2 */
    static struct clk_gate2 csi;
    clk = imx_clk_gate2(
            &csi,
            IMX6UL_CLK_CSI,
            "csi",
            "csi_podf",
            base + 0x70,
            2);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate2 i2c1;
    clk = imx_clk_gate2(
            &i2c1,
            IMX6UL_CLK_I2C1,
            "i2c1",
            "perclk",
            base + 0x70,
            6);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate2 i2c2;
    clk = imx_clk_gate2(
            &i2c2,
            IMX6UL_CLK_I2C2,
            "i2c2",
            "perclk",
            base + 0x70,
            8);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate2 i2c3;
    clk = imx_clk_gate2(
            &i2c3,
            IMX6UL_CLK_I2C3,
            "i2c3",
            "perclk",
            base + 0x70,
            10);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate2 ocotp;
    clk = imx_clk_gate2(
            &ocotp,
            IMX6UL_CLK_OCOTP,
            "ocotp",
            "ipg",
            base + 0x70,
            12);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate2 iomuxc_ipt_clk_io;
    clk = imx_clk_gate2(
            &iomuxc_ipt_clk_io,
            IMX6UL_CLK_IOMUXC_IPT_CLK_IO,
            "iomuxc_ipt_clk_io",
            "lcdif_podf",
            base + 0x70,
            14);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate2 ipmux1;
    clk = imx_clk_gate2(
            &ipmux1,
            IMX6UL_CLK_IPMUX1,
            "ipmux1",
            "ipg",
            base + 0x70,
            16);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate2 ipmux2;
    clk = imx_clk_gate2(
            &ipmux2,
            IMX6UL_CLK_IPMUX2,
            "ipmux2",
            "ipg",
            base + 0x70,
            18);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate2 ipmux3;
    clk = imx_clk_gate2(
            &ipmux3,
            IMX6UL_CLK_IPMUX3,
            "ipmux3",
            "ipg",
            base + 0x70,
            20);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate2 tzasc_aclk;
    clk = imx_clk_gate2(
            &tzasc_aclk,
            IMX6UL_CLK_TZASC_ACLK,
            "tzasc_aclk",
            "mmdc_podf",
            base + 0x70,
            22);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate2 gpio3_clk;
    clk = imx_clk_gate2(
            &gpio3_clk,
            IMX6UL_CLK_GPIO3,
            "gpio3_clk",
            "ipg",
            base + 0x70,
            26);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate2 lcdif_apb;
    clk = imx_clk_gate2(
            &lcdif_apb,
            IMX6UL_CLK_LCDIF_APB,
            "lcdif_apb",
            "axi",
            base + 0x70,
            28);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate2 pxp;
    clk = imx_clk_gate2(
            &pxp,
            IMX6UL_CLK_PXP,
            "pxp",
            "axi",
            base + 0x70,
            30);
    CLK_BUG_ON(NULL == clk);

    /* CCGR3 */
    static struct clk_gate2 uart5_ipg;
    static unsigned int uart5_shared_count = 0;
    clk = imx_clk_gate2_shared(
            &uart5_ipg,
            IMX6UL_CLK_UART5_IPG,
            "uart5_ipg",
            "ipg",
            base + 0x74,
            2,
            &uart5_shared_count);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate2 uart5_serial;
    clk = imx_clk_gate2_shared(
            &uart5_serial,
            IMX6UL_CLK_UART5_SERIAL,
            "uart5_serial",
            "uart_podf",
            base + 0x74,
            2,
            &uart5_shared_count);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate2 enet;
    static unsigned int enet_shared_count = 0;
    clk = imx_clk_gate2_shared(
            &enet,
            IMX6UL_CLK_ENET,
            "enet",
            "ipg",
            base + 0x74,
            4,
            &enet_shared_count);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate2 enet_ahb;
    clk = imx_clk_gate2_shared(
            &enet_ahb,
            IMX6UL_CLK_ENET_AHB,
            "enet_ahb",
            "ahb",
            base + 0x74,
            4,
            &enet_shared_count);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate2 uart6_ipg;
    static unsigned int uart6_shared_count = 0;
    clk = imx_clk_gate2_shared(
            &uart6_ipg,
            IMX6UL_CLK_UART6_IPG,
            "uart6_ipg",
            "ipg",
            base + 0x74,
            6,
            &uart6_shared_count);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate2 uart6_serial;
    clk = imx_clk_gate2_shared(
            &uart6_serial,
            IMX6UL_CLK_UART6_SERIAL,
            "uart6_serial",
            "uart_podf",
            base + 0x74,
            6,
            &uart6_shared_count);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate2 ca7_ccm_dap;
    clk = imx_clk_gate2(
            &ca7_ccm_dap,
            IMX6UL_CLK_CA7_CCM_DAP,
            "ca7_ccm_dap",
            "ipg",
            base + 0x74,
            8);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate2 lcdif_pix;
    clk = imx_clk_gate2(
            &lcdif_pix,
            IMX6UL_CLK_LCDIF_PIX,
            "lcdif_pix",
            "lcdif_podf",
            base + 0x74,
            10);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate2 gpio4_clk;
    clk = imx_clk_gate2(
            &gpio4_clk,
            IMX6UL_CLK_GPIO4,
            "gpio4_clk",
            "ipg",
            base + 0x74,
            12);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate2 qspi1;
    clk = imx_clk_gate2(
            &qspi1,
            IMX6UL_CLK_QSPI,
            "qspi1",
            "qspi1_podf",
            base + 0x74,
            14);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate2 wdog1;
    clk = imx_clk_gate2(
            &wdog1,
            IMX6UL_CLK_WDOG1,
            "wdog1",
            "ipg",
            base + 0x74,
            16);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate2 ca7_clkdiv_patch_ipg;
    clk = imx_clk_gate2(
            &ca7_clkdiv_patch_ipg,
            IMX6UL_CLK_CA7_CLKDIV_PATCH_IPG,
            "ca7_clkdiv_patch_ipg",
            "ahb",
            base + 0x74,
            18);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate2 mmdc_p0_fast;
    clk = imx_clk_busy_gate(
            &mmdc_p0_fast,
            IMX6UL_CLK_MMDC_P0_FAST,
            "mmdc_p0_fast",
            "mmdc_podf",
            base + 0x74,
            20);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate2 mmdc_p0_ipg;
    clk = imx_clk_gate2(
            &mmdc_p0_ipg,
            IMX6UL_CLK_MMDC_P0_IPG,
            "mmdc_p0_ipg",
            "ipg",
            base + 0x74,
            24);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate2 mmdc_core_ipg_clk_p1;
    clk = imx_clk_gate2(
            &mmdc_core_ipg_clk_p1,
            IMX6UL_CLK_MMDC_CORE_IPG_CLK_P1,
            "mmdc_core_ipg_clk_p1",
            "ckil",
            base + 0x74,
            26);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate2 ocram;
    clk = imx_clk_gate2(
            &ocram,
            IMX6UL_CLK_OCRAM,
            "ocram",
            "axi",
            base + 0x74,
            28);
    CLK_BUG_ON(NULL == clk);


    /* CCGR4 */
    static struct clk_gate2 pcie_root;
    clk = imx_clk_gate2(
            &pcie_root,
            IMX6UL_CLK_PCIE_ROOT,
            "pcie_root",
            "ahb",
            base + 0x78,
            0);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate2 iomuxc;
    clk = imx_clk_gate2(
            &iomuxc,
            IMX6UL_CLK_IOMUXC,
            "iomuxc",
            "ipg",
            base + 0x78,
            2);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate2 iomuxc_gpr;
    clk = imx_clk_gate2(
            &iomuxc_gpr,
            IMX6UL_CLK_IOMUXC_GPR,
            "iomuxc_gpr",
            "ipg",
            base + 0x78,
            4);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate2 sim_cpu;
    clk = imx_clk_gate2(
            &sim_cpu,
            IMX6UL_CLK_SIM_CPU,
            "sim_cpu",
            "ipg",
            base + 0x78,
            6);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate2 cxapnsyncbridge_slave;
    clk = imx_clk_gate2(
            &cxapnsyncbridge_slave,
            IMX6UL_CLK_CXAPBSYNCBRIDGE_SLAVE,
            "cxapnsyncbridge_slave",
            "ipg",
            base + 0x78,
            8);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate2 tsc_dig;
    clk = imx_clk_gate2(
            &tsc_dig,
            IMX6UL_CLK_TSC_DIG,
            "tsc_dig",
            "ipg",
            base + 0x78,
            10);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate2 per_bch;
    clk = imx_clk_gate2(
            &per_bch,
            IMX6UL_CLK_PER_BCH,
            "per_bch",
            "bch_podf",
            base + 0x78,
            12);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate2 pl301_mx6qper2_mainclk;
    clk = imx_clk_gate2(
            &pl301_mx6qper2_mainclk,
            IMX6UL_CLK_PL301_MX6QPER2_MAINCLK,
            "pl301_mx6qper2_mainclk",
            "ipg",
            base + 0x78,
            14);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate2 pwm1;
    clk = imx_clk_gate2(
            &pwm1,
            IMX6UL_CLK_PWM1,
            "pwm1",
            "perclk",
            base + 0x78,
            16);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate2 pwm2;
    clk = imx_clk_gate2(
            &pwm2,
            IMX6UL_CLK_PWM2,
            "pwm2",
            "perclk",
            base + 0x78,
            18);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate2 pwm3;
    clk = imx_clk_gate2(
            &pwm3,
            IMX6UL_CLK_PWM3,
            "pwm3",
            "perclk",
            base + 0x78,
            20);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate2 pwm4;
    clk = imx_clk_gate2(
            &pwm4,
            IMX6UL_CLK_PWM4,
            "pwm4",
            "perclk",
            base + 0x78,
            22);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate2 gpmi_bch_apb;
    clk = imx_clk_gate2(
            &gpmi_bch_apb,
            IMX6UL_CLK_GPMI_BCH_APB,
            "gpmi_bch_apb",
            "bch_podf",
            base + 0x78,
            24);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate2 gpmi_bch;
    clk = imx_clk_gate2(
            &gpmi_bch,
            IMX6UL_CLK_GPMI_BCH,
            "gpmi_bch",
            "gpmi_podf",
            base + 0x78,
            26);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate2 gpmi_io;
    clk = imx_clk_gate2(
            &gpmi_io,
            IMX6UL_CLK_GPMI_IO,
            "gpmi_io",
            "enfc_podf",
            base + 0x78,
            28);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate2 gpmi_apb;
    clk = imx_clk_gate2(
            &gpmi_apb,
            IMX6UL_CLK_GPMI_APB,
            "gpmi_apb",
            "bch_podf",
            base + 0x78,
            30);
    CLK_BUG_ON(NULL == clk);

    /* CCGR5 */
    static struct clk_gate2 rom;
    clk = imx_clk_gate2(
            &rom,
            IMX6UL_CLK_ROM,
            "rom",
            "ahb",
            base + 0x7c,
            0);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate2 stcr;
    clk = imx_clk_gate2(
            &stcr,
            IMX6UL_CLK_STCR,
            "stcr",
            "ipg",
            base + 0x7c,
            2);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate2 snvs_dryice;
    clk = imx_clk_gate2(
            &snvs_dryice,
            IMX6UL_CLK_SNVS_DRYICE,
            "snvs_dryice",
            "ipg",
            base + 0x7c,
            4);
    CLK_BUG_ON(NULL == clk);


    static struct clk_gate2 sdma;
    clk = imx_clk_gate2(
            &sdma,
            IMX6UL_CLK_SDMA,
            "sdma",
            "ahb",
            base + 0x7c,
            6);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate2 kpp;
    clk = imx_clk_gate2(
            &kpp,
            IMX6UL_CLK_KPP,
            "kpp",
            "ipg",
            base + 0x7c,
            8);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate2 wdog2;
    clk = imx_clk_gate2(
            &wdog2,
            IMX6UL_CLK_WDOG2,
            "wdog2",
            "ipg",
            base + 0x7c,
            10);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate2 spba;
    clk = imx_clk_gate2(
            &spba,
            IMX6UL_CLK_SPBA,
            "spba",
            "ipg",
            base + 0x7c,
            12);
    CLK_BUG_ON(NULL == clk);

    static unsigned int share_count_audio;
    static struct clk_gate2 spdif;
    clk = imx_clk_gate2_shared(
            &spdif,
            IMX6UL_CLK_SPDIF,
            "spdif",
            "spdif_podf",
            base + 0x7c,
            14,
            &share_count_audio);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate2 spdif_gclk;
    clk = imx_clk_gate2_shared(
            &spdif_gclk,
            IMX6UL_CLK_SPDIF_GCLK,
            "spdif_gclk",
            "ipg",
            base + 0x7c,
            14,
            &share_count_audio);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate2 sim_main;
    clk = imx_clk_gate2(
            &sim_main,
            IMX6UL_CLK_SIM_MAIN,
            "sim_main",
            "ipg",
            base + 0x7c,
            16);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate2 snvs_hp;
    clk = imx_clk_gate2(
            &snvs_hp,
            IMX6UL_CLK_SNVS_HP,
            "snvs_hp",
            "ipg",
            base + 0x7c,
            18);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate2 snvs_lp;
    clk = imx_clk_gate2(
            &snvs_lp,
            IMX6UL_CLK_SNVS_LP,
            "snvs_lp",
            "ipg",
            base + 0x7c,
            20);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate2 sai3;
    static unsigned int share_count_sai3;
    clk = imx_clk_gate2_shared(
            &sai3,
            IMX6UL_CLK_SAI3,
            "sai3",
            "sai3_podf",
            base + 0x7c,
            22,
            &share_count_sai3);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate2 sai3_ipg;
    clk = imx_clk_gate2_shared(
            &sai3_ipg,
            IMX6UL_CLK_SAI3_IPG,
            "sai3_ipg",
            "ipg",
            base + 0x7c,
            22,
            &share_count_sai3);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate2 uart1_ipg;
    static unsigned int uart1_shared_count = 0;
    clk = imx_clk_gate2_shared(
            &uart1_ipg,
            IMX6UL_CLK_UART1_IPG,
            "uart1_ipg",
            "ipg",
            base + 0x7c,
            24,
            &uart1_shared_count);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate2 uart1_serial;
    clk = imx_clk_gate2_shared(
            &uart1_serial,
            IMX6UL_CLK_UART1_SERIAL,
            "uart1_serial",
            "uart_podf",
            base + 0x7c,
            24,
            &uart1_shared_count);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate2 uart7_ipg;
    static unsigned int uart7_shared_count = 0;
    clk = imx_clk_gate2_shared(
            &uart7_ipg,
            IMX6UL_CLK_UART7_IPG,
            "uart7_ipg",
            "ipg",
            base + 0x7c,
            26,
            &uart7_shared_count);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate2 uart7_serial;
    clk = imx_clk_gate2_shared(
            &uart7_serial,
            IMX6UL_CLK_UART7_SERIAL,
            "uart7_serial",
            "uart_podf",
            base + 0x7c,
            26,
            &uart7_shared_count);
    CLK_BUG_ON(NULL == clk);

    static unsigned int share_count_sai1;
    static struct clk_gate2 sai1;
    clk = imx_clk_gate2_shared(
            &sai1,
            IMX6UL_CLK_SAI1,
            "sai1",
            "sai1_podf",
            base + 0x7c,
            28,
            &share_count_sai1);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate2 sai1_ipg;
    clk = imx_clk_gate2_shared(
            &sai1_ipg,
            IMX6UL_CLK_SAI1_IPG,
            "sai1_ipg",
            "ipg",
            base + 0x7c,
            28,
            &share_count_sai1);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate2 sai2;
    static unsigned int share_count_sai2;
    clk = imx_clk_gate2_shared(
            &sai2,
            IMX6UL_CLK_SAI2,
            "sai2",
            "sai2_podf",
            base + 0x7c,
            30,
            &share_count_sai2);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate2 sai2_ipg;
    clk = imx_clk_gate2_shared(
            &sai2_ipg,
            IMX6UL_CLK_SAI2_IPG,
            "sai2_ipg",
            "ipg",
            base + 0x7c,
            30,
            &share_count_sai2);
    CLK_BUG_ON(NULL == clk);

    /* CCGR6 */
    static struct clk_gate2 usboh3;
    clk = imx_clk_gate2(
            &usboh3,
            IMX6UL_CLK_USBOH3,
            "usboh3",
            "ipg",
            base + 0x80,
            0);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate2 usdhc1;
    clk = imx_clk_gate2(
            &usdhc1,
            IMX6UL_CLK_USDHC1,
            "usdhc1",
            "usdhc1_podf",
            base + 0x80,
            2);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate2 usdhc2;
    clk = imx_clk_gate2(
            &usdhc2,
            IMX6UL_CLK_USDHC2,
            "usdhc2",
            "usdhc2_podf",
            base + 0x80,
            4);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate2 sim1;
    clk = imx_clk_gate2(
            &sim1,
            IMX6UL_CLK_SIM1,
            "sim1",
            "sim_sel",
            base + 0x80,
            6);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate2 sim2;
    clk = imx_clk_gate2(
            &sim2,
            IMX6UL_CLK_SIM2,
            "sim2",
            "sim_sel",
            base + 0x80,
            8);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate2 eim;
    clk = imx_clk_gate2(
            &eim,
            IMX6UL_CLK_EIM,
            "eim",
            "eim_slow_podf",
            base + 0x80,
            10);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate2 uart8_ipg;
    static unsigned int uart8_shared_count;
    clk = imx_clk_gate2_shared(
            &uart8_ipg,
            IMX6UL_CLK_UART8_IPG,
            "uart8_ipg",
            "ipg",
            base + 0x80,
            14,
            &uart8_shared_count);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate2 uart8_serial;
    clk = imx_clk_gate2_shared(
            &uart8_serial,
            IMX6UL_CLK_UART8_SERIAL,
            "uart8_serial",
            "uart_podf",
            base + 0x80,
            14,
            &uart8_shared_count);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate2 pwm8;
    clk = imx_clk_gate2(
            &pwm8,
            IMX6UL_CLK_PWM8,
            "pwm8",
            "perclk",
            base + 0x80,
            16);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate2 wdog3;
    clk = imx_clk_gate2(
            &wdog3,
            IMX6UL_CLK_WDOG3,
            "wdog3",
            "ipg",
            base + 0x80,
            20);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate2 anadig;
    clk = imx_clk_gate2(
            &anadig,
            IMX6UL_CLK_ANADIG,
            "anadig",
            "ipg",
            base + 0x80,
            22);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate2 i2c4;
    clk = imx_clk_gate2(
            &i2c4,
            IMX6UL_CLK_I2C4,
            "i2c4",
            "perclk",
            base + 0x80,
            24);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate2 pwm5;
    clk = imx_clk_gate2(
            &pwm5,
            IMX6UL_CLK_PWM5,
            "pwm5",
            "perclk",
            base + 0x80,
            26);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate2 pwm6;
    clk = imx_clk_gate2(
            &pwm6,
            IMX6UL_CLK_PWM6,
            "pwm6",
            "perclk",
            base + 0x80,
            28);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate2 Pwm7;
    clk = imx_clk_gate2(
            &Pwm7,
            IMX6UL_CLK_PWM7,
            "Pwm7",
            "perclk",
            base + 0x80,
            30);
    CLK_BUG_ON(NULL == clk);

    base = (void *)IMX6UL_IOMUXC_GPR_REGS_BASE_ADDR;
    static struct clk_gate arm_a7_ahb;
    clk = imx_clk_gate_iomuxc_gpr(
            &arm_a7_ahb,
            IMX6UL_CLK_ARM_A7_AHB,
            "arm_a7_ahb",
            "ahb",
            base + 0x4,
            26);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate arm_a7_atb;
    clk = imx_clk_gate_iomuxc_gpr(
            &arm_a7_atb,
            IMX6UL_CLK_ARM_A7_ATB,
            "arm_a7_atb",
            "ahb",
            base + 0x4,
            25);
    CLK_BUG_ON(NULL == clk);

    static struct clk_gate arm_a7_apb_dbg;
    clk = imx_clk_gate_iomuxc_gpr(
            &arm_a7_apb_dbg,
            IMX6UL_CLK_ARM_A7_APB_DBG,
            "arm_a7_apb_dbg",
            "ahb",
            base + 0x4,
            24);
    CLK_BUG_ON(NULL == clk);

    static struct clk_user_fixed_rate enet1_tx_clk_input;
    clk = clk_register_user_fixed_rate(
            &enet1_tx_clk_input,
            IMX6UL_CLK_ENET1_TX_CLK_INPUT,
            "enet1_tx_clk_input",
            0);
    CLK_BUG_ON(NULL == clk);

    static struct clk_user_fixed_rate enet2_tx_clk_input;
    clk = clk_register_user_fixed_rate(
            &enet2_tx_clk_input,
            IMX6UL_CLK_ENET2_TX_CLK_INPUT,
            "enet2_tx_clk_input",
            0);
    CLK_BUG_ON(NULL == clk);

    static struct clk_enet_tx_ref enet1_tx_ref;
    static const char *enet1_tx_ref_sels[] =
                { "enet1_ref_125m", "enet1_tx_clk_input",};
    clk = imx_clk_enet_tx_clk_ref(
            &enet1_tx_ref,
            IMX6UL_CLK_ENET1_TX_REF_CLK,
            "enet1_tx_ref",
            enet1_tx_ref_sels,
            2,
            0x4,
            13,
            17 );
    CLK_BUG_ON(NULL == clk);

    static struct clk_enet_tx_ref enet2_tx_ref;
    static const char *enet2_tx_ref_sels[] =
                { "enet2_ref_125m", "enet2_tx_clk_input",};
    clk = imx_clk_enet_tx_clk_ref(
            &enet2_tx_ref,
            IMX6UL_CLK_ENET2_TX_REF_CLK,
            "enet2_tx_ref",
            enet2_tx_ref_sels,
            2,
            0x4,
            14,
            18 );
    CLK_BUG_ON(NULL == clk);
	
	/* mask handshake of mmdc */
    base = (void *)0x020c4000;
    writel(BM_CCM_CCDR_MMDC_CH0_MASK, base + CCDR);

    // 初始化DDR相关的时钟
    aw_clk_enable(IMX6UL_CLK_MMDC_P0_FAST);
    aw_clk_enable(IMX6UL_CLK_MMDC_CORE_IPG_CLK_P1);
    clk = __g_all_clk[IMX6UL_CLK_MMDC_P0_FAST];
    // 设置所有DDR相关的时钟不可改变父时钟或设置频率
    while(clk != NULL) {
        __clk_set_flags(clk,CLK_CANNOT_CHANGE);
        clk = clk->parent;
    }
	aw_clk_enable(IMX6UL_CLK_MMDC_P0_IPG);
	
	// 使能PLL2,PLL3
    aw_clk_enable(IMX6UL_CLK_PLL2_BUS);
    aw_clk_enable(IMX6UL_CLK_PLL3_USB_OTG);
	
	// 先设置AHB时钟至一个较低的值，以便进行父时钟的切换
    aw_clk_rate_set(IMX6UL_CLK_AHB,99000000);
    /* Change periph_pre clock to pll2_bus to adjust AXI rate to 264MHz */
    aw_clk_parent_set(IMX6UL_CLK_PERIPH_CLK2_SEL, IMX6UL_CLK_PLL3_USB_OTG);
    aw_clk_parent_set(IMX6UL_CLK_PERIPH, IMX6UL_CLK_PERIPH_CLK2);
    aw_clk_parent_set(IMX6UL_CLK_PERIPH_PRE, IMX6UL_CLK_PLL2_BUS);
    aw_clk_parent_set(IMX6UL_CLK_PERIPH, IMX6UL_CLK_PERIPH_PRE);
    aw_clk_rate_set(IMX6UL_CLK_AHB, 132000000);


    aw_clk_parent_set(IMX6UL_CLK_PLL1_SW,IMX6UL_CLK_PLL1_SYS);
    aw_clk_parent_set(IMX6UL_CLK_STEP,IMX6UL_CLK_OSC);
    aw_clk_parent_set(IMX6UL_CLK_PLL1_SW,IMX6UL_CLK_STEP);

    aw_clk_rate_set(IMX6UL_CLK_PLL1_SYS,1056000000);
    aw_clk_parent_set(IMX6UL_CLK_PLL1_SW,IMX6UL_CLK_PLL1_SYS);
	
	// 禁用PLL2,PLL3
	aw_clk_disable(IMX6UL_CLK_PLL2_BUS);
    aw_clk_disable(IMX6UL_CLK_PLL3_USB_OTG);

    // 设置LCD相关的时钟
    aw_clk_parent_set(IMX6UL_CLK_LCDIF_SEL,IMX6UL_CLK_LCDIF_PODF);
    aw_clk_parent_set(IMX6UL_CLK_LCDIF_PRE_SEL,IMX6UL_CLK_PLL2_BUS);
    aw_clk_rate_set(IMX6UL_CLK_LCDIF_PODF,8000000);

    // 使能所必须的时钟
    // ARM
    aw_clk_enable(IMX6UL_CLK_ARM);
    aw_clk_enable(IMX6UL_CLK_CA7_CLKDIV_PATCH_IPG);
    aw_clk_enable(IMX6UL_CLK_ARM_DBG);
    aw_clk_enable(IMX6UL_CLK_ARM_A7_ATB);
    aw_clk_enable(IMX6UL_CLK_ARM_A7_APB_DBG);
    aw_clk_enable(IMX6UL_CLK_ARM_A7_AHB);
    aw_clk_enable(IMX6UL_CLK_PL301_MX6QPER2_MAINCLK);

    //OCRAM
    aw_clk_enable(IMX6UL_CLK_OCRAM);
    //AIPSTZ
    aw_clk_enable(IMX6UL_CLK_AIPSTZ1);
    aw_clk_enable(IMX6UL_CLK_AIPSTZ2);
    //aw_clk_enable(IMX6UL_CLK_AIPSTZ3);
    // IPMUX
    aw_clk_enable(IMX6UL_CLK_IPMUX1);
    aw_clk_enable(IMX6UL_CLK_IPMUX2);
    aw_clk_enable(IMX6UL_CLK_IPMUX3);
    // CCM
    aw_clk_enable(IMX6UL_CLK_ANADIG);
    // OCOTP
    aw_clk_enable(IMX6UL_CLK_OCOTP);

    //SIM
    aw_clk_enable(IMX6UL_CLK_SIM_MAIN);
    aw_clk_enable(IMX6UL_CLK_SIM_S);
    aw_clk_enable(IMX6UL_CLK_SIM_CPU);

    //IOMUX
    aw_clk_enable(IMX6UL_CLK_IOMUXC_GPR);
    aw_clk_enable(IMX6UL_CLK_IOMUXC);
    aw_clk_enable(IMX6UL_CLK_IOMUXC_IPT_CLK_IO);

    /* 如果关闭EIM的时钟，则调试器可能会掉 */
    aw_clk_enable(IMX6UL_CLK_EIM);

    for (i = imx6ul_clk_max_count-1;i>=0;i--) {
        clk = __g_all_clk[i];
        if (NULL != clk) {
            if (NULL != clk->ops) {
                if ( NULL != clk->ops->disable) {
                    if (0 == clk->ref_count ) {
                        clk->ops->disable(clk);
                    }
                }
            }
        }
    }


    aw_clk_parent_set(IMX6UL_CLK_PERCLK_SEL,IMX6UL_CLK_OSC);
    // 设置uart时钟使用OSC
    aw_clk_parent_set(IMX6UL_CLK_UART_SEL, IMX6UL_CLK_OSC);

    // 设置enfc_sel使用PLL2 PFD2
    aw_clk_parent_set(IMX6UL_CLK_ENFC_SEL,IMX6UL_CLK_PLL2_PFD2);
    aw_clk_rate_set(IMX6UL_CLK_GPMI_IO,79200000);

    aw_clk_parent_set(IMX6UL_CLK_GPMI_PODF,IMX6UL_CLK_PLL2_PFD2);
    aw_clk_rate_set(IMX6UL_CLK_GPMI_PODF,99000000);
    aw_clk_parent_set(IMX6UL_CLK_BCH_PODF,IMX6UL_CLK_PLL2_PFD2);
    aw_clk_rate_set(IMX6UL_CLK_BCH_PODF,99000000);

    aw_clk_parent_set(IMX6UL_CLK_SAI1_SEL,IMX6UL_CLK_PLL4_AUDIO_DIV);
    // 为了方便音频输出，将IMX6UL_CLK_PLL4_AUDIO_DIV的输出设为451584000
    aw_clk_rate_set(IMX6UL_CLK_PLL4_AUDIO_DIV,451584000);

    aw_clk_parent_set(IMX6UL_CLK_ECSPI_SEL,IMX6UL_CLK_OSC);
    aw_clk_parent_set(IMX6UL_CLK_SIM_PRE_SEL,IMX6UL_CLK_PLL2_PFD0);
    aw_clk_parent_set(IMX6UL_CLK_SIM_SEL,IMX6UL_CLK_SIM_PODF);
    aw_clk_rate_set(IMX6UL_CLK_SIM_PODF,44000000);

    // QSPI
    aw_clk_parent_set(IMX6UL_CLK_QSPI1_SEL,IMX6UL_CLK_PLL2_BUS);
    aw_clk_rate_set(IMX6UL_CLK_QSPI,66000000);

    //CAN
    aw_clk_parent_set(IMX6UL_CLK_CAN_SEL,IMX6UL_CLK_PLL3_80M);
    aw_clk_rate_set(IMX6UL_CLK_CAN_PODF,40000000);
}
