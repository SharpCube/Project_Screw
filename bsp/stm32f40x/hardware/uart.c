//#include "sys.h"
#include "uart.h"
#include "timer1.h"
//#include "led.h"
//#include "delay.h"

u8 receCount;
u8 receBuf[32];
u8 sendCount;	 
u8 sendBuf[120];
u8 sendPosi;
u8 checkoutError;

u8 receCount2;
u8 receBuf2[32];
u8 sendCount2;	 
u8 sendBuf2[32];
u8 sendPosi2;
u8 checkoutError2;

void uart1_init(u32 bound)
{
		//GPIO�˿�����
		GPIO_InitTypeDef GPIO_InitStructure;
		USART_InitTypeDef USART_InitStructure;
		NVIC_InitTypeDef NVIC_InitStructure;
	 
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //GPIOA
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//USART1
	
		GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
		GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);
	
		//USART1�˿�����
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10; //GPIOA9��GPIOA10
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�50MHz
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
		GPIO_Init(GPIOA,&GPIO_InitStructure); //��ʼ��PA9��PA10

		//Usart1 NVIC ����
		NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;  //��ռ3
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		// ����1

		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
		NVIC_Init(&NVIC_InitStructure);	//����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���USART1
  
   //USART ��ʼ������
   
		   //USART1 ��ʼ������
		USART_InitStructure.USART_BaudRate = bound;//����������
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
		USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
		USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
		USART_Init(USART1, &USART_InitStructure); //��ʼ������1

   

    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//���������ж�
//	USART_ClearFlag(USART1, USART_FLAG_TC);		  	//���㷢����λ��׼!!
		USART_ITConfig(USART1,USART_IT_TC,ENABLE);	   //�������ж�   
    USART_Cmd(USART1, ENABLE);                    //ʹ�ܴ���
}


void uart2_init(u32 bound)					//����2����
{
    //GPIO�˿�����
    GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //GPIOA
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);
    
	 //USART1�˿�����
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3; //GPIOA2��GPIOA3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
	GPIO_Init(GPIOA,&GPIO_InitStructure); //��ʼ��PA2��PA3

   //Usart2 NVIC ����

    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;	//	��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;			//	��Ӧ���ȼ�2

	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���USART1
  
   //USART ��ʼ������
   
	USART_InitStructure.USART_BaudRate = bound;//����������
	USART_InitStructure.USART_WordLength = USART_WordLength_9b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_Even;//żУ��
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
	USART_Init(USART2, &USART_InitStructure); //��ʼ������2

    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//���������ж�
//	USART_ClearFlag(USART2, USART_FLAG_TC);		  	//���㷢����λ ???????
		USART_ITConfig(USART2,USART_IT_TC,ENABLE);	   //�������ж�   
    USART_Cmd(USART2, ENABLE);                    //ʹ�ܴ��� 
}

void beginSend(void)
{	
	//sendEnable();	//��Ϊ����//
	sendPosi = 0;
	if(sendCount > 1)
		sendCount--;
	USART_SendData(USART1,sendBuf[0]);
}


void beginSend2(void)
{	
	//sendEnable2();	//��Ϊ����//
	sendPosi2 = 0;
	if(sendCount2 > 1)
		sendCount2--;
	USART_SendData(USART2,sendBuf2[0]);
}


//void USART1_IRQHandler(void)                	//����1�жϷ������
//{
////	if(USART1->SR&(1<<5))//���յ�����
////	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
//	if(USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
//	{
//		
////			receBuf[receCount] =USART1->DR;
//				receBuf[receCount]= USART_ReceiveData(USART1);	 
//	    	receCount++;          //���յ�ַƫ�ƼĴ�����1
//				Timer3_enable();		
//	} 
//	else if(USART_GetITStatus(USART1,USART_IT_TC) == SET)
//	{
//		USART_ClearFlag(USART1,USART_FLAG_TC);		 //ע������ط���д����stm32���ڷ����жϵ�д��,Ҫ�����־λ
//		if(sendPosi < sendCount) //character has been transmitted
//	    {
//	        sendPosi++;
//	        USART_SendData(USART1,sendBuf[sendPosi]);
//	    }
//	    else
//	    {
//	       // receEnable();    //�������485���ڽ���״̬
//	        receCount = 0;   //����յ�ַƫ�ƼĴ���//
//	        checkoutError = 0;
//	    } 
//	}	
//} 



//*******************��Ϊ����******************
void USART2_IRQHandler(void)                	//����2�жϷ������
{
	if(USART_GetITStatus(USART2, USART_IT_RXNE) ==SET)		  //�����ж�
	{
		
		receBuf2[receCount2]= USART_ReceiveData(USART2);
	    receCount2++;          //���յ�ַƫ�ƼĴ�����1
		Timer2_enable();
		
	}

	else if(USART_GetITStatus(USART2,USART_IT_TC) == SET)
	{
		USART_ClearFlag(USART2,USART_FLAG_TC);		 //ע������ط���д����stm32���ڷ����жϵ�д��,Ҫ�����־λ
		if(sendPosi2 < sendCount2)
	    {
	        sendPosi2++;
	        USART_SendData(USART2,sendBuf2[sendPosi2]);
	    }
	    else
	    {
	       // receEnable2();    //�������485���ڽ���״̬
	        receCount2 = 0;   //����յ�ַƫ�ƼĴ���//
	        checkoutError2 = 0;
	    } 
	}	
} 
