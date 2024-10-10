/**
******************************************************************************
* @author  	Eunchong Kim
* @Mod		2024-10-10
* @brief   	Embedded Controller: LAB_PWM_DCmotor
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

#define DIR_PIN PC_2
#define PWM_PIN PA_0

volatile uint32_t count = 0;		// counter
volatile uint8_t speed = 0;			// speed factor
volatile uint8_t m_state = 0;		// moter state factor, 0 false, 1 true

void setup(void);

	
int main(void) { 
	// Initialiization --------------------------------------------------------
	setup();
	// Inifinite Loop ----------------------------------------------------------
	while(1){}
}

// Initialiization 
void setup(void){
	RCC_PLL_init();						// system clocks initialisation
	SysTick_init();						// SysTick initialisation
	
	EXTI_init(BUTTON_PIN, FALL, 0);		// EXTI initialisation fall trigger type
	
	GPIO_init(BUTTON_PIN, INPUT);		// button pin initialise as input
	GPIO_pupd(BUTTON_PIN, 01);			// Pull-up
	
	GPIO_init(DIR_PIN, OUTPUT);			// direction pin initialise as output
	GPIO_otype(BUTTON_PIN, 0);			// Push-Pull
	
	PWM_init(PWM_PIN);					// PWM initialisation on PA_0
	GPIO_otype(PWM_PIN, 0);				// Push-Pull
	GPIO_pupd(PWM_PIN, 01);				// Pull-up
	GPIO_ospeed(PWM_PIN, 10);			// Fast
	PWM_period(PWM_PIN, 1);				// PWM period to 1ms
	PWM_duty(PWM_PIN, 0.25);			// initial PWM duty to 0.25 (25%)
	TIM_UI_init(TIM3, 500);				// initialse timer to interrupt every 500ms
}

void TIM3_IRQHandler(void){
	if (is_UIF(TIM3)){								// check UIF
		if(!m_state){								// check motor state is false 
			count++;								// counter increment
			if(count == 4){							// set condition to counter = 4 (2s)
				speed ^= 1;							// toggle speed factor
				float duty = 0.25 + 0.5 * speed;	// determine duty
				PWM_duty(PWM_PIN, duty);			// set PWM duty
				count = 0;							// count clear
			}
		}
		clear_UIF(TIM3);							// clear UIF
	}
}

void EXTI15_10_IRQHandler(void) {
	if (is_pending_EXTI(BUTTON_PIN)) {				// Pending check 
		m_state ^= 1;								// toggle motor state factor
		if(m_state){								// check motor state is true 
			PWM_duty(PWM_PIN, 0);					// set PWM duty to 0
		} else {									// when motor state is false
			float duty = 0.25 + 0.5 * speed;		// set duty to previous value
			PWM_duty(PWM_PIN, duty);				// set PWM duty
		}
		clear_pending_EXTI(BUTTON_PIN);				// Pending clear
	}
}
