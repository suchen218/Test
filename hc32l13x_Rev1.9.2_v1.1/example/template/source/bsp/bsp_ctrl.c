#include "bsp_ctrl.h"

#if 1//CONFIG_RTT_SPI_ENABLED
#define CTRL_Print(M, ...)	LOG_Print("CTRL", M, ##__VA_ARGS__)
#else
#define CTRL_Print(M, ...)
#endif

uint8_t     g_u8SweepLRFlag;



// 刷子 2号电机
#define CTRL_MOTOR_BRUSH_P                      bsp_Motor2DirSet (MOTOR_DIR_P) 
#define CTRL_MOTOR_BRUSH_N                      bsp_Motor2DirSet (MOTOR_DIR_N) 
#define CTRL_MOTOR_BRUSH_STOP                   bsp_Motor2Stop () 

// 洗马桶进水
#define CTRL_CLEAN_WATER_IN
#define CTRL_CLEAN_WATER_STOP

// 洗马桶进液
#define CTRL_CLEAN_LIQUID_IN 
#define CTRL_CLEAN_LIQUID_STOP

// 洗地板进水
#define CTRL_GROUND_WATER_IN
#define CTRL_GROUND_WATER_STOP

// 洗地板进液
#define CTRL_GROUND_LIQUID_IN 
#define CTRL_GROUND_LIQUID_STOP

// 自清洗进水
#define CTRL_BRUSH_WATER_IN
#define CTRL_BRUSH_WATER_STOP

// 自清洗进液
#define CTRL_BRUSH_LIQUID_IN 
#define CTRL_BRUSH_LIQUID_STOP

// 气缸抬起扫把
#define CTRL_AIR_BESOM_UP                   //AIR_11_ENABLE
// 气缸放下扫把
#define CTRL_AIR_BESOM_DOWN                 //AIR_11_DISABLE

// 气缸收起，坐垫可抬起
#define CTRL_AIR_CUSHION_IN

// 气缸伸出，坐垫不可抬起
#define CTRL_AIR_CUSHION_OUT

// 洗完马桶吹干
#define CTRL_AIR_DRYING_RUN     
#define CTRL_AIR_DRYING_STOP

#define CTRL_PIR_NO_HUMAN           1
#define CTRL_CICTERN_IS_NORMAL      1
#define CTRL_CICTERN_IS_PAUSE       1





// 当前处于流程的哪一步
uint8_t     g_u8CtrlSteps = CTRL_STEPS_FIRST;

// 设备已有事件
uint32_t    g_u32DevAllEvt = DEV_EVT_CLEAN_COMPLETE;

// 设备当前执行事件
uint32_t    g_u32DevCurrEvt;

#define SECONDS_TICKS   (50)

// 从里面往座便器方向挪动
#define CTRL_HORIZONTAL_OUT_FIRST_TICKS    (46 * SECONDS_TICKS)  
// 在坐便内部刷洗挪动
#define CTRL_HORIZONTAL_OUT_SECOND_TICKS    (2 * SECONDS_TICKS)
// 坐便池左右清洗时间
#define CTRL_CLEAN_BRUSH_TICKS          (10 * SECONDS_TICKS)
// 坐便池舱内清洗上升高度时间
#define CTRL_CLEAN_UP_TICKS          (4 * SECONDS_TICKS)
// 清洗坐便池循环次数
#define CTRL_CLEAN_REPEAT           (2)
// 消毒时长
#define CTRL_CLEAN_UV_TICKS               (10 * SECONDS_TICKS)
// 自清洗刷子半周期时长
#define CTRL_MYSELF_BRUSH_TICKS     (30 * SECONDS_TICKS)  

// 测试相关
// 刷子旋转
#define CTRL_TEST_BRUSH_ROTATE_TICKS    (60 * SECONDS_TICKS)   
// 清洗仓喷水
#define CTRL_TEST_CLEAN_WATER_TICKS     (60 * SECONDS_TICKS)
// 清洗仓喷液
#define CTRL_TEST_CLEAN_LIQUID_TICKS    (30 * SECONDS_TICKS)
// UV测试
#define CTRL_TEST_UV_TICKS              (30 * SECONDS_TICKS)
// 清洗仓喷气
#define CTRL_TEST_CLEAN_AIR_TICKS       (30 * SECONDS_TICKS)
// 地面喷水
#define CTRL_TEST_GROUND_WATER_TICKS    (60 * SECONDS_TICKS)
// 地面喷液
#define CTRL_TEST_GROUND_LIQUID_TICKS   (30 * SECONDS_TICKS)
// 地面气缸
#define CTRL_TEST_GROUND_AIR_TICKS   (2 * SECONDS_TICKS)
// 刷子喷水
#define CTRL_TEST_BRUSH_WATER_TICKS    (60 * SECONDS_TICKS)
// 刷子喷液
#define CTRL_TEST_BRUSH_LIQUID_TICKS   (30 * SECONDS_TICKS)

// 左右移动的计时次数
uint16_t    g_u16LRMoveTicks;
// 上下移动的计时次数
uint16_t    g_u16UDMoveTicks;

// 清洗的计时次数
uint16_t    g_u16CleanTicks;
// 清洗的次数
uint8_t     g_u8CleanRepeat;
// 清洗暂停的次数
uint8_t     g_u8CleanPause;

void bsp_MotorTest (void)
{
//    CTRL_MOTOR_BRUSH_P; 
}

void bsp_CtrlEvtGet (void)
{
    uint8_t     i;
    
    g_u32DevCurrEvt = DEV_EVT_NONE;
    
    for (i = 0; i < 32; i++)
    {
        if (g_u32DevAllEvt & (1 << i))
        {
            g_u32DevCurrEvt = (1 << i);
            break;
        }
    }
    
    g_u32DevAllEvt ^= g_u32DevCurrEvt;
}

void bsp_CtrlEvtJump (void)
{
    g_u16LRMoveTicks = 0;
// 上下移动的计时次数
    g_u16UDMoveTicks = 0;
// 清洗的计时次数
    g_u16CleanTicks = 0;
// 清洗的次数
    g_u8CleanRepeat = 0;
// 清洗暂停的次数
    g_u8CleanPause = 0;
    
    switch (g_u32DevCurrEvt)
    {
        case DEV_EVT_CLEAN_COMPLETE:// = 0x01,
//    快速清洗 洗马桶 洗毛刷 拖地
        case DEV_EVT_CLEAN_FAST:// = 0x02,
            g_u8CtrlSteps = CTRL_STEPS_HORIZONTAL_READY_OUT;
            break;
//    自清洗   洗毛刷
        case DEV_EVT_CLEAN_MYSELF:// = 0x04,
            g_u8CtrlSteps = CTRL_STEPS_BRUSH_MYSELF;
            break;
//    消毒
        case DEV_EVT_CLEAN_UV:// = 0x08,
            g_u8CtrlSteps = CTRL_STEPS_MOVE_OUT;
            break;
//    地面清洗
        case DEV_EVT_CLEAN_GROUND:// = 0x10,
            g_u8CtrlSteps = CTRL_STEPS_GROUND_CLEAN_1;
            break;
//    
//    清洗仓轨道推出收回检测
        case DEV_EVT_TEST_MOTOR_HORIZONTAL:// = 0x20,
            g_u8CtrlSteps = CTRL_STEPS_TEST_HORIZONTAL;
            break;
//    清洗仓轨道上下检测
        case DEV_EVT_TEST_MOTOR_VERTICAL:// = 0x40,
            g_u8CtrlSteps = CTRL_STEPS_MOVE_OUT;
            break;
//    清洗仓毛刷旋转检测
        case DEV_EVT_TEST_MOTOR_BRUSH:// = 0x80,
            g_u8CtrlSteps = CTRL_STEPS_TEST_BRUSH_ROTATE;
            break;
//    清洗仓喷水检测
        case DEV_EVT_TEST_WATER_CLEAN:// = 0x100,
            g_u8CtrlSteps = CTRL_STEPS_TEST_CLEAN_WATER;
            break;
//    清洗仓喷液检测
        case DEV_EVT_TEST_LIQUID_CLEAN:// = 0x200,
            g_u8CtrlSteps = CTRL_STEPS_TEST_CLEAN_LIQUID;
            break;
//    清洗仓紫外灯检测
        case DEV_EVT_TEST_UV_CLEAN:// = 0x400,
            g_u8CtrlSteps = CTRL_STEPS_TEST_UV;
            break;
//    清洗仓喷气检测
        case DEV_EVT_TEST_AIR_CLEAN:// = 0x800,
            g_u8CtrlSteps = CTRL_STEPS_TEST_CLEAN_AIR;
            break;
//    地面清洗的轨道检测
        case DEV_EVT_TEST_GROUND_MOTOR:// = 0x1000,
            g_u8CtrlSteps = CTRL_STEPS_TEST_GROUND_MOTOR;
            break;
//    地面清洗的气压杆检测
        case DEV_EVT_TEST_GROUND_AIR:// = 0x2000,
            g_u8CtrlSteps = CTRL_STEPS_TEST_GROUND_AIR;
            break;
//    地面清洗的喷水检测
        case DEV_EVT_TEST_GROUND_WATER:// = 0x4000,
            g_u8CtrlSteps = CTRL_STEPS_TEST_GROUND_WATER;
            break;
//    地面清洗的喷液检测
        case DEV_EVT_TEST_GROUND_LIQUID:// = 0x8000,
            g_u8CtrlSteps = CTRL_STEPS_TEST_GROUND_LIQUID;
            break;
////    刷子喷水
//        case DEV_EVT_TEST_BRUSH_WATER:
//            g_u8CtrlSteps = CTRL_STEPS_TEST_BRUSH_WATER;
//            break;
////    刷子喷液   
//        case DEV_EVT_TEST_BRUSH_LIQUID:
//            g_u8CtrlSteps = CTRL_STEPS_TEST_BRUSH_LIQUID;
//            break;
    }
}

void bsp_CtrlSteps (void)
{
    switch (g_u8CtrlSteps)
    {
        case CTRL_STEPS_FIRST:     
            CTRL_AIR_CUSHION_OUT;
            if (CTRL_LIMITER_VERTICAL_UP_IS_SET)    // 上升到顶部
            {
                CTRL_MOTOR_VERTICAL_STOP;
                g_u8CtrlSteps = CTRL_STEPS_SECOND;    // 已经到顶部，下一步收回清洗台
            }
            else
            {
                CTRL_MOTOR_VERTICAL_UP;     // 持续上升
            }
            break;
        
        case CTRL_STEPS_SECOND:
            if (CTRL_LIMITER_HORIZONTAL_IN_IS_SET)  // 清洗台已经收回
            {
                CTRL_MOTOR_HORIZONTAL_STOP;
                g_u8CtrlSteps = CTRL_STEPS_THIRD;
                
                if ((FALSE == CTRL_LIMITER_SWEEP1_IN_IS_SET) || (FALSE == CTRL_LIMITER_SWEEP2_IN_IS_SET))
                {
                    // 抬起气缸
//                    CTRL_AIR_BESOM_UP;
                    CTRL_MOTOR_SWEEP_GO_IN;
                    g_u8SweepLRFlag = 0;
                }
            }
            else
            {
                CTRL_MOTOR_HORIZONTAL_IN;
            }
            break;
            
        case CTRL_STEPS_THIRD:  // 洗地板刷子回收
            // 扫把两个电机都已经回收
            if (CTRL_LIMITER_SWEEP1_IN_IS_SET)
            {
                // 扫把电机1已到位
                CTRL_MOTOR_SWEEP1_STOP;
                g_u8SweepLRFlag |= LIMITER_FLAG_SWEEP_LEFT_IN;
            }
 
            if (CTRL_LIMITER_SWEEP2_IN_IS_SET)
            {
                // 扫把电机2已到位
                CTRL_MOTOR_SWEEP2_STOP;
                g_u8SweepLRFlag |= LIMITER_FLAG_SWEEP_RIGHT_IN;
            }
            
            if ((CTRL_LIMITER_SWEEP1_IN_IS_SET && CTRL_LIMITER_SWEEP2_IN_IS_SET) || (g_u8SweepLRFlag == (LIMITER_FLAG_SWEEP_LEFT_IN | LIMITER_FLAG_SWEEP_RIGHT_IN)))
            {
                CTRL_MOTOR_SWEEP1_STOP;
                CTRL_MOTOR_SWEEP2_STOP;
                g_u8CtrlSteps = CTRL_STEPS_IDLE;
                g_u8SweepLRFlag = 0;
                // 气缸放下扫把
//                CTRL_AIR_BESOM_DOWN;
            }
            break;
            
        case CTRL_STEPS_IDLE:
            // PIR未感应到人，水箱正常  水箱未工作
//            if (CTRL_PIR_NO_HUMAN && CTRL_CICTERN_IS_NORMAL && CTRL_CICTERN_IS_PAUSE)
            {
                // 有事件
                if (g_u32DevAllEvt)
                {
                    bsp_CtrlEvtGet ();  // 获取当前该执行事件
                    bsp_CtrlEvtJump (); // 跳转到该执行事件
                }
            }
            break;
            
        case CTRL_STEPS_HORIZONTAL_READY_OUT: // 水平电机往外挪，准备洗马桶
            g_u16LRMoveTicks++;
            if ((g_u16LRMoveTicks >= CTRL_HORIZONTAL_OUT_FIRST_TICKS) || (CTRL_LIMITER_HORIZONTAL_OUT_IS_SET)) // 到达靠里面的下放位置
//                if (CTRL_LIMITER_HORIZONTAL_OUT_IS_SET)
        {
                g_u16LRMoveTicks = 0;
                g_u8CleanRepeat = 0;
                g_u16UDMoveTicks = 0;
                CTRL_MOTOR_HORIZONTAL_STOP;
                g_u8CtrlSteps = CTRL_STEPS_VERITAL_READY_DOWN;
            }
            else
            {
                CTRL_MOTOR_HORIZONTAL_OUT;
            }
            break;
            
        case CTRL_STEPS_VERITAL_READY_DOWN:     // 降下刷子
            CTRL_MOTOR_BRUSH_P;     // 刷子开始工作
        
            if (g_u8CleanRepeat & 0x01)
            {
                CTRL_CLEAN_WATER_IN;
            }
            else
            {
                CTRL_CLEAN_LIQUID_IN;   // 进清洁液
            }
        
            if (CTRL_LIMITER_VERTICAL_DOWN_IS_SET)
            {
                CTRL_MOTOR_VERTICAL_STOP;
                g_u8CleanPause = 0;
                g_u16UDMoveTicks = 0;
                g_u8CtrlSteps = CTRL_STEPS_CLEAN_DELAY;
            }
            else
            {
                CTRL_MOTOR_VERTICAL_DOWN;
            }
            break;
            
        case CTRL_STEPS_CLEAN_DELAY:   // 停在此处喷液或水
            g_u16UDMoveTicks++;
            if (g_u16UDMoveTicks >= CTRL_CLEAN_BRUSH_TICKS)
            {
                g_u16UDMoveTicks = 0;
                g_u8CleanPause++;
                
                if (3 == g_u8CleanPause)
                    g_u8CleanRepeat++;
                g_u8CtrlSteps = CTRL_STEPS_BRUSH_MOVE;
            }
            break;
            
        case CTRL_STEPS_BRUSH_MOVE:   // 应该设置最大等待时间
            // 水平方向已经到了最外边
            switch (g_u8CleanPause)
            {
                case 1: // 左走
                    g_u16LRMoveTicks++;
                    if ((CTRL_LIMITER_HORIZONTAL_OUT_IS_SET) || (g_u16LRMoveTicks >= CTRL_HORIZONTAL_OUT_SECOND_TICKS))
                    {
//                        CTRL_Print ("g_u16LRMoveTicks: %d", g_u16LRMoveTicks);
                        CTRL_MOTOR_HORIZONTAL_STOP;
                        g_u8CtrlSteps = CTRL_STEPS_CLEAN_DELAY;
                    }
                    else
                    {
                        CTRL_MOTOR_HORIZONTAL_OUT;
                    }
                    break;
                
                case 2: // 升起坐垫
                    g_u16UDMoveTicks++;
                    if ((CTRL_LIMITER_VERTICAL_UP_IS_SET) || (g_u16UDMoveTicks >= CTRL_CLEAN_UP_TICKS))
                    {
                        CTRL_MOTOR_VERTICAL_STOP;
                        g_u8CtrlSteps = CTRL_STEPS_CLEAN_DELAY;
                    }
                    else
                    {
                        CTRL_MOTOR_VERTICAL_UP;
                    }
                    break;
                
                case 3: // 往右回退，启动下一次轮回
                    CTRL_MOTOR_BRUSH_STOP;
                    CTRL_CLEAN_WATER_STOP;
                    CTRL_CLEAN_LIQUID_STOP;
                
//                    CTRL_Print ("g_u16LRMoveTicks: %d,%d", g_u16LRMoveTicks, g_u8CleanRepeat);
                
                    
                    if (g_u8CleanRepeat >= CTRL_CLEAN_REPEAT)
                    {
                        // 结束
                        g_u8CtrlSteps = CTRL_STEPS_CUSHION_DOWN;
                    }
                    else
                    {
                        // 回退倒计时
                        if (g_u16LRMoveTicks)
                            g_u16LRMoveTicks--;
                        
                        
                        if ((g_u16LRMoveTicks) && (0 == CTRL_LIMITER_HORIZONTAL_IN_IS_SET))
                        {
                            CTRL_MOTOR_HORIZONTAL_IN;
                        }
                        else
                        {
                            CTRL_MOTOR_HORIZONTAL_STOP;
                            g_u8CtrlSteps = CTRL_STEPS_VERITAL_READY_DOWN;
                            
                        }
                    }
                    break;
            }
            break;
            
        case CTRL_STEPS_CUSHION_DOWN: // 马桶清洗结束，放下坐垫

            if (CTRL_LIMITER_VERTICAL_DOWN_IS_SET)
            {
                CTRL_MOTOR_VERTICAL_STOP;
                g_u16UDMoveTicks = 0;
                g_u8CtrlSteps = CTRL_STEPS_CUSHION_DELAY;
            }
            else
            {
                CTRL_MOTOR_VERTICAL_DOWN;
            }
            break;
            
        case CTRL_STEPS_CUSHION_DELAY:  // 等待小段时间，使坐垫完全被气缸压住
            CTRL_AIR_CUSHION_OUT;
            g_u16UDMoveTicks++;
            if (g_u16UDMoveTicks >= 50)
            {
                g_u16UDMoveTicks = 0;
                
                if (g_u32DevCurrEvt == DEV_EVT_CLEAN_COMPLETE)
                    g_u8CtrlSteps = CTRL_STEPS_UV;
                else
                    g_u8CtrlSteps = CTRL_STEPS_BRUSH_GO_IN_1;
            }
            break;
            
        case CTRL_STEPS_UV: // UV消毒
            UV_ENABLE;
            g_u16UDMoveTicks++;
            if (g_u16UDMoveTicks >= CTRL_CLEAN_UV_TICKS)
            {
                g_u16UDMoveTicks = 0;
                UV_DISABLE;
                g_u8CtrlSteps = CTRL_STEPS_BRUSH_GO_IN_1;
            }
            break;
            
        case CTRL_STEPS_BRUSH_GO_IN_1:      // 垂直电机升起，准备返回舱内  
            if (CTRL_LIMITER_VERTICAL_UP_IS_SET)
            {
                CTRL_MOTOR_VERTICAL_STOP;
                CTRL_AIR_CUSHION_IN;
                CTRL_AIR_DRYING_RUN;    // 吹干
                g_u16UDMoveTicks = 0;
                g_u8CtrlSteps = CTRL_STEPS_BRUSH_GO_IN_2;
            }
            else
            {
                CTRL_MOTOR_VERTICAL_UP;
            }
            break;
            
        case CTRL_STEPS_BRUSH_GO_IN_2:  // 水平电机右移，返回舱内
            if (CTRL_LIMITER_HORIZONTAL_IN_IS_SET) // 到达靠里面的下放位置
            {
                g_u16LRMoveTicks = 0;
                g_u8CleanRepeat = 0;
                g_u16UDMoveTicks = 0;
                CTRL_AIR_CUSHION_IN;
                CTRL_MOTOR_HORIZONTAL_STOP;
                CTRL_AIR_DRYING_STOP;
                
                // 若只是消毒，则返回 空闲状态，否则还要清洗刷子
                if (g_u32DevCurrEvt == DEV_EVT_CLEAN_UV)
                    g_u8CtrlSteps = CTRL_STEPS_IDLE;
                else
                    g_u8CtrlSteps = CTRL_STEPS_BRUSH_MYSELF;
            }
            else
            {
                CTRL_MOTOR_HORIZONTAL_IN;
            }
            break;  

        case CTRL_STEPS_BRUSH_MYSELF: // 自清洗步骤开始
            CTRL_MOTOR_BRUSH_P;
            g_u16UDMoveTicks++;
            if (g_u16UDMoveTicks >= CTRL_MYSELF_BRUSH_TICKS)
            {
                g_u16UDMoveTicks = 0;
                g_u8CleanRepeat++;
            }
            
            switch (g_u8CleanRepeat)
            {
                case 0: // 第一遍出液
                    CTRL_BRUSH_LIQUID_IN;
                    break;
                
                case 1: // 第二遍出水
                    CTRL_BRUSH_WATER_IN;
                    break;
                
                case 2: // 结束
                    CTRL_BRUSH_WATER_STOP;
                    CTRL_MOTOR_BRUSH_STOP;
                    g_u8CleanRepeat = 0;
                
                    // 自清洗到此结束，若是完整或快速清洗还要洗地板
                    if (g_u32DevCurrEvt == DEV_EVT_CLEAN_MYSELF)
                        g_u8CtrlSteps = CTRL_STEPS_IDLE;
                    else
                        g_u8CtrlSteps = CTRL_STEPS_GROUND_CLEAN_1;
                    break;
            }
            break;
            
        case CTRL_STEPS_GROUND_CLEAN_1: // 洗地板
            // 0000抬起  0001左走 0010降下 0011右走
            // 0100抬起  0101左走 0110降下 0111右走
            
            if (g_u8CleanRepeat & 0x02)
            {
                CTRL_AIR_BESOM_DOWN;
                if (g_u8CleanRepeat & 0x04)
                {
                    CTRL_BRUSH_WATER_IN;
                }
                else
                {
                    CTRL_BRUSH_LIQUID_IN;
                }
            }
            else
            {
                CTRL_AIR_BESOM_UP;
            }
            
            
            g_u16UDMoveTicks++;
            if (g_u16UDMoveTicks >= 50)
            {
                g_u16UDMoveTicks = 0;
                g_u8CleanRepeat++;
                CTRL_Print ("g_u8CleanRepeat: %d", g_u8CleanRepeat);
                
                if (g_u8CleanRepeat & 0x02)
                {
                    CTRL_MOTOR_SWEEP_GO_IN;
                }
                else
                {
                    CTRL_MOTOR_SWEEP_GO_OUT;
                }
                g_u8CtrlSteps = CTRL_STEPS_GROUND_CLEAN_2;
            }
            break;
            
        case CTRL_STEPS_GROUND_CLEAN_2:
            if (g_u8CleanRepeat & 0x02)
            {
                if (CTRL_LIMITER_SWEEP1_IN_IS_SET)
                {
                    CTRL_MOTOR_SWEEP1_STOP;
                    g_u8SweepLRFlag |= LIMITER_FLAG_SWEEP_LEFT_IN;
                }
                
                if (CTRL_LIMITER_SWEEP2_IN_IS_SET)
                {
                    CTRL_MOTOR_SWEEP2_STOP;
                    g_u8SweepLRFlag |= LIMITER_FLAG_SWEEP_RIGHT_IN;
                }
                
                if ((CTRL_LIMITER_SWEEP1_IN_IS_SET && CTRL_LIMITER_SWEEP2_IN_IS_SET) || (g_u8SweepLRFlag == (LIMITER_FLAG_SWEEP_LEFT_IN | LIMITER_FLAG_SWEEP_RIGHT_IN)))
                {
                    CTRL_MOTOR_SWEEP1_STOP;
                    CTRL_MOTOR_SWEEP2_STOP;
                    g_u8SweepLRFlag = 0;
                    g_u8CleanRepeat++;
                    CTRL_Print ("g_u8CleanRepeat: %d", g_u8CleanRepeat);
                    if (g_u8CleanRepeat >= 8)
                    {
                        // 结束
                        g_u8CtrlSteps = CTRL_STEPS_IDLE;
                    }
                    else
                    {
                        g_u8CtrlSteps = CTRL_STEPS_GROUND_CLEAN_1;
                    }
                }
            }
            else
            {
                if (CTRL_LIMITER_SWEEP1_OUT_IS_SET)
                {
                    CTRL_MOTOR_SWEEP1_STOP;
                    g_u8SweepLRFlag |= LIMITER_FLAG_SWEEP_LEFT_OUT;
                }
                
                if (CTRL_LIMITER_SWEEP2_OUT_IS_SET)
                {
                    CTRL_MOTOR_SWEEP2_STOP;
                    g_u8SweepLRFlag |= LIMITER_FLAG_SWEEP_RIGHT_OUT;
                }
                
                if ((CTRL_LIMITER_SWEEP1_OUT_IS_SET && CTRL_LIMITER_SWEEP2_OUT_IS_SET) || (g_u8SweepLRFlag == (LIMITER_FLAG_SWEEP_RIGHT_OUT | LIMITER_FLAG_SWEEP_LEFT_OUT)))
                {
                    CTRL_MOTOR_SWEEP1_STOP;
                    CTRL_MOTOR_SWEEP2_STOP;
                    g_u8SweepLRFlag = 0;
                    g_u8CleanRepeat++;
                    CTRL_Print ("g_u8CleanRepeat: %d", g_u8CleanRepeat);
                    g_u8CtrlSteps = CTRL_STEPS_GROUND_CLEAN_1;
                }
            }
            break;
            
            
        case CTRL_STEPS_MOVE_OUT:   // 水平电机移动到最外面
            if (CTRL_LIMITER_HORIZONTAL_OUT_IS_SET) // 到达靠里面的下放位置
            {
                g_u16LRMoveTicks = 0;
                g_u8CleanRepeat = 0;
                g_u16UDMoveTicks = 0;
                CTRL_MOTOR_HORIZONTAL_STOP;
                
                if (DEV_EVT_TEST_MOTOR_VERTICAL == g_u32DevCurrEvt)
                    g_u8CtrlSteps = CTRL_STEPS_TEST_VERTICAL;
                else
                    g_u8CtrlSteps = CTRL_STEPS_CUSHION_DOWN;
            }
            else
            {
                CTRL_MOTOR_HORIZONTAL_OUT;
            }
            break;
            
        case CTRL_STEPS_TEST_HORIZONTAL:
            if (0 == g_u8CleanRepeat)
            {
                if (CTRL_LIMITER_HORIZONTAL_OUT_IS_SET) // 往外移动
                {
                    CTRL_MOTOR_HORIZONTAL_STOP;
                    g_u8CleanRepeat++;
                }
                else
                {
                    CTRL_MOTOR_HORIZONTAL_OUT;
                }
            }
            else
            {
                if (CTRL_LIMITER_HORIZONTAL_IN_IS_SET) // 往内移动
                {
                    CTRL_MOTOR_HORIZONTAL_STOP;
                    g_u8CtrlSteps = CTRL_STEPS_IDLE;
                }
                else
                {
                    CTRL_MOTOR_HORIZONTAL_IN;
                }
            }
            break;
            
        case CTRL_STEPS_TEST_VERTICAL:
            if (0 == g_u8CleanRepeat)
            {
                if (CTRL_LIMITER_VERTICAL_DOWN_IS_SET) // 往外移动
                {
                    CTRL_MOTOR_VERTICAL_STOP;
                    g_u8CleanRepeat++;
                }
                else
                {
                    CTRL_MOTOR_VERTICAL_DOWN;
                }
            }
            else
            {
                if (CTRL_LIMITER_VERTICAL_UP_IS_SET) // 往内移动
                {
                    CTRL_MOTOR_VERTICAL_STOP;
                    g_u8CtrlSteps = CTRL_STEPS_IDLE;
                }
                else
                {
                    CTRL_MOTOR_VERTICAL_UP;
                }
            }
            break;
            
        case CTRL_STEPS_TEST_BRUSH_ROTATE:
            g_u16LRMoveTicks++;
            if (g_u16LRMoveTicks >= CTRL_TEST_BRUSH_ROTATE_TICKS)
            {
                CTRL_MOTOR_BRUSH_STOP;
                g_u8CtrlSteps = CTRL_STEPS_IDLE;
            }
            else
            {
                CTRL_MOTOR_BRUSH_P;
            }
            break;
       
        case CTRL_STEPS_TEST_CLEAN_WATER:
            CTRL_CLEAN_WATER_IN;
            CTRL_BRUSH_WATER_IN;
            g_u16LRMoveTicks = CTRL_TEST_CLEAN_WATER_TICKS;
            g_u8CtrlSteps = CTRL_STEPS_TEST_STOP;
            break;
        
        case CTRL_STEPS_TEST_CLEAN_LIQUID:
            CTRL_CLEAN_LIQUID_IN;
            CTRL_BRUSH_LIQUID_IN;
            g_u16LRMoveTicks = CTRL_TEST_CLEAN_LIQUID_TICKS;
            g_u8CtrlSteps = CTRL_STEPS_TEST_STOP;
            break;
        
        case CTRL_STEPS_TEST_UV:
            UV_ENABLE;
            g_u16LRMoveTicks = CTRL_TEST_UV_TICKS;
            g_u8CtrlSteps = CTRL_STEPS_TEST_STOP;
            break;
        
        case CTRL_STEPS_TEST_CLEAN_AIR:
            CTRL_AIR_DRYING_RUN;
            g_u16LRMoveTicks = CTRL_TEST_CLEAN_AIR_TICKS;
            g_u8CtrlSteps = CTRL_STEPS_TEST_STOP;
            break;
        
        case CTRL_STEPS_TEST_GROUND_MOTOR:
            switch (g_u8CleanRepeat)
            {
                case 0:
                    CTRL_MOTOR_SWEEP_GO_OUT;
                    g_u8CleanRepeat++;
                    break;
                
                case 1:
                    if (CTRL_LIMITER_SWEEP1_OUT_IS_SET)
                    {
                        // 扫把电机1已到位
                        CTRL_MOTOR_SWEEP1_STOP;
                        g_u8SweepLRFlag |= LIMITER_FLAG_SWEEP_LEFT_OUT;
                    }
         
                    if (CTRL_LIMITER_SWEEP2_OUT_IS_SET)
                    {
                        // 扫把电机2已到位
                        CTRL_MOTOR_SWEEP2_STOP;
                        g_u8SweepLRFlag |= LIMITER_FLAG_SWEEP_RIGHT_OUT;
                    }
                
                    if ((CTRL_LIMITER_SWEEP1_OUT_IS_SET && CTRL_LIMITER_SWEEP2_OUT_IS_SET) || (g_u8SweepLRFlag == (LIMITER_FLAG_SWEEP_RIGHT_OUT | LIMITER_FLAG_SWEEP_LEFT_OUT)))
                    {
                        CTRL_MOTOR_SWEEP1_STOP;
                        CTRL_MOTOR_SWEEP2_STOP;
                        g_u8SweepLRFlag = 0;
                        CTRL_MOTOR_SWEEP_GO_IN;
                        g_u8CleanRepeat++;
                    }
                    break;
                    
                case 2:
                    if (CTRL_LIMITER_SWEEP1_IN_IS_SET)
                    {
                        // 扫把电机1已到位
                        CTRL_MOTOR_SWEEP1_STOP;
                        g_u8SweepLRFlag |= LIMITER_FLAG_SWEEP_LEFT_IN;
                    }
         
                    if (CTRL_LIMITER_SWEEP2_IN_IS_SET)
                    {
                        // 扫把电机2已到位
                        CTRL_MOTOR_SWEEP2_STOP;
                        g_u8SweepLRFlag |= LIMITER_FLAG_SWEEP_RIGHT_IN;
                    }
            
                    if ((CTRL_LIMITER_SWEEP1_IN_IS_SET && CTRL_LIMITER_SWEEP2_IN_IS_SET) || (g_u8SweepLRFlag == (LIMITER_FLAG_SWEEP_LEFT_IN | LIMITER_FLAG_SWEEP_RIGHT_IN)))
                    {
                        CTRL_MOTOR_SWEEP1_STOP;
                        CTRL_MOTOR_SWEEP2_STOP;
                        g_u8CtrlSteps = CTRL_STEPS_IDLE;
                        g_u8SweepLRFlag = 0;
                    }
                    break;
                
            }
            break;
        
        case CTRL_STEPS_TEST_GROUND_AIR:  
            CTRL_AIR_BESOM_UP;
            g_u16LRMoveTicks = CTRL_TEST_GROUND_AIR_TICKS;
            g_u8CtrlSteps = CTRL_STEPS_TEST_STOP;
            break;
        
        case CTRL_STEPS_TEST_GROUND_WATER:
            CTRL_GROUND_WATER_IN;
            g_u16LRMoveTicks = CTRL_TEST_GROUND_WATER_TICKS;
            g_u8CtrlSteps = CTRL_STEPS_TEST_STOP;
            break;
        
        case CTRL_STEPS_TEST_GROUND_LIQUID:
            CTRL_GROUND_LIQUID_IN;
            g_u16LRMoveTicks = CTRL_TEST_GROUND_LIQUID_TICKS;
            g_u8CtrlSteps = CTRL_STEPS_TEST_STOP;
            break;
        
        case CTRL_STEPS_TEST_STOP:
            if (g_u16LRMoveTicks)
                g_u16LRMoveTicks--;
            
            if (0 == g_u16LRMoveTicks)
            {
                CTRL_BRUSH_LIQUID_STOP;
                CTRL_BRUSH_WATER_STOP;
                CTRL_CLEAN_LIQUID_STOP;
                CTRL_CLEAN_WATER_STOP;
                CTRL_GROUND_LIQUID_STOP;
                CTRL_GROUND_WATER_STOP;
                CTRL_AIR_BESOM_DOWN;
                CTRL_AIR_CUSHION_IN;
                CTRL_AIR_DRYING_STOP;
                UV_DISABLE;
                g_u8CtrlSteps = CTRL_STEPS_IDLE;
            }
            break;
    }
}
