#include "stm32f10x.h"

/* Functional declaration */
void GPIO_Config(void);
void SystemClock_Config(void);
void Tim_Config(void);
//void LED_Display(uint8_t digit, uint8_t position);
void LED_Display(uint8_t cnt);
void TIM1_UP_IRQHandler(void);
void TIM2_IRQHandler(void);
void TIM1_IRQHandler(void);
void EXTI0_IRQHandler(void);
void EXTI_Config(void);
/*------------------------*/

/* Variable declaration */
static uint8_t counter = 0;  
static uint8_t led_index = 0; 
static uint16_t LED[10]={0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90}; // Anot chung
/*------------------------*/

/* Timer configuration */
void Tim_Config(void)
{
	RCC->APB2ENR |= 1<<11;												// Enable Timer 1
	TIM1->PSC = 72-1;															// 72Mhz/72=1Mhz -> 1us	
	TIM1->ARR = 500-1;														// 1Mhz/500=1kHz -> 20ms        
	TIM1->CR1 |= 1<<0;   
	TIM1->DIER |= TIM_DIER_UIE;  
	NVIC_EnableIRQ(TIM1_UP_IRQn);  								// Bat ngat cho Timer1
	
	RCC->APB1ENR |= 1<<0;													// Enable Timer 2
	TIM2->PSC = 7200-1;														// 72Mhz/7200=10khz -> 0.1ms	
	TIM2->ARR = 60000-1;													// 6s  
	TIM2->CR1 |= 1<<0;
	TIM2->DIER |= TIM_DIER_UIE;
	NVIC_EnableIRQ(TIM2_IRQn); 
}

void EXTI_Config(void)
{
	RCC->APB2ENR |= (1 << 0); 										// AFIO Clock enable
	AFIO->EXTICR[0] &= ~(AFIO_EXTICR1_EXTI0);			// Reset EXTICR register
	AFIO->EXTICR[0] |= AFIO_EVCR_PORT_PA;					// PA0 to EXTI0
	EXTI->IMR |= EXTI_IMR_MR0;										// Turn Interrupt Mask 0
	EXTI->FTSR |= (EXTI_RTSR_TR0);	
	NVIC_EnableIRQ(EXTI0_IRQn);
	NVIC_SetPriority(EXTI0_IRQn, 0);
}
/*------------------------*/

/* GPIO configuration */
void GPIO_Config (void)
{
	RCC->APB2ENR |= 1<<3|1<<2; 										// Enable clock for PortA,B
	
	GPIOB->CRH = 0;
	GPIOB->CRH |= 0x03333333;											//	Output PushPull PB8-PB14
	GPIOB->CRL = 0;
	GPIOB->CRL |= 0x00003333;											//	Output PushPull PB0-PB3
	
	GPIOA->CRL = 0;
	GPIOA->CRL |= 3<<4;														//	Output PushPull PA1
	GPIOA->CRL |= 8<<0;														//	Input PushPull PA0
	GPIOA->ODR |= 1<<0;
}
/*------------------------*/

/* System Clock configuration */
void SystemClock_Config(void)
{
	RCC->CR = 0;
	RCC->CR |= 1<<16;														// Bat HSE clock
	while (!(RCC->CR & (1<<17)));			   				// Cho den khi HSE duoc bat
	/* Cau hinh PLL */
	RCC->CR &= ~(1<<24);  								 			// PLLON = 0, tat PLL truoc khi cau hinh
	RCC->CFGR |= 6<<24;													// HSE select
	RCC->CFGR |= 7<<18; 												// MUL 9 -> 8*9=72Mhz
	RCC->CR |= 1<<24; 												 	// Bat PLL
	while (!(RCC->CR & (1<<25)));  	   					// Cho cho den khi PLL san sang
	RCC->CFGR |= 2<<0;  						 					 	// Chon PLL lam SYSCLK
	while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL); // Cho cho PLL duoc chon
	/* Khoi tao AHB, APB buses */
	RCC->CFGR |= 0<<7;   												// AHB = SYSCLK
	RCC->CFGR |= 4<<8;  												// APB1 = HCLK/2
	RCC->CFGR |= 0<<13;  												// APB2 = HCLK 
}
/*------------------------*/

void TIM2_IRQHandler(void)
{
	if (TIM2->SR & TIM_SR_UIF) {  
		TIM2->SR &= ~TIM_SR_UIF;  
		// Bat tat LED sau moi 6s			
		GPIOA->ODR ^= GPIO_ODR_ODR1;  
	}
}

void TIM1_IRQHandler(void)
{
	if (TIM1->SR & TIM_SR_UIF) {  
		TIM1->SR &= ~TIM_SR_UIF;  
		LED_Display(counter);
	}
}
void EXTI0_IRQHandler(void)
{
	if (EXTI->PR & EXTI_PR_PR0)  						// Kiem tra co ngat
	{
		EXTI->PR |= EXTI_PR_PR0;							// Xoa co ngat
		counter++;
		if (counter > 9999)
			counter = 0;
	}
}
void LED_Display(uint8_t cnt)
{
	uint8_t digit[4];
	digit[0] = cnt / 1000;
	digit[1] = cnt / 100;
	digit[2] = cnt / 10;    
	digit[3] = cnt % 10;   
	
	GPIOB->BSRR = (0x0F << 16);
	
	GPIOB->ODR = 0;
	GPIOB->ODR |= LED[digit[led_index]]<<8;  // Hien thi so lên PB8-PB14	
	
	GPIOB->BSRR = 1<<led_index;
	
	led_index = (led_index + 1) % 4;
}
int main(void)
{
//	SystemClock_Config();
	GPIO_Config();
	Tim_Config();
	EXTI_Config();
	while(1)
	{

	} 
}


/* Ham delay dung timer1 */
//static void delay( uint32_t ms )
//{
//  uint32_t i;
//  for( i = 0; i <= ms; i++ )
//  {
//    // Clear the count
//    TIM2->CNT = 0;   
//    // Wait UIF to be set
//    while(!(TIM2->SR & (1<<0)));    			//	Update interrupt flag 
//    // Reset UIF 
//    TIM2->SR &= ~(1<<0);
//  }
//}