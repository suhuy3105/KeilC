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
/*------------------------*/

/* Variable declaration */
volatile uint8_t counter = 0;  
volatile uint8_t led_index = 0; 
uint8_t current_led = 0;
const uint16_t LED[10]={0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90}; // Anot chung