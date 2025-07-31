/*
 * Copyright (c) 2021, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#include "main.h"
#include "Grayscale_Sensor.h"
#include "stdio.h"
#include "AT8236.h"
#define TIME_PER_ROUND 18 //s

No_MCU_Sensor sensor;
char Gray_rx_buff[256] = "";
unsigned short Anolog[8] = {0};
unsigned short white[8] = {2724,2811,3238,3169,3162,3220,3125,2842};
unsigned short black[8] = {1183,1433,1887,1463,1347,1512,1145,780};
unsigned short Normal[8];
unsigned char Digtal;
uint32_t counter_10ms = 0;


void Delay_ms(uint32_t ms)
{
    while(ms --)
    delay_cycles(32000);
}

void UART_sendString(UART_Regs *uart, const char *str)
{
    if (str == NULL)                    /* 防御性检查 */
        return;

    while (*str != '\0') {
        /* 等待 TX FIFO 非满（可选，DriverLib 也提供中断/DMA方式） */
        while (DL_UART_isTXFIFOFull(uart)) {
            /* busy-wait，也可用 WFI 省电 */
        }
        DL_UART_Main_transmitData(uart, *str++);
    }
}

/***********ADC********/

int i = 0;
int main(void)
{
    SYSCFG_DL_init();
    DL_ADC12_startConversion(ADC12_0_INST);
    NVIC_EnableIRQ(TIM_GAP_INST_INT_IRQN);
    MotorInit();
    /**********************灰度传感器初始化**********************/

    //初始化传感器，不带黑白
    No_MCU_Ganv_Sensor_Init_Frist(&sensor); //结构体归零
    No_Mcu_Ganv_Sensor_Task_Without_tick(&sensor); //无校准读取数据
    Get_Anolog_Value(&sensor, Anolog);
    sprintf(Gray_rx_buff, "Anolog %d,%d,%d,%d,%d,%d,%d,%d\r\n", Anolog[0], Anolog[1], Anolog[2], Anolog[3], Anolog[4],
            Anolog[5], Anolog[6], Anolog[7]);
    UART_sendString(UART_0_INST, Gray_rx_buff);
    Delay_ms(100);

    //得到黑白校准值之后，初始化传感器
    No_MCU_Ganv_Sensor_Init(&sensor, white, black);
    Delay_ms(100);

    DL_TimerA_startCounter(TIM_GAP_INST);
    while(1)
    {
        
    }
}

unsigned short adc_getValue() 
{
    uint16_t ADC_Val = DL_ADC12_getMemResult(ADC12_0_INST, DL_ADC12_MEM_IDX_0);
    return ADC_Val;
}

void TIM_GAP_INST_IRQHandler(void)
{
    switch (DL_TimerA_getPendingInterrupt(TIM_GAP_INST)) {
        case DL_TIMER_IIDX_ZERO:
            counter_10ms ++;
            No_Mcu_Ganv_Sensor_Task_Without_tick(&sensor);
            Digtal = Get_Digtal_For_User(&sensor);
            GrayControl();
            i++;
            if(i>=100)
            { 
                i = 0;
                sprintf(Gray_rx_buff, "Digtal %d-%d-%d-%d-%d-%d-%d-%d\r\n", (Digtal >> 0) & 0x01, (Digtal >> 1) & 0x01,
                (Digtal >> 2) & 0x01, (Digtal >> 3) & 0x01, (Digtal >> 4) & 0x01, (Digtal >> 5) & 0x01,
                (Digtal >> 6) & 0x01, (Digtal >> 7) & 0x01);

               UART_sendString(UART_0_INST, Gray_rx_buff);
            }
            break;
        default:
            break;
    }
}

// void ADC0_7_INST_IRQHandler(void)
// {
//     switch (DL_ADC12_getPendingInterrupt(ADC0_7_INST)) 
//     {
//         case DL_ADC12_IIDX_MEM0_RESULT_LOADED:
//             ADC_Flag = true;
//             break;
//         default:
//             break;
//     }
// }
