/******************************************************************************
* Copyright (C) 2017, Huada Semiconductor Co.,Ltd All rights reserved.
*
* This software is owned and published by:
* Huada Semiconductor Co.,Ltd ("HDSC").
*
* BY DOWNLOADING, INSTALLING OR USING THIS SOFTWARE, YOU AGREE TO BE BOUND
* BY ALL THE TERMS AND CONDITIONS OF THIS AGREEMENT.
*
* This software contains source code for use with HDSC
* components. This software is licensed by HDSC to be adapted only
* for use in systems utilizing HDSC components. HDSC shall not be
* responsible for misuse or illegal use of this software for devices not
* supported herein. HDSC is providing this software "AS IS" and will
* not be responsible for issues arising from incorrect user implementation
* of the software.
*
* Disclaimer:
* HDSC MAKES NO WARRANTY, EXPRESS OR IMPLIED, ARISING BY LAW OR OTHERWISE,
* REGARDING THE SOFTWARE (INCLUDING ANY ACOOMPANYING WRITTEN MATERIALS),
* ITS PERFORMANCE OR SUITABILITY FOR YOUR INTENDED USE, INCLUDING,
* WITHOUT LIMITATION, THE IMPLIED WARRANTY OF MERCHANTABILITY, THE IMPLIED
* WARRANTY OF FITNESS FOR A PARTICULAR PURPOSE OR USE, AND THE IMPLIED
* WARRANTY OF NONINFRINGEMENT.
* HDSC SHALL HAVE NO LIABILITY (WHETHER IN CONTRACT, WARRANTY, TORT,
* NEGLIGENCE OR OTHERWISE) FOR ANY DAMAGES WHATSOEVER (INCLUDING, WITHOUT
* LIMITATION, DAMAGES FOR LOSS OF BUSINESS PROFITS, BUSINESS INTERRUPTION,
* LOSS OF BUSINESS INFORMATION, OR OTHER PECUNIARY LOSS) ARISING FROM USE OR
* INABILITY TO USE THE SOFTWARE, INCLUDING, WITHOUT LIMITATION, ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL OR CONSEQUENTIAL DAMAGES OR LOSS OF DATA,
* SAVINGS OR PROFITS,
* EVEN IF Disclaimer HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* YOU ASSUME ALL RESPONSIBILITIES FOR SELECTION OF THE SOFTWARE TO ACHIEVE YOUR
* INTENDED RESULTS, AND FOR THE INSTALLATION OF, USE OF, AND RESULTS OBTAINED
* FROM, THE SOFTWARE.
*
* This software may be replicated in part or whole for the licensed use,
* with the restriction that this Disclaimer and Copyright notice must be
* included with each copy of this software, whether used in part or whole,
* at all times.
*/
/******************************************************************************/
/** \file main.c
 **
 ** A detailed description is available at
 ** @link Sample Group Some description @endlink
 **
 **   - 2019-04-19 Husj    First Version
 **
 ******************************************************************************/

/******************************************************************************
 * Include files
 ******************************************************************************/
#include "adt.h"
#include "gpio.h"
#include "bt.h"

/******************************************************************************
 * Local pre-processor symbols/macros ('#define')                            
 ******************************************************************************/


/******************************************************************************
 * Global variable definitions (declared in header file with 'extern')
 ******************************************************************************/
volatile uint16_t u16TIM6_HW_Sync_Cnt;

/******************************************************************************
 * Local type definitions ('typedef')                                         
 ******************************************************************************/

/******************************************************************************
 * Local function prototypes ('static')
 ******************************************************************************/


/******************************************************************************
 * Local variable definitions ('static')                                      *
 ******************************************************************************/


/*****************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/
///< Timer4 ??????
 void Tim4_IRQHandler(void)
{
    if(TRUE == Adt_GetIrqFlag(M0P_ADTIM4, AdtCMAIrq))
    { 
        u16TIM6_HW_Sync_Cnt = Adt_GetCount(M0P_ADTIM6);
        Adt_ClearIrqFlag(M0P_ADTIM4, AdtCMAIrq);
        Adt_StopCount(M0P_ADTIM4);
    }
}

///< Timer5 ??????
void Tim5_IRQHandler(void)
{
    if(TRUE == Adt_GetIrqFlag(M0P_ADTIM5, AdtCMAIrq))
    { 
        u16TIM6_HW_Sync_Cnt = Adt_GetCount(M0P_ADTIM6);
        Adt_ClearIrqFlag(M0P_ADTIM5, AdtCMAIrq);
        Adt_StopCount(M0P_ADTIM5);
    }
}

///< Timer0 ??????
void Tim0_IRQHandler(void)
{
    if(TRUE == Bt_GetIntFlag(TIM0, BtUevIrq))
    {
        u16TIM6_HW_Sync_Cnt = Adt_GetCount(M0P_ADTIM6);
        Bt_ClearIntFlag(TIM0,BtUevIrq);
        Bt_M0_Stop(TIM0);
    }
}

///< AdvTimer4?????????
void App_AdvTimer4Init(void)
{
    uint16_t                u16Period;
    uint16_t                u16GCMAValue;
    stc_adt_basecnt_cfg_t   stcAdtBaseCntCfg;
    stc_adt_irq_trig_cfg_t  stcAdtIrqTrigCfg;

    DDL_ZERO_STRUCT(stcAdtBaseCntCfg);
    DDL_ZERO_STRUCT(stcAdtIrqTrigCfg);

    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralAdvTim, TRUE); //ADT??????????????????
    
    stcAdtBaseCntCfg.enCntMode = AdtSawtoothMode;
    stcAdtBaseCntCfg.enCntDir = AdtCntUp;
    stcAdtBaseCntCfg.enCntClkDiv = AdtClkPClk0;
    Adt_Init(M0P_ADTIM4, &stcAdtBaseCntCfg);                   //ADT????????????????????????????????????

    u16Period = 0xF000;
    Adt_SetPeriod(M0P_ADTIM4, u16Period);                      //????????????
    
    u16GCMAValue = 0x6000;
    Adt_SetCompareValue(M0P_ADTIM4, AdtCompareA, u16GCMAValue);     //??????????????????????????????A??????
    
    Adt_ClearAllIrqFlag(M0P_ADTIM4);
    Adt_CfgIrq(M0P_ADTIM4, AdtCMAIrq, TRUE);  //??????TIM4????????????
    EnableNvic(ADTIM4_IRQn, IrqLevel3, TRUE);
    
    stcAdtIrqTrigCfg.bAdtCntMatchATrigEn = 1;
    Adt_IrqTrigCfg(M0P_ADTIM4, &stcAdtIrqTrigCfg);               //TIM4 ????????????A??????AOS????????????
}

///< AdvTimer5?????????
void App_AdvTimer5Init(void)
{
    uint16_t                u16Period;
    uint16_t                u16GCMAValue;
    stc_adt_basecnt_cfg_t   stcAdtBaseCntCfg;
    stc_adt_irq_trig_cfg_t  stcAdtIrqTrigCfg;

    
    DDL_ZERO_STRUCT(stcAdtBaseCntCfg);
    DDL_ZERO_STRUCT(stcAdtIrqTrigCfg);

    
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralAdvTim, TRUE); //ADT??????????????????
 
    
    stcAdtBaseCntCfg.enCntMode = AdtSawtoothMode;
    stcAdtBaseCntCfg.enCntDir = AdtCntUp;
    stcAdtBaseCntCfg.enCntClkDiv = AdtClkPClk0;
    Adt_Init(M0P_ADTIM5, &stcAdtBaseCntCfg);                       //ADT????????????????????????????????????

    u16Period = 0xF000;
    Adt_SetPeriod(M0P_ADTIM5, u16Period);                           //????????????
    
    
    u16GCMAValue = 0xE000;
    Adt_SetCompareValue(M0P_ADTIM5, AdtCompareA, u16GCMAValue);     //??????????????????????????????A??????
    
    Adt_ClearAllIrqFlag(M0P_ADTIM5);
    Adt_CfgIrq(M0P_ADTIM5, AdtCMAIrq, TRUE);  //??????TIM5????????????
    EnableNvic(ADTIM5_IRQn, IrqLevel3, TRUE);
    
    stcAdtIrqTrigCfg.bAdtCntMatchATrigEn = 1;
    Adt_IrqTrigCfg(M0P_ADTIM5, &stcAdtIrqTrigCfg);               //TIM5 ????????????A??????AOS????????????
}

///< AdvTimer6?????????
void App_AdvTimer6Init(void)
{
    uint16_t                u16Period;
    stc_adt_basecnt_cfg_t   stcAdtBaseCntCfg;
    stc_adt_aos_trig_cfg_t  stcAdtAosTrigCfg;
    
    DDL_ZERO_STRUCT(stcAdtBaseCntCfg);
    DDL_ZERO_STRUCT(stcAdtAosTrigCfg);
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralAdvTim, TRUE); //ADT??????????????????
    
    stcAdtBaseCntCfg.enCntMode = AdtSawtoothMode;
    stcAdtBaseCntCfg.enCntDir = AdtCntUp;
    stcAdtBaseCntCfg.enCntClkDiv = AdtClkPClk0;
    Adt_Init(M0P_ADTIM6, &stcAdtBaseCntCfg);                        //ADT????????????????????????????????????

    u16Period = 0xF000;
    Adt_SetPeriod(M0P_ADTIM6, u16Period);                           //????????????
    
    stcAdtAosTrigCfg.enAos0TrigSrc = AdtAosxTrigSelTim0Int;      //AOS0??????TIM0??????
    stcAdtAosTrigCfg.enAos1TrigSrc = AdtAosxTrigSelTim4Int;      //AOS1??????TIM4
    stcAdtAosTrigCfg.enAos2TrigSrc = AdtAosxTrigSelTim5Int;      //AOS2??????TIM5
    Adt_AosTrigCfg(&stcAdtAosTrigCfg);
        
    Adt_CfgHwStart(M0P_ADTIM6, AdtHwTrigAos0);                   //AOS0????????????TIM6??????
    Adt_CfgHwStop(M0P_ADTIM6, AdtHwTrigAos1);                    //AOS1?????? ??????TIM6
    Adt_CfgHwClear(M0P_ADTIM6, AdtHwTrigAos2);                   //AOS2?????? ??????TIM6

    Adt_EnableHwStart(M0P_ADTIM6);                                  //??????????????????
    Adt_EnableHwStop(M0P_ADTIM6);                                   //??????????????????
    Adt_EnableHwClear(M0P_ADTIM6);                                  //??????????????????
}

///< Timer0?????????
void App_Timer0Init(void)
{
    stc_bt_mode0_cfg_t   stcBtBaseCfg;

    DDL_ZERO_STRUCT(stcBtBaseCfg);
    
    //??????BT,GPIO????????????
    Sysctrl_SetPeripheralGate(SysctrlPeripheralBaseTim, TRUE); //Base Timer??????????????????
    
    stcBtBaseCfg.enWorkMode = BtWorkMode0;                  //???????????????
    stcBtBaseCfg.enCT       = BtTimer;                      //???????????????????????????????????????PCLK
    stcBtBaseCfg.enPRS      = BtPCLKDiv1;                   //PCLK
    stcBtBaseCfg.enCntMode  = Bt16bitArrMode;               //????????????16????????????/?????????
    stcBtBaseCfg.bEnTog     = FALSE;
    stcBtBaseCfg.bEnGate    = FALSE;
    stcBtBaseCfg.enGateP    = BtGatePositive;
    Bt_Mode0_Init(TIM0, &stcBtBaseCfg);                     //TIM0 ?????????0???????????????
    
    Bt_M0_ARRSet(TIM0, 0xFF00);                             //???????????????(?????? = 0x10000 - ARR)
    Bt_M0_Cnt16Set(TIM0, 0xFF00);                           //??????????????????
    
    Bt_ClearIntFlag(TIM0,BtUevIrq);                         //???????????????   
    Bt_Mode0_EnableIrq(TIM0);                               //??????TIM0??????(??????0?????????????????????)
    EnableNvic(TIM0_IRQn, IrqLevel3, TRUE);                 //TIM0????????????
}

/**
 ******************************************************************************
 ** \brief  Main function of project
 **
 ** \return uint32_t return value, if needed
 **
 ** This sample
 **
 ******************************************************************************/
int32_t main(void)
{
    stc_adt_sw_sync_t       stcAdtSwSync;

    DDL_ZERO_STRUCT(stcAdtSwSync);
    

    App_AdvTimer4Init();  //AdvTimer4?????????
    
    App_AdvTimer5Init();  //AdvTimer5?????????
    
    App_AdvTimer6Init();  //AdvTimer6?????????
    
    App_Timer0Init();     //Timer0?????????
    
    
    stcAdtSwSync.bAdTim4 = TRUE;    //AdvTimer4 ??????????????????
    stcAdtSwSync.bAdTim5 = TRUE;    //AdvTimer4 ??????????????????
    
    Bt_M0_Run(TIM0);                //TIM0 ??????
     
    Adt_SwSyncStart(&stcAdtSwSync); //AdvTimer ??????????????????
    
    while(1);
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/


