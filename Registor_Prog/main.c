//#include "stm32f10x.h"
//#include "stdio.h"


//void GPIO_Config(void);
//void SystemClock_Config(void);
//void Tim_Config(void);
//void display7segment(uint8_t digit);

//volatile  uint8_t count = 0;
//	
//const uint16_t LED[10]={0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90}; // Anot chung

//void Tim_Config(void)
//{
//	// Enable Timer 1
//	RCC->APB2ENR |= 1<<11;	
//	TIM1->PSC = 72-1;			//	72Mhz/72=1Mhz -> 1us	
//	TIM1->ARR = 1000-1;		//	1Mhz/1000=1kHz -> 1ms        
//	TIM1->CR1 |= 1<<0;           
//	
//}
//void GPIO_Config (void)
//{
//	RCC->APB2ENR |= 1<<3|1<<2; 										// Enable clock for PortA,B
//	
//	GPIOB->CRH = 0;
//	GPIOB->CRH |= 0x03333333;											//	Output PushPull PB8-PB14
//	GPIOB->CRL |= 0x00003333;											//	Output PushPull PB0-PB3
//	
//	GPIOA->CRL = 0;
//	GPIOA->CRL |= 8<<0; 													// Input Push-pull
//	GPIOA->ODR |= 1<<0;

//	RCC->APB2ENR |= (1 << 0); 										// AFIO Clock enable
//	AFIO->EXTICR[0] &= ~(AFIO_EXTICR1_EXTI0);			// Reset EXTICR register
//	AFIO->EXTICR[0] |= AFIO_EVCR_PORT_PA;					// PA0 to EXTI0
//	
//	EXTI->IMR |= EXTI_IMR_MR0;										// Turn Interrupt Mask 0
//	//EXTI->RTSR |= EXTI_RTSR_TR0;
//	EXTI->FTSR |= (EXTI_RTSR_TR0);
//	
//	NVIC_EnableIRQ(EXTI0_IRQn);
//	NVIC_SetPriority(EXTI0_IRQn, 0);
//}

//void SystemClock_Config(void)
//{
//	RCC->CR = 0;
//	RCC->CR |= 1<<16;															 // Bat HSE clock
//	while (!(RCC->CR & (1<<17)));			   					// Cho den khi HSE duoc bat
//	//Cau hinh PLL
//	RCC->CR &= ~(1<<24);  								 			// PLLON = 0, tat PLL truoc khi cau hinh
//	RCC->CFGR |= 6<<24;													// HSE select
//	RCC->CFGR |= 7<<18; 												 // MUL 9 -> 8*9=72Mhz
//	RCC->CR |= 1<<24; 												 	// Bat PLL
//	while (!(RCC->CR & (1<<25)));  	   					// Cho cho den khi PLL san sang
//	RCC->CFGR |= 2<<0;  						 					 	// Chon PLL lam SYSCLK
//	while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL); // Cho cho PLL duoc chon
//	//Khoi tao AHB, APB buses
//	RCC->CFGR |= 0<<7;   											// AHB = SYSCLK
//	RCC->CFGR |= 4<<8;  											// APB1 = HCLK/2
//	RCC->CFGR |= 0<<13;  											// APB2 = HCLK 
//}

///* Ham delay dung timer1 */
//static void delay( uint32_t ms )
//{
//  uint32_t i;
//  for( i = 0; i <= ms; i++ )
//  {
//    // Clear the count
//    TIM1->CNT = 0;   
//    // Wait UIF to be set
//    while(!(TIM1->SR & (1<<0)));    			//	Update interrupt flag 
//    // Reset UIF 
//    TIM1->SR &= ~(1<<0);
//  }
//}

///* Ham hien thi LED 7 doan */
//void display7segment(uint8_t digit)
//{
//	GPIOB->ODR = 0;
//	GPIOB->ODR |= LED[digit]<<8;
//}

////uint8_t button_Pressed(void)
////{
////    if (!(GPIOA->IDR & GPIO_IDR_IDR0))
////    {
////			return 1;
////    }
////    return 0;  
////}

///* Ham xu ly ngat */
//void EXTI0_IRQHandler(void)
//{
//	if (EXTI->PR & EXTI_PR_PR0)  						// Kiem tra co ngat
//	{
//		EXTI->PR |= EXTI_PR_PR0;							// Xoa co ngat
//		
//		display7segment(count);
//		count++;
//		if (count>9)
//			count=0;		
//	}
//}

//int main(void)
// {
//	 SystemClock_Config();
//	 GPIO_Config();
//	 Tim_Config();
//	 EXTI0_IRQHandler();
//		while(1)
//		{
//			
//		} 
//}

////Dùng ngat ngoài de dem s? l?n b?m phím
////Dùng ngat timer1 de hien th? 4 led 7 do?n
////Dùng ngat Timer2 de tao b? h?n gi?: ví d? sau 1phuts 30s thì b?t t?t led don

#include "stm32f10x.h"

uint8_t LED[10] = {0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90};

volatile uint8_t counter = 0;  
volatile uint8_t led_index = 0;

void GPIO_Config(void) 
{
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN;

	GPIOB->CRH = 0x33333333; 
	GPIOB->CRL = 0x00003333; 
	
	GPIOA->CRL = 0;
	GPIOA->CRL |= 3<<4;														//	Output PushPull PA1
	GPIOA->CRL |= 8<<0;														//	Input PushPull PA0
	GPIOA->ODR |= 1<<0;
}

void EXTI_Config(void)
{
	RCC->APB2ENR |= (1 << 0); 										// AFIO Clock enable
	AFIO->EXTICR[0] &= ~(AFIO_EXTICR1_EXTI0);			// Reset EXTICR register
	AFIO->EXTICR[0] |= AFIO_EXTICR1_EXTI0_PA;					// PA0 to EXTI0
	EXTI->IMR |= EXTI_IMR_MR0;										// Turn Interrupt Mask 0
	EXTI->FTSR |= (EXTI_FTSR_TR0);	
	NVIC_EnableIRQ(EXTI0_IRQn);
	NVIC_SetPriority(EXTI0_IRQn, 0);
}
/*------------------------*/

void TIM1_Config(void) 
{
	RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;

	TIM1->PSC = 7999;        // Prescaler: chia t?n s? xu?ng 1kHz (1ms)
	TIM1->ARR = 250;         // Auto-reload: quét LED m?i 250ms
	TIM1->DIER |= TIM_DIER_UIE; // Kích ho?t ng?t c?p nh?t
	TIM1->CR1 |= TIM_CR1_CEN;   // B?t d?u Timer

	NVIC_EnableIRQ(TIM1_UP_IRQn);
}

void UpdateDisplay(uint8_t cnt) 
{
	uint8_t digit[4];
	digit[0] = cnt / 1000;
	digit[1] = cnt / 100;
	digit[2] = cnt / 10;    
	digit[3] = cnt % 10; 

	GPIOB->BSRR = (0x0F << 16);  

	GPIOB->ODR |= 0x80<<8;
	GPIOB->ODR |= LED[digit[led_index]]<<8; 

	GPIOB->BSRR = (1 << led_index);  

	led_index = (led_index + 1) % 4;
}

void TIM1_UP_IRQHandler(void) 
{
	if (TIM1->SR & TIM_SR_UIF) 
	{  
		TIM1->SR &= ~TIM_SR_UIF;  
		UpdateDisplay(counter);    
	}
}
void EXTI0_IRQHandler(void)
{
	if (EXTI->PR & EXTI_PR_PR0)  						// Kiem tra co ngat
	{
		EXTI->PR |= EXTI_PR_PR0;							// Xoa co ngat
		counter = counter + 1;
	}
}
int main(void) 
{
	GPIO_Config();
	EXTI_Config();
	TIM1_Config();

	while (1) 
	{
	}
}
