#include "io.h"
void gpio_config(GPIO_T *port, unsigned pin, unsigned mode, unsigned cnf)
{
    if (pin > 15u) return;
    volatile uint32_t *reg = pin < 8u ? &port->CRL : &port->CRH;
    unsigned shift = (pin & 7u) * 4u;
    *reg = (*reg & ~(15u << shift)) | (((cnf << 2) | mode) << shift);
}
void io_init(void)
{
    RCC_APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN | RCC_APB2ENR_IOPCEN | RCC_APB2ENR_AFIOEN;
    /* LP9110 direction inputs are held low before PWM pins are enabled. */
    GPIOA->BRR = (1u << 1) | (1u << 4);
    GPIOB->BRR = 1u << 0;
    GPIOC->BRR = 1u << 1;
    gpio_config(GPIOA, 1, 1, 0);
    gpio_config(GPIOA, 4, 1, 0);
    gpio_config(GPIOB, 0, 1, 0);
    gpio_config(GPIOC, 1, 1, 0);
    gpio_config(GPIOA, 0, 3, 2);  /* TIM2_CH1 */
    gpio_config(GPIOA, 10, 3, 2); /* TIM1_CH3 */
    gpio_config(GPIOA, 6, 3, 2);  /* TIM3_CH1 */
    gpio_config(GPIOA, 7, 3, 2);  /* TIM3_CH2 */
}
