//21900526 seung wan lee
//         eun chong kim
#include "stm32f4xx.h"
#include "ecSTM32F4v2.h"
#include "ecPWM2.h"
#include "ecICAP2.h"
#include "ecGPIO2.h"
#include "stdio.h"

#define TRIG PA_6
#define ECHO PB_6
PinName_t PWM_PIN1 = PA_0;
#define DIR_PIN1 PC_2
#define SCK_PIN PB_9
#define DT_PIN PB_8
#define OTHER_MCU_PIN PB_5 // Pin for receiving signal from another MCU
#define MANUAL_PIN PB_4 // manual pin for motor control
float distance = 0;
float weight = 0;
uint32_t time1 = 0, time2 = 0, timeInterval = 0, ovf_cnt = 0;
float predistance = 0;
long value = 0;
float scale_factor = 1000.0;
float offset = 0.0;

float weight_threshold = 0.0005;  // [kg]= 5g for setting
float distance_threshold = 10.0;  // [cm]

void setup(void);
void HX711_check_weight(void);
void HX711_calibrate(void);
void HX711_tare(void);
void supply_food(void);
void stop_motor(void);
void TIM4_IRQHandler(void);
void motor_run_one_second(void);
int main() {
    setup();

    while (1) {
			
			
			
				int new_pin_signal = GPIO_read(MANUAL_PIN);
        if (new_pin_signal == 1) {
						supply_food();
        } else {
						stop_motor();
				}
        // Read the state of OTHER_MCU_PIN
        int other_mcu_signal = GPIO_read(OTHER_MCU_PIN);

        if (other_mcu_signal == 1) { // If the pin is HIGH
            HX711_check_weight(); // Check weight
            printf("Distance: %.2f cm, Weight: %.6f kg\r\n", distance, weight);// the tared value printing

            // Check if conditions are met
            if (weight >= 0 && weight < weight_threshold && 
                distance > 0 && distance < distance_threshold) {
                supply_food();
            } else {
                stop_motor();
            }
        } else {
						stop_motor();
            printf("It's not time for a food ...\r\n");
        }

        delay_ms(100);
    }
}

void setup(void) {
    RCC_PLL_init();
    SysTick_init();
    UART2_init();

    printf("System Initialization Complete.\r\n");

    PWM_init(TRIG);
    PWM_period_us(TRIG, 50000);
    PWM_pulsewidth_us(TRIG, 10);

    ICAP_init(ECHO);
    ICAP_counter_us(ECHO, 1);
    ICAP_setup(ECHO, 1, IC_RISE);
    ICAP_setup(ECHO, 2, IC_FALL);

    PWM_init(PWM_PIN1);
    PWM_period_ms(PWM_PIN1, 20);
    GPIO_init(DIR_PIN1, OUTPUT);
    GPIO_otype(DIR_PIN1, 0);

    GPIO_init(SCK_PIN, OUTPUT);
    GPIO_init(DT_PIN, INPUT);
    HX711_tare();
		// Initialize pin for receiving signal from another MCU
    GPIO_init(OTHER_MCU_PIN, INPUT);
    GPIO_pupd(OTHER_MCU_PIN, 2);  // Enable internal pull-down resistor
		GPIO_init(MANUAL_PIN, INPUT);
		GPIO_pupd(MANUAL_PIN, 2); // Enable internal pull-down resistor
    printf("Setup Complete.\r\n");	
}

void TIM4_IRQHandler(void) {
    if (is_UIF(TIM4)) {
        ovf_cnt++;
        clear_UIF(TIM4);
    }

    if (is_CCIF(TIM4, 1)) {
        time1 = ICAP_capture(TIM4, 1);
        
        clear_CCIF(TIM4, 1);
    } else if (is_CCIF(TIM4, 2)) {
        time2 = ICAP_capture(TIM4, 2);
            timeInterval = (time2 - time1) + (ovf_cnt * (TIM4->ARR + 1));
        predistance = distance;
        distance = (timeInterval * 0.034) / 2.0;
        if (fabs(distance) > 1000.0)
            distance = predistance;
        ovf_cnt = 0;
        clear_CCIF(TIM4, 2);
    }
}

void HX711_check_weight(void) {
    value = 0;
    GPIO_write(SCK_PIN, LOW);

    int timeout = 100000;
    while (GPIO_read(DT_PIN)) {
        if (--timeout <= 0) {
            printf("HX711 Timeout.\r\n");
            weight = -1;
            return;
        }
    }

    for (int i = 0; i < 24; i++) {
        GPIO_write(SCK_PIN, HIGH);
        value = (value << 1) | GPIO_read(DT_PIN);
        GPIO_write(SCK_PIN, LOW);
    }

    GPIO_write(SCK_PIN, HIGH);
    GPIO_write(SCK_PIN, LOW);

    if (value & 0x800000) {
        value |= 0xFF000000;
    }

    weight = ((float)value - offset) / scale_factor;
    weight = (weight < 0) ? 0.0 : weight;
}

void supply_food(void) {
    GPIO_write(DIR_PIN1, 1);
    PWM_duty(PWM_PIN1, 0.65);
    printf("Food dispensing...\r\n");
}

void stop_motor(void) {
    GPIO_write(DIR_PIN1, 0);
    PWM_duty(PWM_PIN1, 0.0);
    printf("Motor stopped.\r\n");
}

void HX711_tare(void) {
    printf("Taring... Ensure no weight on the sensor.\r\n");
    delay_ms(3000);

    long raw_value = 0;
    for (int i = 0; i < 10; i++) {
        HX711_check_weight();
        raw_value += value;
        delay_ms(100);
    }
    raw_value /= 10;

    offset = (float)raw_value;
    printf("Tare Complete. Offset: %.2f\r\n", offset);
      scale_factor = (4294967 - offset) / 1;
}
void motor_run_one_second(void) {
    GPIO_write(DIR_PIN1, 1);
    PWM_duty(PWM_PIN1, 0.0); // Motor on
    printf("Motor running for 1 second...\r\n");
    
    
}