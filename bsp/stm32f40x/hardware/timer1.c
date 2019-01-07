#include "timer1.h"
//#include "led.h"
#include "master.h"
#include "modbus.h"

extern u8 modbus_com2_over;



//ͨ�ö�ʱ���жϳ�ʼ��	ͨ�ö�ʱ�� 2��3��4
//����ʱ��ѡ��ΪAPB1��2������APB1Ϊ36M
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//����ʹ�õ��Ƕ�ʱ��2,3!
//Tout=((arr+1)*(psc+1))/Ft us.
//Ft:Mhz
//TIM1��TIM8~TIM11ʹ��APB2ʱ��Ƶ��168M,TIM2~TIM7��TIM12~TIM14ʹ��APB1ʱ��Ƶ��Ϊ84M

void Timer3_Init(u16 arr,u16 psc)
{
		TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
		NVIC_InitTypeDef NVIC_InitStructure;

		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //ʱ��ʹ��	  
//	TIM_TimeBaseStructure.TIM_Period = 5000; //��ʱ��ʱ��84M,��Ƶϵ��8400,����84M/8400=10Khz�ļ���Ƶ��,����5000��Ϊ500ms
//	TIM_TimeBaseStructure.TIM_Prescaler =(7200-1); //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ  10Khz�ļ���Ƶ��
		TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	 ������5000Ϊ500ms
		TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ  10Khz�ļ���Ƶ��
		TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
		TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
		TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
//	TIM_ITConfig(  //ʹ�ܻ���ʧ��ָ����TIM�ж�
//		TIM3, //TIM3
//		TIM_IT_Update  |  //TIM �ж�Դ
//		TIM_IT_Trigger,   //TIM �����ж�Դ 
//		ENABLE  //ʹ��
//		);
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���

	TIM_ClearFlag(TIM3, TIM_FLAG_Update); //�������жϱ�־

//	TIM_Cmd(TIM3, ENABLE);  //ʹ��TIMx����
							 
}

void Timer3_enable( void )
{
		TIM_ClearFlag(TIM3, TIM_FLAG_Update); //�������жϱ�־
		TIM_SetCounter(TIM3,0x00);			//���������ֵ
		TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);
		TIM_Cmd(TIM3,ENABLE);
}

void Timer3_disable (void)
{
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, DISABLE); //ʱ��ʧ��
//	TIM_ITConfig(TIM2,TIM_IT_Update,DISABLE);
		TIM_ITConfig(TIM3, TIM_IT_Update | TIM_IT_Trigger,DISABLE );
		TIM_Cmd(TIM3, DISABLE);  //ʧ��TIMx����
}


void Timer2_Init(u16 arr, u16 psc)
{
		TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
		NVIC_InitTypeDef NVIC_InitStructure;

		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //ʱ��ʹ��	 
//	TIM_TimeBaseStructure.TIM_Period = 5000; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	 ������5000Ϊ500ms
//	TIM_TimeBaseStructure.TIM_Prescaler =(7200-1); //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ  10Khz�ļ���Ƶ��
		TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	 ������5000Ϊ500ms
		TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ  10Khz�ļ���Ƶ��
		TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
		TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
		TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ

//	TIM_ITConfig(  //ʹ�ܻ���ʧ��ָ����TIM�ж�
//		TIM2, //TIM2
//		TIM_IT_Update  |  //TIM �ж�Դ
//		TIM_IT_Trigger,   //TIM �����ж�Դ 
//		ENABLE  //ʹ��
//		);

		NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  //TIM2�ж�
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;  //��ռ���ȼ�0��
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //�����ȼ�3��
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
		NVIC_Init(&NVIC_InitStructure);  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���

		TIM_ClearFlag(TIM2,TIM_FLAG_Update);

//	TIM_Cmd(TIM2, ENABLE);  //ʹ��TIMx����							 
}


void Timer2_enable( void )
{
		TIM_ClearFlag(TIM2, TIM_FLAG_Update); //�������жϱ�־
		TIM_SetCounter(TIM2,0x00);			//���������ֵ
		TIM_ITConfig(TIM2,TIM_IT_Update|TIM_IT_Trigger,ENABLE);
		TIM_Cmd(TIM2,ENABLE);
}

void Timer2_disable (void)
{
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, DISABLE); //ʱ��ʧ��
//	TIM_ITConfig(TIM2,TIM_IT_Update,DISABLE);
		TIM_ITConfig(TIM2, TIM_IT_Update | TIM_IT_Trigger,DISABLE );
		TIM_Cmd(TIM2, DISABLE);  //ʧ��TIMx����
}


void Timer4_enable(u16 arr ,u16 psc)	   	//TIM4ʹ��
{
		TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
		NVIC_InitTypeDef NVIC_InitStructure;

		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); //ʱ��ʹ��	
		TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	 ������5000Ϊ500ms
		TIM_TimeBaseStructure.TIM_Prescaler = psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ  10Khz�ļ���Ƶ��  
		TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
		TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
		TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
		TIM_ITConfig(  //ʹ�ܻ���ʧ��ָ����TIM�ж�
		TIM4, //TIM2
		TIM_IT_Update  |  //TIM �ж�Դ
		TIM_IT_Trigger,   //TIM �����ж�Դ 
		ENABLE  //ʹ��
		);

		NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;  //TIM3�ж�
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;  //��ռ���ȼ�0��
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //�����ȼ�3��
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
		NVIC_Init(&NVIC_InitStructure);  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���

		TIM_Cmd(TIM4, ENABLE);  //ʹ��TIMx����							 
}

void Timer4_disable (void)					   //TIM4ʧ��
{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, DISABLE); //ʱ��ʧ��
		TIM_ITConfig(TIM4, TIM_IT_Update | TIM_IT_Trigger,DISABLE );
		TIM_Cmd(TIM4, DISABLE);  //ʧ��TIMx����
}




#define SYS_TIMx					TIM6
#define SYS_RCC_TIMx			RCC_APB1Periph_TIM6

void Timer_Sys_Init(u32 period_num)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	
	//�������ã�ʱ���ͱȽ�������ã���������ֻ�趨ʱ�����Բ���OC�Ƚ����
	RCC_APB1PeriphClockCmd(SYS_RCC_TIMx,ENABLE);
	
	TIM_DeInit(SYS_TIMx);

	TIM_TimeBaseStructure.TIM_Period=period_num;//װ��ֵ
	//prescaler is 1200,that is 168000000/168/500=2000Hz;
	TIM_TimeBaseStructure.TIM_Prescaler=84-1;//��Ƶϵ��

	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; //or TIM_CKD_DIV2 or TIM_CKD_DIV4

	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
	
	TIM_TimeBaseInit(SYS_TIMx,&TIM_TimeBaseStructure);

	TIM_ClearFlag(SYS_TIMx,TIM_FLAG_Update);

	TIM_ITConfig(SYS_TIMx,TIM_IT_Update,ENABLE);

//	TIM_Cmd(SYS_TIMx,ENABLE);
}

void Timer_Sys_Run(u8 ena)
{
	if(ena)
		TIM_Cmd(SYS_TIMx,ENABLE);
	else
		TIM_Cmd(SYS_TIMx,DISABLE);
}



//****************timer3���ӻ�֡��ʱ����***********************
void TIM3_IRQHandler(void)   //TIM3�ж�                
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) //���ָ����TIM�жϷ������:TIM �ж�Դ 
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);  //���TIMx���жϴ�����λ:TIM �ж�Դ 
		//LED1=!LED1;									   //�����������
		Timer3_disable();
		//modbus_com_over=2;
		//checkComm0Modbus();
	}
}


//****************timer2������֡��ʱ��***********************
void TIM2_IRQHandler(void)   //TIM2�ж�
{
		if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) //���ָ����TIM�жϷ������:TIM �ж�Դ 
		{
				TIM_ClearITPendingBit(TIM2, TIM_IT_Update);  //���TIMx���жϴ�����λ:TIM �ж�Դ 
				//LED0=!LED0;									   //�����������
				Timer2_disable();
				modbus_com2_over=2;
		}
}

//��ʱ�ɼ�����
void TIM4_IRQHandler(void)   //TIM4�ж�
{
	if (TIM_GetITStatus(TIM4, TIM_IT_Update) == SET) //���ָ����TIM�жϷ������:TIM �ж�Դ 
	{
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);  //���TIMx���жϴ�����λ:TIM �ж�Դ 
		//modbus_rtu();
	}
}


/***********************************************************************
�����жϵ����ȼ�����

�ж�Դ  | ����     | ��ռ��  |   ��Ӧ��
----------------------------------------
UART1 	| �Ժ�̨   |	3	 |	   2
----------------------------------------
UART2 	| �Դӻ�   |	3	 |	   2
----------------------------------------
TIM3 	| �Ժ�̨   |	3	 |	   3
----------------------------------------
TIM2 	| �Դӻ�   |	3	 |	   3
----------------------------------------



***********************************************************************/








