#include "stm32f7xx_hal.h"
#include <string.h>

#include "mb.h"
#include "mbport.h"

#include "lwip.h"
#include "lwip/api.h"
#include "lwip/tcp.h"

#define REG_INPUT_START 0x01
#define REG_INPUT_NREGS 100
#define REG_HOLDING_START 0x01
#define REG_HOLDING_NREGS 100

static USHORT usRegInputStart = REG_INPUT_START;
static USHORT usRegInputBuf[REG_INPUT_NREGS];
static USHORT usRegHoldingStart = REG_HOLDING_START;
static USHORT usRegHoldingBuf[REG_HOLDING_NREGS];

static UCHAR Vendor[11] = "Modbus TCP";
extern struct netif gnetif;

void User_notification(struct netif *netif);

void ModbusTask()
{ 
	  BSP_LCD_DisplayStringAt(20, 20, (uint8_t *)"TCP Server", CENTER_MODE);

  
  for(int i=0; i < 100; i++)
  {
    usRegInputBuf[i] = i;
  }
  
  for(int i=0; i < 100; i++)
  {
    usRegHoldingBuf[i] = i+100;
  }
    
  
  User_notification(&gnetif);
  
  if(eMBTCPInit(0) != MB_ENOERR)
    return;     
    
  if(eMBSetSlaveID( 1, TRUE, Vendor, sizeof(Vendor)) != MB_ENOERR)
    return;
  
  if(eMBEnable() != MB_ENOERR)
    return;
  
  while(1) 
  {   
    eMBPoll(); 
    MX_LWIP_Process();
  }    
}

void User_notification(struct netif *netif) 
{
  if (netif_is_up(netif))
  {
    /* Turn On LED 1 to indicate ETH and LwIP init success*/

  }
  else
  {     
    /* Turn On LED 2 to indicate ETH and LwIP init error */
  } 
}

eMBErrorCode eMBRegInputCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs )
{
  eMBErrorCode    eStatus = MB_ENOERR;
  int             iRegIndex;  
    
  if( ( usAddress >= REG_INPUT_START ) && ( usAddress + usNRegs <= REG_INPUT_START + REG_INPUT_NREGS ) )
  {
    iRegIndex = ( int )( usAddress - usRegInputStart );
    while( usNRegs > 0 )
    {
      *pucRegBuffer++ =
        ( unsigned char )( usRegInputBuf[iRegIndex] >> 8 );
      *pucRegBuffer++ =
        ( unsigned char )( usRegInputBuf[iRegIndex] & 0xFF );
      iRegIndex++;
      usNRegs--;
    }
  }
  else
  {    
    eStatus = MB_ENOREG;
  }
  
  

  return eStatus;
}

eMBErrorCode eMBRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs, eMBRegisterMode eMode )
{
  eMBErrorCode    eStatus = MB_ENOERR;
  int             iRegIndex;
      
  if( ( usAddress >= REG_HOLDING_START ) && ( usAddress + usNRegs <= REG_HOLDING_START + REG_HOLDING_NREGS ) )
  {
    iRegIndex = ( int )( usAddress - usRegHoldingStart );
    switch ( eMode )
    {
    case MB_REG_READ:
      while( usNRegs > 0 )
      {
        *pucRegBuffer++ = ( unsigned char )( usRegHoldingBuf[iRegIndex] >> 8 );
        *pucRegBuffer++ = ( unsigned char )( usRegHoldingBuf[iRegIndex] & 0xFF );
        iRegIndex++;
        usNRegs--;
      }
      break;
      
    case MB_REG_WRITE:
      while( usNRegs > 0 )
      {
        usRegHoldingBuf[iRegIndex] = *pucRegBuffer++ << 8;
        usRegHoldingBuf[iRegIndex] |= *pucRegBuffer++;
        iRegIndex++;
        usNRegs--;
      }
    }
  }
  else
  {
    eStatus = MB_ENOREG;
  }
  
  
  return eStatus;
}


eMBErrorCode eMBRegCoilsCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNCoils, eMBRegisterMode eMode )
{
  return MB_ENOREG;
}

eMBErrorCode eMBRegDiscreteCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete )
{
  return MB_ENOREG;
}
