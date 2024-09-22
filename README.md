# LAB: GPIO Digital InOut 7-segment

## LAB: GPIO Digital InOut 7-segment

**Date:** 2024-09-1

**Author/Partner:** Eunchong Kim

**Github:** repository link

**Demo Video:** Youtube link

**PDF version:**

## Introduction

In this lab, you are required to create a simple program to control a 7-segment display to show a decimal number (0~9) that increases by pressing a push-button.

You must submit

- LAB Report (*.pdf)
- Zip source files(LAB***.c, ecRCC2.h, ecGPIO2.h etc...).
  - Only the source files. Do not submit project files

#### Requirement

**Hardware**

- MCU
  - NUCLEO-F411RE
- Actuator/Sensor/Others:
  - 7-segment display(5101ASR)
  - Array resistor (330 ohm)
  - decoder chip(74LS47)
  - breadboard

**Software**

- Keil uVision, CMSIS, EC_HAL library

## Exercise

Fill in the table



| **Port/Pin**   | **Description**              | **Register setting**                                         |
| -------------- | ---------------------------- | ------------------------------------------------------------ |
| Port A Pin 5   | Clear Pin5 mode              | GPIOA->MODER &=~(3<<(Pin5 *2));                              |
| Port A Pin 5   | Set Pin5 mode = Output       | GPIOA->MODER \|=1<<(Pin5 *2);                                |
| Port A Pin 6   | Clear Pin6 mode              | GPIOA->MODER &=~(3<<(Pin6 *2));                              |
| Port A Pin 6   | Set Pin6 mode = Output       | GPIOA->MODER \|=1<<(Pin6 *2);                                |
| Port A Pin Y   | Clear PinY mode              | GPIOA->MODER &=~(3<<(PinY *2));                              |
| Port A Pin Y   | Set PinY mode = Output       | GPIOA->MODER \|=1<<(PinY *2);                                |
| Port A Pin 5~9 | Clear Pin5~9 mode            | GPIOA->MODER &=~(3<<(Pin5~9 *2));                            |
|                | Set Pin5~9 mode = Output     | GPIOA->MODER \|=1<<(Pin5~9 *2);                              |
| Port X Pin Y   | Clear Pin Y mode             | GPIOX->MODER &=~(3<<(PinY *2));                              |
|                | Set Pin Y mode = Output      | GPIOX->MODER \|=1<<(PinY *2);                                |
| Port A Pin5    | Set Pin5 otype=push-pull     | GPIOA->OTYPER &=~(1<<Pin5);                                  |
| Port A PinY    | Set PinY otype=push-pull     | GPIOA-> OTYPER &=~(1<<PinY);                                 |
| Port A Pin5    | Set Pin5 ospeed=Fast         | GPIOA->OSPEEDR &=~(3<<(Pin5 *2));<br />GPIOA->OSPEEDR \|=2<<(Pin5 *2); |
| Port A PinY    | Set PinY ospeed=Fast         | GPIOA-> OSPEEDR &=~(3<<(PinY *2));<br />GPIOA-> OSPEEDR \|=2<<(PinY *2); |
| Port A Pin 5   | Set Pin5 PUPD=no pullup/down | GPIOA->PUPDR &=~(3<<(Pin5 *2));                              |
| Port A Pin Y   | Set PinY PUPD=no pullup/down | GPIOA-> PUPDR &=~(3<<(PinY *2));                             |

------

## Problem 0: Connection of 7-Segment Display and Decoder

### Procedure

Review 7-segment Decoder and Display from Digital Logic lecture.

- Read here: [7-segment tutorial](https://ykkim.gitbook.io/ec/ec-course/tutorial/tutorial-7segment-display)
- Read here: [How to connect 7-segment decoder to MCU](https://ykkim.gitbook.io/ec/ec-course/hardware/experiment-hardware/electronic-chips#7-segment-and-decoder)

#### 1. 7-segment display connection

First, connect the common anode 7-segment display with the given array resistors.

Then, apply VCC and GND to the 7-segment display.

Check that all LEDs of 7-segment work properly

- Give 'H' signal to each 7-segment pin of 'a'~'g' . Observe if that LED is turned ON or OFF
- Example: Connect VCC to all 'a'~'g' pins

#### 2. BCD 7-segment decoder connection

The popular BCD 7-segment decoder chip is **74LS47.** With the BCD chip, you need only 4 DOUT pins of MCU to display from 0 to 9.

Connect the decoder chip (**74LS47**) on the bread board.

![img](https://ykkim.gitbook.io/~gitbook/image?url=https%3A%2F%2F3453146497-files.gitbook.io%2F%7E%2Ffiles%2Fv0%2Fb%2Fgitbook-x-prod.appspot.com%2Fo%2Fspaces%252F-MgmrEstOHxu62gXxq1t%252Fuploads%252Fgit-blob-23bb2e14810893feeb805c7f7d93de5d472f83cb%252Fimage.png%3Falt%3Dmedia&width=768&dpr=4&quality=100&sign=892b912d&sv=1)

Then, Check that the decoder chip works properly

- Supply a combination of VCC/GND to the pins of 'A'~'D' of the decoder.
- Check if the 7-segment LED display shows the correct number

#### Connection Diagram

Circuit diagram

![1](https://github.com/user-attachments/assets/bd57357d-a65b-4a5d-8078-292e543748c6)

![img](https://ykkim.gitbook.io/~gitbook/image?url=https%3A%2F%2Fuser-images.githubusercontent.com%2F38373000%2F192134563-72f68b29-4127-42ac-b064-2eda95a9a52a.png&width=768&dpr=4&quality=100&sign=60f2bbed&sv=1)

image

#### Discussion

1. Draw the truth table for the BCD 7-segment decoder with the 4-bit input.

> Answer discussion questions

```
** YOUR Truth-table  goes here**
```

1. What are the common cathode and common anode of 7-segment display?

> Answer discussion questions

1. Does the LED of a 7-segment display (common anode) pin turn ON when 'HIGH' is given to the LED pin from the MCU?

> Answer discussion questions

------

## Problem 1: Display a Number with Button Press

### Procedure

1. Create a new project under the directory `\repos\EC\LAB\LAB_GPIO_7segment`

- The project name is “**LAB_GPIO_7segment”.**

- Create a new source file named as “**LAB_GPIO_7segment.c”**

- Refer to the [sample code](https://github.com/ykkimhgu/EC-student/tree/main/tutorial/tutorial-student)

  > You MUST write your name on the source file inside the comment section.

1. Include your updated library in `\repos\EC\lib\` to your project.

- **ecGPIO.h, ecGPIO.c**
- **ecRCC.h, ecRCC.c**

1. Declare and Define the following functions in your library

```
void sevensegment_display_init(PinNames_t pinNameA, PinNames_t pinNameB, PinNames_t pinNameC, PinNames_t pinNameD); 
void sevensegment_display(uint8_t  num);
```

- num: 0 to 9 only (unsigned)

1. Configure and connect the MCU to the circuit
2. First, check that every number, 0 to 9, can be displayed properly
3. Then, create a code that increase the displayed number from 0 to 9 with each button press.
   - After the number '9', it should start from '0' again.

### Configuration

Configure the MCU



| Digital In for Button (B1) | Digital Out for 7-Segment                     |
| :------------------------- | :-------------------------------------------- |
| Digital In                 | Digital Out                                   |
| PC13                       | PA7, PB6, PC7, PA9                            |
| PULL-UP                    | Push-Pull, No Pull-up-Pull-down, Medium Speed |

#### Code

Your code goes here: [ADD Code LINK such as github](https://github.com/ykkimhgu/EC-student/)


```
#include "stm32f4xx.h"
#include "ecGPIO2.h"
#include "ecRCC2.h"

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
```

### Results

Experiment images and results

> Show experiment images /results

Add [demo video link](https://github.com/ykkimhgu/course-doc/blob/master/course/lab/link/README.md)

### Discussion

Analyze the result and explain any other necessary discussion.

------

## Problem 2: Program BCD-7-segment decoder

Instead of using the decoder chip, we are going to make the 7-segment decoder with the MCU programming.

> Do not use the 7-segment decoder for this problem

### Procedure

1. Use the same project and source file.
2. Include your updated library in `\repos\EC\lib\` to your project.

- **ecGPIO.h, ecGPIO.c**
- **ecRCC.h, ecRCC.c**

1. Declare and Define the following functions in your library

```
void sevensegment_decoder_init(void); 
void sevensegment_decoder(uint8_t  num);
```

- num: 0 to 9 only (unsigned)

1. Configure and connect the MCU to the circuit
2. First, check that every number, 0 to 9, can be displayed properly
3. Then, create a code that increases the displayed number from 0 to 9 with each button press.
   - After the number '9', it should start from '0' again.

### Configuration

Configure the MCU



| Digital In for Button (B1) | Digital Out for 7-Segment                                    |
| :------------------------- | :----------------------------------------------------------- |
| Digital In                 | Digital Out                                                  |
| PC13                       | PA5, PA6, PA7, PB6, PC7, PA9, PA8, PB10 ('a'~'h', respectively) |
| PULL-UP                    | Push-Pull, No Pull-up-Pull-down, Medium Speed                |

### Code

Your code goes here: [ADD Code LINK such as github](https://github.com/ykkimhgu/EC-student/)

> Explain your source code with the necessary comments.

```
#include "stm32f4xx.h"
#include "ecGPIO2.h"
#include "ecRCC2.h"

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
```

### Connection Diagram

Circuit diagram

> You need to include the circuit diagram

![img](https://ykkim.gitbook.io/~gitbook/image?url=https%3A%2F%2Fuser-images.githubusercontent.com%2F38373000%2F192134563-72f68b29-4127-42ac-b064-2eda95a9a52a.png&width=768&dpr=4&quality=100&sign=60f2bbed&sv=1)

​								image

### Results

Experiment images and results

> Show experiment images /results

Add [demo video link](https://github.com/ykkimhgu/course-doc/blob/master/course/lab/link/README.md)

### Discussion

Analyze the result and explain any other necessary discussion.

------

## Reference

Complete list of all references used (github, blog, paper, etc)

```

```

------

## Troubleshooting

(Option) You can write Troubleshooting section
