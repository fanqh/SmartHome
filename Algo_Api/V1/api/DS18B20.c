
#include "ds18b20.h"


#define DS18B20_DATA                        GPIO_Pin_1
#define DS18B20_GPIO                        GPIOA
#define DS18B20_PERIPHCLK                   RCC_APB2Periph_GPIOA

#define set_high()                          GPIO_SetBits(DS18B20_GPIO, DS18B20_DATA)
#define set_low()                           GPIO_ResetBits(DS18B20_GPIO, DS18B20_DATA)
#define read_db_data()                      GPIO_ReadInputDataBit(DS18B20_GPIO, DS18B20_DATA)


#define DS18B20_PORT        GPIOA
#define DS18B20_BIT         GPIO_Pin_1
#define DS18B20_BITSET      GPIO_SetBits(DS18B20_PORT, DS18B20_BIT)
#define DS18B20_BITRESET    GPIO_ResetBits(DS18B20_PORT, DS18B20_BIT)
#define DS18B20_BITCHECK    GPIO_ReadInputDataBit(DS18B20_PORT, DS18B20_BIT)


/*
指令                    代码
Read ROM(读ROM)         [33H]
Match ROM(匹配ROM)      [55H]
Skip ROM(跳过ROM]       [CCH]
Search ROM(搜索ROM)     [F0H]
Alarm search(告警搜索)  [ECH]

指令 							代码
Write Scratchpad(写暂存存储器) 	[4EH]
Read Scratchpad(读暂存存储器) 	[BEH]
Copy Scratchpad(复制暂存存储器) [48H]
Convert Temperature(温度变换) 	[44H]
Recall EPROM(重新调出) 			[B8H]
Read Power supply(读电源) 		[B4H]
*/  

void ds18b20_io_init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    
    RCC_APB2PeriphClockCmd(DS18B20_PERIPHCLK,ENABLE);	
    
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = DS18B20_DATA;
	GPIO_Init(DS18B20_GPIO,&GPIO_InitStructure);	
}


void ds18b20_io_output(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    
    
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = DS18B20_DATA;
	GPIO_Init(DS18B20_GPIO,&GPIO_InitStructure);	
}
void ds18b20_io_input(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    
    
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 		 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = DS18B20_DATA;
	GPIO_Init(DS18B20_GPIO,&GPIO_InitStructure);	
}


void delayus(int times)
{
    int i;
    while(times--)
    {
       
        i=3;
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        
        __NOP();
        __NOP();
        while(i--)
        {
           __NOP();
           __NOP();
           __NOP();
           __NOP();
           __NOP();
           
        }
    }

}
void delay_x_10us(int times)		
{
     while(times--)
      delayus(11);  

}
#if 0

void rst_ds18b20(void)
{
    uint8 flg;
    uint8 i=0;
    flg=1;
    
    #if 0
    ds18b20_io_output();
    set_high();
    while(flg)
    {
        set_low();
        delay_x_10us(65);
        set_high();
        delay_x_10us(5);
        ds18b20_io_input();
        if(read_db_data())
            flg=1;
        else
            flg=0;   
        ds18b20_io_output();   
        delay_x_10us(45);
            
       
    }
    #else
    ds18b20_io_output();
    set_high();
    delay_x_10us(1);
    set_low();
    delay_x_10us(70);
    set_high();
    ds18b20_io_input();
    delay_x_10us(3);
    while(read_db_data())
    {   
      /*delay_us(1);
        i++;
        if(i>100)
            break;*/

    }
    ds18b20_io_output();
    set_high();
    delay_x_10us(10); 
    #endif
}
void Init_DS18B20(void)
{
    ds18b20_io_init();
    rst_ds18b20();

}
void write_data(uint8 data)
{
  unsigned char i;
  ds18b20_io_output();
  for(i = 0;i < 8;i++)
  {
     set_low();
    if(data & 0x01)
    {
       set_high();
    }
    else
    {
       set_low();
    }
    data >>= 1;
    delay_x_10us(3);
     set_high();
  }
   set_high();
}
int8 read_data(void)
{
    char i,Data = 0;
  ds18b20_io_output();
  for(i = 0;i < 8;i++)
  {
     set_low();
    Data >>= 1;
     set_high();
    ds18b20_io_input();
    if(read_db_data())
    {
      Data |= 0x80;
    }
    delay_x_10us(10);
    ds18b20_io_output();
  }
  return Data;
}
int GetTemperature(void)
{
	unsigned char a=0;
	unsigned char b=0;
	int Temperature;
	int tmp = 0;
    

	rst_ds18b20();
	write_data(0xCC); // 跳过读序号列号的操作
	write_data(0x44); // 启动温度转换
	delay_x_10us(51);
	rst_ds18b20();
	write_data(0xCC); //跳过读序号列号的操作 
	write_data(0xBE); //读取温度寄存器等（共可读9个寄存器） 前两个就是温度
	a=read_data();
	b=read_data();
	
	//U1_send(b);
	//U1_send(a);
	//U1_send(' ');
		
	tmp = b;
	tmp<<=8;
	tmp |= a;	//组合温度
	
	Temperature = (tmp&(~0xF800))*0.0625;	
	return (tmp&0xF800)?-Temperature:Temperature;
}
#else
void Delay15us()		
{
    delay_x_10us(2);
}

void Delay45us()		
{
    delay_x_10us(1);
    delay_x_10us(2);
    delay_x_10us(2);
}

void Init_DS18B20()
{
    ds18b20_io_init();
	set_high();
	delay_x_10us(1);
	set_low();
	delay_x_10us(51);
	set_high();
	delay_x_10us(3);
	if(read_db_data())
	{
		delay_x_10us(3);
	}
	delay_x_10us(51);
	set_high();	
}

unsigned char ReadOneChar()
{
	unsigned char i,dat=0;
	for(i = 0;i < 8;i++)
	{
		dat >>= 1;	
		set_high();
		__NOP();
		__NOP();		
		set_low();
		__NOP();
		__NOP();
		__NOP();	
		set_high();
		__NOP();
		__NOP();
		__NOP();		
		__NOP();
		__NOP();		
		if(read_db_data())
			dat |= 0x80;
		delay_x_10us(4);
	}
	set_high();
	delay_x_10us(4);
	return dat;
}

void WriteOneChar(unsigned char dat)
{
	unsigned char i;
	for(i = 0;i < 8;i++)
	{
		set_low();
		Delay15us();
        if(dat&0x01)
            set_high();
        else
           set_low(); 
		dat >>= 1;
		Delay45us();
		set_high();
	}	
	set_high();
	delay_x_10us(7);
}


int GetTemperature(void)
{
	unsigned char a=0;
	unsigned char b=0;
	int Temperature;
	int tmp = 0;

	Init_DS18B20();
	WriteOneChar(0xCC); // 跳过读序号列号的操作
	WriteOneChar(0x44); // 启动温度转换
	delay_x_10us(51);
	Init_DS18B20();
	WriteOneChar(0xCC); //跳过读序号列号的操作 
	WriteOneChar(0xBE); //读取温度寄存器等（共可读9个寄存器） 前两个就是温度
	a=ReadOneChar();
	b=ReadOneChar();
	
	//U1_send(b);
	//U1_send(a);
	//U1_send(' ');
		
	tmp = b;
	tmp<<=8;
	tmp |= a;	//组合温度
	
	Temperature = (tmp&(~0xF800))*0.0625;	
	return (tmp&0xF800)?-Temperature:Temperature;
}
#endif


