//#include <stm32f103c8t6.h>
#include <dma1.h>

unsigned char source[134];
unsigned char destiny[134];
unsigned long count = 0;

void main()
{
//  //cài đặt chân B12 là output push pull 50MHz
//  *((unsigned long*)(0x40021000 + 0x18)) |= 0x08;       //bật clock cho GPIOB
//  GPIOB.CRH.BITS.MODE12 = 3;   //chân B12 là out put max 50MHz
//  GPIOB.CRH.BITS.CNF12 = 0;    //chân B12 là General purpose output push-pull
//  
//  GPIOB.ODR.BITS.b8 = 1;        //chân B8 là pullup
//  GPIOB.CRH.BITS.MODE8 = 0;    //chân B8 là input
//  GPIOB.CRH.BITS.CNF8 = 2;     //chân B8 input pullup/pulldown
//  
//  GPIOB.ODR.BITS.b6 = 1;        //chân B6 là pullup
//  GPIOB.CRL.BITS.MODE6 = 0;    //chân B6 là input
//  GPIOB.CRL.BITS.CNF6 = 2;     //chân B6 input pullup/pulldown
  
  *((unsigned long*)(0x40021000 + 0x14)) |= 0x01;       //bật clock cho DMA1
  
  for (unsigned long i = 0; i < 134; i++)
  {
    source[i] = i;
    destiny[i] = 0;
  }
  /////  
  DMA1.Channel_1.CPAR = (unsigned long)source;       //source
  DMA1.Channel_1.CMAR = (unsigned long)destiny;       //destiny
  DMA1.Channel_1.CNTR = 134;   //copy 134 byte
  
  DMA1.Channel_1.CCR.BITS.MEM2MEM = 1;  //thực hiện copy từ vùng nhớ ram sang vùng nhớ ram khác
  DMA1.Channel_1.CCR.BITS.MSIZE = 0;    //chuyển 1 byte 1 lần
  DMA1.Channel_1.CCR.BITS.PSIZE = 0;    //chuyển 1 byte 1 lần
  DMA1.Channel_1.CCR.BITS.MINC = 1;     //tự động tăng địa chỉ của memory (ram)
  DMA1.Channel_1.CCR.BITS.PINC = 1;     //tự động tăng địa chỉ của peri (cũng là ram)
  DMA1.Channel_1.CCR.BITS.CIRC = 0;     //không lặp
  DMA1.Channel_1.CCR.BITS.DIR = 0;      //copy từ PER đến mem
  DMA1.Channel_1.CCR.BITS.TEIE = 0;     //tắt ngắt
  DMA1.Channel_1.CCR.BITS.HTIE = 0;     //...
  DMA1.Channel_1.CCR.BITS.TCIE = 0;     //...
  DMA1.Channel_1.CCR.BITS.EN = 1;
  
  while (DMA1.Channel_1.CNTR)
    count++;
  
  while (1)
  {
    
  }
}



