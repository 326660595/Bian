/*-------------------------------------------------------------------------------
�ļ����ƣ�main.c
Ӳ��ƽ̨��STM32F103ZET6��ģ������
��д��������
�̼���  ��V3.5
��    ע��ͨ�����޸Ŀ�����ֲ�����������壬����������Դ�����硣
---------------------------------------------------------------------------------*/
#include "stm32f10x.h"
#include <stdio.h>
#include "led.h"
#include "Delay.h"
#include "key.h"
#include "timer.h"
#include "beep.h"
#include "usart.h"
#include "adc.h"
#include "lcd.h"
#include "24cxx.h" 
#include "flash.h" 
#include "can.h"

#include "main.h"
#include "mfrc522.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
 
//#include "remote.h"
//#include "ds18b20.h"
#include "24l01.h"

void GPIO_ini()
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  
  /* Enable the GPIO Clock */
  RCC_APB2PeriphClockCmd(MF522_RST_CLK, ENABLE);

  /* Configure the GPIO pin */
  GPIO_InitStructure.GPIO_Pin = MF522_RST_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;

  GPIO_Init(MF522_RST_PORT, &GPIO_InitStructure);
	
  /* Enable the GPIO Clock */
  RCC_APB2PeriphClockCmd(MF522_MISO_CLK, ENABLE);

  /* Configure the GPIO pin */
  GPIO_InitStructure.GPIO_Pin = MF522_MISO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;

  GPIO_Init(MF522_MISO_PORT, &GPIO_InitStructure);
	
  /* Enable the GPIO Clock */
  RCC_APB2PeriphClockCmd(MF522_MOSI_CLK, ENABLE);

  /* Configure the GPIO pin */
  GPIO_InitStructure.GPIO_Pin = MF522_MOSI_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;

  GPIO_Init(MF522_MOSI_PORT, &GPIO_InitStructure);
	
  /* Enable the GPIO Clock */
  RCC_APB2PeriphClockCmd(MF522_SCK_CLK, ENABLE);

  /* Configure the GPIO pin */
  GPIO_InitStructure.GPIO_Pin = MF522_SCK_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;

  GPIO_Init(MF522_SCK_PORT, &GPIO_InitStructure);
	
  /* Enable the GPIO Clock */
  RCC_APB2PeriphClockCmd(MF522_NSS_CLK, ENABLE);

  /* Configure the GPIO pin */
  GPIO_InitStructure.GPIO_Pin = MF522_NSS_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;

  GPIO_Init(MF522_NSS_PORT, &GPIO_InitStructure);
	
  /* Enable the GPIO Clock */
  RCC_APB2PeriphClockCmd(LED_CLK, ENABLE);

  /* Configure the GPIO pin */
  GPIO_InitStructure.GPIO_Pin = LED_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;

  GPIO_Init(LED_PORT, &GPIO_InitStructure);
}

void InitializeSystem()
{
	LED_OFF;
	delay_10ms(10);
	PcdReset();
	PcdAntennaOff(); 
	PcdAntennaOn();  
	M500PcdConfigISOType( 'A' );
	LED_ON;
	delay_10ms(10);	
	LED_OFF;
	delay_10ms(10);
	LED_ON;
	delay_10ms(10);	
	LED_OFF;
}




int main(void)
{		
	 /* 
*/
	u8 key;
	
	u8 t=0,zf=2,zt=2;//�޷�������
	u8 time=0,minute=0,hour=0;//ʱ��
	
	int cnt=0;
	int fk,jg,ye=100,je[5]={1,3,5,10,20};//����ָʾ֧�����
	int k=2;
	
	///////////////////////////rc522����
	
	char status;
	//unsigned char sj;
	unsigned char snr, buf[16], TagType[2], SelectedSnr[4],DefaultKey[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; //���� �Ĵ���

	GPIO_ini();  
  InitializeSystem( );
	
	
	
	LED_Init();//LED��ʼ��
	//LED2_REV; LED3_REV;//ȡ���ر�led	
  KEY_Init();//������ʼ��
  SysTick_Init();//��ʱ��ʼ��
	BEEP_Init();   //��������ʼ��
	//BEEP_REV;ȡ��������ʹ�䷢��
	USART1_Int(9600);//���ڳ�ʼ��9600Hz
	LCD_Init();//��ʾ����ʼ��
  NRF24L01_Init(); 
	
	POINT_COLOR=BLACK;//��ɫ 
  LCD_ShowString(100,40,200,16,16,"MCU ATM");	
	LCD_ShowString(00,00,200,16,16,"Time:");			//��ʾ��ǰ����ֵ	
	LCD_ShowString(60,250,200,16,16,"Developer Mr.Luo");	//������
 	POINT_COLOR=RED;//��������Ϊ��ɫ 
	LCD_ShowString(60,70,200,16,16,"Mineral water 1RMB");	//��Ȫˮ1Ԫ
	LCD_ShowString(60,90,200,16,16,"Cola 3RMB");//����3Ԫ
	LCD_ShowString(60,130,200,16,16,"Alcohol 10RMb");	 //��10Ԫ
	LCD_ShowString(60,150,200,16,16,"Tobacco 20RMB");//��	20Ԫ	
	LCD_ShowString(60,190,200,16,16,"Please select");		//��ʾѡ��
  POINT_COLOR=BLUE;//��������Ϊ��ɫ	  
	LCD_ShowString(60,110,200,16,16,"Milky tea 5RMB");//�̲�5Ԫ
	//printf("\n\r�����ߣ����ݴ�ѧӦ�ü���ѧԺ��18��-����\r");
 	while(1)
	{
		//////////////////////////////////////////////////////////////
		char xl;
//		xl=char xlh(unsigned char auth_mode,unsigned char addr,unsigned char *pKey,unsigned char *pSnr);
		
	//LCD_ShowString(60,170,200,16,16,"sucsessful");
		
		status= PcdRequest(REQ_ALL,TagType);
		if(!status)
		{
			
			status = PcdAnticoll(SelectedSnr);
			if(!status)
			{
				status=PcdSelect(SelectedSnr);
				if(!status)
				{
					snr = 1;  //������1
          status = PcdAuthState(KEYA, (snr*4+3), DefaultKey, SelectedSnr);// У��1�������룬����λ��ÿһ������3��
					xl = xlh(KEYA, (snr*4+3), DefaultKey, SelectedSnr);//��ȡ���к�
					//LCD_ShowxNum(120,00,xl,6 ,16,0X80);
					{
						if(!status)
						{
							status = PcdRead((snr*4+0), buf);  // ��������ȡ1����0�����ݵ�buf[0]-buf[16] 
							//status = PcdWrite((snr*4+0), buf);  // д������buf[0]-buf[16]д��1����0��
							if(!status)
							{
								//��д�ɹ�������LED
								//	LED_ON;	
								zf=1;
								
								Delay_ms(10);
									/*
									if(zf=1)
									{
									BEEP_REV;
									Delay_ms(100);BEEP_REV;
									LCD_ShowString(60,170,200,16,16,"PaySucsessful");
									zf=0;
									}
								*/
								WaitCardOff();
							}
						}
					}
				}
			}
		}	
		//LED_OFF; 
		/*
		for( n=0;n<=15;++n)
		{
		 sj=buf[n];
		}
		*/
		//LCD_ShowString(60,210,200,16,16,zf);
		//LCD_ShowxNum(100,00,zf,2,16,0X80);//֧����Ϣ�鿴
		////////////////////////////////////////////////////////////////
		key=KEY_Scan(0);
	
		if (key==KEY4)//����ѡ��
		{
			++k;
			zt=0;
			POINT_COLOR=RED;
			LCD_ShowString(60,190,200,16,16,"Please select    ");	//��ն������ʾ
			LCD_ShowString(60,190,200,16,16,"Please select");
			LCD_ShowString(60,210,200,16,16,"                 ");
			POINT_COLOR=BLUE;
			if (k==2)
			{
				LCD_ShowString(60,110,200,16,16,"Milky tea 5RMB");
				POINT_COLOR=RED;
				LCD_ShowString(60,90,200,16,16,"Cola 3RMB");
			}
			else if (k==3)
			{
				LCD_ShowString(60,130,200,16,16,"Alcohol 10RMb");
				POINT_COLOR=RED;
				LCD_ShowString(60,110,200,16,16,"Milky tea 5RMB");
			}
			else if(k==4)
			{
				LCD_ShowString(60,150,200,16,16,"Tobacco 20RMB");
				POINT_COLOR=RED;
				LCD_ShowString(60,130,200,16,16,"Alcohol 10RMb");
			}
			else if(k==5)
			{
				k=0;
				LCD_ShowString(60,70,200,16,16,"Mineral water 1RMB");	
				POINT_COLOR=RED;
				LCD_ShowString(60,150,200,16,16,"Tobacco 20RMB");
			}
			else if(k==1)
			{
				LCD_ShowString(60,90,200,16,16,"Cola 3RMB");
				POINT_COLOR=RED;
				LCD_ShowString(60,70,200,16,16,"Mineral water 1RMB");
			}	
		}
		
		else if(key==KEY3)//����ѡ��
		{
			--k;
			zt=0;
			POINT_COLOR=RED;
			LCD_ShowString(60,190,200,16,16,"Please select    ");	//��ն������ʾ
			LCD_ShowString(60,190,200,16,16,"Please select");	
			LCD_ShowString(60,210,200,16,16,"                 ");
			POINT_COLOR=BLUE;
			if (k==2)
			{
				LCD_ShowString(60,110,200,16,16,"Milky tea 5RMB");
				POINT_COLOR=RED;
				LCD_ShowString(60,130,200,16,16,"Alcohol 10RMb");
			}
			else if(k==1)
			{
				LCD_ShowString(60,90,200,16,16,"Cola 3RMB");
				POINT_COLOR=RED;
				LCD_ShowString(60,110,200,16,16,"Milky tea 5RMB");
			}	
			else if(k==0)
			{
				LCD_ShowString(60,70,200,16,16,"Mineral water 1RMB");	
				POINT_COLOR=RED;
				LCD_ShowString(60,90,200,16,16,"Cola 3RMB");
			}
			else if(k<0)
			{
				k=4;
				LCD_ShowString(60,150,200,16,16,"Tobacco 20RMB");
				POINT_COLOR=RED;
				LCD_ShowString(60,70,200,16,16,"Mineral water 1RMB");	
			}
			else if (k==3)
			{
				LCD_ShowString(60,130,200,16,16,"Alcohol 10RMb");
				POINT_COLOR=RED;
				LCD_ShowString(60,150,200,16,16,"Tobacco 20RMB");
			}
			
			
			
		}

		///////////////////
		
	  else if(key==KEY2)//��key2����ѡ��֧��
		{
			zt=1;
			zf=0;
			BEEP_REV; Delay_ms(100); BEEP_REV;//��ѡ�������
			POINT_COLOR=BLUE;//��������Ϊ��ɫ
			LCD_ShowString(60,210,200,16,16,"               ");
				if(k==0){LCD_ShowString(60,190,200,16,16,"Please pay:1RMB");}
				else if(k==1){LCD_ShowString(60,190,200,16,16,"Please pay:3RMB");}
				else if(k==2){LCD_ShowString(60,190,200,16,16,"Please pay:5RMB");}
				else if(k==3){LCD_ShowString(60,190,200,16,16,"Please pay:10RMB");}
				else if(k==4){LCD_ShowString(60,190,200,16,16,"Please pay:20RMB");}	
				
				
				
				
		}
		else if(key==KEY1)//key1����ȡ��ѡ��
		{	 
			zt=0;
			k=2;
			POINT_COLOR=BLUE;
			LCD_ShowString(60,110,200,16,16,"Milky tea 5RMB");
			POINT_COLOR=RED;
			LCD_ShowString(60,70,200,16,16,"Mineral water 1RMB");	//��Ȫˮ1Ԫ
	    LCD_ShowString(60,90,200,16,16,"Cola 3RMB");//����3Ԫ
	    LCD_ShowString(60,130,200,16,16,"Alcohol 10RMb");	 //��10Ԫ
	    LCD_ShowString(60,150,200,16,16,"Tobacco 20RMB");//��	20Ԫ
			BEEP_REV;Delay_ms(100);BEEP_REV;//��ѡ�������
			LCD_ShowString(60,190,200,16,16,"Please select    ");	//��ն������ʾ
			LCD_ShowString(60,190,200,16,16,"Please select");	
			LCD_ShowString(60,210,200,16,16,"                      ");
		}

		//////////
		//if(zt==1&&zf==1&&xl==104)//zt=1��ʾ���ڴ�֧��״̬��zf=1��ʾˢ��״̬��
		
		if(zt==1&&zf==1&&xl==104)//����ѡ�����п����ҿ�����Ҫ��ſ�ˢ����
		{
			BEEP_REV;
			fk=je[k];ye-=fk;//������
			Delay_ms(100);BEEP_REV;
			POINT_COLOR=BLUE;
			zf=0;zt=0;	
			if(ye>=0){LCD_ShowString(60,190,200,16,16,"PaySucsessful       ");jg=1;
				LCD_ShowString(60,210,200,16,16,"Balance:");LCD_ShowxNum(130,210,ye,2,16,0X80);
			}
			else if(ye<0){LCD_ShowString(60,190,200,16,16,"PayFail          ");jg=0;
				ye+=fk;
				LCD_ShowString(60,210,200,16,16,"Balance:");LCD_ShowxNum(130,210,ye,2,16,0X80);
			}
			fs(fk,hour,minute,time,ye,jg);
		}
		else if(zt==1&&zf==1){LCD_ShowString(60,190,200,16,16,"Error Car         ");}
		
		
		/////////////////////////
		
		t++; 
		Delay_ms(10);//��ʱ10ms
		if(t==10)//ʱ��
		{
			LED2_REV; LED3_REV;//��ʾϵͳ��������	
			t=0;
			cnt++;
			if(cnt==301){cnt=1;}
			time=cnt/5;//time��1ʱ��1s;
			if(time==60)
			{
				minute++;
				if(minute==61){minute=1;}
				if(minute==60){hour++;}
			}
			POINT_COLOR=BLACK;
			LCD_ShowxNum(80,00,time,2,16,0X80);	//��ʾ����
			LCD_ShowxNum(60,00,minute,2,16,0X80);
			LCD_ShowxNum(40,00,hour,2,16,0X80);
		}	

		
	}
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  
}
#endif

/**
  * @}
  */


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/

