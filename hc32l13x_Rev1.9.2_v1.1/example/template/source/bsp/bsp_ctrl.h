#ifndef __BSP_CTRL_H__
#define __BSP_CTRL_H__

#include "bsp_debug.h"

// �账���¼�
typedef enum
{
    DEV_EVT_NONE = 0,
//    ������ϴ ϴ��Ͱ ϴëˢ ���� �ϵ�
    DEV_EVT_CLEAN_COMPLETE = 0x01,
//    ������ϴ ϴ��Ͱ ϴëˢ �ϵ�
    DEV_EVT_CLEAN_FAST = 0x02,
//    ����ϴ   ϴëˢ
    DEV_EVT_CLEAN_MYSELF = 0x04,
//    ����
    DEV_EVT_CLEAN_UV = 0x08,
//    ������ϴ
    DEV_EVT_CLEAN_GROUND = 0x10,
//    
//    ��ϴ�ֹ���Ƴ��ջؼ��
    DEV_EVT_TEST_MOTOR_HORIZONTAL = 0x20,
//    ��ϴ�ֹ�����¼��
    DEV_EVT_TEST_MOTOR_VERTICAL = 0x40,
//    ��ϴ��ëˢ��ת���
    DEV_EVT_TEST_MOTOR_BRUSH = 0x80,
//    ��ϴ����ˮ���
    DEV_EVT_TEST_WATER_CLEAN = 0x100,
//    ��ϴ����Һ���
    DEV_EVT_TEST_LIQUID_CLEAN = 0x200,
//    ��ϴ������Ƽ��
    DEV_EVT_TEST_UV_CLEAN = 0x400,
//    ��ϴ���������
    DEV_EVT_TEST_AIR_CLEAN = 0x800,
//    ������ϴ�Ĺ�����
    DEV_EVT_TEST_GROUND_MOTOR = 0x1000,
//    ������ϴ����ѹ�˼��
    DEV_EVT_TEST_GROUND_AIR = 0x2000,
//    ������ϴ����ˮ���
    DEV_EVT_TEST_GROUND_WATER = 0x4000,
//    ������ϴ����Һ���
    DEV_EVT_TEST_GROUND_LIQUID = 0x8000,
//    ˢ����ˮ
//    DEV_EVT_TEST_BRUSH_WATER = 0x10000,
////    ˢ����Һ     
//    DEV_EVT_TEST_BRUSH_LIQUID = 0x20000,
    
}DEV_EVT_E;

typedef enum
{
    CTRL_STEPS_FIRST,
    CTRL_STEPS_SECOND,
    CTRL_STEPS_THIRD,   // ϴ�ذ�ˢ�ӻ���
    CTRL_STEPS_IDLE,
    CTRL_STEPS_HORIZONTAL_READY_OUT,    // ˮƽ���׼�����
    CTRL_STEPS_VERITAL_READY_DOWN,
    CTRL_STEPS_CLEAN_DELAY, // ��ϴ��Ͱ�ȴ�ʱ��
    CTRL_STEPS_BRUSH_MOVE,  // ��ϴ��Ͱ��ˢ�Ӿ���ѭ���ƶ�
    CTRL_STEPS_CUSHION_DOWN,    // ��ϴ������������
    CTRL_STEPS_CUSHION_DELAY,   // �ȴ��������������ѹס����
    CTRL_STEPS_UV,  // ��������
    CTRL_STEPS_BRUSH_GO_IN_1,   // ˢ�ӻ���ڵ�һ������ֱ�������
    CTRL_STEPS_BRUSH_GO_IN_2,   // ˢ�ӻ���ڵ�һ����ˮƽ������ƻز���
    CTRL_STEPS_BRUSH_MYSELF,    // ����ϴˢ��
    CTRL_STEPS_GROUND_CLEAN_1,    // ϴ�ذ岽��һ�������ƶ�
    CTRL_STEPS_GROUND_CLEAN_2,    // ϴ�ذ岽����������ƶ�

    
    CTRL_STEPS_MOVE_OUT,        // ֻ������ˢ���Ƶ������
    
    // ����Ϊ���Բ���
    CTRL_STEPS_TEST_HORIZONTAL, // ����ˮƽ����Ƴ����ջ�
    CTRL_STEPS_TEST_VERTICAL,   // ���Դ�ֱ��������ƶ�
    CTRL_STEPS_TEST_BRUSH_ROTATE,   // ����ˢ����ת
    CTRL_STEPS_TEST_CLEAN_WATER,    // ��ϴ����ˮ ˢ��һ����ˮ
    CTRL_STEPS_TEST_CLEAN_LIQUID,   // ��ϴ����Һ ˢ��һ����Һ
    CTRL_STEPS_TEST_UV,             // ����UV
    CTRL_STEPS_TEST_CLEAN_AIR,      // ��ϴ����������
    CTRL_STEPS_TEST_GROUND_MOTOR,   // ����������
    CTRL_STEPS_TEST_GROUND_AIR,     // �������ײ���
    CTRL_STEPS_TEST_GROUND_WATER,   // ������ˮ����
    CTRL_STEPS_TEST_GROUND_LIQUID,  // ������Һ����
//    CTRL_STEPS_TEST_BRUSH_WATER,    // ˢ����ˮ����
//    CTRL_STEPS_TEST_BRUSH_LIQUID,   // ˢ����Һ����
    
    CTRL_STEPS_TEST_STOP,  // ���Գ�Һ����ˮ�����ס�UV����
    
}CTRL_STEPS_E;

// ������ɨ����  ���3�ŵ�����ұ�4�ŵ��
#define CTRL_MOTOR_SWEEP_GO_OUT                 bsp_Motor3_4DirSet (MOTOR_DIR_P)   
#define CTRL_MOTOR_SWEEP_GO_IN                  bsp_Motor3_4DirSet (MOTOR_DIR_N) 
#define CTRL_MOTOR_SWEEP1_STOP                  bsp_Motor3_4_PwmStop (MOTOR_3_PWM_STOP) // ���
#define CTRL_MOTOR_SWEEP2_STOP                  bsp_Motor3_4_PwmStop (MOTOR_4_PWM_STOP) // �ұ�
#define CTRL_LIMITER_SWEEP1_OUT_IS_SET          (0 == LIMITER_2_READ)
#define CTRL_LIMITER_SWEEP1_IN_IS_SET           (0 == LIMITER_1_READ)
#define CTRL_LIMITER_SWEEP2_OUT_IS_SET          (0 == LIMITER_4_READ)
#define CTRL_LIMITER_SWEEP2_IN_IS_SET           (0 == LIMITER_3_READ)

// ��ֱ�����б�բ  5�ŵ��
#define CTRL_MOTOR_VERTICAL_UP                  bsp_Motor5DirSet (MOTOR_DIR_N) 
#define CTRL_MOTOR_VERTICAL_DOWN                bsp_Motor5DirSet (MOTOR_DIR_P) 
#define CTRL_MOTOR_VERTICAL_STOP                bsp_Motor5Stop () 
#define CTRL_LIMITER_VERTICAL_UP_IS_SET         (0 == LIMITER_8_READ)
#define CTRL_LIMITER_VERTICAL_DOWN_IS_SET       (0 == LIMITER_7_READ)

// ˮƽ���� 1�ŵ��
#define CTRL_MOTOR_HORIZONTAL_OUT               bsp_Motor1DirSet (MOTOR_DIR_P) 
#define CTRL_MOTOR_HORIZONTAL_IN                bsp_Motor1DirSet (MOTOR_DIR_N) 
#define CTRL_MOTOR_HORIZONTAL_STOP              bsp_Motor1Stop () 
#define CTRL_LIMITER_HORIZONTAL_OUT_IS_SET      (0 == LIMITER_6_READ)
#define CTRL_LIMITER_HORIZONTAL_IN_IS_SET       (0 == LIMITER_5_READ)

extern uint32_t    g_u32DevAllEvt;

void bsp_CtrlSteps (void);

void bsp_MotorTest (void);

#endif
