#include "i2c.h"
#include "io.h"
#include "registers.h"
#define SB (1u << 0)
#define ADDR (1u << 1)
#define BTF (1u << 2)
#define RXNE (1u << 6)
#define TXE (1u << 7)
#define AF (1u << 10)
#define ERRORS ((1u << 8) | (1u << 9) | AF | (1u << 11))
#define LIMIT 200000u
static int wait_flag(uint32_t flag)
{
    for (uint32_t n = 0; n < LIMIT; ++n) {
        uint32_t sr = I2C1->SR1;
        if (sr & ERRORS) return 0;
        if (sr & flag) return 1;
    }
    return 0;
}
static int fail(void)
{
    I2C1->CR1 |= 1u << 9;
    I2C1->SR1 &= ~ERRORS;
    return 0;
}
static int begin(uint8_t address, int read)
{
    I2C1->CR1 |= 1u << 8;
    if (!wait_flag(SB)) return 0;
    I2C1->DR = (uint32_t)(address << 1) | (read ? 1u : 0u);
    return wait_flag(ADDR);
}
static void clear_addr(void)
{
    (void)I2C1->SR1;
    (void)I2C1->SR2;
}
void i2c_init(void)
{
    RCC_APB1ENR |= RCC_APB1ENR_I2C1EN;
    RCC_APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPBEN;
    AFIO_MAPR |= 1u << 1; /* I2C1 remap: PB8 SCL, PB9 SDA. External pullups required. */
    gpio_config(GPIOB, 8, 3, 3);
    gpio_config(GPIOB, 9, 3, 3);
    I2C1->CR1 = 1u << 15;
    I2C1->CR1 = 0;
    I2C1->CR2 = 36u;
    I2C1->CCR = (1u << 15) | 30u; /* Fast mode, 400 kHz at PCLK1=36 MHz. */
    I2C1->TRISE = 11u;
    I2C1->CR1 = 1u;
}
int i2c_write_reg(uint8_t address, uint8_t reg, uint8_t value)
{
    if (!begin(address, 0)) return fail();
    clear_addr();
    if (!wait_flag(TXE)) return fail();
    I2C1->DR = reg;
    if (!wait_flag(TXE)) return fail();
    I2C1->DR = value;
    if (!wait_flag(BTF)) return fail();
    I2C1->CR1 |= 1u << 9;
    return 1;
}
int i2c_read_reg(uint8_t address, uint8_t reg, uint8_t *value)
{
    if (!value || !begin(address, 0)) return fail();
    clear_addr();
    if (!wait_flag(TXE)) return fail();
    I2C1->DR = reg;
    if (!wait_flag(BTF)) return fail();
    if (!begin(address, 1)) return fail();
    I2C1->CR1 &= ~(1u << 10); /* NACK the only byte. */
    clear_addr();
    I2C1->CR1 |= 1u << 9;
    if (!wait_flag(RXNE)) return fail();
    *value = (uint8_t)I2C1->DR;
    return 1;
}
