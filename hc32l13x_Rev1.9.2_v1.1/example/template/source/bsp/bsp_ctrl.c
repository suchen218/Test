#include "bsp_ctrl.h"

#if 1//CONFIG_RTT_SPI_ENABLED
#define CTRL_Print(M, ...)	LOG_Print("CTRL", M, ##__VA_ARGS__)
#else
#define CTRL_Print(M, ...)
#endif

uint8_t     g_u8SweepLRFlag;



// ˢ�� 2�ŵ��
#define CTRL_MOTOR_BRUSH_P                      bsp_Motor2DirSet (MOTOR_DIR_P) 
#define CTRL_MOTOR_BRUSH_N                      bsp_Motor2DirSet (MOTOR_DIR_N) 
#define CTRL_MOTOR_BRUSH_STOP                   bsp_Motor2Stop () 

// ϴ��Ͱ��ˮ
#define CTRL_CLEAN_WATER_IN
#define CTRL_CLEAN_WATER_STOP

// ϴ��Ͱ��Һ
#define CTRL_CLEAN_LIQUID_IN 
#define CTRL_CLEAN_LIQUID_STOP

// ϴ�ذ��ˮ
#define CTRL_GROUND_WATER_IN
#define CTRL_GROUND_WATER_STOP

// ϴ�ذ��Һ
#define CTRL_GROUND_LIQUID_IN 
#define CTRL_GROUND_LIQUID_STOP

// ����ϴ��ˮ
#define CTRL_BRUSH_WATER_IN
#define CTRL_BRUSH_WATER_STOP

// ����ϴ��Һ
#define CTRL_BRUSH_LIQUID_IN 
#define CTRL_BRUSH_LIQUID_STOP

// ����̧��ɨ��
#define CTRL_AIR_BESOM_UP                   //AIR_11_ENABLE
// ���׷���ɨ��
#define CTRL_AIR_BESOM_DOWN                 //AIR_11_DISABLE

// �������������̧��
#define CTRL_AIR_CUSHION_IN

// ������������治��̧��
#define CTRL_AIR_CUSHION_OUT

// ϴ����Ͱ����
#define CTRL_AIR_DRYING_RUN     
#define CTRL_AIR_DRYING_STOP

#define CTRL_PIR_NO_HUMAN           1
#define CTRL_CICTERN_IS_NORMAL      1
#define CTRL_CICTERN_IS_PAUSE       1





// ��ǰ�������̵���һ��
uint8_t     g_u8CtrlSteps = CTRL_STEPS_FIRST;

// �豸�����¼�
uint32_t    g_u32DevAllEvt = DEV_EVT_CLEAN_COMPLETE;

// �豸��ǰִ���¼�
uint32_t    g_u32DevCurrEvt;

#define SECONDS_TICKS   (50)

// ������������������Ų��
#define CTRL_HORIZONTAL_OUT_FIRST_TICKS    (46 * SECONDS_TICKS)  
// �������ڲ�ˢϴŲ��
#define CTRL_HORIZONTAL_OUT_SECOND_TICKS    (2 * SECONDS_TICKS)
// �����������ϴʱ��
#define CTRL_CLEAN_BRUSH_TICKS          (10 * SECONDS_TICKS)
// ����ز�����ϴ�����߶�ʱ��
#define CTRL_CLEAN_UP_TICKS          (4 * SECONDS_TICKS)
// ��ϴ�����ѭ������
#define CTRL_CLEAN_REPEAT           (2)
// ����ʱ��
#define CTRL_CLEAN_UV_TICKS               (10 * SECONDS_TICKS)
// ����ϴˢ�Ӱ�����ʱ��
#define CTRL_MYSELF_BRUSH_TICKS     (30 * SECONDS_TICKS)  

// �������
// ˢ����ת
#define CTRL_TEST_BRUSH_ROTATE_TICKS    (60 * SECONDS_TICKS)   
// ��ϴ����ˮ
#define CTRL_TEST_CLEAN_WATER_TICKS     (60 * SECONDS_TICKS)
// ��ϴ����Һ
#define CTRL_TEST_CLEAN_LIQUID_TICKS    (30 * SECONDS_TICKS)
// UV����
#define CTRL_TEST_UV_TICKS              (30 * SECONDS_TICKS)
// ��ϴ������
#define CTRL_TEST_CLEAN_AIR_TICKS       (30 * SECONDS_TICKS)
// ������ˮ
#define CTRL_TEST_GROUND_WATER_TICKS    (60 * SECONDS_TICKS)
// ������Һ
#define CTRL_TEST_GROUND_LIQUID_TICKS   (30 * SECONDS_TICKS)
// ��������
#define CTRL_TEST_GROUND_AIR_TICKS   (2 * SECONDS_TICKS)
// ˢ����ˮ
#define CTRL_TEST_BRUSH_WATER_TICKS    (60 * SECONDS_TICKS)
// ˢ����Һ
#define CTRL_TEST_BRUSH_LIQUID_TICKS   (30 * SECONDS_TICKS)

// �����ƶ��ļ�ʱ����
uint16_t    g_u16LRMoveTicks;
// �����ƶ��ļ�ʱ����
uint16_t    g_u16UDMoveTicks;

// ��ϴ�ļ�ʱ����
uint16_t    g_u16CleanTicks;
// ��ϴ�Ĵ���
uint8_t     g_u8CleanRepeat;
// ��ϴ��ͣ�Ĵ���
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
// �����ƶ��ļ�ʱ����
    g_u16UDMoveTicks = 0;
// ��ϴ�ļ�ʱ����
    g_u16CleanTicks = 0;
// ��ϴ�Ĵ���
    g_u8CleanRepeat = 0;
// ��ϴ��ͣ�Ĵ���
    g_u8CleanPause = 0;
    
    switch (g_u32DevCurrEvt)
    {
        case DEV_EVT_CLEAN_COMPLETE:// = 0x01,
//    ������ϴ ϴ��Ͱ ϴëˢ �ϵ�
        case DEV_EVT_CLEAN_FAST:// = 0x02,
            g_u8CtrlSteps = CTRL_STEPS_HORIZONTAL_READY_OUT;
            break;
//    ����ϴ   ϴëˢ
        case DEV_EVT_CLEAN_MYSELF:// = 0x04,
            g_u8CtrlSteps = CTRL_STEPS_BRUSH_MYSELF;
            break;
//    ����
        case DEV_EVT_CLEAN_UV:// = 0x08,
            g_u8CtrlSteps = CTRL_STEPS_MOVE_OUT;
            break;
//    ������ϴ
        case DEV_EVT_CLEAN_GROUND:// = 0x10,
            g_u8CtrlSteps = CTRL_STEPS_GROUND_CLEAN_1;
            break;
//    
//    ��ϴ�ֹ���Ƴ��ջؼ��
        case DEV_EVT_TEST_MOTOR_HORIZONTAL:// = 0x20,
            g_u8CtrlSteps = CTRL_STEPS_TEST_HORIZONTAL;
            break;
//    ��ϴ�ֹ�����¼��
        case DEV_EVT_TEST_MOTOR_VERTICAL:// = 0x40,
            g_u8CtrlSteps = CTRL_STEPS_MOVE_OUT;
            break;
//    ��ϴ��ëˢ��ת���
        case DEV_EVT_TEST_MOTOR_BRUSH:// = 0x80,
            g_u8CtrlSteps = CTRL_STEPS_TEST_BRUSH_ROTATE;
            break;
//    ��ϴ����ˮ���
        case DEV_EVT_TEST_WATER_CLEAN:// = 0x100,
            g_u8CtrlSteps = CTRL_STEPS_TEST_CLEAN_WATER;
            break;
//    ��ϴ����Һ���
        case DEV_EVT_TEST_LIQUID_CLEAN:// = 0x200,
            g_u8CtrlSteps = CTRL_STEPS_TEST_CLEAN_LIQUID;
            break;
//    ��ϴ������Ƽ��
        case DEV_EVT_TEST_UV_CLEAN:// = 0x400,
            g_u8CtrlSteps = CTRL_STEPS_TEST_UV;
            break;
//    ��ϴ���������
        case DEV_EVT_TEST_AIR_CLEAN:// = 0x800,
            g_u8CtrlSteps = CTRL_STEPS_TEST_CLEAN_AIR;
            break;
//    ������ϴ�Ĺ�����
        case DEV_EVT_TEST_GROUND_MOTOR:// = 0x1000,
            g_u8CtrlSteps = CTRL_STEPS_TEST_GROUND_MOTOR;
            break;
//    ������ϴ����ѹ�˼��
        case DEV_EVT_TEST_GROUND_AIR:// = 0x2000,
            g_u8CtrlSteps = CTRL_STEPS_TEST_GROUND_AIR;
            break;
//    ������ϴ����ˮ���
        case DEV_EVT_TEST_GROUND_WATER:// = 0x4000,
            g_u8CtrlSteps = CTRL_STEPS_TEST_GROUND_WATER;
            break;
//    ������ϴ����Һ���
        case DEV_EVT_TEST_GROUND_LIQUID:// = 0x8000,
            g_u8CtrlSteps = CTRL_STEPS_TEST_GROUND_LIQUID;
            break;
////    ˢ����ˮ
//        case DEV_EVT_TEST_BRUSH_WATER:
//            g_u8CtrlSteps = CTRL_STEPS_TEST_BRUSH_WATER;
//            break;
////    ˢ����Һ   
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
            if (CTRL_LIMITER_VERTICAL_UP_IS_SET)    // ����������
            {
                CTRL_MOTOR_VERTICAL_STOP;
                g_u8CtrlSteps = CTRL_STEPS_SECOND;    // �Ѿ�����������һ���ջ���ϴ̨
            }
            else
            {
                CTRL_MOTOR_VERTICAL_UP;     // ��������
            }
            break;
        
        case CTRL_STEPS_SECOND:
            if (CTRL_LIMITER_HORIZONTAL_IN_IS_SET)  // ��ϴ̨�Ѿ��ջ�
            {
                CTRL_MOTOR_HORIZONTAL_STOP;
                g_u8CtrlSteps = CTRL_STEPS_THIRD;
                
                if ((FALSE == CTRL_LIMITER_SWEEP1_IN_IS_SET) || (FALSE == CTRL_LIMITER_SWEEP2_IN_IS_SET))
                {
                    // ̧������
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
            
        case CTRL_STEPS_THIRD:  // ϴ�ذ�ˢ�ӻ���
            // ɨ������������Ѿ�����
            if (CTRL_LIMITER_SWEEP1_IN_IS_SET)
            {
                // ɨ�ѵ��1�ѵ�λ
                CTRL_MOTOR_SWEEP1_STOP;
                g_u8SweepLRFlag |= LIMITER_FLAG_SWEEP_LEFT_IN;
            }
 
            if (CTRL_LIMITER_SWEEP2_IN_IS_SET)
            {
                // ɨ�ѵ��2�ѵ�λ
                CTRL_MOTOR_SWEEP2_STOP;
                g_u8SweepLRFlag |= LIMITER_FLAG_SWEEP_RIGHT_IN;
            }
            
            if ((CTRL_LIMITER_SWEEP1_IN_IS_SET && CTRL_LIMITER_SWEEP2_IN_IS_SET) || (g_u8SweepLRFlag == (LIMITER_FLAG_SWEEP_LEFT_IN | LIMITER_FLAG_SWEEP_RIGHT_IN)))
            {
                CTRL_MOTOR_SWEEP1_STOP;
                CTRL_MOTOR_SWEEP2_STOP;
                g_u8CtrlSteps = CTRL_STEPS_IDLE;
                g_u8SweepLRFlag = 0;
                // ���׷���ɨ��
//                CTRL_AIR_BESOM_DOWN;
            }
            break;
            
        case CTRL_STEPS_IDLE:
            // PIRδ��Ӧ���ˣ�ˮ������  ˮ��δ����
//            if (CTRL_PIR_NO_HUMAN && CTRL_CICTERN_IS_NORMAL && CTRL_CICTERN_IS_PAUSE)
            {
                // ���¼�
                if (g_u32DevAllEvt)
                {
                    bsp_CtrlEvtGet ();  // ��ȡ��ǰ��ִ���¼�
                    bsp_CtrlEvtJump (); // ��ת����ִ���¼�
                }
            }
            break;
            
        case CTRL_STEPS_HORIZONTAL_READY_OUT: // ˮƽ�������Ų��׼��ϴ��Ͱ
            g_u16LRMoveTicks++;
            if ((g_u16LRMoveTicks >= CTRL_HORIZONTAL_OUT_FIRST_TICKS) || (CTRL_LIMITER_HORIZONTAL_OUT_IS_SET)) // ���￿������·�λ��
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
            
        case CTRL_STEPS_VERITAL_READY_DOWN:     // ����ˢ��
            CTRL_MOTOR_BRUSH_P;     // ˢ�ӿ�ʼ����
        
            if (g_u8CleanRepeat & 0x01)
            {
                CTRL_CLEAN_WATER_IN;
            }
            else
            {
                CTRL_CLEAN_LIQUID_IN;   // �����Һ
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
            
        case CTRL_STEPS_CLEAN_DELAY:   // ͣ�ڴ˴���Һ��ˮ
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
            
        case CTRL_STEPS_BRUSH_MOVE:   // Ӧ���������ȴ�ʱ��
            // ˮƽ�����Ѿ����������
            switch (g_u8CleanPause)
            {
                case 1: // ����
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
                
                case 2: // ��������
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
                
                case 3: // ���һ��ˣ�������һ���ֻ�
                    CTRL_MOTOR_BRUSH_STOP;
                    CTRL_CLEAN_WATER_STOP;
                    CTRL_CLEAN_LIQUID_STOP;
                
//                    CTRL_Print ("g_u16LRMoveTicks: %d,%d", g_u16LRMoveTicks, g_u8CleanRepeat);
                
                    
                    if (g_u8CleanRepeat >= CTRL_CLEAN_REPEAT)
                    {
                        // ����
                        g_u8CtrlSteps = CTRL_STEPS_CUSHION_DOWN;
                    }
                    else
                    {
                        // ���˵���ʱ
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
            
        case CTRL_STEPS_CUSHION_DOWN: // ��Ͱ��ϴ��������������

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
            
        case CTRL_STEPS_CUSHION_DELAY:  // �ȴ�С��ʱ�䣬ʹ������ȫ������ѹס
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
            
        case CTRL_STEPS_UV: // UV����
            UV_ENABLE;
            g_u16UDMoveTicks++;
            if (g_u16UDMoveTicks >= CTRL_CLEAN_UV_TICKS)
            {
                g_u16UDMoveTicks = 0;
                UV_DISABLE;
                g_u8CtrlSteps = CTRL_STEPS_BRUSH_GO_IN_1;
            }
            break;
            
        case CTRL_STEPS_BRUSH_GO_IN_1:      // ��ֱ�������׼�����ز���  
            if (CTRL_LIMITER_VERTICAL_UP_IS_SET)
            {
                CTRL_MOTOR_VERTICAL_STOP;
                CTRL_AIR_CUSHION_IN;
                CTRL_AIR_DRYING_RUN;    // ����
                g_u16UDMoveTicks = 0;
                g_u8CtrlSteps = CTRL_STEPS_BRUSH_GO_IN_2;
            }
            else
            {
                CTRL_MOTOR_VERTICAL_UP;
            }
            break;
            
        case CTRL_STEPS_BRUSH_GO_IN_2:  // ˮƽ������ƣ����ز���
            if (CTRL_LIMITER_HORIZONTAL_IN_IS_SET) // ���￿������·�λ��
            {
                g_u16LRMoveTicks = 0;
                g_u8CleanRepeat = 0;
                g_u16UDMoveTicks = 0;
                CTRL_AIR_CUSHION_IN;
                CTRL_MOTOR_HORIZONTAL_STOP;
                CTRL_AIR_DRYING_STOP;
                
                // ��ֻ���������򷵻� ����״̬������Ҫ��ϴˢ��
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

        case CTRL_STEPS_BRUSH_MYSELF: // ����ϴ���迪ʼ
            CTRL_MOTOR_BRUSH_P;
            g_u16UDMoveTicks++;
            if (g_u16UDMoveTicks >= CTRL_MYSELF_BRUSH_TICKS)
            {
                g_u16UDMoveTicks = 0;
                g_u8CleanRepeat++;
            }
            
            switch (g_u8CleanRepeat)
            {
                case 0: // ��һ���Һ
                    CTRL_BRUSH_LIQUID_IN;
                    break;
                
                case 1: // �ڶ����ˮ
                    CTRL_BRUSH_WATER_IN;
                    break;
                
                case 2: // ����
                    CTRL_BRUSH_WATER_STOP;
                    CTRL_MOTOR_BRUSH_STOP;
                    g_u8CleanRepeat = 0;
                
                    // ����ϴ���˽��������������������ϴ��Ҫϴ�ذ�
                    if (g_u32DevCurrEvt == DEV_EVT_CLEAN_MYSELF)
                        g_u8CtrlSteps = CTRL_STEPS_IDLE;
                    else
                        g_u8CtrlSteps = CTRL_STEPS_GROUND_CLEAN_1;
                    break;
            }
            break;
            
        case CTRL_STEPS_GROUND_CLEAN_1: // ϴ�ذ�
            // 0000̧��  0001���� 0010���� 0011����
            // 0100̧��  0101���� 0110���� 0111����
            
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
                        // ����
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
            
            
        case CTRL_STEPS_MOVE_OUT:   // ˮƽ����ƶ���������
            if (CTRL_LIMITER_HORIZONTAL_OUT_IS_SET) // ���￿������·�λ��
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
                if (CTRL_LIMITER_HORIZONTAL_OUT_IS_SET) // �����ƶ�
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
                if (CTRL_LIMITER_HORIZONTAL_IN_IS_SET) // �����ƶ�
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
                if (CTRL_LIMITER_VERTICAL_DOWN_IS_SET) // �����ƶ�
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
                if (CTRL_LIMITER_VERTICAL_UP_IS_SET) // �����ƶ�
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
                        // ɨ�ѵ��1�ѵ�λ
                        CTRL_MOTOR_SWEEP1_STOP;
                        g_u8SweepLRFlag |= LIMITER_FLAG_SWEEP_LEFT_OUT;
                    }
         
                    if (CTRL_LIMITER_SWEEP2_OUT_IS_SET)
                    {
                        // ɨ�ѵ��2�ѵ�λ
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
                        // ɨ�ѵ��1�ѵ�λ
                        CTRL_MOTOR_SWEEP1_STOP;
                        g_u8SweepLRFlag |= LIMITER_FLAG_SWEEP_LEFT_IN;
                    }
         
                    if (CTRL_LIMITER_SWEEP2_IN_IS_SET)
                    {
                        // ɨ�ѵ��2�ѵ�λ
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
