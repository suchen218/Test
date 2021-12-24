#ifndef __BSP_MOTOR_H__
#define __BSP_MOTOR_H__

#include "bsp_debug.h"

// MOTOR3和MOTOR4分别为TIM6的AB通道，此处选择关闭的通道
typedef enum
{
    MOTOR_3_PWM_STOP,
    MOTOR_4_PWM_STOP,
    MOTOR_3_4_PWM_STOP,
}MOTOR_34_PWM_STOP_E;

// 电机方向
typedef enum
{
    MOTOR_DIR_NONE,
    MOTOR_DIR_P,
    MOTOR_DIR_N,
} MOTOR_DIR_E;

// 电机标志
typedef enum
{
    MOTOR_FLAG_1 = 0x01,
    MOTOR_FLAG_2 = 0x02,
    MOTOR_FLAG_3 = 0x04,
    MOTOR_FLAG_4 = 0x08,
    MOTOR_FLAG_5 = 0x10,
}MOTOR_FLAG_E;

// 暂不确定哪个电机需要抱闸
#define MOTOR_BRAKE_PORT      GpioPortD
#define MOTOR_BRAKE_PIN       GpioPin5
#define MOTOR_BRAKE_HIGH       M0P_GPIO->PDBSET |= (1 << 5) 
#define MOTOR_BRAKE_LOW       M0P_GPIO->PDBCLR |= (1 << 5) 


#define MOTOR_1_PWR_PORT      GpioPortC
#define MOTOR_1_PWR_PIN       GpioPin0
#define MOTOR_1_PWR_ENABLE       M0P_GPIO->PCBSET |= (1 << 0) 
#define MOTOR_1_PWR_DISABLE       M0P_GPIO->PCBCLR |= (1 << 0) 

#define MOTOR_1_PULSE_PORT      GpioPortA
#define MOTOR_1_PULSE_PIN       GpioPin0
#define MOTOR_1_PULSE_HIGH       M0P_GPIO->PABSET |= (1 << 0) 
#define MOTOR_1_PULSE_LOW       M0P_GPIO->PABCLR |= (1 << 0) 

#define MOTOR_1_DIR_PORT      GpioPortA
#define MOTOR_1_DIR_PIN       GpioPin1
#define MOTOR_1_DIR_HIGH       M0P_GPIO->PABSET |= (1 << 1) 
#define MOTOR_1_DIR_LOW       M0P_GPIO->PABCLR |= (1 << 1)
#define MOTOR_1_DIR_P        MOTOR_1_DIR_HIGH
#define MOTOR_1_DIR_N        MOTOR_1_DIR_LOW

#define MOTOR_1_NP_PORT      GpioPortA
#define MOTOR_1_NP_PIN       GpioPin2
#define MOTOR_1_NP_READ       (M0P_GPIO->PAIN & (1 << 2)) 

#define MOTOR_1_NN_PORT      GpioPortA
#define MOTOR_1_NN_PIN       GpioPin3
#define MOTOR_1_NN_READ       (M0P_GPIO->PAIN & (1 << 3)) 

#define MOTOR_1_BRAKE_ENABLE  
#define MOTOR_1_BRAKE_DISABLE 
//////////////////////////////////////////////////////////

#define MOTOR_2_PWR_PORT      GpioPortC
#define MOTOR_2_PWR_PIN       GpioPin1
#define MOTOR_2_PWR_ENABLE       M0P_GPIO->PCBSET |= (1 << 1) 
#define MOTOR_2_PWR_DISABLE       M0P_GPIO->PCBCLR |= (1 << 1) 

#define MOTOR_2_PULSE_PORT      GpioPortA
#define MOTOR_2_PULSE_PIN       GpioPin4
#define MOTOR_2_PULSE_HIGH       M0P_GPIO->PABSET |= (1 << 4) 
#define MOTOR_2_PULSE_LOW       M0P_GPIO->PABCLR |= (1 << 4) 

#define MOTOR_2_DIR_PORT      GpioPortA
#define MOTOR_2_DIR_PIN       GpioPin5
#define MOTOR_2_DIR_HIGH       M0P_GPIO->PABSET |= (1 << 5) 
#define MOTOR_2_DIR_LOW       M0P_GPIO->PABCLR |= (1 << 5) 
#define MOTOR_2_DIR_P        MOTOR_2_DIR_HIGH
#define MOTOR_2_DIR_N        MOTOR_2_DIR_LOW

#define MOTOR_2_NP_PORT      GpioPortA
#define MOTOR_2_NP_PIN       GpioPin6
#define MOTOR_2_NP_READ       (M0P_GPIO->PAIN & (1 << 6)) 

#define MOTOR_2_NN_PORT      GpioPortA
#define MOTOR_2_NN_PIN       GpioPin7
#define MOTOR_2_NN_READ       (M0P_GPIO->PAIN & (1 << 7)) 

#define MOTOR_2_BRAKE_ENABLE        
#define MOTOR_2_BRAKE_DISABLE       
//////////////////////////////////////////////////////////

#define MOTOR_3_PWR_PORT      GpioPortC
#define MOTOR_3_PWR_PIN       GpioPin2
#define MOTOR_3_PWR_ENABLE       M0P_GPIO->PCBSET |= (1 << 2) 
#define MOTOR_3_PWR_DISABLE       M0P_GPIO->PCBCLR |= (1 << 2) 

#define MOTOR_3_PULSE_PORT      GpioPortC
#define MOTOR_3_PULSE_PIN       GpioPin5
#define MOTOR_3_PULSE_HIGH       M0P_GPIO->PCBSET |= (1 << 5) 
#define MOTOR_3_PULSE_LOW       M0P_GPIO->PCBCLR |= (1 << 5) 
              
#define MOTOR_3_DIR_PORT      GpioPortB
#define MOTOR_3_DIR_PIN       GpioPin0
#define MOTOR_3_DIR_HIGH       M0P_GPIO->PBBSET |= (1 << 0) 
#define MOTOR_3_DIR_LOW       M0P_GPIO->PBBCLR |= (1 << 0) 
#define MOTOR_3_DIR_P        MOTOR_3_DIR_HIGH
#define MOTOR_3_DIR_N        MOTOR_3_DIR_LOW
              
#define MOTOR_3_NP_PORT      GpioPortB
#define MOTOR_3_NP_PIN       GpioPin1
#define MOTOR_3_NP_READ       (M0P_GPIO->PBIN & (1 << 1)) 
              
#define MOTOR_3_NN_PORT      GpioPortB
#define MOTOR_3_NN_PIN       GpioPin2
#define MOTOR_3_NN_READ       (M0P_GPIO->PBIN & (1 << 2)) 

#define MOTOR_3_BRAKE_ENABLE 
#define MOTOR_3_BRAKE_DISABLE
//////////////////////////////////////////////////////////

#define MOTOR_4_PWR_PORT      GpioPortC
#define MOTOR_4_PWR_PIN       GpioPin3
#define MOTOR_4_PWR_ENABLE       M0P_GPIO->PCBSET |= (1 << 3) 
#define MOTOR_4_PWR_DISABLE       M0P_GPIO->PCBCLR |= (1 << 3) 

#define MOTOR_4_PULSE_PORT      GpioPortB
#define MOTOR_4_PULSE_PIN       GpioPin12
#define MOTOR_4_PULSE_HIGH       M0P_GPIO->PBBSET |= (1 << 12) 
#define MOTOR_4_PULSE_LOW       M0P_GPIO->PBBCLR |= (1 << 12) 
              
#define MOTOR_4_DIR_PORT      GpioPortB
#define MOTOR_4_DIR_PIN       GpioPin13
#define MOTOR_4_DIR_HIGH       M0P_GPIO->PBBSET |= (1 << 13) 
#define MOTOR_4_DIR_LOW       M0P_GPIO->PBBCLR |= (1 << 13) 
#define MOTOR_4_DIR_P       MOTOR_4_DIR_LOW 
#define MOTOR_4_DIR_N        MOTOR_4_DIR_HIGH
              
#define MOTOR_4_NP_PORT      GpioPortB
#define MOTOR_4_NP_PIN       GpioPin14
#define MOTOR_4_NP_READ       (M0P_GPIO->PBIN & (1 << 14)) 
              
#define MOTOR_4_NN_PORT      GpioPortB
#define MOTOR_4_NN_PIN       GpioPin15
#define MOTOR_4_NN_READ       (M0P_GPIO->PBIN & (1 << 15)) 

#define MOTOR_4_BRAKE_ENABLE 
#define MOTOR_4_BRAKE_DISABLE
//////////////////////////////////////////////////////////

#define MOTOR_5_PWR_PORT      GpioPortC
#define MOTOR_5_PWR_PIN       GpioPin4
#define MOTOR_5_PWR_ENABLE       M0P_GPIO->PCBSET |= (1 << 4) 
#define MOTOR_5_PWR_DISABLE       M0P_GPIO->PCBCLR |= (1 << 4) 
              
#define MOTOR_5_PULSE_PORT      GpioPortC
#define MOTOR_5_PULSE_PIN       GpioPin6
#define MOTOR_5_PULSE_HIGH       M0P_GPIO->PCBSET |= (1 << 6) 
#define MOTOR_5_PULSE_LOW       M0P_GPIO->PCBCLR |= (1 << 6) 
              
#define MOTOR_5_DIR_PORT      GpioPortC
#define MOTOR_5_DIR_PIN       GpioPin7
#define MOTOR_5_DIR_HIGH       M0P_GPIO->PCBSET |= (1 << 7) 
#define MOTOR_5_DIR_LOW       M0P_GPIO->PCBCLR |= (1 << 7) 
#define MOTOR_5_DIR_P        MOTOR_5_DIR_HIGH
#define MOTOR_5_DIR_N        MOTOR_5_DIR_LOW
             
#define MOTOR_5_NP_PORT      GpioPortC
#define MOTOR_5_NP_PIN       GpioPin8
#define MOTOR_5_NP_READ       (M0P_GPIO->PCIN & (1 << 8)) 
              
#define MOTOR_5_NN_PORT      GpioPortC
#define MOTOR_5_NN_PIN       GpioPin9
#define MOTOR_5_NN_READ       (M0P_GPIO->PCIN & (1 << 9)) 

#define MOTOR_5_BRAKE_ENABLE        MOTOR_BRAKE_HIGH
#define MOTOR_5_BRAKE_DISABLE       MOTOR_BRAKE_LOW

// 定时器6 通道A和B 关闭
#define MOTOR_3_PWM_DISABLE    M0P_ADTIM6->PCONR_f.OUTENB = 0
#define MOTOR_4_PWM_DISABLE    M0P_ADTIM6->PCONR_f.OUTENA = 0

#define MOTOR_3_PWM_ENABLE    M0P_ADTIM6->PCONR_f.OUTENB = 1
#define MOTOR_4_PWM_ENABLE    M0P_ADTIM6->PCONR_f.OUTENA = 1

extern  volatile uint8_t     g_u8MotorMoveFlag;

void bsp_MotorGpioInit (void);
void bsp_Motor1TimerInit (void);
void bsp_Motor1Start (void);
void bsp_Motor1Stop (void);
void bsp_Motor1DirSet (uint8_t _u8Dir);

void bsp_Motor2TimerInit (void);
void bsp_Motor2Start (void);
void bsp_Motor2Stop (void);
void bsp_Motor2DirSet (uint8_t _u8Dir);

void bsp_Motor3_4TimerInit (void);
void bsp_Motor3_4Start (void);
void bsp_Motor3_4Stop (void);
void bsp_Motor3_4DirSet (uint8_t _u8Dir);
void bsp_Motor3_4_PwmStop (uint8_t _u8Type);

void bsp_Motor5TimerInit (void);
void bsp_Motor5Start (void);
void bsp_Motor5Stop (void);
void bsp_Motor5DirSet (uint8_t _u8Dir);

void bsp_MotorInit (void);

#endif
