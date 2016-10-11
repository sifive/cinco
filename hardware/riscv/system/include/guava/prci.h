// See LICENSE for license details.

#ifndef SIFIVE_PRCI_H
#define SIFIVE_PRCI_H

#define PRCI_hfrosccfg   (0x0000)
#define PRCI_hfxosccfg   (0x0004)
#define PRCI_pllcfg      (0x0008)
#define PRCI_plldiv      (0x000C)
#define PRCI_procmoncfg  (0x00F0)

#define ROSC_DIV_offset    0
#define ROSC_TRIM_offset   16
#define ROSC_EN_offset     30 
#define ROSC_RDY_offset    31

#define XOSC_EN_offset     30
#define XOSC_RDY_offset    31

#define PLL_R_offset       0
// single reserved bit for F LSB.
#define PLL_F_offset       4
#define PLL_Q_offset       10
#define PLL_SEL_offset     16
#define PLL_REFSEL_offset  17
#define PLL_BYPASS_offset  18
#define PLL_LOCK_offset    31

#define PLL_R_default 0x1
#define PLL_F_default 0x1F
#define PLL_Q_default 0x3

#define PLL_FINAL_DIV_offset 0
#define PLL_FINAL_DIV_BY_1_offset 8

#define PROCMON_DIV_offset   0
#define PROCMON_TRIM_offset  8
#define PROCMON_EN_offset    16
#define PROCMON_SEL_offset   24
#define PROCMON_NT_EN_offset 28

#define PROCMON_SEL_HFCLK     0
#define PROCMON_SEL_HFXOSCIN  1
#define PROCMON_SEL_PLLOUTDIV 2
#define PROCMON_SEL_PROCMON   3

#endif
