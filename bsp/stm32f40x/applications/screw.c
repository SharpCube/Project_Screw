/*
 * File      : screw.c
 * This file is part of Project-screw
 *
 *
 * Change Logs:
 * Date           Author       Notes
 * 2019-01-02     sharp      the first version 
 */
 
#include <rtthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <dfs_posix.h>

#include "led.h"
#include "key.h"
#include "encode.h"
#include "pid.h"
#include "can.h"
#include "delay.h"
#include "step_time.h"

/**************************************************************************
*                            ȫ�ֱ���                                     *
***************************************************************************/

static struct rt_messagequeue mq;
static char msg_pool[2048];

rt_uint16_t count=0;
rt_uint16_t count1=0;
rt_uint16_t count_1=0;
rt_int16_t clock_position=0;
rt_uint16_t mode=4;

rt_int16_t Desire_value = 0;
rt_int16_t flag = 0;




/**************************************************************************
*                            �߳����                                     *
***************************************************************************/

void pid_thread_entry(void* parameter)
{
	 while(1)
	 {
			 count = TIM4->CNT;
			 count1 = TIM8->CNT;
		   rt_enter_critical();
			 count_1=count-count1;
			 count_1=(count_1+4096)%4096;
		   rt_exit_critical();
		  // rt_kprintf("%d,%d\r\n",count,count1);
		    // rt_kprintf("%d\r\n",count_1);
		 
			 ResultValue =SeqIntPID(4, 0,50,Desire_value,count_1);		
			 if(ResultValue>1800)
					 ResultValue=1800;
			 else if (ResultValue<-1800)
					 ResultValue=-1800; 

			 CAN_RoboModule_DRV_Current_Velocity_Mode(0,1,6000,ResultValue);
			 rt_thread_delay(4);
	}
}
	
void strategy_thread_entry(void* parameter)
{
	while(1)
	{
	  count = TIM4->CNT;
		count1 = TIM8->CNT;
		count_1=count-count1;
		count_1=(count_1+4096)%4096;
		if(mode == 0)
		{
			Desire_value = 0;
		}
		else if(mode == 1)
		{
			Desire_value = -500;
		}
		else
		{
			if(flag == 0)
			{
				//if(count_1 > 0 && count_1< 100)
				//{
				    Desire_value = 160;
				//}
			  if(count_1 < 2048 && count_1 > 100)
			  {
				    Desire_value = 0;   //��������
					  rt_thread_delay(50);
				    flag = 1;
			  }
			}
			if(flag == 1)
			{
				//if(count_1 > 0 && count_1 < 150)   //����ط�����BUG 
				//{
				    Desire_value = 200;    //ֵ���ظ�������
				//}
			  if(count_1 < 2048 && count_1 > 150)
			  {
				    Desire_value = 0;
					  rt_thread_delay(1000);
					  mode = 1;
					  flag = 0;
			  }
			}
		}
	}
		rt_thread_delay(20);
}
	
void key_thread_entry(void* parameter)
{
		rt_uint8_t key;
    while(1)
    {
			key=KEY_Scan(0);
			
			if(key == KEY0_PRES)
			{
				mode=1;
			}	
			
			else if(key == KEY1_PRES)
			{	
				mode=2;
			}		
			
			else if(key == WKUP_PRES)
			{	
				mode=0;
			}	
			
			else
			{
			  mode = 4;
			}
			
			rt_thread_delay(10);
   }
}

void stepmotor_thread_entry(void* parameter)
{
	//rt_uint8_t key;
	while(1)
	{
		//key=KEY_Scan(0);			  
		if(mode == 1)
		{		 
			 LED0_DIR=0;
			 Pulse_output(1000,800);//1KHZ,8000???
		}
		else if(mode == 2)	
		{
			 LED0_DIR=1;
			 Pulse_output(1000,800);
		}
		else if(mode == 0)	
		{
			while(mode != 2)
			{
				 Desire_value = 360;
				 LED0_DIR=0;
				 Pulse_output(265,11000);//3.3KHZ,12000
				 //count = TIM4->CNT;
		     //count1 = TIM8->CNT;

				 while(count_1 < 140 || (count_1 > 2048&&count_1<4096))
				 {//rt_kprintf("q%d\r\n",count_1);
				 rt_thread_delay(1);
				 }
				 Desire_value = 0;
				 rt_thread_delay(100);
         Desire_value = 380;
				 while(count_1 < 320 || (count_1 > 2048&&count_1<4096))
				 {//rt_kprintf("w%d\r\n",count_1);
				 rt_thread_delay(1);
					 }
				 Desire_value = 0;
				 rt_thread_delay(2000);				
				 LED0_DIR=1;
				 Desire_value = -500;
				 Pulse_output(205,11000);//3.3KHZ,12000
				 Desire_value = 0;
				 rt_thread_delay(2000);
			} 
			mode = 4;
			Desire_value = 0;
		}			
	  rt_thread_delay(10);
	}
}
	
void display_thread_entry(void* parameter)
{
	char buf[32];
	int fd;
	int count=100;
	if((fd=open("/test/1.txt",O_WRONLY | O_CREAT))==-1)
        printf("open 1.txt error\n");
	
    while (count--)
    {
        rt_memset(&buf[0], 0, sizeof(buf));
       
        if (rt_mq_recv(&mq, &buf[0], sizeof(buf), RT_WAITING_FOREVER)
                == RT_EOK)
        {            
          write(fd,buf,strlen(buf));  
					rt_kprintf("content:%s\n", buf);
        }

        
        rt_thread_delay(1);
    }
		close(fd);
}
	
void collect_thread_entry(void* parameter)
{
//		int i, result;
//    char buf[] = "message No.x\r\n";
//		

//    while (1)
//    {
//        for (i = 0; i < 10; i++)
//        {
//            buf[sizeof(buf) - 4] = '0' + i;

//           // rt_kprintf("%s\n", buf);
//            result = rt_mq_send(&mq, &buf[0], sizeof(buf));
//            if ( result == -RT_EFULL)
//            {
//               
//               // rt_kprintf("message queue full, delay 1s\n");
//                rt_thread_delay(1000);
//            }
//						rt_thread_delay(10);
//        }       
//    }
     while(1)
		 {
			  if(Desire_value >= 0)
				{
			  if(count_1 > 2048)
		    rt_kprintf("%d\r\n",(rt_int16_t)(count_1-4096));
				else
				rt_kprintf("%d\r\n",count_1);
			  }
				else
				rt_kprintf("%d\r\n",0);
			  rt_thread_delay(10);
		 }
}

/**************************************************************************
*                            �����߳�                                     *
***************************************************************************/

int key_thread_init()
{
	rt_thread_t tid;

    tid = rt_thread_create("key",
        key_thread_entry, RT_NULL,
        2048, 5, 20);

    if (tid != RT_NULL)
        rt_thread_startup(tid);
	

    return 0;
}
INIT_APP_EXPORT(key_thread_init);

int collect_thread_init()
{
	rt_thread_t tid;

    tid = rt_thread_create("collect",
        collect_thread_entry, RT_NULL,
        2048, 6, 20);

    if (tid != RT_NULL)
        rt_thread_startup(tid);
	

    return 0;
}
INIT_APP_EXPORT(collect_thread_init);
	
int pid_thread_init()
{
	rt_thread_t tid;
	
	 /* ��ʼ����Ϣ���� */
    rt_mq_init(&mq, "mqt", 
        &msg_pool[0], /* �ڴ��ָ��msg_pool */
        128 - sizeof(void*), /* ÿ����Ϣ�Ĵ�С�� 128 - void* */
        sizeof(msg_pool),  /* �ڴ�صĴ�С��msg_pool�Ĵ�С */
        RT_IPC_FLAG_FIFO); /* ����ж���̵߳ȴ�������FIFO�ķ���������Ϣ */


    tid = rt_thread_create("pid",
        pid_thread_entry, RT_NULL,
        2048, 7, 20);

    if (tid != RT_NULL)
        rt_thread_startup(tid);
	

    return 0;
}
INIT_APP_EXPORT(pid_thread_init);

int stepmotor_thread_init()
{
	rt_thread_t tid;

    tid = rt_thread_create("stepmotor",
        stepmotor_thread_entry, RT_NULL,
        2048, 8, 20);

    if (tid != RT_NULL)
        rt_thread_startup(tid);
	

    return 0;
}
INIT_APP_EXPORT(stepmotor_thread_init);

int strategy_thread_init()
{
	rt_thread_t tid;

    tid = rt_thread_create("strategy",
        strategy_thread_entry, RT_NULL,
        2048, 8, 20);

    if (tid != RT_NULL)
        rt_thread_startup(tid);
	

    return 0;
}
//INIT_APP_EXPORT(strategy_thread_init);
	

int display_thread_init()
{
	rt_thread_t tid;

    tid = rt_thread_create("display",
        display_thread_entry, RT_NULL,
        2048, 15, 20);

    if (tid != RT_NULL)
        rt_thread_startup(tid);
	
    return 0;
}
//INIT_APP_EXPORT(display_thread_init);

/**************************************************************************
*                            ����IPC����                                  *
***************************************************************************/

int messageq_simple_init()
{
    /* ��ʼ����Ϣ���� */
    rt_mq_init(&mq, "mqt", 
        &msg_pool[0], /* �ڴ��ָ��msg_pool */
        32 - sizeof(void*), /* ÿ����Ϣ�Ĵ�С�� 128 - void* */
        sizeof(msg_pool),  /* �ڴ�صĴ�С��msg_pool�Ĵ�С */
        RT_IPC_FLAG_FIFO); /* ����ж���̵߳ȴ�������FIFO�ķ���������Ϣ */
				
    return 0;
}
INIT_ENV_EXPORT(messageq_simple_init);

/**************************************************************************
*                            �Զ���Ӳ����ʼ��                             *
***************************************************************************/
int hardware_init()
{
	 rt_hw_led_init();
	
	 rt_hw_key_init();
	
	 encode_Init();
	
	 CAN1_Configuration(); 
	 
	 delay_ms(500);                                      //���͸�λָ������ʱ����Ҫ�У��ȴ��������ٴγ�ʼ�����
	 CAN_RoboModule_DRV_Reset(0,1);
	 delay_ms(500); 
   CAN_RoboModule_DRV_Config(0,1,100,0);               //1������������Ϊ100ms����һ������
   delay_us(200);                                      //�˴���ʱΪ�˲��ô�������ʱ��4����һ��
	 CAN_RoboModule_DRV_Mode_Choice(0,1,Current_Velocity_Mode);  //0������������� �����뿪��ģʽ
   delay_ms(500);                                      //����ģʽѡ��ָ���Ҫ�ȴ�����������ģʽ������������ʱҲ������ȥ����

	 return 0;
}
INIT_DEVICE_EXPORT(hardware_init);

	
	

	 

