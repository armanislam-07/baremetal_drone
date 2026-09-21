#ifndef REGISTERS_H
#define REGISTERS_H
#include <stdint.h>
#define REG32(a) (*(volatile uint32_t *)(a))
#define RCC_CR REG32(0x40021000u)
#define RCC_CFGR REG32(0x40021004u)
#define RCC_APB2ENR REG32(0x40021018u)
#define RCC_APB1ENR REG32(0x4002101cu)
#define FLASH_ACR REG32(0x40022000u)
#define AFIO_MAPR REG32(0x40010004u)
#define SYST_CSR REG32(0xe000e010u)
#define SYST_RVR REG32(0xe000e014u)
#define SYST_CVR REG32(0xe000e018u)
#define RCC_APB2ENR_AFIOEN (1u<<0)
#define RCC_APB2ENR_IOPAEN (1u<<2)
#define RCC_APB2ENR_IOPBEN (1u<<3)
#define RCC_APB2ENR_IOPCEN (1u<<4)
#define RCC_APB2ENR_TIM1EN (1u<<11)
#define RCC_APB1ENR_TIM2EN (1u<<0)
#define RCC_APB1ENR_TIM3EN (1u<<1)
#define RCC_APB1ENR_I2C1EN (1u<<21)
typedef struct { volatile uint32_t CRL, CRH, IDR, ODR, BSRR, BRR, LCKR; } GPIO_T;
#define GPIOA ((GPIO_T *)0x40010800u)
#define GPIOB ((GPIO_T *)0x40010c00u)
#define GPIOC ((GPIO_T *)0x40011000u)
typedef struct { volatile uint32_t CR1, CR2, OAR1, OAR2, DR, SR1, SR2, CCR, TRISE; } I2C_T;
#define I2C1 ((I2C_T *)0x40005400u)
typedef struct { volatile uint32_t CR1, CR2, SMCR, DIER, SR, EGR, CCMR1, CCMR2, CCER, CNT, PSC, ARR, RCR, CCR1, CCR2, CCR3, CCR4, BDTR; } TIM_T;
#define TIM1 ((TIM_T *)0x40012c00u)
#define TIM2 ((TIM_T *)0x40000000u)
#define TIM3 ((TIM_T *)0x40000400u)
#endif
