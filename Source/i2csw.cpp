//----------------------------------------------------------------------------

//������ ����������� ���������� ���� I2C

//----------------------- ������������ �������: ------------------------------

//������������ ���� SCL (PB8) � SDA (PB9).
//��� ������������ ��������� ���� ������������ ������ TIM16.

//----------------------------------------------------------------------------

#include "main.h"
#include "systimer.h"
#include "i2csw.h"

//----------------------------- ���������: -----------------------------------

#define I2C_CLK    100 //������� ���� I2C, ���

#define I2C_DELAY (APB2_CLOCK / 1000 / I2C_CLK / 2 - 18)

//----------------------------------------------------------------------------
//----------------------------- ����� TI2Csw: --------------------------------
//----------------------------------------------------------------------------

//----------------------------- �������������: -------------------------------

TGpio<PORTB, PIN8> TI2Csw::Pin_SCL; 
TGpio<PORTB, PIN9> TI2Csw::Pin_SDA;

void TI2Csw::Init(void)
{
  Pin_SCL.Init(OUT_OD_2M, OUT_HI);
  Pin_SDA.Init(OUT_OD_2M, OUT_HI);

  RCC->APB2ENR |= RCC_APB2ENR_TIM16EN; //��������� ������������ TIM16
  TIM16->PSC = 0;                      //�������� ����������
  TIM16->ARR = 0xFFFF;                 //������
  TIM16->CR1 = TIM_CR1_CEN;            //���������� �������
  
  Free(); //����� I2C
  Stop();
}

//------------------------ ������������ ���� I2C: ----------------------------

void TI2Csw::Free(void)
{
  for(char i = 0; i < 9; i++)
  {
    if(Pin_SDA) break;
    Pin_SCL = 0;
    BitDelay();
    Pin_SCL = 1;
    BitDelay();
  }
}

//------------------- ��������� ������� "�����" �� I2C: ----------------------

void TI2Csw::Start(void)
{
  Pin_SDA = 0;
  BitDelay();
}

//------------------------- ������ ����� �� I2C: -----------------------------

bool TI2Csw::Write(char data)
{
  bool ack = 0;
  for(char i = 0; i < 8; i++)
  {
    Pin_SCL = 0;
    if(data & 0x80)
      Pin_SDA = 1;
        else Pin_SDA = 0;
    data <<= 1;    
    BitDelay();
    Pin_SCL = 1;
    BitDelay();
  }
  Pin_SCL = 0;
  Pin_SDA = 1;
  BitDelay();
  Pin_SCL = 1;
  BitDelay();
  ack = !Pin_SDA;
  BitDelay();
  return(ack);
}

//------------------------- ������ ����� �� I2C: -----------------------------

char TI2Csw::Read(bool ack)
{
  char data;
  for(char i = 0; i < 8; i++)
  {
    Pin_SCL = 0;
    Pin_SDA = 1;
    BitDelay();
    Pin_SCL = 1;
    BitDelay();
    data = data << 1;
    if(Pin_SDA) data |= 0x01;
  }
  Pin_SCL = 0;
  Pin_SDA = !ack;
  BitDelay();
  Pin_SCL = 1;
  BitDelay();
  return(data);
}

//------------------- ��������� ������� "����" �� I2C: -----------------------

void TI2Csw::Stop(void)
{
  Pin_SCL = 0;
  Pin_SDA = 0;
  BitDelay();
  Pin_SCL = 1;
  BitDelay();
  Pin_SDA = 1;
  BitDelay();
}

//---------------------- �������� 1/2 ������� I2C: ---------------------------

void TI2Csw::BitDelay(void)
{
  TIM16->CNT = 0;
  while(TIM16->CNT < I2C_DELAY);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
