/** @file
 *
 * (C) Copyright 2015 Google, Inc
 * (C) 2017 Theobroma Systems Design und Consulting GmbH
 * (c) Copyright 2019, Andrei Warkentin <andrey.warkentin@gmail.com>
 *
 * SPDX-License-Identifier:     GPL-2.0
 */

#include <PiDxe.h>
#include <Library/DebugLib.h>
#include <Library/ArmLib.h>
#include <Library/BaseLib.h>
#include <Library/TimerLib.h>
#include <Library/IoLib.h>
#include <Library/CRULib.h>
#include <Rk3399/Rk3399.h>

#define OSC_HZ          (24*MHz)
#define GPLL_HZ         (800 * MHz)
#define CPLL_HZ         (800 * MHz)
#define NPLL_HZ         (1000 * MHz)
#define PPLL_HZ         (676*MHz)

#define PMU_PCLK_HZ     (48*MHz)

#define ACLKM_CORE_HZ   (300*MHz)
#define ATCLK_CORE_HZ   (300*MHz)
#define PCLK_DBG_HZ     (100*MHz)

#define PERIHP_ACLK_HZ  (150 * MHz)
#define PERIHP_HCLK_HZ  (75 * MHz)
#define PERIHP_PCLK_HZ  (37500 * KHz)

#define PERILP0_ACLK_HZ (300 * MHz)
#define PERILP0_HCLK_HZ (100 * MHz)
#define PERILP0_PCLK_HZ (50 * MHz)

#define PERILP1_HCLK_HZ (100 * MHz)
#define PERILP1_PCLK_HZ (50 * MHz)

#define PWM_CLOCK_HZ    PMU_PCLK_HZ

struct rk3399_pmucru {
        UINT32 ppll_con[6];
        UINT32 reserved[0x1a];
        UINT32 pmucru_clksel[6];
        UINT32 pmucru_clkfrac_con[2];
        UINT32 reserved2[0x18];
        UINT32 pmucru_clkgate_con[3];
        UINT32 reserved3;
        UINT32 pmucru_softrst_con[2];
        UINT32 reserved4[2];
        UINT32 pmucru_rstnhold_con[2];
        UINT32 reserved5[2];
        UINT32 pmucru_gatedis_con[2];
} *pmucru = (void *) RK3399_PMU_CRU_BASE;

struct rk3399_cru {
        UINT32 apll_l_con[6];
        UINT32 reserved[2];
        UINT32 apll_b_con[6];
        UINT32 reserved1[2];
        UINT32 dpll_con[6];
        UINT32 reserved2[2];
        UINT32 cpll_con[6];
        UINT32 reserved3[2];
        UINT32 gpll_con[6];
        UINT32 reserved4[2];
        UINT32 npll_con[6];
        UINT32 reserved5[2];
        UINT32 vpll_con[6];
        UINT32 reserved6[0x0a];
        UINT32 clksel_con[108];
        UINT32 reserved7[0x14];
        UINT32 clkgate_con[35];
        UINT32 reserved8[0x1d];
        UINT32 softrst_con[21];
        UINT32 reserved9[0x2b];
        UINT32 glb_srst_fst_value;
        UINT32 glb_srst_snd_value;
        UINT32 glb_cnt_th;
        UINT32 misc_con;
        UINT32 glb_rst_con;
        UINT32 glb_rst_st;
        UINT32 reserved10[0x1a];
        UINT32 sdmmc_con[2];
        UINT32 sdio0_con[2];
        UINT32 sdio1_con[2];
} *cru = (VOID *) RK3399_CRU_BASE;

struct rk3399_clk_info {
  unsigned long id;
  CHAR8 *name;
  BOOLEAN is_cru;
};

#define DIV_ROUND_UP(n, d) (((n) + (d) - 1) / (d))
#define GENMASK(h, l) \
  (((~0UL) << (l)) & (~0UL >> ((sizeof(UINTN) * 8) - 1 - (h))))

struct pll_div {
  UINT32 refdiv;
  UINT32 fbdiv;
  UINT32 postdiv1;
  UINT32 postdiv2;
  UINT32 frac;
  UINT32 freq;
};

#define RATE_TO_DIV(input_rate, output_rate)    \
  ((input_rate) / (output_rate) - 1);
#define DIV_TO_RATE(input_rate, div)    ((input_rate) / ((div) + 1))

#define PLL_DIVISORS(hz, _refdiv, _postdiv1, _postdiv2) {               \
    .refdiv = _refdiv,                                                  \
      .fbdiv = (UINT32)((UINT64)hz * _refdiv * _postdiv1 * _postdiv2 / OSC_HZ), \
      .postdiv1 = _postdiv1, .postdiv2 = _postdiv2, .freq = hz};

static const struct pll_div ppll_init_cfg = PLL_DIVISORS(PPLL_HZ, 2, 2, 1);
static const struct pll_div gpll_init_cfg = PLL_DIVISORS(GPLL_HZ, 1, 3, 1);
static const struct pll_div npll_init_cfg = PLL_DIVISORS(NPLL_HZ, 1, 3, 1);
static const struct pll_div cpll_init_cfg = PLL_DIVISORS(CPLL_HZ, 1, 3, 1);
static const struct pll_div apll_1700_cfg = PLL_DIVISORS(1700*MHz, 1, 1, 1);
static const struct pll_div apll_1600_cfg = PLL_DIVISORS(1600*MHz, 3, 1, 1);
static const struct pll_div apll_1300_cfg = PLL_DIVISORS(1300*MHz, 1, 1, 1);
static const struct pll_div apll_816_cfg = PLL_DIVISORS(816 * MHz, 1, 2, 1);
static const struct pll_div apll_600_cfg = PLL_DIVISORS(600*MHz, 1, 2, 1);

static const struct pll_div *apll_cfgs[] = {
  [APLL_1700_MHZ] = &apll_1700_cfg,
  [APLL_1600_MHZ] = &apll_1600_cfg,
  [APLL_1300_MHZ] = &apll_1300_cfg,
  [APLL_816_MHZ] = &apll_816_cfg,
  [APLL_600_MHZ] = &apll_600_cfg,
};

enum {
  /* PLL_CON0 */
  PLL_FBDIV_MASK                 = 0xfff,
  PLL_FBDIV_SHIFT                = 0,

  /* PLL_CON1 */
  PLL_POSTDIV2_SHIFT             = 12,
  PLL_POSTDIV2_MASK              = 0x7 << PLL_POSTDIV2_SHIFT,
  PLL_POSTDIV1_SHIFT             = 8,
  PLL_POSTDIV1_MASK              = 0x7 << PLL_POSTDIV1_SHIFT,
  PLL_REFDIV_MASK                = 0x3f,
  PLL_REFDIV_SHIFT               = 0,

  /* PLL_CON2 */
  PLL_LOCK_STATUS_SHIFT          = 31,
  PLL_LOCK_STATUS_MASK           = 1 << PLL_LOCK_STATUS_SHIFT,
  PLL_FRACDIV_MASK               = 0xffffff,
  PLL_FRACDIV_SHIFT              = 0,

  /* PLL_CON3 */
  PLL_MODE_SHIFT                 = 8,
  PLL_MODE_MASK                  = 3 << PLL_MODE_SHIFT,
  PLL_MODE_SLOW                  = 0,
  PLL_MODE_NORM,
  PLL_MODE_DEEP,
  PLL_DSMPD_SHIFT                = 3,
  PLL_DSMPD_MASK                 = 1 << PLL_DSMPD_SHIFT,
  PLL_INTEGER_MODE               = 1,

  /* PMUCRU_CLKSEL_CON0 */
  PMU_PCLK_DIV_CON_MASK          = 0x1f,
  PMU_PCLK_DIV_CON_SHIFT         = 0,

  /* PMUCRU_CLKSEL_CON1 */
  SPI3_PLL_SEL_SHIFT             = 7,
  SPI3_PLL_SEL_MASK              = 1 << SPI3_PLL_SEL_SHIFT,
  SPI3_PLL_SEL_24M               = 0,
  SPI3_PLL_SEL_PPLL              = 1,
  SPI3_DIV_CON_SHIFT             = 0x0,
  SPI3_DIV_CON_MASK              = 0x7f,

  /* PMUCRU_CLKSEL_CON2 */
  I2C_DIV_CON_MASK               = 0x7f,
  CLK_I2C8_DIV_CON_SHIFT         = 8,
  CLK_I2C0_DIV_CON_SHIFT         = 0,

  /* PMUCRU_CLKSEL_CON3 */
  CLK_I2C4_DIV_CON_SHIFT         = 0,

  /* CLKSEL_CON0 / CLKSEL_CON2 */
  ACLKM_CORE_DIV_CON_MASK        = 0x1f,
  ACLKM_CORE_DIV_CON_SHIFT       = 8,
  CLK_CORE_PLL_SEL_MASK          = 3,
  CLK_CORE_PLL_SEL_SHIFT         = 6,
  CLK_CORE_PLL_SEL_ALPLL         = 0x0,
  CLK_CORE_PLL_SEL_ABPLL         = 0x1,
  CLK_CORE_PLL_SEL_DPLL          = 0x10,
  CLK_CORE_PLL_SEL_GPLL          = 0x11,
  CLK_CORE_DIV_MASK              = 0x1f,
  CLK_CORE_DIV_SHIFT             = 0,

  /* CLKSEL_CON1 / CLKSEL_CON3 */
  PCLK_DBG_DIV_MASK              = 0x1f,
  PCLK_DBG_DIV_SHIFT             = 0x8,
  ATCLK_CORE_DIV_MASK            = 0x1f,
  ATCLK_CORE_DIV_SHIFT           = 0,

  /* CLKSEL_CON14 */
  PCLK_PERIHP_DIV_CON_SHIFT      = 12,
  PCLK_PERIHP_DIV_CON_MASK       = 0x7 << PCLK_PERIHP_DIV_CON_SHIFT,
  HCLK_PERIHP_DIV_CON_SHIFT      = 8,
  HCLK_PERIHP_DIV_CON_MASK       = 3 << HCLK_PERIHP_DIV_CON_SHIFT,
  ACLK_PERIHP_PLL_SEL_SHIFT      = 7,
  ACLK_PERIHP_PLL_SEL_MASK       = 1 << ACLK_PERIHP_PLL_SEL_SHIFT,
  ACLK_PERIHP_PLL_SEL_CPLL       = 0,
  ACLK_PERIHP_PLL_SEL_GPLL       = 1,
  ACLK_PERIHP_DIV_CON_SHIFT      = 0,
  ACLK_PERIHP_DIV_CON_MASK       = 0x1f,

  /* CLKSEL_CON21 */
  ACLK_EMMC_PLL_SEL_SHIFT        = 7,
  ACLK_EMMC_PLL_SEL_MASK         = 0x1 << ACLK_EMMC_PLL_SEL_SHIFT,
  ACLK_EMMC_PLL_SEL_GPLL         = 0x1,
  ACLK_EMMC_DIV_CON_SHIFT        = 0,
  ACLK_EMMC_DIV_CON_MASK         = 0x1f,

  /* CLKSEL_CON22 */
  CLK_EMMC_PLL_SHIFT             = 8,
  CLK_EMMC_PLL_MASK              = 0x7 << CLK_EMMC_PLL_SHIFT,
  CLK_EMMC_PLL_SEL_GPLL          = 0x1,
  CLK_EMMC_PLL_SEL_24M           = 0x5,
  CLK_EMMC_DIV_CON_SHIFT         = 0,
  CLK_EMMC_DIV_CON_MASK          = 0x7f << CLK_EMMC_DIV_CON_SHIFT,

  /* CLKSEL_CON23 */
  PCLK_PERILP0_DIV_CON_SHIFT     = 12,
  PCLK_PERILP0_DIV_CON_MASK      = 0x7 << PCLK_PERILP0_DIV_CON_SHIFT,
  HCLK_PERILP0_DIV_CON_SHIFT     = 8,
  HCLK_PERILP0_DIV_CON_MASK      = 3 << HCLK_PERILP0_DIV_CON_SHIFT,
  ACLK_PERILP0_PLL_SEL_SHIFT     = 7,
  ACLK_PERILP0_PLL_SEL_MASK      = 1 << ACLK_PERILP0_PLL_SEL_SHIFT,
  ACLK_PERILP0_PLL_SEL_CPLL      = 0,
  ACLK_PERILP0_PLL_SEL_GPLL      = 1,
  ACLK_PERILP0_DIV_CON_SHIFT     = 0,
  ACLK_PERILP0_DIV_CON_MASK      = 0x1f,

  /* CRU_CLK_SEL24_CON */
  CRYPTO0_PLL_SEL_SHIFT          = 6,
  CRYPTO0_PLL_SEL_MASK           = 3 << CRYPTO0_PLL_SEL_SHIFT,
  CRYPTO_PLL_SEL_CPLL            = 0,
  CRYPTO_PLL_SEL_GPLL,
  CRYPTO_PLL_SEL_PPLL            = 0,
  CRYPTO0_DIV_SHIFT              = 0,
  CRYPTO0_DIV_MASK               = 0x1f << CRYPTO0_DIV_SHIFT,

  /* CLKSEL_CON25 */
  PCLK_PERILP1_DIV_CON_SHIFT     = 8,
  PCLK_PERILP1_DIV_CON_MASK      = 0x7 << PCLK_PERILP1_DIV_CON_SHIFT,
  HCLK_PERILP1_PLL_SEL_SHIFT     = 7,
  HCLK_PERILP1_PLL_SEL_MASK      = 1 << HCLK_PERILP1_PLL_SEL_SHIFT,
  HCLK_PERILP1_PLL_SEL_CPLL      = 0,
  HCLK_PERILP1_PLL_SEL_GPLL      = 1,
  HCLK_PERILP1_DIV_CON_SHIFT     = 0,
  HCLK_PERILP1_DIV_CON_MASK      = 0x1f,

  /* CLKSEL_CON26 */
  CLK_SARADC_DIV_CON_SHIFT       = 8,
  CLK_SARADC_DIV_CON_MASK        = GENMASK(15, 8),
  CLK_SARADC_DIV_CON_WIDTH       = 8,
  CRYPTO1_PLL_SEL_SHIFT          = 6,
  CRYPTO1_PLL_SEL_MASK           = 3 << CRYPTO1_PLL_SEL_SHIFT,
  CRYPTO1_DIV_SHIFT              = 0,
  CRYPTO1_DIV_MASK               = 0x1f << CRYPTO1_DIV_SHIFT,

  /* CLKSEL_CON27 */
  CLK_TSADC_SEL_X24M             = 0x0,
  CLK_TSADC_SEL_SHIFT            = 15,
  CLK_TSADC_SEL_MASK             = 1 << CLK_TSADC_SEL_SHIFT,
  CLK_TSADC_DIV_CON_SHIFT        = 0,
  CLK_TSADC_DIV_CON_MASK         = 0x3ff,

  /* CLKSEL_CON42 */
  ACLK_HDCP_PLL_SEL_SHIFT        = 14,
  ACLK_HDCP_PLL_SEL_MASK         = 0x3 << ACLK_HDCP_PLL_SEL_SHIFT,
  ACLK_HDCP_PLL_SEL_CPLL         = 0x0,
  ACLK_HDCP_PLL_SEL_GPLL         = 0x1,
  ACLK_HDCP_PLL_SEL_PPLL         = 0x2,
  ACLK_HDCP_DIV_CON_SHIFT        = 8,
  ACLK_HDCP_DIV_CON_MASK         = 0x1f << ACLK_HDCP_DIV_CON_SHIFT,

  ACLK_VIO_PLL_SEL_SHIFT        = 6,
  ACLK_VIO_PLL_SEL_MASK         = 0x3 << ACLK_VIO_PLL_SEL_SHIFT,
  ACLK_VIO_PLL_SEL_CPLL         = 0x0,
  ACLK_VIO_PLL_SEL_GPLL         = 0x1,
  ACLK_VIO_PLL_SEL_PPLL         = 0x2,
  ACLK_VIO_DIV_CON_SHIFT        = 0,
  ACLK_VIO_DIV_CON_MASK         = 0x1f << ACLK_VIO_DIV_CON_SHIFT,

  /* CLKSEL_CON43 */
  PCLK_HDCP_DIV_CON_SHIFT       = 10,
  PCLK_HDCP_DIV_CON_MASK        = 0x1f << PCLK_HDCP_DIV_CON_SHIFT,
  HCLK_HDCP_DIV_CON_SHIFT       = 5,
  HCLK_HDCP_DIV_CON_MASK        = 0x1f << HCLK_HDCP_DIV_CON_SHIFT,
  PCLK_VIO_DIV_CON_SHIFT        = 0,
  PCLK_VIO_DIV_CON_MASK         = 0x1f << PCLK_VIO_DIV_CON_SHIFT,

  /* CLKSEL_CON47 & CLKSEL_CON48 */
  ACLK_VOP_PLL_SEL_SHIFT         = 6,
  ACLK_VOP_PLL_SEL_MASK          = 0x3 << ACLK_VOP_PLL_SEL_SHIFT,
  ACLK_VOP_PLL_SEL_VPLL          = 0x0,
  ACLK_VOP_PLL_SEL_CPLL          = 0x1,
  ACLK_VOP_PLL_SEL_GPLL          = 0x2,
  ACLK_VOP_PLL_SEL_NPLL          = 0x3,
  ACLK_VOP_DIV_CON_SHIFT         = 0,
  ACLK_VOP_DIV_CON_MASK          = 0x1f << ACLK_VOP_DIV_CON_SHIFT,
  HCLK_VOP_DIV_CON_SHIFT         = 8,
  HCLK_VOP_DIV_CON_MASK          = 0x1f << HCLK_VOP_DIV_CON_SHIFT,

  /* CLKSEL_CON49 & CLKSEL_CON50 */
  DCLK_VOP_DCLK_SEL_SHIFT        = 11,
  DCLK_VOP_DCLK_SEL_MASK         = 1 << DCLK_VOP_DCLK_SEL_SHIFT,
  DCLK_VOP_DCLK_SEL_DIVOUT       = 0,
  DCLK_VOP_PLL_SEL_SHIFT         = 8,
  DCLK_VOP_PLL_SEL_MASK          = 3 << DCLK_VOP_PLL_SEL_SHIFT,
  DCLK_VOP_PLL_SEL_VPLL          = 0,
  DCLK_VOP_PLL_SEL_CPLL          = 1,
  DCLK_VOP_DIV_CON_MASK          = 0xff,
  DCLK_VOP_DIV_CON_SHIFT         = 0,

  /* CLKSEL_CON51 and CLKSEL_CON52 */
  CLK_VOP_PWM_PLL_SEL_SHIFT     = 6,
  CLK_VOP_PWM_PLL_SEL_MASK      = 1 << CLK_VOP_PWM_PLL_SEL_SHIFT,
  CLK_VOP_PWM_PLL_SEL_VPLL      = 0,
  CLK_VOP_PWM_PLL_SEL_CPLL      = 1,
  CLK_VOP_PWM_PLL_SEL_GPLL      = 2,
  CLK_VOP_PWM_DIV_CON_SHIFT     = 0,
  CLK_VOP_PWM_DIV_CON_MASK      = 0x1f << CLK_VOP_PWM_DIV_CON_SHIFT,

  /* CLKSEL_CON57 */
  PCLK_ALIVE_DIV_CON_SHIFT      = 0,
  PCLK_ALIVE_DIV_CON_MASK       = 0x1f << PCLK_ALIVE_DIV_CON_SHIFT,

  /* CLKSEL_CON58 */
  CLK_SPI_PLL_SEL_WIDTH         = 1,
  CLK_SPI_PLL_SEL_MASK          = ((1 < CLK_SPI_PLL_SEL_WIDTH) - 1),
  CLK_SPI_PLL_SEL_CPLL          = 0,
  CLK_SPI_PLL_SEL_GPLL          = 1,
  CLK_SPI_PLL_DIV_CON_WIDTH     = 7,
  CLK_SPI_PLL_DIV_CON_MASK      = ((1 << CLK_SPI_PLL_DIV_CON_WIDTH) - 1),

  CLK_SPI5_PLL_DIV_CON_SHIFT    = 8,
  CLK_SPI5_PLL_SEL_SHIFT        = 15,

  /* CLKSEL_CON59 */
  CLK_SPI1_PLL_SEL_SHIFT        = 15,
  CLK_SPI1_PLL_DIV_CON_SHIFT    = 8,
  CLK_SPI0_PLL_SEL_SHIFT        = 7,
  CLK_SPI0_PLL_DIV_CON_SHIFT    = 0,

  /* CLKSEL_CON60 */
  CLK_SPI4_PLL_SEL_SHIFT        = 15,
  CLK_SPI4_PLL_DIV_CON_SHIFT    = 8,
  CLK_SPI2_PLL_SEL_SHIFT        = 7,
  CLK_SPI2_PLL_DIV_CON_SHIFT    = 0,

  /* CLKSEL_CON61 */
  CLK_I2C_PLL_SEL_MASK          = 1,
  CLK_I2C_PLL_SEL_CPLL          = 0,
  CLK_I2C_PLL_SEL_GPLL          = 1,
  CLK_I2C5_PLL_SEL_SHIFT        = 15,
  CLK_I2C5_DIV_CON_SHIFT        = 8,
  CLK_I2C1_PLL_SEL_SHIFT        = 7,
  CLK_I2C1_DIV_CON_SHIFT        = 0,

  /* CLKSEL_CON62 */
  CLK_I2C6_PLL_SEL_SHIFT        = 15,
  CLK_I2C6_DIV_CON_SHIFT        = 8,
  CLK_I2C2_PLL_SEL_SHIFT        = 7,
  CLK_I2C2_DIV_CON_SHIFT        = 0,

  /* CLKSEL_CON63 */
  CLK_I2C7_PLL_SEL_SHIFT        = 15,
  CLK_I2C7_DIV_CON_SHIFT        = 8,
  CLK_I2C3_PLL_SEL_SHIFT        = 7,
  CLK_I2C3_DIV_CON_SHIFT        = 0,

  /* CRU_SOFTRST_CON4 */
  RESETN_DDR0_REQ_SHIFT         = 8,
  RESETN_DDR0_REQ_MASK          = 1 << RESETN_DDR0_REQ_SHIFT,
  RESETN_DDRPHY0_REQ_SHIFT      = 9,
  RESETN_DDRPHY0_REQ_MASK               = 1 << RESETN_DDRPHY0_REQ_SHIFT,
  RESETN_DDR1_REQ_SHIFT         = 12,
  RESETN_DDR1_REQ_MASK          = 1 << RESETN_DDR1_REQ_SHIFT,
  RESETN_DDRPHY1_REQ_SHIFT      = 13,
  RESETN_DDRPHY1_REQ_MASK               = 1 << RESETN_DDRPHY1_REQ_SHIFT,
};

#define VCO_MAX_KHZ     (3200 * (MHz / KHz))
#define VCO_MIN_KHZ     (800 * (MHz / KHz))
#define OUTPUT_MAX_KHZ  (3200 * (MHz / KHz))
#define OUTPUT_MIN_KHZ  (16 * (MHz / KHz))

/*
 *  the div restructions of pll in integer mode, these are defined in
 *  * CRU_*PLL_CON0 or PMUCRU_*PLL_CON0
 */
#define PLL_DIV_MIN     16
#define PLL_DIV_MAX     3200

#define RK_CLRSETBITS(clr, set)         ((((clr) | (set)) << 16) | (set))
#define RK_SETBITS(set)                 RK_CLRSETBITS(0, set)
#define RK_CLRBITS(clr)                 RK_CLRSETBITS(clr, 0)

#define rk_clrsetreg(addr, clr, set) MmioWrite32((UINTN) addr, ((clr) | (set)) << 16 | (set))
#define rk_clrreg(addr, clr)         MmioWrite32((UINTN) addr, (clr) << 16)
#define rk_setreg(addr, set)         MmioWrite32((UINTN) addr, (set) << 16 | (set))

/*
 * How to calculate the PLL(from TRM V0.3 Part 1 Page 63):
 * Formulas also embedded within the Fractional PLL Verilog model:
 * If DSMPD = 1 (DSM is disabled, "integer mode")
 * FOUTVCO = FREF / REFDIV * FBDIV
 * FOUTPOSTDIV = FOUTVCO / POSTDIV1 / POSTDIV2
 * Where:
 * FOUTVCO = Fractional PLL non-divided output frequency
 * FOUTPOSTDIV = Fractional PLL divided output frequency
 *               (output of second post divider)
 * FREF = Fractional PLL input reference frequency, (the OSC_HZ 24MHz input)
 * REFDIV = Fractional PLL input reference clock divider
 * FBDIV = Integer value programmed into feedback divide
 *
 */

static UINT32
rkclk_pll_get_rate(
  IN  UINT32 *pll_con
  )
{
  UINT32 refdiv, fbdiv, postdiv1, postdiv2;
  UINT32 con;

  con = MmioRead32((UINTN) &pll_con[3]);
  switch ((con & PLL_MODE_MASK) >> PLL_MODE_SHIFT) {
  case PLL_MODE_SLOW:
    return OSC_HZ;
  case PLL_MODE_NORM:
    /* normal mode */
    con = MmioRead32((UINTN) &pll_con[0]);
    fbdiv = (con & PLL_FBDIV_MASK) >> PLL_FBDIV_SHIFT;
    con = MmioRead32((UINTN) &pll_con[1]);
    postdiv1 = (con & PLL_POSTDIV1_MASK) >> PLL_POSTDIV1_SHIFT;
    postdiv2 = (con & PLL_POSTDIV2_MASK) >> PLL_POSTDIV2_SHIFT;
    refdiv = (con & PLL_REFDIV_MASK) >> PLL_REFDIV_SHIFT;
    return (24 * fbdiv / (refdiv * postdiv1 * postdiv2)) * 1000000;
  case PLL_MODE_DEEP:
  default:
    return 32768;
  }
}

UINT32
rk3399_pll_get_rate(
  IN  UINTN id
  )
{
  UINT32 *pll_con;

  switch (id) {
  case PLL_PPLL:
    pll_con = &pmucru->ppll_con[0];
    break;
  case PLL_APLLL:
    pll_con = &cru->apll_l_con[0];
    break;
  case PLL_APLLB:
    pll_con = &cru->apll_b_con[0];
    break;
  case PLL_DPLL:
    pll_con = &cru->dpll_con[0];
    break;
  case PLL_CPLL:
    pll_con = &cru->cpll_con[0];
    break;
  case PLL_GPLL:
    pll_con = &cru->gpll_con[0];
    break;
  case PLL_NPLL:
    pll_con = &cru->npll_con[0];
    break;
  case PLL_VPLL:
    pll_con = &cru->vpll_con[0];
    break;
  default:
    pll_con = &cru->vpll_con[0];
    break;
  }

  return rkclk_pll_get_rate(pll_con);
}

static VOID
rkclk_set_pll(
  IN  UINT32 *pll_con,
  IN const struct pll_div *div
  )
{
  /* All 8 PLLs have same VCO and output frequency range restrictions. */
  UINT32 vco_khz = OSC_HZ / 1000 * div->fbdiv / div->refdiv;
  UINT32 output_khz = vco_khz / div->postdiv1 / div->postdiv2;

  DEBUG((EFI_D_INFO,"PLL at %p: fbdiv=%d, refdiv=%d, postdiv1=%d, "
         "postdiv2=%d, vco=%u khz, output=%u khz\n",
         pll_con, div->fbdiv, div->refdiv, div->postdiv1,
         div->postdiv2, vco_khz, output_khz));
  ASSERT(vco_khz >= VCO_MIN_KHZ && vco_khz <= VCO_MAX_KHZ &&
         output_khz >= OUTPUT_MIN_KHZ && output_khz <= OUTPUT_MAX_KHZ &&
         div->fbdiv >= PLL_DIV_MIN && div->fbdiv <= PLL_DIV_MAX);

  /*
   * When power on or changing PLL setting,
   * we must force PLL into slow mode to ensure output stable clock.
   */
  rk_clrsetreg(&pll_con[3], PLL_MODE_MASK,
               PLL_MODE_SLOW << PLL_MODE_SHIFT);

  /* use integer mode */
  rk_clrsetreg(&pll_con[3], PLL_DSMPD_MASK,
               PLL_INTEGER_MODE << PLL_DSMPD_SHIFT);

  rk_clrsetreg(&pll_con[0], PLL_FBDIV_MASK,
               div->fbdiv << PLL_FBDIV_SHIFT);
  rk_clrsetreg(&pll_con[1],
               PLL_POSTDIV2_MASK | PLL_POSTDIV1_MASK |
               PLL_REFDIV_MASK | PLL_REFDIV_SHIFT,
               (div->postdiv2 << PLL_POSTDIV2_SHIFT) |
               (div->postdiv1 << PLL_POSTDIV1_SHIFT) |
               (div->refdiv << PLL_REFDIV_SHIFT));

  /* waiting for pll lock */
  while (!(MmioRead32((UINTN) &pll_con[2]) & (1 << PLL_LOCK_STATUS_SHIFT)))
    MicroSecondDelay(1);

  /* pll enter normal mode */
  rk_clrsetreg(&pll_con[3], PLL_MODE_MASK,
               PLL_MODE_NORM << PLL_MODE_SHIFT);
}

VOID
rk3399_configure_cpu(
  IN  apll_frequencies freq,
  IN  cpu_cluster cluster
  )
{
  UINT32 aclkm_div;
  UINT32 pclk_dbg_div;
  UINT32 atclk_div, apll_hz;
  int con_base, parent;
  UINT32 *pll_con;

  switch (cluster) {
  case CPU_CLUSTER_LITTLE:
    con_base = 0;
    parent = CLK_CORE_PLL_SEL_ALPLL;
    pll_con = &cru->apll_l_con[0];
    break;
  case CPU_CLUSTER_BIG:
  default:
    con_base = 2;
    parent = CLK_CORE_PLL_SEL_ABPLL;
    pll_con = &cru->apll_b_con[0];
    break;
  }

  apll_hz = apll_cfgs[freq]->freq;
  rkclk_set_pll(pll_con, apll_cfgs[freq]);

  aclkm_div = apll_hz / ACLKM_CORE_HZ - 1;
  ASSERT((aclkm_div + 1) * ACLKM_CORE_HZ <= apll_hz &&
         aclkm_div < 0x1f);

  pclk_dbg_div = apll_hz / PCLK_DBG_HZ - 1;
  ASSERT((pclk_dbg_div + 1) * PCLK_DBG_HZ <= apll_hz &&
         pclk_dbg_div < 0x1f);

  atclk_div = apll_hz / ATCLK_CORE_HZ - 1;
  ASSERT((atclk_div + 1) * ATCLK_CORE_HZ <= apll_hz &&
         atclk_div < 0x1f);

  rk_clrsetreg(&cru->clksel_con[con_base],
               ACLKM_CORE_DIV_CON_MASK | CLK_CORE_PLL_SEL_MASK |
               CLK_CORE_DIV_MASK,
               aclkm_div << ACLKM_CORE_DIV_CON_SHIFT |
               parent << CLK_CORE_PLL_SEL_SHIFT |
               0 << CLK_CORE_DIV_SHIFT);

  rk_clrsetreg(&cru->clksel_con[con_base + 1],
               PCLK_DBG_DIV_MASK | ATCLK_CORE_DIV_MASK,
               pclk_dbg_div << PCLK_DBG_DIV_SHIFT |
               atclk_div << ATCLK_CORE_DIV_SHIFT);
}

VOID
rk3399_pmu_clock_init(
  VOID
  )
{
  UINT32 pclk_div;

  /*  configure pmu pll(ppll) */
  rkclk_set_pll(&pmucru->ppll_con[0], &ppll_init_cfg);

  /*  configure pmu pclk */
  pclk_div = PPLL_HZ / PMU_PCLK_HZ - 1;
  rk_clrsetreg(&pmucru->pmucru_clksel[0],
               PMU_PCLK_DIV_CON_MASK,
               pclk_div << PMU_PCLK_DIV_CON_SHIFT);
}

VOID
rk3399_clock_init(
  VOID
  )
{
  UINT32 aclk_div;
  UINT32 hclk_div;
  UINT32 pclk_div;

  DEBUG((EFI_D_INFO, "Boot PLLs:\n"));
  DEBUG((EFI_D_INFO, "APLLL = %u\n", rkclk_pll_get_rate(&cru->apll_l_con[0])));
  DEBUG((EFI_D_INFO, "APLLB = %u\n", rkclk_pll_get_rate(&cru->apll_b_con[0])));
  DEBUG((EFI_D_ERROR, "CPLL = %u\n", rkclk_pll_get_rate(&cru->cpll_con[0])));
  DEBUG((EFI_D_ERROR, "DPLL = %u\n", rkclk_pll_get_rate(&cru->dpll_con[0])));
  DEBUG((EFI_D_ERROR, "GPLL = %u\n", rkclk_pll_get_rate(&cru->gpll_con[0])));
  DEBUG((EFI_D_ERROR, "NPLL = %u\n", rkclk_pll_get_rate(&cru->npll_con[0])));
  DEBUG((EFI_D_ERROR, "VPLL = %u\n", rkclk_pll_get_rate(&cru->vpll_con[0])));

  rk3399_configure_cpu(APLL_816_MHZ, CPU_CLUSTER_LITTLE);
  rk3399_configure_cpu(APLL_816_MHZ, CPU_CLUSTER_BIG);

  /*
   * some cru registers changed by bootrom, we'd better reset them to
   * reset/default values described in TRM to avoid confusion in kernel.
   * Please consider these three lines as a fix of bootrom bug.
   */
  rk_clrsetreg(&cru->clksel_con[12], 0xffff, 0x4101);
  rk_clrsetreg(&cru->clksel_con[19], 0xffff, 0x033f);
  rk_clrsetreg(&cru->clksel_con[56], 0x0003, 0x0003);

  rkclk_set_pll(&cru->gpll_con[0], &gpll_init_cfg);
  rkclk_set_pll(&cru->npll_con[0], &npll_init_cfg);
  rkclk_set_pll(&cru->cpll_con[0], &cpll_init_cfg);

  /* configure perihp aclk, hclk, pclk */
  aclk_div = DIV_ROUND_UP(GPLL_HZ, PERIHP_ACLK_HZ) - 1;

  hclk_div = PERIHP_ACLK_HZ / PERIHP_HCLK_HZ - 1;
  ASSERT((hclk_div + 1) * PERIHP_HCLK_HZ <=
         PERIHP_ACLK_HZ && (hclk_div <= 0x3));

  pclk_div = PERIHP_ACLK_HZ / PERIHP_PCLK_HZ - 1;
  ASSERT((pclk_div + 1) * PERIHP_PCLK_HZ <=
         PERIHP_ACLK_HZ && (pclk_div <= 0x7));

  rk_clrsetreg(&cru->clksel_con[14],
               PCLK_PERIHP_DIV_CON_MASK | HCLK_PERIHP_DIV_CON_MASK |
               ACLK_PERIHP_PLL_SEL_MASK | ACLK_PERIHP_DIV_CON_MASK,
               pclk_div << PCLK_PERIHP_DIV_CON_SHIFT |
               hclk_div << HCLK_PERIHP_DIV_CON_SHIFT |
               ACLK_PERIHP_PLL_SEL_GPLL << ACLK_PERIHP_PLL_SEL_SHIFT |
               aclk_div << ACLK_PERIHP_DIV_CON_SHIFT);

  /* configure perilp0 aclk, hclk, pclk */
  aclk_div = DIV_ROUND_UP(GPLL_HZ, PERILP0_ACLK_HZ) - 1;

  hclk_div = PERILP0_ACLK_HZ / PERILP0_HCLK_HZ - 1;
  ASSERT((hclk_div + 1) * PERILP0_HCLK_HZ <=
         PERILP0_ACLK_HZ && (hclk_div <= 0x3));

  pclk_div = PERILP0_ACLK_HZ / PERILP0_PCLK_HZ - 1;
  ASSERT((pclk_div + 1) * PERILP0_PCLK_HZ <=
         PERILP0_ACLK_HZ && (pclk_div <= 0x7));

  rk_clrsetreg(&cru->clksel_con[23],
               PCLK_PERILP0_DIV_CON_MASK | HCLK_PERILP0_DIV_CON_MASK |
               ACLK_PERILP0_PLL_SEL_MASK | ACLK_PERILP0_DIV_CON_MASK,
               pclk_div << PCLK_PERILP0_DIV_CON_SHIFT |
               hclk_div << HCLK_PERILP0_DIV_CON_SHIFT |
               ACLK_PERILP0_PLL_SEL_GPLL << ACLK_PERILP0_PLL_SEL_SHIFT |
               aclk_div << ACLK_PERILP0_DIV_CON_SHIFT);

  /* perilp1 hclk select gpll as source */
  hclk_div = DIV_ROUND_UP(GPLL_HZ, PERILP1_HCLK_HZ) - 1;
  ASSERT((hclk_div + 1) * PERILP1_HCLK_HZ <=
         GPLL_HZ && (hclk_div <= 0x1f));

  pclk_div = PERILP1_HCLK_HZ / PERILP1_PCLK_HZ - 1;
  ASSERT((pclk_div + 1) * PERILP1_PCLK_HZ <=
         PERILP1_HCLK_HZ && (pclk_div <= 0x7));

  rk_clrsetreg(&cru->clksel_con[25],
               PCLK_PERILP1_DIV_CON_MASK | HCLK_PERILP1_DIV_CON_MASK |
               HCLK_PERILP1_PLL_SEL_MASK,
               pclk_div << PCLK_PERILP1_DIV_CON_SHIFT |
               hclk_div << HCLK_PERILP1_DIV_CON_SHIFT |
               HCLK_PERILP1_PLL_SEL_GPLL << HCLK_PERILP1_PLL_SEL_SHIFT);

  /*
   * EMMC clock.
   */
  rk_clrsetreg(&cru->clksel_con[21],
               ACLK_EMMC_PLL_SEL_MASK | ACLK_EMMC_DIV_CON_MASK,
               ACLK_EMMC_PLL_SEL_GPLL << ACLK_EMMC_PLL_SEL_SHIFT |
               (4 - 1) << ACLK_EMMC_DIV_CON_SHIFT);
  rk_clrsetreg(&cru->clksel_con[22], 0x3f << 0, 7 << 0);

  DEBUG((EFI_D_INFO, "After clock init:\n"));
  DEBUG((EFI_D_INFO, "APLLL = %u\n", rkclk_pll_get_rate(&cru->apll_l_con[0])));
  DEBUG((EFI_D_INFO, "APLLB = %u\n", rkclk_pll_get_rate(&cru->apll_b_con[0])));
  DEBUG((EFI_D_ERROR, "CPLL = %u\n", rkclk_pll_get_rate(&cru->cpll_con[0])));
  DEBUG((EFI_D_ERROR, "DPLL = %u\n", rkclk_pll_get_rate(&cru->dpll_con[0])));
  DEBUG((EFI_D_ERROR, "GPLL = %u\n", rkclk_pll_get_rate(&cru->gpll_con[0])));
  DEBUG((EFI_D_ERROR, "NPLL = %u\n", rkclk_pll_get_rate(&cru->npll_con[0])));
  DEBUG((EFI_D_ERROR, "VPLL = %u\n", rkclk_pll_get_rate(&cru->vpll_con[0])));
}

static int
pll_para_config(UINT32 freq_hz, struct pll_div *div)
{
  UINT32 ref_khz = OSC_HZ / KHz, refdiv, fbdiv = 0;
  UINT32 postdiv1, postdiv2 = 1;
  UINT32 fref_khz;
  UINT32 diff_khz, best_diff_khz;
  const UINT32 max_refdiv = 63, max_fbdiv = 3200, min_fbdiv = 16;
  const UINT32 max_postdiv1 = 7, max_postdiv2 = 7;
  UINT32 vco_khz;
  UINT32 freq_khz = freq_hz / KHz;

  if (!freq_hz) {
    DEBUG((EFI_D_ERROR, "%s: the frequency can't be 0 Hz\n", __func__));
    return -1;
  }

  postdiv1 = DIV_ROUND_UP(VCO_MIN_KHZ, freq_khz);
  if (postdiv1 > max_postdiv1) {
    postdiv2 = DIV_ROUND_UP(postdiv1, max_postdiv1);
    postdiv1 = DIV_ROUND_UP(postdiv1, postdiv2);
  }

  vco_khz = freq_khz * postdiv1 * postdiv2;

  if (vco_khz < VCO_MIN_KHZ || vco_khz > VCO_MAX_KHZ ||
      postdiv2 > max_postdiv2) {
    DEBUG((EFI_D_ERROR, "%s: Cannot find out a supported VCO"
           " for Frequency (%uHz).\n", __func__, freq_hz));
    return -1;
  }

  div->postdiv1 = postdiv1;
  div->postdiv2 = postdiv2;

  best_diff_khz = vco_khz;
  for (refdiv = 1; refdiv < max_refdiv && best_diff_khz; refdiv++) {
    fref_khz = ref_khz / refdiv;

    fbdiv = vco_khz / fref_khz;
    if ((fbdiv >= max_fbdiv) || (fbdiv <= min_fbdiv))
      continue;
    diff_khz = vco_khz - fbdiv * fref_khz;
    if (fbdiv + 1 < max_fbdiv && diff_khz > fref_khz / 2) {
      fbdiv++;
      diff_khz = fref_khz - diff_khz;
    }

    if (diff_khz >= best_diff_khz)
      continue;

    best_diff_khz = diff_khz;
    div->refdiv = refdiv;
    div->fbdiv = fbdiv;
  }

  if (best_diff_khz > 4 * (MHz/KHz)) {
    DEBUG((EFI_D_ERROR, "%s: Failed to match output frequency %u, "
           "difference is %u Hz,exceed 4MHZ\n", __func__, freq_hz,
           best_diff_khz * KHz));
    return -1;
  }
  return 0;
}

void
rk3399_hdcp_set_clk(UINT32 hz)
{
  UINT32 div;
  UINT32 aclk = hz;
  UINT32 hpclk = hz / 2;

  div = GPLL_HZ / aclk;
  ASSERT(div - 1 < 32);

  rk_clrsetreg(&cru->clksel_con[42],
               ACLK_HDCP_PLL_SEL_MASK | ACLK_HDCP_DIV_CON_MASK,
               ACLK_HDCP_PLL_SEL_GPLL << ACLK_HDCP_PLL_SEL_SHIFT |
               (div - 1) << ACLK_HDCP_DIV_CON_SHIFT);

  div = GPLL_HZ / hpclk;
  ASSERT(div - 1 < 32);

  rk_clrsetreg(&cru->clksel_con[43],
               PCLK_HDCP_DIV_CON_MASK |
               HCLK_HDCP_DIV_CON_MASK,
               (div - 1) << PCLK_HDCP_DIV_CON_SHIFT |
               (div - 1) << HCLK_HDCP_DIV_CON_SHIFT);
}

void
rk3399_vio_set_clk(UINT32 hz)
{
  UINT32 div;
  UINT32 aclk = hz;
  UINT32 pclk = hz / 2;

  div = GPLL_HZ / aclk;
  ASSERT(div - 1 < 32);

  rk_clrsetreg(&cru->clksel_con[42],
               ACLK_VIO_PLL_SEL_MASK | ACLK_VIO_DIV_CON_MASK,
               ACLK_VIO_PLL_SEL_GPLL << ACLK_VIO_PLL_SEL_SHIFT |
               (div - 1) << ACLK_VIO_DIV_CON_SHIFT);

  div = GPLL_HZ / pclk;
  ASSERT(div - 1 < 32);

  rk_clrsetreg(&cru->clksel_con[43],
               PCLK_VIO_DIV_CON_MASK,
               (div - 1) << PCLK_VIO_DIV_CON_SHIFT);
}

UINT32
rk3399_vop_set_clk(UINT32 clk_id, UINT32 hz)
{
	struct pll_div vpll_config = {0};
	int aclk_vop = 400*MHz;
	int hclk_vop = 100*MHz;
	int pwm_vop = 100*MHz;
	void *aclkreg_addr, *dclkreg_addr, *pwmreg_addr;
	UINT32 div;

	if (clk_id == DCLK_VOP0) {
		aclkreg_addr = &cru->clksel_con[47];
		dclkreg_addr = &cru->clksel_con[49];
		pwmreg_addr = &cru->clksel_con[51];
	} else {
		aclkreg_addr = &cru->clksel_con[48];
		dclkreg_addr = &cru->clksel_con[50];
		pwmreg_addr = &cru->clksel_con[52];
	}

	/* vop aclk/hclk source clk: gpll */
	div = GPLL_HZ / aclk_vop;
	ASSERT(div - 1 < 32);

	rk_clrsetreg(aclkreg_addr,
		     ACLK_VOP_PLL_SEL_MASK | ACLK_VOP_DIV_CON_MASK,
		     ACLK_VOP_PLL_SEL_GPLL << ACLK_VOP_PLL_SEL_SHIFT |
		     (div - 1) << ACLK_VOP_DIV_CON_SHIFT);

	div = GPLL_HZ / hclk_vop;
	ASSERT(div - 1 < 32);

	rk_clrsetreg(aclkreg_addr,
		     HCLK_VOP_DIV_CON_MASK,
		     (div - 1) << HCLK_VOP_DIV_CON_SHIFT);

	/* vop PWM clk */

	div = GPLL_HZ / pwm_vop;
	ASSERT(div - 1 < 32);

	rk_clrsetreg(pwmreg_addr,
		     CLK_VOP_PWM_PLL_SEL_MASK | CLK_VOP_PWM_DIV_CON_MASK,
		     CLK_VOP_PWM_PLL_SEL_GPLL << CLK_VOP_PWM_PLL_SEL_SHIFT |
		     (div - 1) << CLK_VOP_PWM_DIV_CON_MASK);

	/* vop dclk source from vpll, and equals to vpll(means div == 1) */

	if (pll_para_config(hz, &vpll_config)) {
		DEBUG((EFI_D_ERROR, "failed to configure DCLK\n"));
		ASSERT(0);
		return -1;
	}

	rkclk_set_pll(&cru->vpll_con[0], &vpll_config);

	rk_clrsetreg(dclkreg_addr,
		     DCLK_VOP_DCLK_SEL_MASK | DCLK_VOP_PLL_SEL_MASK|
		     DCLK_VOP_DIV_CON_MASK,
		     DCLK_VOP_DCLK_SEL_DIVOUT << DCLK_VOP_DCLK_SEL_SHIFT |
		     DCLK_VOP_PLL_SEL_VPLL << DCLK_VOP_PLL_SEL_SHIFT |
		     (1 - 1) << DCLK_VOP_DIV_CON_SHIFT);

	return hz;
}

UINT32
rk3399_clk_get_rate(
  UINTN id
  )
{
  switch (id) {
  case PLL_PPLL:
  case PLL_APLLL:
  case PLL_APLLB:
  case PLL_DPLL:
  case PLL_CPLL:
  case PLL_GPLL:
  case PLL_NPLL:
  case PLL_VPLL:
    return rk3399_pll_get_rate(id);
  /* case HCLK_SDMMC: */
  /* case SCLK_SDMMC: */
  /* case SCLK_EMMC: */
  /*   return rk3399_mmc_get_clk(id); */
  /* case SCLK_I2C1: */
  /* case SCLK_I2C2: */
  /* case SCLK_I2C3: */
  /* case SCLK_I2C5: */
  /* case SCLK_I2C6: */
  /* case SCLK_I2C7: */
  /*   return rk3399_i2c_get_clk(id); */
  /* case SCLK_SPI0...SCLK_SPI5: */
  /*   return rk3399_spi_get_clk(id); */
  case SCLK_UART0:
  case SCLK_UART1:
  case SCLK_UART2:
  case SCLK_UART3:
    return 24000000;
  case PCLK_HDMI_CTRL:
    return 0;
  case DCLK_VOP0:
  case DCLK_VOP1:
    return 0;
  case PCLK_EFUSE1024NS:
    return 0;
  /* case SCLK_SARADC: */
  /*   return rk3399_saradc_get_clk(); */
  /* case SCLK_TSADC: */
  /*   return rk3399_tsadc_get_clk(); */
  /* case SCLK_CRYPTO0: */
  /* case SCLK_CRYPTO1: */
  /*   return rk3399_crypto_get_clk(id); */
  /* case ACLK_PERIHP: */
  /* case HCLK_PERIHP: */
  /* case PCLK_PERIHP: */
  /* case ACLK_PERILP0: */
  /* case HCLK_PERILP0: */
  /* case PCLK_PERILP0: */
  /* case HCLK_PERILP1: */
  /* case PCLK_PERILP1: */
  /*   return rk3399_peri_get_clk(id); */
  /* case PCLK_ALIVE: */
  /* case PCLK_WDT: */
  /*   return rk3399_alive_get_clk(); */
  }
  ASSERT_EFI_ERROR (EFI_NOT_FOUND);
  return 0;
}
