#include "AT8236.h"

extern unsigned char Digtal;
uint8_t Turn_Index = 0;
#define FORWARD_SPEED 0.2
void MotorInit(void)
{
    DL_TimerG_startCounter(PWM_0_INST);
    DL_TimerG_setCaptureCompareValue(PWM_0_INST, 0, DL_TIMER_CC_0_INDEX);
    DL_TimerG_setCaptureCompareValue(PWM_0_INST, 0, DL_TIMER_CC_1_INDEX);
}

void MotorTestRight(void){
    int duty=0;
    //反转慢衰减 -- IN1=PWM，IN2=1
    DL_TimerG_setCaptureCompareValue(PWM_0_INST, PWMPeriod, DL_TIMER_CC_3_INDEX);
    DL_TimerG_setCaptureCompareValue(PWM_0_INST, duty, DL_TIMER_CC_2_INDEX);
    while(duty <= 3200){
        duty += 100;
        DL_TimerG_setCaptureCompareValue(PWM_0_INST, duty, DL_TIMER_CC_2_INDEX);
        delay_cycles(1600000);
    }
}

void MotorTestLeft(void){
    int duty=0;
    //正转慢衰减 -- IN1=1，IN2=PWM
    DL_TimerG_setCaptureCompareValue(PWM_0_INST, PWMPeriod, DL_TIMER_CC_0_INDEX);
    DL_TimerG_setCaptureCompareValue(PWM_0_INST, duty, DL_TIMER_CC_1_INDEX);
    while(duty <= 3200){
        duty += 100;
        DL_TimerG_setCaptureCompareValue(PWM_0_INST, duty, DL_TIMER_CC_1_INDEX);
        delay_cycles(1600000);
    }
}

//a general function for moving forward
//parameter: dutyL, dutyR = percentage of CCR
void ForwardGeneral(float dutyL, float dutyR){
    //left
    //CCR = capture compare register
    int ccrL = dutyL * PWMPeriod;
    DL_TimerG_setCaptureCompareValue(PWM_0_INST, PWMPeriod, DL_TIMER_CC_0_INDEX);
    DL_TimerG_setCaptureCompareValue(PWM_0_INST, ccrL, DL_TIMER_CC_1_INDEX);

    //right
    int ccrR = dutyR * PWMPeriod;
    DL_TimerG_setCaptureCompareValue(PWM_0_INST, PWMPeriod, DL_TIMER_CC_3_INDEX);
    DL_TimerG_setCaptureCompareValue(PWM_0_INST, ccrR, DL_TIMER_CC_2_INDEX);

}

//直行
void MoveForward(void){
    float duty = FORWARD_SPEED;
    ForwardGeneral(duty, duty);
}   

//左行
void TurnLeft(void){
    ForwardGeneral(FORWARD_SPEED - 0.075,  FORWARD_SPEED);
}   

//右行
void TurnRight(void){
    ForwardGeneral(FORWARD_SPEED, FORWARD_SPEED - 0.075);
}   

void TurnRightContinue(void){
    ForwardGeneral(FORWARD_SPEED + 0.15,FORWARD_SPEED - 0.15);
}

void TurnLeftContinue(void){
    ForwardGeneral(FORWARD_SPEED - 0.15,  FORWARD_SPEED + 0.15);
}

//turn left small
void TurnLS(void){
    ForwardGeneral(FORWARD_SPEED - 0.05, FORWARD_SPEED);

}

void TurnRS(void){
    ForwardGeneral(FORWARD_SPEED, FORWARD_SPEED - 0.05);

}

//turn left tiny
void TurnLT(void){
    ForwardGeneral(FORWARD_SPEED - 0.05, FORWARD_SPEED);

}

void TurnRT(void){
    ForwardGeneral(FORWARD_SPEED, FORWARD_SPEED - 0.05);

}

void RotateLeft(void){
    //left tire backward, right tire forward
    int ccrL = 0.7 * PWMPeriod;
    DL_TimerG_setCaptureCompareValue(PWM_0_INST, PWMPeriod, DL_TIMER_CC_1_INDEX);
    DL_TimerG_setCaptureCompareValue(PWM_0_INST, ccrL, DL_TIMER_CC_0_INDEX);

    //right
    int ccrR = 0.7 * PWMPeriod;
    DL_TimerG_setCaptureCompareValue(PWM_0_INST, PWMPeriod, DL_TIMER_CC_3_INDEX);
    DL_TimerG_setCaptureCompareValue(PWM_0_INST, ccrR, DL_TIMER_CC_2_INDEX);

}

void RotateRight(void){
    //right tire backward, left tire forward
    int ccrL = 0.7 * PWMPeriod;
    DL_TimerG_setCaptureCompareValue(PWM_0_INST, PWMPeriod, DL_TIMER_CC_0_INDEX);
    DL_TimerG_setCaptureCompareValue(PWM_0_INST, ccrL, DL_TIMER_CC_1_INDEX);

    //right
    int ccrR = 0.7 * PWMPeriod;
    DL_TimerG_setCaptureCompareValue(PWM_0_INST, PWMPeriod, DL_TIMER_CC_2_INDEX);
    DL_TimerG_setCaptureCompareValue(PWM_0_INST, ccrR, DL_TIMER_CC_3_INDEX);

}

void Stop(void){
    ForwardGeneral(0, 0);
}

/*************************灰度部分*************************/
volatile uint8_t Turn_Delay_Flag = 0;   // 是否处于延时状态
volatile uint16_t Turn_Delay_Counter = 0; // 延时计数器

void GrayControl(void){
        if (Turn_Delay_Flag) {
        Turn_Delay_Counter++;
        if (Turn_Delay_Counter >= 55) { 
            Turn_Delay_Flag = 0;
            Turn_Delay_Counter = 0;
        }
        return;
    }





    switch (Digtal)
    {       
        case 0b00000000:
            Stop();
            break;

        // 理想前进：中间两路都检测到黑线
        case 0b11100111:
            MoveForward();
            break;

        case 0b11110111:
            TurnLT();
            break;
        case 0b11101111:
            TurnRT();
            break;
        // 轻微偏左：中间偏左黑
        case 0b11110011:
        case 0b11111011:

            TurnLS(); // 微调右转
            break;

        // 轻微偏右：中间偏右黑
        case 0b11011111:
        case 0b11001111:

            TurnRS(); // 微调左转
            break;

        // 明显偏左：左侧多路黑
        case 0b11111001:
        case 0b11111101:
        case 0b11110001:
        case 0b11111110:

            TurnLeft(); // 明显右转
            break;

        // 明显偏右：右侧多路黑
        case 0b10111111:
        case 0b10011111:
        case 0b01111111:
        case 0b00111111:
        case 0b10001111:
            TurnRight(); // 明显左转
            break;

        case 0b11111000:
        case 0b11110000:
        case 0b11100000:
        case 0b11000000:
        TurnLeftContinue();
        Turn_Index++;
        Turn_Delay_Flag = 1;
        Turn_Delay_Counter = 0;
        break;
        // 全白：无黑线（脱线）
        case 0b11111111:
            MoveForward(); // 或执行原地旋转找线
            break;

            
        default:
            MoveForward();
            break;
    }
    
}