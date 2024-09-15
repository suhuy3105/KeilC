#include "stm32f10x.h"
#include "stdio.h"


void GPIO_Config(void);
void SystemClock_Config(void);
void Tim_Config(void);
void TIM2_IRQHandler(void);
void Tim_Config(void)
{	
	// Enable Timer 2
	RCC->APB1ENR |= 1<<0;	
	TIM2->PSC = 7200-1;			//	72Mhz/7200=10khz -> 0.1ms	
	TIM2->ARR = 60000-1;			//	6s  
	TIM2->DIER |= TIM_DIER_UIE;
	TIM2->CR1 |= TIM_CR1_CEN;
	NVIC_EnableIRQ(TIM2_IRQn);
}

void TIM2_IRQHandler(void)
{
    if (TIM2->SR & TIM_SR_UIF) {  
        TIM2->SR &= ~TIM_SR_UIF;  

        // Bat tat LED sau moi 6s
				
        GPIOB->ODR ^= GPIO_ODR_ODR0;  
    }
}
void GPIO_Config (void)
{
	RCC->APB2ENR |= 1<<3; 										// Enable clock for PortB
	
	GPIOB->CRL = 0;
	GPIOB->CRL |= 3<<0;
}

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

/* Ham delay dung timer1 */

int main(void)
{
	SystemClock_Config();
	GPIO_Config();
	Tim_Config();
	while(1)
	{

	} 
}
