#include "drivers/registers.h"
/* Board schematic specifies a 16 MHz crystal. HSE/2 * 9 = 72 MHz. */
void SystemInit(void)
{
    RCC_CR |= 1u << 16;
    while (!(RCC_CR & (1u << 17))) { }
    FLASH_ACR = (FLASH_ACR & ~7u) | 2u | (1u << 4);
    /* APB1 = 36 MHz, APB2 = 72 MHz; PLL source HSE/2, multiplier 9. */
    RCC_CFGR = (RCC_CFGR & ~((7u << 8) | (3u << 14) | (15u << 18) | (3u << 0)))
             | (4u << 8) | (1u << 16) | (1u << 17) | (7u << 18);
    RCC_CR |= 1u << 24;
    while (!(RCC_CR & (1u << 25))) { }
    RCC_CFGR = (RCC_CFGR & ~3u) | 2u;
    while (((RCC_CFGR >> 2) & 3u) != 2u) { }
}
