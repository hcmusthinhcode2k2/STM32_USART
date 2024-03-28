#include <gpio.h>
#include <cortex_m3.h>
#include <uart.h>
#include <stm32f103c8t6.h>

struct
{
  struct
  {
    unsigned char MsTick;
    unsigned char RX;
  } Event;
  unsigned long MsTime;
} System = 
{
  .Event = 
  {
    .MsTick = 0, 
    .RX = 0,
  }, 
  .MsTime = 0,
};

struct
{
  unsigned char Size;
  unsigned char TimeOut;
  unsigned char Buff[32];
} UART1_RX = {.Size = 0, .TimeOut = 0, };

void SysTick_Handler()          //trigger 1ms
{
  System.Event.MsTick = 1;
  System.MsTime++;
}

void BaseProcess()
{
  if (System.Event.MsTick)
  {
    System.Event.MsTick = 0;
    if (UART1_RX.TimeOut)
    {
      UART1_RX.TimeOut++;
      if (UART1_RX.TimeOut > 10)
      {
        UART1_RX.TimeOut = 0;
        System.Event.RX = 1;
      }
    }
  }
}

void UART1_IRQHandler()         //chỉ ngắt khi nhận 1 byte
{
  unsigned char data = UART1.DR;
  UART1_RX.TimeOut = 1;
  if (UART1_RX.Size < 32 && !System.Event.RX)
  {
    UART1_RX.Buff[UART1_RX.Size] = data;
    UART1_RX.Size++;
  }
}

void Delay_ms(unsigned long Time)
{
  unsigned long now = System.MsTime;
  while ((System.MsTime - now) < Time)
    BaseProcess();
}

void main()
{
  *((unsigned long*)(0x40021000 + 0x18)) |= 0x04;       //bật clock cho GPIOA  
  //UART1-TX
  GPIOA.CRH.BITS.MODE_9 = 3;     //chân A9 là out put max 50MHz
  GPIOA.CRH.BITS.CNF_9 = 2;      //chân A9 là af output push-pull
  //UART1-RX
  GPIOA.ODR.BITS.b10 = 1;       //chân A10 là pullup
  GPIOA.CRH.BITS.MODE_10 = 0;    //chân A10 là input
  GPIOA.CRH.BITS.CNF_10 = 2;     //chân A10 input pullup/pulldown  
  
  *((unsigned long*)(0x40021000 + 0x18)) |= 0x4000;       //bật clock cho UART1, clock base = 8MHz
  UART1.BRR.BITS.MANTISSA = 52;
  UART1.BRR.BITS.FRACTION = 1;
  
  UART1.CR3.BITS.CTSIE = 0;
  UART1.CR3.BITS.CTSE = 0;
  UART1.CR3.BITS.RTSE = 0;
  
  UART1.CR3.BITS.DMAT = 0;
  UART1.CR3.BITS.DMAR = 0;
  
  UART1.CR3.BITS.SCEN = 0;
  UART1.CR3.BITS.NACK = 0;
  
  UART1.CR3.BITS.HDSEL = 0;
  
  UART1.CR3.BITS.IRLP = 0;
  UART1.CR3.BITS.EIE = 0;
  
  UART1.CR2.BITS.LINEN = 0;
  UART1.CR2.BITS.STOP = 0;      //gửi 1 bit stop
  UART1.CR2.BITS.CLKEN = 0;
  UART1.CR2.BITS.CPOL = 0;
  UART1.CR2.BITS.CPHA = 0;
  UART1.CR2.BITS.LBCL = 0;
  UART1.CR2.BITS.LBDIE = 0;
  UART1.CR2.BITS.ADD = 0;
  
  UART1.CR1.BITS.M = 0;         //1 start > 8 data > n bit stop
  UART1.CR1.BITS.WAKE = 0;
  UART1.CR1.BITS.PCE = 0;
  UART1.CR1.BITS.PS = 0;
  UART1.CR1.BITS.PEIE = 0;     //không ngắt lỗi Parity
  UART1.CR1.BITS.TXEIE = 0;    //không ngắt TXE
  UART1.CR1.BITS.TCIE = 0;     //không ngắt TC
  UART1.CR1.BITS.RXNEIE = 1;   //ngắt khi nhận
  UART1.CR1.BITS.IDLEIE = 0;   //không ngắt khi rơi vào nghỉ
  UART1.CR1.BITS.TE = 1;        //cho phép truyền
  UART1.CR1.BITS.RE = 1;        //cho phép nhận
  UART1.CR1.BITS.RWU = 0;
  UART1.CR1.BITS.SBK = 0;
   
  *((unsigned long*)(0xE000E100 + 4)) |= 0x20;    //bật ngắt UART1 trong NVIC
  
  //system tick clock source = AHB = 8MHz
  STK.LOAD = 8000;              //1ms
  STK.CTRL.BITS.TICK_INT = 1;   //bật ngắt system tick timer
  STK.CTRL.BITS.CLK_SOURCE = 1;
  
  UART1.CR1.BITS.UE = 1;        //bật bus UART1 
  STK.CTRL.BITS.ENABLE = 1;     //bật system tick timer

  while (1)
  {
    BaseProcess();
    if (System.Event.RX)
    {
      for (unsigned long i = 0; i < UART1_RX.Size; i++)
      {
        UART1.DR = UART1_RX.Buff[i];
        while (!UART1.SR.BITS.TC);
      }
      UART1_RX.Size = 0;
      System.Event.RX = 0;
    }
  }
}