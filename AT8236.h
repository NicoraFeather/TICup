#include "ti_msp_dl_config.h"

#define PWMPeriod 3200


void MotorInit(void); //初始化，电机速度为0
void MotorTestLeft(void);  //左侧轮速度逐渐增加
void MotorTestRight(void);  //右侧轮速度逐渐增加
void MoveForward(void);  //直行
void TurnLeft(void);
void TurnRight(void);
void TurnLS(void);
void TurnLT(void);
void TurnRS(void);
void TurnRT(void);
void RotateLeft(void);
void RotateRight(void);
void Stop(void);
void GrayControl(void);