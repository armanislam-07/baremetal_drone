#include "pwm.h"
#include "registers.h"
/* 72 MHz timer clock / 3600 counts = 20 kHz; 1 count = 0.278 permille. */
#define PERIOD 3599u
static void timer_init(TIM_T *t)
{
    t->CR1 = 0;
    t->PSC = 0;
    t->ARR = PERIOD;
    t->EGR = 1;
}
void motor_stop_all(void)
{
    TIM2->CCR1 = 0; TIM1->CCR3 = 0; TIM3->CCR1 = 0; TIM3->CCR2 = 0;
}
void motor_init(void)
{
    RCC_APB1ENR |= RCC_APB1ENR_TIM2EN | RCC_APB1ENR_TIM3EN;
    RCC_APB2ENR |= RCC_APB2ENR_TIM1EN;
    timer_init(TIM1); timer_init(TIM2); timer_init(TIM3);
    TIM2->CCMR1 = (6u << 4) | (1u << 3);
    TIM1->CCMR2 = (6u << 4) | (1u << 3);
    TIM3->CCMR1 = (6u << 4) | (1u << 3) | (6u << 12) | (1u << 11);
    motor_stop_all();
    TIM1->CCER = 1u << 8;
    TIM2->CCER = 1u;
    TIM3->CCER = (1u << 0) | (1u << 4);
    TIM1->BDTR = 1u << 15;
    TIM1->CR1 = 1u << 7; TIM2->CR1 = 1u << 7; TIM3->CR1 = 1u << 7;
    TIM1->EGR = 1; TIM2->EGR = 1; TIM3->EGR = 1;
    TIM1->CR1 |= 1; TIM2->CR1 |= 1; TIM3->CR1 |= 1;
}
void motor_set(unsigned motor, uint16_t permille)
{
    if (permille > 1000u) permille = 1000u;
    uint32_t duty = ((uint32_t)permille * (PERIOD + 1u)) / 1000u;
    switch (motor) {
    case 0: TIM2->CCR1 = duty; break;
    case 1: TIM1->CCR3 = duty; break;
    case 2: TIM3->CCR1 = duty; break;
    case 3: TIM3->CCR2 = duty; break;
    default: break;
    }
}
