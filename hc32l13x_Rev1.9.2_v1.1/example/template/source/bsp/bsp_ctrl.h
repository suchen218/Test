#ifndef __BSP_CTRL_H__
#define __BSP_CTRL_H__

#include "bsp_debug.h"

// 需处理事件
typedef enum
{
    DEV_EVT_NONE = 0,
//    完整清洗 洗马桶 洗毛刷 消毒 拖地
    DEV_EVT_CLEAN_COMPLETE = 0x01,
//    快速清洗 洗马桶 洗毛刷 拖地
    DEV_EVT_CLEAN_FAST = 0x02,
//    自清洗   洗毛刷
    DEV_EVT_CLEAN_MYSELF = 0x04,
//    消毒
    DEV_EVT_CLEAN_UV = 0x08,
//    地面清洗
    DEV_EVT_CLEAN_GROUND = 0x10,
//    
//    清洗仓轨道推出收回检测
    DEV_EVT_TEST_MOTOR_HORIZONTAL = 0x20,
//    清洗仓轨道上下检测
    DEV_EVT_TEST_MOTOR_VERTICAL = 0x40,
//    清洗仓毛刷旋转检测
    DEV_EVT_TEST_MOTOR_BRUSH = 0x80,
//    清洗仓喷水检测
    DEV_EVT_TEST_WATER_CLEAN = 0x100,
//    清洗仓喷液检测
    DEV_EVT_TEST_LIQUID_CLEAN = 0x200,
//    清洗仓紫外灯检测
    DEV_EVT_TEST_UV_CLEAN = 0x400,
//    清洗仓喷气检测
    DEV_EVT_TEST_AIR_CLEAN = 0x800,
//    地面清洗的轨道检测
    DEV_EVT_TEST_GROUND_MOTOR = 0x1000,
//    地面清洗的气压杆检测
    DEV_EVT_TEST_GROUND_AIR = 0x2000,
//    地面清洗的喷水检测
    DEV_EVT_TEST_GROUND_WATER = 0x4000,
//    地面清洗的喷液检测
    DEV_EVT_TEST_GROUND_LIQUID = 0x8000,
//    刷子喷水
//    DEV_EVT_TEST_BRUSH_WATER = 0x10000,
////    刷子喷液     
//    DEV_EVT_TEST_BRUSH_LIQUID = 0x20000,
    
}DEV_EVT_E;

typedef enum
{
    CTRL_STEPS_FIRST,
    CTRL_STEPS_SECOND,
    CTRL_STEPS_THIRD,   // 洗地板刷子回收
    CTRL_STEPS_IDLE,
    CTRL_STEPS_HORIZONTAL_READY_OUT,    // 水平电机准备外出
    CTRL_STEPS_VERITAL_READY_DOWN,
    CTRL_STEPS_CLEAN_DELAY, // 清洗马桶等待时间
    CTRL_STEPS_BRUSH_MOVE,  // 清洗马桶，刷子矩形循环移动
    CTRL_STEPS_CUSHION_DOWN,    // 清洗结束放下坐垫
    CTRL_STEPS_CUSHION_DELAY,   // 等待坐垫气缸伸出，压住坐垫
    CTRL_STEPS_UV,  // 消毒环节
    CTRL_STEPS_BRUSH_GO_IN_1,   // 刷子会舱内第一步，垂直电机升起
    CTRL_STEPS_BRUSH_GO_IN_2,   // 刷子会舱内第一步，水平电机右移回舱内
    CTRL_STEPS_BRUSH_MYSELF,    // 自清洗刷子
    CTRL_STEPS_GROUND_CLEAN_1,    // 洗地板步骤一，上下移动
    CTRL_STEPS_GROUND_CLEAN_2,    // 洗地板步骤二，左右移动

    
    CTRL_STEPS_MOVE_OUT,        // 只消毒，刷子移到最左边
    
    // 以下为测试步骤
    CTRL_STEPS_TEST_HORIZONTAL, // 测试水平电机推出、收回
    CTRL_STEPS_TEST_VERTICAL,   // 测试垂直电机上下移动
    CTRL_STEPS_TEST_BRUSH_ROTATE,   // 测试刷子旋转
    CTRL_STEPS_TEST_CLEAN_WATER,    // 清洗仓喷水 刷子一起喷水
    CTRL_STEPS_TEST_CLEAN_LIQUID,   // 清洗仓喷液 刷子一起喷液
    CTRL_STEPS_TEST_UV,             // 测试UV
    CTRL_STEPS_TEST_CLEAN_AIR,      // 清洗仓喷气测试
    CTRL_STEPS_TEST_GROUND_MOTOR,   // 地面电机测试
    CTRL_STEPS_TEST_GROUND_AIR,     // 地面气缸测试
    CTRL_STEPS_TEST_GROUND_WATER,   // 地面喷水测试
    CTRL_STEPS_TEST_GROUND_LIQUID,  // 地面喷液测试
//    CTRL_STEPS_TEST_BRUSH_WATER,    // 刷子喷水测试
//    CTRL_STEPS_TEST_BRUSH_LIQUID,   // 刷子喷液测试
    
    CTRL_STEPS_TEST_STOP,  // 测试出液、出水、气缸、UV结束
    
}CTRL_STEPS_E;

// 往外清扫地面  左边3号电机，右边4号电机
#define CTRL_MOTOR_SWEEP_GO_OUT                 bsp_Motor3_4DirSet (MOTOR_DIR_P)   
#define CTRL_MOTOR_SWEEP_GO_IN                  bsp_Motor3_4DirSet (MOTOR_DIR_N) 
#define CTRL_MOTOR_SWEEP1_STOP                  bsp_Motor3_4_PwmStop (MOTOR_3_PWM_STOP) // 左边
#define CTRL_MOTOR_SWEEP2_STOP                  bsp_Motor3_4_PwmStop (MOTOR_4_PWM_STOP) // 右边
#define CTRL_LIMITER_SWEEP1_OUT_IS_SET          (0 == LIMITER_2_READ)
#define CTRL_LIMITER_SWEEP1_IN_IS_SET           (0 == LIMITER_1_READ)
#define CTRL_LIMITER_SWEEP2_OUT_IS_SET          (0 == LIMITER_4_READ)
#define CTRL_LIMITER_SWEEP2_IN_IS_SET           (0 == LIMITER_3_READ)

// 垂直方向有抱闸  5号电机
#define CTRL_MOTOR_VERTICAL_UP                  bsp_Motor5DirSet (MOTOR_DIR_N) 
#define CTRL_MOTOR_VERTICAL_DOWN                bsp_Motor5DirSet (MOTOR_DIR_P) 
#define CTRL_MOTOR_VERTICAL_STOP                bsp_Motor5Stop () 
#define CTRL_LIMITER_VERTICAL_UP_IS_SET         (0 == LIMITER_8_READ)
#define CTRL_LIMITER_VERTICAL_DOWN_IS_SET       (0 == LIMITER_7_READ)

// 水平方向 1号电机
#define CTRL_MOTOR_HORIZONTAL_OUT               bsp_Motor1DirSet (MOTOR_DIR_P) 
#define CTRL_MOTOR_HORIZONTAL_IN                bsp_Motor1DirSet (MOTOR_DIR_N) 
#define CTRL_MOTOR_HORIZONTAL_STOP              bsp_Motor1Stop () 
#define CTRL_LIMITER_HORIZONTAL_OUT_IS_SET      (0 == LIMITER_6_READ)
#define CTRL_LIMITER_HORIZONTAL_IN_IS_SET       (0 == LIMITER_5_READ)

extern uint32_t    g_u32DevAllEvt;

void bsp_CtrlSteps (void);

void bsp_MotorTest (void);

#endif
