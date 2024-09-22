/**
******************************************************************************
* @author  Eunchong Kim
* @Mod		 2024-09-22
* @brief   Embedded Controller:  Tutorial Digital In/Out 7-segment Display
* 
******************************************************************************
*/

#include "stm32f4xx.h"
#include "ecGPIO2.h"
#include "ecRCC2.h"

/*
void setup(void);
	
int main(void) { 
	// Initialiization --------------------------------------------------------
	setup();
	unsigned int cnt = 0;
	
	// Inifinite Loop ----------------------------------------------------------
	while(1){
		sevensegment_display(cnt % 10); // Display the current value (0-9) on the 7-segment
		if(GPIO_read(BUTTON_PIN) == 0) cnt++; // Increment counter when button is pressed
        if (cnt > 9) cnt = 0; // Reset counter after 9
		delay(250);  // delay_ms(250);
	}
}


// Initialiization 
void setup(void)
{
	RCC_HSI_init();	
	GPIO_init(BUTTON_PIN, INPUT);  // calls RCC_GPIOC_enable()
	GPIO_pupd(BUTTON_PIN, 01);
	sevensegment_display_init(PA_7, PB_6, PC_7, PA_9);  // Decoder input A,B,C,D
	PinName_t pins[] ={PA_7, PB_6, PC_7, PA_9};
	// Loop through all pins to set their otype, pupd , and ospeed
	for(int i = 0; i < 4; i++){
		GPIO_otype(pins[i], 0);
		GPIO_pupd(pins[i], 00);
		GPIO_ospeed(pins[i], 01);
	}
}
*/

void setup(void);
	
int main(void) { 
	// Initialiization --------------------------------------------------------
	setup();
	unsigned int cnt = 0;
	
	// Inifinite Loop ----------------------------------------------------------
	while(1){
		sevensegment_decoder(cnt % 10); // Display the current value (0-9) on the 7-segment
		if(GPIO_read(BUTTON_PIN) == 0) cnt++; // Increment counter when button is pressed
        if (cnt > 9) cnt = 0; // Reset counter after 9
		delay(250);  // delay_ms(250);
	}
}


// Initialiization 
void setup(void)
{
	RCC_HSI_init();	
	GPIO_init(BUTTON_PIN, INPUT);  // calls RCC_GPIOC_enable()
	GPIO_pupd(BUTTON_PIN, 01);
	// Initialize the 7-segment display decoder with the respective pins
	sevensegment_decoder_init(PA_5, PA_6, PA_7, PB_6, PC_7, PA_9, PA_8, PB_10);
	PinName_t pins[] ={PA_5, PA_6, PA_7, PB_6, PC_7, PA_9, PA_8, PB_10};
	// Loop through all pins to set their otype, pupd , and ospeed
	for(int i = 0; i < 8; i++){
		GPIO_otype(pins[i], 0);
		GPIO_pupd(pins[i], 00);
		GPIO_ospeed(pins[i], 01);
	}
}