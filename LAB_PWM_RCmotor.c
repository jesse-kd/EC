/**
******************************************************************************
* @author  	Eunchong Kim
* @Mod		2024-10-10
* @brief   	Embedded Controller: LAB_PWM_RCmotor
* 
******************************************************************************
*/

#include "stm32f411xe.h"
#include "ecGPIO2.h"
#include "ecEXTI2.h"
#include "ecSysTick2.h"
#include "ecRCC2.h"
#include "ecTIM2.h"
#include "ecPWM2.h"

#define PWM_PIN	PA_1

volatile uint32_t angle = 0;		// angle

void setup(void);


int main(void) {
	// Initialization --------------------------------------------------
	setup();
	
	// Infinite Loop ---------------------------------------------------
	while(1){}
}


// Initialization
void setup(void){
	RCC_PLL_init();							// system clocks initialisation
	SysTick_init();							// SysTick initialisation
	
	EXTI_init(BUTTON_PIN, FALL, 0);			// EXTI initialisation fall trigger type
	
	GPIO_init(BUTTON_PIN, INPUT);			// button pin initialise as input
	GPIO_pupd(BUTTON_PIN, 10);				// Pull-up
	
	PWM_init(PWM_PIN);						// PWM initialisation on PA_0
	GPIO_otype(PWM_PIN, 0);					// Push-Pull
	GPIO_pupd(PWM_PIN, 01);					// Pull-up
	GPIO_ospeed(PWM_PIN, 10);				// Fast
	PWM_period(PWM_PIN, 20);				// PWM period to 20ms
	PWM_duty(PWM_PIN, 0.025);				// initial PWM duty to 0.025 (2.5%)
	TIM_UI_init(TIM3, 500);					// initialse timer to interrupt every 500ms
}

void TIM3_IRQHandler(void){
	if (is_UIF(TIM3)){										// check UIF
		angle += 10;										// 10 degree increment
		if (angle > 180) angle = 0;							// angle clear when it exceed 180
		float duty = (0.5 + (angle / 180.0) * 2.0) / 20.0;	// determine duty
		PWM_duty(PWM_PIN, duty);							// set PWM duty
		clear_UIF(TIM3);									// clear UIF
	}
}

void EXTI15_10_IRQHandler(void) {
	if (is_pending_EXTI(BUTTON_PIN)) {						// Pending check 
		angle = 0;											// clear angle
		PWM_duty(PWM_PIN, 0.025);							// set PWM duty to initial value
		clear_pending_EXTI(BUTTON_PIN);						// Pending clear
	}
}