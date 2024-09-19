# LAB: GPIO Digital InOut

## LAB: GPIO Digital InOut

**Date:** 2024-09-15

**Author/Partner:** Eunchong Kim

**Github:** https://github.com/jesse-kd/EC.git

**Demo Video:** [YouTube Video](https://youtu.be/ctv22elTqOc)

## Introduction

In this lab, use a simple program that toggles multiple LEDs with a push-button input. Create HAL drivers for GPIO digital in and out control and use the library.

### Requirement

#### Hardware

- MCU

  - NUCLEO-F411RE

- Actuator/Sensor/Others:

  - LEDs x 3

  - Resistor 330 ohm x 3, breadboard

#### Software

- Keil uVision, CMSIS, EC_HAL library

## Problem 1: Create EC_HAL library

### Procedure

Create the library directory `\repos\EC\lib\`.

Save your header library files in this directory. [See here for detail.](https://ykkim.gitbook.io/ec/uvision/adding-my-api-header-in-uvision)

> DO NOT make duplicates of library files under each project folders

Create your own library for Digital_In and Out : `ecGPIO2.h, ecGPIO2.c`

- [Download library files from here](https://github.com/ykkimhgu/EC-student/tree/main/include/lib-student)

- Use the provided `ecRCC2.h` and `ecRCC2.c`

- Modify `ecGPIO2.c`, `ecGPIO2.h`

- **ecRCC2.h** 

```
void RCC_HSI_init(void);
void RCC_PLL_init(void);
void RCC_GPIOA_enable(void);
void RCC_GPIOB_enable(void);
void RCC_GPIOC_enable(void);
void RCC_GPIOD_enable(void);
void RCC_GPIOH_enable(void);
```

- **ecGPIO2.h**

```
void GPIO_init(PinName_t pinName, int mode);
void GPIO_write(PinName_t pinName, int Output);
int  GPIO_read(PinName_t pinName);
void GPIO_mode(PinName_t pinName, int mode);
void GPIO_ospeed(PinName_t pinName, int speed);
void GPIO_otype(PinName_t pinName, int type);
void GPIO_pupd(PinName_t pinName, int pupd);
```

-  **ecRCC2.c**

```
void RCC_GPIOA_enable()
{
	// RCC Peripheral Clock Enable Register 
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
}

void RCC_GPIOB_enable()
{
	// RCC Peripheral Clock Enable Register 
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
}

void RCC_GPIOC_enable()
{
	// RCC Peripheral Clock Enable Register 
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
}

void RCC_GPIOD_enable()
{
	// RCC Peripheral Clock Enable Register 
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;
}

void RCC_GPIOH_enable()
{
	// RCC Peripheral Clock Enable Register 
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOHEN;
}
```

-  **ecGPIO2.c**

```
void GPIO_init(PinName_t pinName, uint32_t mode){     
	GPIO_TypeDef * Port;
	unsigned int pin;
	ecPinmap(pinName, &Port, &pin);
	
	// mode  : Input(0), Output(1), AlterFunc(2), Analog(3)   
	if (Port == GPIOA)
		RCC_GPIOA_enable();
	if (Port == GPIOC)
		RCC_GPIOC_enable();
	
	//[TO-DO] YOUR CODE GOES HERE
	if (Port == GPIOB)
		RCC_GPIOB_enable();
	if (Port == GPIOD)
		RCC_GPIOD_enable();
	if (Port == GPIOH)
		RCC_GPIOH_enable();

	// You can also make a more general function of
	// void RCC_GPIO_enable(GPIO_TypeDef *Port); 

	GPIO_mode(pinName, mode);
}

void GPIO_write(PinName_t pinName, int Output){
	GPIO_TypeDef * Port;
   unsigned int pin;
   ecPinmap(pinName,&Port,&pin);
   Port->ODR &= ~(1UL<<pin);     
   Port->ODR |= Output<<pin;
}

// GPIO Mode          : Input(00), Output(01), AlterFunc(10), Analog(11)
void GPIO_mode(PinName_t pinName, uint32_t mode){
   GPIO_TypeDef * Port;
   unsigned int pin;
   ecPinmap(pinName,&Port,&pin);
   Port->MODER &= ~(3UL<<(2*pin));
   Port->MODER |= mode<<(2*pin);
}


// GPIO Speed          : Low speed (00), Medium speed (01), Fast speed (10), High speed (11)
void GPIO_ospeed(PinName_t pinName, int speed){
	//[TO-DO] YOUR CODE GOES HERE
   GPIO_TypeDef * Port;
   unsigned int pin;
   ecPinmap(pinName,&Port,&pin);	
	//[TO-DO] YOUR CODE GOES HERE
	Port->OSPEEDR &= ~(3UL<<(2*pin));     
   Port->OSPEEDR |= speed<<(2*pin);  
}

// GPIO Output Type: Output push-pull (0, reset), Output open drain (1)
void GPIO_otype(PinName_t pinName, int type){
   //[TO-DO] YOUR CODE GOES HERE
   GPIO_TypeDef * Port;
   unsigned int pin;
   ecPinmap(pinName,&Port,&pin);
	//[TO-DO] YOUR CODE GOES HERE
   Port->OTYPER &= ~(1UL<<pin);     
   Port->OTYPER |= type<<pin;
}

// GPIO Push-Pull    : No pull-up, pull-down (00), Pull-up (01), Pull-down (10), Reserved (11)
void GPIO_pupd(PinName_t pinName, int pupd){
   //[TO-DO] YOUR CODE GOES HERE
   GPIO_TypeDef * Port;
   unsigned int pin;
   ecPinmap(pinName,&Port,&pin);
	//[TO-DO] YOUR CODE GOES HERE
	Port->PUPDR &= ~(3UL<<(2*pin));     
   Port->PUPDR |= pupd<<(2*pin);
}

int GPIO_read(PinName_t pinName){
   //[TO-DO] YOUR CODE GOES HERE
   GPIO_TypeDef * Port;
   unsigned int pin;
   ecPinmap(pinName,&Port,&pin);
	//[TO-DO] YOUR CODE GOES HERE
	unsigned int btVal;
	btVal = (Port->IDR) & (1UL<<pin);
	if(btVal == 0)
		Port->IDR |= 1UL<<pin;
	else
		Port->IDR &= ~(1UL<<pin);
	return btVal;
}
```

## Problem 2: Toggle LED with Button

### Procedure

1. Create a new project under the directory `\repos\EC\LAB\`

- The project name is “**LAB_GPIO_DIO_LED”.**

- Name the source file as “**LAB_GPIO_DIO_LED.c”**

- Use the [example code provided here](https://github.com/ykkimhgu/EC-student/blob/main/lab/lab-student/LAB_GPIO_DIO_LED_student.c).

\2. Include your library **ecGPIO2.h, ecGPIO2.c** in `\repos\EC\lib\`.

> You MUST write your name in the top of the source file, inside the comment section.

\3. Toggle the LED by pushing the button.

- Push button (LED ON), Push Button (LED OFF) and repeat

### Configuration

| Button (B1)   | LED                               |
| :------------ | :-------------------------------- |
| Digital In    | Digital Out                       |
| GPIOC, Pin 13 | GPIOA, Pin 5                      |
| PULL-UP       | Open-Drain, Pull-up, Medium Speed |

### Code

```
PinName_t LED_pin = PA_5;
PinName_t button_pin = PC_13;

void setup(void);
void delay_ms(int ms);
	
int main(void) { 
	// Initialiization --------------------------------------------------------
	setup();
	int LED_State = 0;
	// Inifinite Loop ----------------------------------------------------------
	while(1){
			
		if(GPIO_read(button_pin) == 0){		// Check if the button is pressed
			LED_State ^= 1;					// Write the new state to the LED
			GPIO_write(LED_pin, LED_State);
			delay_ms(120);					// Debouncing delay
		} else {							// If button is not pressed, keep the LED in its current state
			GPIO_write(LED_pin, LED_State);
			delay_ms(120);
		}
	}
}
```

## Problem 3: Toggle LED with Button

### Procedure

1. Create a new project under the directory `\repos\EC\LAB\`

- The project name is “**LAB_GPIO_DIO_multiLED”.**

- Name the source file as “**LAB_GPIO_DIO_multiLED.c”**

> You MUST write your name in the top of the source file, inside the comment section.

1. Include your library **ecGPIO2.h, ecGPIO2.c** in `\repos\lib\`.

2. Connect 4 LEDs externally with necessary load resistors.

- As Button B1 is Pressed, light one LED at a time, in sequence.

- Example: LED0--> LED1--> …LED3--> …LED0….

### Configuration

| Button        | LED                              |
| :------------ | :------------------------------- |
| Digital In    | Digital Out                      |
| GPIOC, Pin 13 | PA5, PA6, PA7, PB6               |
| PULL-UP       | Push-Pull, Pull-up, Medium Speed |

### Code

```
PinName_t LED_pin0 = PA_5;
PinName_t LED_pin1 = PA_6;
PinName_t LED_pin2 = PA_7;
PinName_t LED_pin3 = PB_6;
PinName_t button_pin = PC_13;

void setup(void);
void delay(int ms);
	
int main(void) { 
	// Initialiization --------------------------------------------------------
	setup();
	int LED_State = 0;
	// Inifinite Loop ----------------------------------------------------------
	while(1){
		
		if(GPIO_read(button_pin) == 0){		// Check if the button is pressed
			LED_State++;					// Increment LED state
			if(LED_State > 4) LED_State = 1;// Reset state if it exceeds 4
			delay(200);						// Debouncing delay
		} else {
			// Turn off all LEDs initially
			GPIO_write(LED_pin0, 0);
			GPIO_write(LED_pin1, 0);
			GPIO_write(LED_pin2, 0);
			GPIO_write(LED_pin3, 0);
			// Turn on the appropriate LED based on the current state
			if(LED_State == 1)
				GPIO_write(LED_pin0, 1);
			else if(LED_State == 2)
				GPIO_write(LED_pin1, 1);
			else if(LED_State == 3)
				GPIO_write(LED_pin2, 1);
			else if(LED_State == 4)
				GPIO_write(LED_pin3, 1);
		}
	}
}
```

### Results

![image-20240919221201331](C:\Users\const\AppData\Roaming\Typora\typora-user-images\image-20240919221201331.png)
[YouTube Video](https://youtu.be/ctv22elTqOc)



### Discussion

1. Find out a typical solution for software debouncing and hardware debouncing.

>   The typical software debouncing would be the Time delay method waiting a short period after detecting the change of button state
>
>   The hardware debouncing would be RC filter that filters the short signal vibration when the button is pressed.

2. What method of debouncing did this NUCLEO board use for the push-button(B1)?

>Time delay method as it is easy to implement