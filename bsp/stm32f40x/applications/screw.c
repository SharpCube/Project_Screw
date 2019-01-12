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
#include "drv_lcd.h" 

/**************************************************************************
*                            全局变量                                     *
***************************************************************************/
/* 消息队列控制块 */
static struct rt_messagequeue mq;
static char msg_pool[1048];
/* 信号量控制块 */
static struct rt_semaphore sem;
static char* file_name;

rt_uint16_t count=0;
rt_uint16_t count1=0;
rt_uint16_t count_1=0;
rt_int16_t clock_position=0;
rt_uint16_t mode=4;


rt_int16_t Desire_value = 0;
rt_int16_t flag = 0;

typedef struct{
    u8 head;
    u16 tick;
    s16 deformation;
    s16 current;
    s16 ad_torque;   
}Comdate;

/**************************************************************************
*                            创建交互函数                                 *
***************************************************************************/
void measure(int argc, char **argv)
{	
	 if (argc < 2)
    {
        rt_kprintf("Usage: measure file_name\n");
        rt_kprintf("Like: measure 1.txt\n");
        return ;
    }
	
	file_name = argv[1];
    mode = 0;
    rt_sem_release(&sem);
	
}

MSH_CMD_EXPORT(measure, measure); 




/**************************************************************************
*                            线程入口                                     *
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
				    Desire_value = 0;   //覆盖问题
					  rt_thread_delay(50);
				    flag = 1;
			  }
			}
			if(flag == 1)
			{
				//if(count_1 > 0 && count_1 < 150)   //这个地方会有BUG 
				//{
				    Desire_value = 200;    //值传回覆盖问题
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
        rt_thread_delay(20);       
	}
		
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
    rt_uint16_t count = 0;
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
			while(count == 1000&&mode != 2)
			{
				count++;
                Desire_value = 360;
				LED0_DIR=0;
				Pulse_output(265,11000);//3.3KHZ,12000
				while(count_1 < 140 || (count_1 > 2048&&count_1<4096))
				rt_thread_delay(1);
				Desire_value = 0;
				rt_thread_delay(100);
                Desire_value = 380;
				while(count_1 < 320 || (count_1 > 2048&&count_1<4096))
				rt_thread_delay(1);
				Desire_value = -1;
				rt_thread_delay(2000);				
				LED0_DIR=1;
				Desire_value = -500;
				Pulse_output(205,11000);//3.3KHZ,12000
				Desire_value = -1;
				rt_thread_delay(2000);
			} 
			mode = 4;
			Desire_value = 0;
            count = 0;
		}			
	  rt_thread_delay(10);
	}
}
	
void display_thread_entry(void* parameter)
{
    Comdate comdate;
	int fd;
    	LCD_ShowString(30,90,200,16,16,"rubber test");
 
	POINT_COLOR=BLUE;//设置字体为蓝色      	 
	LCD_ShowString(30,150,200,16,16,"file:");
    LCD_ShowString(94,150,200,16,16,file_name);    
	LCD_ShowString(30,170,200,16,16,"count:");
    rt_sem_take(&sem, RT_WAITING_FOREVER);
	if((fd=open(file_name,O_WRONLY | O_CREAT))==-1)
        printf("open %s error\r\n",file_name);	      
	
    while (count != 1000)
    {      
        LCD_ShowxNum(94,170,count,1,16,0);
        if (rt_mq_recv(&mq, &comdate, sizeof(comdate), RT_WAITING_FOREVER)
                == RT_EOK)
        {            
          write(fd,&comdate,sizeof(comdate));  
					//rt_kprintf("content:%s\n", buf);
        }
       
        rt_thread_delay(1);
    }
		close(fd);
}
	
void collect_thread_entry(void* parameter)
{
    Comdate comdate;
    int result;
       
    comdate.head=0x55;
    while (1)
    {
        if(Desire_value > 0) {            
            comdate.tick = rt_tick_get();
            comdate.deformation = count_1;        
            comdate.current = Real_Current_Value[0];
            comdate.ad_torque = 0;       
            result = rt_mq_send(&mq, &comdate, sizeof(comdate));
            if ( result == -RT_EFULL) {               
                rt_kprintf("message queue full, delay 1s\n");
                rt_thread_delay(1000);
            }
        }
		rt_thread_delay(10);           
    }   
}

/**************************************************************************
*                            创建线程                                     *
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
INIT_APP_EXPORT(display_thread_init);

/**************************************************************************
*                            创建IPC对象                                  *
***************************************************************************/

int messageq_simple_init()
{
    /* 初始化消息队列 */
    rt_mq_init(&mq, "mqt", 
    &msg_pool[0], /* 内存池指向msg_pool */
    13 - sizeof(void*), /* 每个消息的大小是 128 - void* */
    sizeof(msg_pool),  /* 内存池的大小是msg_pool的大小 */
    RT_IPC_FLAG_FIFO); /* 如果有多个线程等待，按照FIFO的方法分配消息 */
    

        /* 初始化信号量，初始值是0 */
    rt_sem_init(&sem, "sem", 0, 
               RT_IPC_FLAG_FIFO);
             
				
    return 0;
}
INIT_ENV_EXPORT(messageq_simple_init);

/**************************************************************************
*                            自定义硬件初始化                             *
***************************************************************************/
int hardware_init()
{
	 rt_hw_led_init();
	
	 rt_hw_key_init();
	
	 encode_Init();
	
	 CAN1_Configuration(); 
    POINT_COLOR=RED; 

	 
	 delay_ms(500);                                      //发送复位指令后的延时必须要有，等待驱动器再次初始化完成
	 CAN_RoboModule_DRV_Reset(0,1);
	 delay_ms(500); 
   CAN_RoboModule_DRV_Config(0,1,100,0);               //1号驱动器配置为100ms传回一次数据
   delay_us(200);                                      //此处延时为了不让传回数据时候4个不一起传
	 CAN_RoboModule_DRV_Mode_Choice(0,1,Current_Velocity_Mode);  //0组的所有驱动器 都进入开环模式
   delay_ms(500);                                      //发送模式选择指令后，要等待驱动器进入模式就绪。所以延时也不可以去掉。

	 return 0;
}
INIT_DEVICE_EXPORT(hardware_init);



	
	

	 

