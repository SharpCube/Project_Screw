#include "pid.h"
#include <math.h>
#include "stm32f4xx.h"

extern u16	Parameter;


float ResultValue;
float ResultValueBack;
float ResultValueBackOne;
float ResultValueBackTwo;
float GiveValueBack;
float ResultValuess;
float PIDErrADD;
float ErrBack;
float SeqIntPIDErrADD;
float SeqIntErrBack;
float OverIntPIDErrADD;
float OverIntResultBack;
float OverIntErrBack;

float PID(float Kp, float Ki, float Kd, float GiveValue, float ActualValue)
{
float result;
float Err,KpWork, KiWork, KdWork;
Err = GiveValue - ActualValue;
if(fabs(Err) > 2*Parameter)
		Err=4*Parameter-fabs(Err);
KpWork = Kp*Err;
KiWork = Ki*PIDErrADD;
KdWork = Kd*(Err-ErrBack);
result = KpWork+KiWork+KdWork;
PIDErrADD = PIDErrADD + Err;
ErrBack = Err;
return result;
}


float SeqIntPID(float Kp, float Ki, float Kd,  u16 GiveValue, u16 ActualValue)
{
float result;
float KpWork, KiWork, KdWork;
s16 Err = ((GiveValue - ActualValue)+4*Parameter)%(4*Parameter);
	if(Err > 2*Parameter)
		Err=Err-4*Parameter;
KpWork = Kp*Err;
KiWork = Ki*SeqIntPIDErrADD;
KdWork = Kd*(Err-SeqIntErrBack);
if(fabs(Err) > 100)
{
result = KpWork+KdWork;
}else{
result = KpWork+KiWork+KdWork;
}
SeqIntPIDErrADD = SeqIntPIDErrADD + Err;
SeqIntErrBack = Err;
return result;
}


float OverIntPID(float Kp, float Ki, float Kd, float GiveValue, float ActualValue)
{
float result;
float Err,KpWork, KiWork, KdWork;
Err = GiveValue - ActualValue;
	if(fabs(Err) > 2*Parameter)
		Err=4*Parameter-fabs(Err);
if(OverIntResultBack > 120)
{
if(Err < 0)
{
OverIntPIDErrADD = OverIntPIDErrADD + Err;
}
}else if(OverIntResultBack < 120){if(Err > 0)
{
OverIntPIDErrADD = OverIntPIDErrADD + Err;
}
}else{
OverIntPIDErrADD = OverIntPIDErrADD + Err;
}
KpWork = Kp*Err;
KiWork = Ki*OverIntPIDErrADD;
KdWork = Kd*(Err-OverIntErrBack);
result = KpWork+KiWork+KdWork;
OverIntErrBack = Err;
OverIntResultBack = result;
return result;
}
