static uint32_t PRCI_measure_mcycle_freq(uint32_t mtime_ticks, uint32_t mtime_freq)
{

  uint32_t start_mtime = CLINT_REG(CLINT_MTIME);
  uint32_t end_mtime = start_mtime + n + 1;

  // Make sure we won't get rollover.
  while (end_mtime < start_mtime){
    start_mtime = CLINT_REG(CLINT_MTIME);
    end_mtime = start_mtime + n + 1;
  }

  // Don't start measuring until mtime edge.
  uint32_t tmp = start_mtime;
  do {
    start_mtime = CLINT_REG(CLINT_MTIME);
  } while (start_mtime == tmp);
  
  uint64_t start_mcycle;
  rdmcycle(&start_mcycle);
  
  while (CLINT_REG(CLINT_MTIME) < end_mtime){
  }
  
  uint64_t end_mcycle;
  rdmcycle(&start_mcycle);
  uint32_t difference = end_mcycle - start_mcycle;

  uint64_t freq = ((uint64_t) difference * mtime_freq) / mtime_ticks;
  return (uint32_t) freq & 0xFFFFFFFF;
  
}
 

static void PRCI_use_hfrosc(int div, int trim)
{
  // Make sure the HFROSC is running at its default setting
  // It is OK to change this even if we are running off of it.
  
  PRCI_REG(PRCI_HFROSCCFG) = (ROSC_DIV(div) | ROSC_TRIM(trim) | ROSC_EN(1));
  while ((PRCI_REG(PRCI_HFROSCCFG) & ROSC_RDY(1)) == 0) {
    cmb();
  }
  PRCI_REG(PRCI_PLLCFG) &= ~PLL_SEL(1);
}

static void PRCI_use_pll(int refsel, int bypass,
			 int r, int f, int q, int finaldiv,
			 int hfroscdiv, int hfrosctrim)
{
  // Ensure that we aren't running off the PLL before we mess with it.
  if (PRCI_REG(PRCI_PLLCFG) & PLL_SEL(1)) {
    // Make sure the HFROSC is running at its default setting
    PRCI_use_hfrosc(4, 16);
  }
  
  // Set PLL Source to be HFXOSC if available.
  uint32_t config_value = 0;

  config_value |= PLL_REFSEL(refsel);
  
  if (bypass) {
    // Bypass
    config_value |= PLL_BYPASS(1);

    PRCI_REG(PRCI_PLLCFG) = config_value;

    // If we don't have an HFXTAL, this doesn't really matter.
    // Set our Final output divide to divide-by-1:
    PRCI_REG(PRCI_PLLDIV) = (PLL_FINAL_DIV_BY_1(1) | PLL_FINAL_DIV(0));
  } else {
  
    // To overclock, use the hfrosc
    if (hfrosctrim >= 0 && hfroscdiv >= 0) {
      PRCI_use_hfrosc(hfroscdiv, hfrosctrim);
    }
    
    // Set DIV Settings for PLL
    
    // (Legal values of f_REF are 6-48MHz)

    // Set DIVR to divide-by-2 to get 8MHz frequency
    // (legal values of f_R are 6-12 MHz)

    config_value |= PLL_BYPASS(1);
    config_value |= PLL_R(r);

    // Set DIVF to get 512Mhz frequncy
    // There is an implied multiply-by-2, 16Mhz.
    // So need to write 32-1
    // (legal values of f_F are 384-768 MHz)
    config_value |= PLL_F(f);

    // Set DIVQ to divide-by-2 to get 256 MHz frequency
    // (legal values of f_Q are 50-400Mhz)
    config_value |= PLL_Q(q);

    // Set our Final output divide to divide-by-1:
    if (finaldiv == 1){
      PRCI_REG(PRCI_PLLDIV) = (PLL_FINAL_DIV_BY_1(1) | PLL_FINAL_DIV(0));
    } else {
      PRCI_REG(PRCI_PLLDIV) = (PLL_FINAL_DIV(finaldiv));
    }

    PRCI_REG(PRCI_PLLCFG) = config_value;

    // Un-Bypass the PLL.
    PRCI_REG(PRCI_PLLCFG) &= ~PLL_BYPASS(1);

    // Wait for PLL Lock
    // Note that the Lock signal can be glitchy.
    // Need to wait 100 us
    // RTC is running at 32kHz.
    // So wait 4 ticks of RTC.
    uint32_t now = CLINT_REG(CLINT_MTIME_LO);
    while (CLINT_REG(CLINT_MTIME_LO) - now < 4) {
      cmb();
    }

    // Now it is safe to check for PLL Lock
    while ((PRCI_REG(PRCI_PLLCFG) & PLL_LOCK(1)) == 0) {
      cmb();
    }
  }

  // Switch over to PLL Clock source
  PRCI_REG(PRCI_PLLCFG) |= PLL_SEL(1);
}

static void PRCI_use_default_clocks()
{
  // Turn off the LFROSC
  AON_REG(AON_LFROSC) &= ~ROSC_EN(1);

  // Use HFROSC
  PRIC_use_hfrosc(4, 16);
}

// This is a generic function, which
// doesn't span the entire range of HFROSC settings.
// It only adjusts the trim, which can span a hundred MHz or so.
// This function needs improvement to ensure that the PLL settings
// are legal every step of the way.
// It returns the actual measured CPU frequency.

uint32_t PRCI_set_hfrosctrim_for_f_cpu(uint32_t f_cpu)
{

  int hfrosctrim = 16;
  use_default_clocks();
  use_pll(0, 0, 1, 31, 1, 1, 4, 16);
  // Ignore the first run (for icache reasons)
  uint32_t cpu_freq = measure_cpu_freq(1000, RTC_FREQ); 
  cpu_freq = measure_cpu_freq(1000, RTC_FREQ);
  
  if (cpu_freq > F_CPU) {
    while((cpu_freq > F_CPU) && (hfrosctrim >= 0) {  
	use_pll(0, 0, 1, 31, 1, 1, 4, hfrosctrim );
	cpu_freq = measure_cpu_freq(1000, RTC_FREQ);
	hfrosctrim--;
      }
    return cpu_freq;
  }
  
  while(cpu_freq < F_CPU) {
    if (hfrosctrim >= 0x20) {
      break;
    }
    use_pll(0, 0, 1, 31, 1, 1, 4, hfrosctrim );
    cpu_freq = measure_cpu_freq(1000, RTC_FREQ);
    hfrosctrim++;
  }
  return cpu_freq;
}
