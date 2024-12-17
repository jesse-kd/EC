/**
******************************************************************************
* @author  	Eunchong Kim
* @Mod		2024-12-05
* @brief   	Embedded Controller: Project 2
* 
******************************************************************************
*/

#include "ecSTM32F4v2.h"
#include <stdlib.h>
#include <string.h>

#define Interrupt_PIN PA_0
#define Manual_PIN PA_1

// Bluetooth Module
uint8_t btData = 0;
uint8_t p_btData = 0;

//TIM3 intrupt
volatile uint32_t count = 0;
volatile uint32_t Int_count = 0;
volatile uint32_t manual_count = 0;
volatile uint8_t S_manual = 0;

//teraterm display
uint8_t Mode = 0;
uint8_t ControlMode = 0;
uint8_t NofMeal = 0;
char (*FOOD_Time)[3] = NULL;

uint8_t timeVal[2] = {0, 0};

const char *Ecnt[5] = {"1st", "2nd", "3rd", "4th", "5th"};
uint16_t Want_To_Feed = 0;
char buffer[256];

uint8_t current_time[2][3] = {{0, 0, 0},{0, 0, 0}};
uint8_t set_time[2][3] = {{0, 0, 0},{0, 0, 0}};
uint8_t demo_time[3] = {0, 0, 0};
uint8_t change_time = 0;
uint8_t setting_int = 0;
uint8_t demo_set = 0;

void AllocateMealTimes(int NoM);
void FreeMealTimes(void);
void pet_feeding(void);
uint8_t CheckMode(uint8_t num);
uint8_t CheckNumber(uint8_t num);

// Function to initialize the RTC
void RTC_Init(void) {
    // Enable the PWR clock and allow access to the backup domain
    RCC->APB1ENR |= RCC_APB1ENR_PWREN;  // Enable PWR clock
    PWR->CR |= PWR_CR_DBP;              // Enable access to the backup domain

    // Enable the LSE clock
    RCC->BDCR |= RCC_BDCR_LSEON;        // Turn on LSE clock
    while (!(RCC->BDCR & RCC_BDCR_LSERDY));  // Wait until LSE is stable

    // Select the RTC clock source (LSE) and enable the RTC
    RCC->BDCR |= RCC_BDCR_RTCSEL_0;     // Select LSE as RTC clock source (RTCSEL = 01)
    RCC->BDCR |= RCC_BDCR_RTCEN;        // Enable the RTC
}

// Function to read and print the current time and date
void RTC_GetDateTime(void) {
    // Ensure RTC registers are synchronized
    if (!(RTC->ISR & RTC_ISR_RSF)) {
        RTC->ISR &= ~RTC_ISR_RSF;           // Clear RSF flag
        while (!(RTC->ISR & RTC_ISR_RSF));  // Wait for synchronization
    }

    uint32_t time = RTC->TR;            // Read the time register
    uint32_t date = RTC->DR;            // Read the date register

    // Convert BCD to decimal and extract time components
    uint8_t hours = ((time >> 20) & 0x3) * 10 + ((time >> 16) & 0xF);
    uint8_t minutes = ((time >> 12) & 0x7) * 10 + ((time >> 8) & 0xF);
    uint8_t seconds = ((time >> 4) & 0x7) * 10 + (time & 0xF);
        
    // Extract date components
    uint8_t year = ((date >> 20) & 0xF) * 10 + ((date >> 16) & 0xF);  // Extract year
    uint8_t month = (date >> 8) & 0xF;  // Extract month
    uint8_t day = ((date >> 4) & 0x3) * 10 + (date & 0xF);           // Extract day
		
		current_time[0][0] = year;
		current_time[0][1] = month;
		current_time[0][2] = day;		
		current_time[1][0] = hours;
		current_time[1][1] = minutes;
		current_time[1][2] = seconds;

		
    // Print the current time
    printf("Time: %02d:%02d:%02d\r\n", current_time[1][0], current_time[1][1], current_time[1][2]);
    printf("Date: 20%02d-%02d-%02d\r\n", current_time[0][0], current_time[0][1], current_time[0][2]);
}
                                             
void RTC_SetDate(uint8_t year, uint8_t month, uint8_t day, uint8_t hours, uint8_t minutes, uint8_t seconds) {
    RTC->WPR = 0xCA;
    RTC->WPR = 0x53;
        
    RTC->ISR |= RTC_ISR_INIT;
    while (!(RTC->ISR & RTC_ISR_INITF));
        
    uint32_t bcd_hours = ((hours / 10) << 4) | (hours % 10);
    uint32_t bcd_minutes = ((minutes / 10) << 4) | (minutes % 10);
    uint32_t bcd_seconds = ((seconds / 10) << 4) | (seconds % 10);
        
    uint32_t bcd_year = ((year / 10) << 4) | (year % 10);
    uint32_t bcd_month = month;
    uint32_t bcd_day = ((day / 10) << 4) | (day % 10);
        
    RTC->TR = (bcd_hours << 16) | (bcd_minutes << 8) | bcd_seconds;
    RTC->DR = (bcd_year << 16) | (bcd_month << 8) | bcd_day;

    RTC->ISR &= ~RTC_ISR_INIT;
    while (!(RTC->ISR & RTC_ISR_RSF));

    RTC->WPR = 0xFF;
}

// Example main function to receive time via UART
int main(void) {
    RCC_PLL_init();
    SysTick_init();
		UART1_init();
    UART2_init();
    RTC_Init();
    TIM_UI_init(TIM3, 500);
		GPIO_init(Interrupt_PIN, OUTPUT);
		GPIO_init(Manual_PIN, OUTPUT);
		GPIO_otype(Interrupt_PIN, 0);
		GPIO_pupd(Interrupt_PIN, 00);
		GPIO_ospeed(Interrupt_PIN, 01);
		GPIO_otype(Manual_PIN, 0);
		GPIO_pupd(Manual_PIN, 00);
		GPIO_ospeed(Manual_PIN, 01);
		
		// GPIO_write(Interrupt_PIN, HIGH);
		
    while (1) {
			//printf("Choose the mode (A,M,S): ");
			snprintf(buffer, sizeof(buffer), "\r\nChoose the mode (A,M,S,D): ");
			USART_write(USART1, (uint8_t *)buffer, strlen(buffer));
			
			// Input Mode
			while(1){
				if (Mode != 0 && btData == '\r') break;
				else if (btData == 65 || btData == 97 || btData == 77 || btData == 109 || btData == 85 || btData == 115 || btData == 68 || btData == 100) Mode = btData;
			}
			btData = 0;
			
			switch(Mode){
				case 65: 	ControlMode = 1; break;
				case 77:	ControlMode = 2; break;
				case 83:	ControlMode = 3; break;
				case 68:	ControlMode = 4; break;
				case 97:	ControlMode = 1; break;
				case 109:	ControlMode	= 2; break;
				case 115:	ControlMode	= 3; break;
				case 100:	ControlMode = 4; break;
				default: snprintf(buffer, sizeof(buffer), "\r\nInvalid mode. Please choose A, M, S and D.\r\n"); USART_write(USART1, (uint8_t *)buffer, strlen(buffer)); continue;
			}
			Mode = 0;
			
			// Auto mode setting, setting
			if(ControlMode == 1){
				// printf("The number of meals for your pet a day (MAX 5 times a day): ");
				snprintf(buffer, sizeof(buffer), "\r\nThe number of meals for your pet a day (MAX 5 times a day): ");
				USART_write(USART1, (uint8_t *)buffer, strlen(buffer));

				while(1){
					if(NofMeal != 0 && btData == '\r') break;
					else if (btData == 48 || btData == 49 || btData == 50 || btData == 51 || btData == 52 || btData == 53 || btData == 54 || btData == 55 || btData == 56 || btData == 57) NofMeal = btData;
				}
				btData = 0;
				
				switch(NofMeal){
					case 49: NofMeal = 1; break;
					case 50: NofMeal = 2; break;
					case 51: NofMeal = 3; break;
					case 52: NofMeal = 4; break;
					case 53: NofMeal = 5; break;
					default: snprintf(buffer, sizeof(buffer), "Invalid Number. Please choose 1 to 5.\r\n"); USART_write(USART1, (uint8_t *)buffer, strlen(buffer)); continue;
				}
				AllocateMealTimes(NofMeal);
				
				for(int i = 0; i < NofMeal; i++){
					// printf("Please set the %s meal time (hour minute second)", Ecnt[i]);
					snprintf(buffer, sizeof(buffer), "\r\nPlease set the %s meal time (hour minute second): ", Ecnt[i]);
					USART_write(USART1, (uint8_t *)buffer, strlen(buffer));
					for(int j = 0; j < 3; j++){
						for(int i = 0; i < 2; i++){
							while(1){
								if(i == 0 && timeVal[0] != 0) break;
								else if(i == 1 && timeVal[1] != 0 && btData == ' ') break;
								else if(i == 1 && timeVal[1] != 0 && btData == '\r') break;
								else if(btData == 48 || btData == 49 || btData == 50 || btData == 51 || btData == 52 || btData == 53 || btData == 54 || btData == 55 || btData == 56 || btData == 57) timeVal[i] = btData;
							}
							btData = 0;
							switch(timeVal[i]){
								case 48: timeVal[i] = 0; break;
								case 49: timeVal[i] = 1; break;
								case 50: timeVal[i] = 2; break;
								case 51: timeVal[i] = 3; break;
								case 52: timeVal[i] = 4; break;
								case 53: timeVal[i] = 5; break;
								case 54: timeVal[i] = 6; break;
								case 55: timeVal[i] = 7; break;
								case 56: timeVal[i] = 8; break;
								case 57: timeVal[i] = 9; break;
								default: snprintf(buffer, sizeof(buffer), "Invalid Number. Please choose 0 to 9.\r\n"); USART_write(USART1, (uint8_t *)buffer, strlen(buffer)); continue;
							}
						}
						FOOD_Time[i][j] = timeVal[0] * 10 + timeVal[1];
						timeVal[0] = 0;
						timeVal[1] = 0;
					}
				}
				for(int i = 0; i < NofMeal; i++){
					// printf("FOOD_Time[%d]: %02d, %02d, %02d\r\n", i, FOOD_Time[i][0], FOOD_Time[i][1], FOOD_Time[i][2]);
					if(i == 0) USART_write(USART1, (uint8_t *)"\r\n", 2);
					snprintf(buffer, sizeof(buffer), "FOOD_Time[%d]: %02d, %02d, %02d\r\n", i, FOOD_Time[i][0], FOOD_Time[i][1], FOOD_Time[i][2]);
					USART_write(USART1, (uint8_t *)buffer, strlen(buffer));
				}
				// printf("The time has been set\r\n");
				snprintf(buffer, sizeof(buffer), "The time has been set\r\n");
				USART_write(USART1, (uint8_t *)buffer, strlen(buffer));
			}
			
			// Manual mode
			else if(ControlMode == 2){
				// printf("Do you want to feed your pet? (Y or N) ");
				snprintf(buffer, sizeof(buffer), "\r\nDo you want to feed your pet? (Y or N) ");
				USART_write(USART1, (uint8_t *)buffer, strlen(buffer));
				while(1){
					if (Want_To_Feed != 0 && btData == '\r') break;
					else if (btData == 89 || btData == 121 || btData == 78 || btData == 110) Want_To_Feed = btData;
				}
				btData = 0;
				
				if(Want_To_Feed == 89 || Want_To_Feed == 121){
					snprintf(buffer, sizeof(buffer), "\r\nSucceed\r\n");
					USART_write(USART1, (uint8_t *)buffer, strlen(buffer));
					S_manual = 1;
				} else if(Want_To_Feed == 78 || Want_To_Feed == 110){
					snprintf(buffer, sizeof(buffer), "\r\nCancelled\r\n");
					USART_write(USART1, (uint8_t *)buffer, strlen(buffer));
				}
				Want_To_Feed = 0;
			}
			
			// Setting mode
			else if(ControlMode == 3){
				RTC_GetDateTime();
				snprintf(buffer, sizeof(buffer), "\r\nThe current time is %02d-%02d-%02d %02d:%02d:%02d", current_time[0][0], current_time[0][1], current_time[0][2], current_time[1][0], current_time[1][1], current_time[1][2]);
				USART_write(USART1, (uint8_t *)buffer, strlen(buffer));
				snprintf(buffer, sizeof(buffer), "\r\nDo you want to change? (Y or N) ");
				USART_write(USART1, (uint8_t *)buffer, strlen(buffer));
				while(1){
					if (change_time != 0 && btData == '\r') break;
					else if (btData == 89 || btData == 121 || btData == 78 || btData == 110) change_time = btData;
				}
				btData = 0;
				
				if(change_time == 89 || change_time == 121){
					setting_int = 1;
					snprintf(buffer, sizeof(buffer), "\r\nTime setting");
					USART_write(USART1, (uint8_t *)buffer, strlen(buffer));
				} else if(change_time == 78 || change_time == 110){
					snprintf(buffer, sizeof(buffer), "\r\nCancelled\r\n");
					USART_write(USART1, (uint8_t *)buffer, strlen(buffer));
				}
				
				if(setting_int == 1){
					setting_int = 0;
					for(int i = 0; i < 2; i++){
						if(i == 0){
							snprintf(buffer, sizeof(buffer), "\r\nPlease set the date (year month day): ");
							USART_write(USART1, (uint8_t *)buffer, strlen(buffer));
						} else if(i == 1){
							snprintf(buffer, sizeof(buffer), "\r\nPlease set the time (hour minute second): ");
							USART_write(USART1, (uint8_t *)buffer, strlen(buffer));
						}
						for(int j = 0; j < 3; j++){
							for(int i = 0; i < 2; i++){
								while(1){
									if(i == 0 && timeVal[0] != 0) break;
									else if(i == 1 && timeVal[1] != 0 && btData == ' ') break;
									else if(i == 1 && timeVal[1] != 0 && btData == '\r') break;
									else if(btData == 48 || btData == 49 || btData == 50 || btData == 51 || btData == 52 || btData == 53 || btData == 54 || btData == 55 || btData == 56 || btData == 57) timeVal[i] = btData;
								}
								btData = 0;
								switch(timeVal[i]){
									case 48: timeVal[i] = 0; break;
									case 49: timeVal[i] = 1; break;
									case 50: timeVal[i] = 2; break;
									case 51: timeVal[i] = 3; break;
									case 52: timeVal[i] = 4; break;
									case 53: timeVal[i] = 5; break;
									case 54: timeVal[i] = 6; break;
									case 55: timeVal[i] = 7; break;
									case 56: timeVal[i] = 8; break;
									case 57: timeVal[i] = 9; break;
									default: snprintf(buffer, sizeof(buffer), "\r\nInvalid Number. Please choose 0 to 9.\r\n"); USART_write(USART1, (uint8_t *)buffer, strlen(buffer)); continue;
								}
							}
							set_time[i][j] = timeVal[0] * 10 + timeVal[1];
							timeVal[0] = 0;
							timeVal[1] = 0;
						}
					}
					RTC_SetDate(set_time[0][0], set_time[0][1], set_time[0][2], set_time[1][0], set_time[1][1], set_time[1][2]);
					snprintf(buffer, sizeof(buffer), "\r\nTime has been setted");
					USART_write(USART1, (uint8_t *)buffer, strlen(buffer));
					RTC_GetDateTime();
					snprintf(buffer, sizeof(buffer), "\r\nThe current time is %02d-%02d-%02d %02d:%02d:%02d\r\n", current_time[0][0], current_time[0][1], current_time[0][2], current_time[1][0], current_time[1][1], current_time[1][2]);
					USART_write(USART1, (uint8_t *)buffer, strlen(buffer));
					for(int i = 0; i < 2; i++)
						for(int j = 0; j < 3; j++)
							set_time[i][j] = 0;
				}
			}
			
			// Demo mode
			else if(ControlMode == 4){
				snprintf(buffer, sizeof(buffer), "\r\nSecond? ");
				USART_write(USART1, (uint8_t *)buffer, strlen(buffer));
				for(int i = 0; i < 2; i++){
					while(1){
						if(i == 0 && timeVal[0] != 0) break;
						else if(i == 1 && timeVal[1] != 0 && btData == ' ') break;
						else if(i == 1 && timeVal[1] != 0 && btData == '\r') break;
						else if(btData == 48 || btData == 49 || btData == 50 || btData == 51 || btData == 52 || btData == 53 || btData == 54 || btData == 55 || btData == 56 || btData == 57) timeVal[i] = btData;
					}
					btData = 0;
					switch(timeVal[i]){
						case 48: timeVal[i] = 0; break;
						case 49: timeVal[i] = 1; break;
						case 50: timeVal[i] = 2; break;
						case 51: timeVal[i] = 3; break;
						case 52: timeVal[i] = 4; break;
						case 53: timeVal[i] = 5; break;
						case 54: timeVal[i] = 6; break;
						case 55: timeVal[i] = 7; break;
						case 56: timeVal[i] = 8; break;
						case 57: timeVal[i] = 9; break;
						default: snprintf(buffer, sizeof(buffer), "\r\nInvalid Number. Please choose 0 to 9.\r\n"); USART_write(USART1, (uint8_t *)buffer, strlen(buffer)); continue;
					}
				}
				demo_set = timeVal[0] * 10 + timeVal[1];
				timeVal[0] = 0;
				timeVal[1] = 0;
				
				demo_time[2] = current_time[1][2] + demo_set;
				if(demo_time[2] >= 60) {
					demo_time[2] = demo_time[2] % 60;
					demo_time[1] = current_time[1][1] + 1;
				} else { 
					demo_time[1] = current_time[1][1];
				}
				if(demo_time[1] >= 60) {
					demo_time[1] = demo_time[1] % 60;
					demo_time[0] = current_time[1][0] + 1;
				} else { 
					demo_time[0] = current_time[1][0];
				}
				
				snprintf(buffer, sizeof(buffer), "\r\nDemo gonna start after %02d second\r\n", demo_set);
				USART_write(USART1, (uint8_t *)buffer, strlen(buffer));
			}
		}
		FreeMealTimes();
}

void TIM3_IRQHandler(void){
	if (is_UIF(TIM3)){
		count++;
		if(count >= 2){
			pet_feeding();
			if(GPIO_read(Interrupt_PIN)) {
				printf("Interrupt_PIN: HIGHHHHHH\r\n");
				Int_count++;
			}
			if(GPIO_read(Manual_PIN)){
				printf("Manual_PIN: HIGHHHHHH\r\n");
				manual_count++;
			}
			if(!GPIO_read(Interrupt_PIN)) printf("Interrupt_PIN: LOWWWWWWW\r\n");
			if(!GPIO_read(Manual_PIN)) printf("Manual_PIN: LOWWWWWWW\r\n");
			if(Int_count > 60){
				GPIO_write(Interrupt_PIN, LOW);
				Int_count = 0;
				if(demo_set != 0) demo_set = 0;
			}
			if(manual_count > 30){
				GPIO_write(Manual_PIN, LOW);
				manual_count = 0;
				S_manual = 0;
			}
			count = 0;
		}
		clear_UIF(TIM3);
	}
}

void USART1_IRQHandler(){         //USART1 INT 
	if(is_USART_RXNE(USART1)){
		btData = USART_read(USART1);
		USART_write(USART1, &btData, 1);
	}
}

void AllocateMealTimes(int NoM) {
    if (NoM >= 1 && NoM <= 5) {
        FOOD_Time = (char (*)[3])malloc(NoM * 3 * sizeof(char));
        if (FOOD_Time == NULL) {
            printf("Memory allocate fail\r\n");
            return;
        }

        for (int i = 0; i < NoM; i++) {
            FOOD_Time[i][0] = 0;
            FOOD_Time[i][1] = 0;
            FOOD_Time[i][2] = 0;
        }
    } else {
        printf("Invalid number\r\n");
        return;
    }
}

void FreeMealTimes() {
    if (FOOD_Time != NULL) {
        free(FOOD_Time);
        FOOD_Time = NULL;
    }
}

void pet_feeding(void){
	RTC_GetDateTime();
	for(int i = 0; i < NofMeal; i++){
		if(current_time[1][0] == FOOD_Time[i][0] && current_time[1][1] == FOOD_Time[i][1] && current_time[1][2] == FOOD_Time[i][2]){
			GPIO_write(Interrupt_PIN, HIGH);
		}
	}
	if(S_manual == 1)
		GPIO_write(Manual_PIN, HIGH);
	if(demo_set != 0){
		if(current_time[1][0] == demo_time[0] && current_time[1][1] == demo_time[1] && current_time[1][2] == demo_time[2]){
			GPIO_write(Interrupt_PIN, HIGH);
		}
	}
}

uint8_t CheckMode(uint8_t num){
	if (num == 65 || num == 97 || num == 77 || num == 109){
		return 1;
	} else return 0;
}

uint8_t CheckNumber(uint8_t num){
	if (num == 48 || num == 49 || num == 50 || num == 51 || num == 52 || num == 53 || num == 54 || num == 55 || num == 56 || num == 57){
		return 1;
	} else return 0;
}