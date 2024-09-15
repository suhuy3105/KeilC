#include "stm32f10x.h"
#include "Driver_I2C.h"
#include "main.h"

void SystemClock_Config(void)
{
	RCC->CR = 0;
	RCC->CR |= 1<<16;															 // Bat HSE clock
	while (!(RCC->CR & (1<<17)));			   					// Cho den khi HSE duoc bat
	//Cau hinh PLL
	RCC->CR &= ~(1<<24);  								 			// PLLON = 0, tat PLL truoc khi cau hinh
	RCC->CFGR |= 6<<24;													// HSE select
	RCC->CFGR |= 7<<18; 												 // MUL 9 -> 8*9=72Mhz
	RCC->CR |= 1<<24; 												 	// Bat PLL
	while (!(RCC->CR & (1<<25)));  	   					// Cho cho den khi PLL san sang
	RCC->CFGR |= 2<<0;  						 					 	// Chon PLL lam SYSCLK
	while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL); // Cho cho PLL duoc chon
	//Khoi tao AHB, APB buses
	RCC->CFGR |= 0<<7;   											// AHB = SYSCLK
	RCC->CFGR |= 4<<8;  											// APB1 = HCLK/2
	RCC->CFGR |= 0<<13;  											// APB2 = HCLK 
}

void GPIO_Config (void)
{
	RCC->APB2ENR |= 1<<3|1<<2; 										// Enable clock for PortA,B
	
	GPIOB->CRH = 0;
	GPIOB->CRH |= 0x03333333;											//	Output PushPull PB8-PB14
	GPIOB->CRL |= 0x00003333;											//	Output PushPull PB0-PB3
	
	GPIOA->CRL = 0;
	GPIOA->CRL |= 8<<0; 													// Input Push-pull
	GPIOA->ODR |= 1<<0;

	RCC->APB2ENR |= (1 << 0); 										// AFIO Clock enable
	AFIO->EXTICR[0] &= ~(AFIO_EXTICR1_EXTI0);			// Reset EXTICR register
	AFIO->EXTICR[0] |= AFIO_EVCR_PORT_PA;					// PA0 to EXTI0
	
	EXTI->IMR |= EXTI_IMR_MR0;										// Turn Interrupt Mask 0
	//EXTI->RTSR |= EXTI_RTSR_TR0;
	EXTI->FTSR |= (EXTI_RTSR_TR0);
	
	NVIC_EnableIRQ(EXTI0_IRQn);
	NVIC_SetPriority(EXTI0_IRQn, 0);
}