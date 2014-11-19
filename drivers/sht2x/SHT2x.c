//==============================================================================
//    S E N S I R I O N   AG,  Laubisruetistr. 50, CH-8712 Staefa, Switzerland
//==============================================================================
// Project   :  SHT2x Sample Code (V1.2)
// File      :  SHT2x.c
// Author    :  MST
// Controller:  NEC V850/SG3 (uPD70F3740)
// Compiler  :  IAR compiler for V850 (3.50A)
// Brief     :  Sensor layer. Functions for sensor access
//==============================================================================

//---------- Includes ----------------------------------------------------------
#include "SHT2x.h"
#include "periph/i2c.h"
#include "hwtimer.h"
#define ENABLE_DEBUG    (1)
#include "debug.h"
//==============================================================================
uint8_t SHT2x_CheckCrc(uint8_t data[], uint8_t nbrOfBytes, uint8_t checksum)
//==============================================================================
{
  uint8_t crc = 0;	
  uint8_t byteCtr;
  //calculates 8-Bit checksum with given polynomial
  for (byteCtr = 0; byteCtr < nbrOfBytes; ++byteCtr)
  { 
    crc ^= (data[byteCtr]);
    for (uint8_t bit = 8; bit > 0; --bit)
    { 
      if (crc & 0x80) {
        crc = ((crc << 1) ^ I2C_POLYNOMIAL);
      }
      else
        { 
          crc = (crc << 1); 
        }
    }
  }
  if (crc != checksum) 
    return -1;
  else   
    return 0;
}

//===========================================================================
uint8_t SHT2x_ReadUserRegister(uint8_t *pRegisterValue)
//===========================================================================
{
 // uint8_t checksum;   //variable for checksum byte
  int error=0;    //variable for error code


  error = i2c_read_reg(I2C_0, I2C_ADR_W, USER_REG_R, (char*)pRegisterValue);

  // error |= I2c_WriteByte (I2C_ADR_W);
  // error |= I2c_WriteByte (USER_REG_R);
  
  // error |= I2c_WriteByte (I2C_ADR_R);
  // *pRegisterValue = I2c_ReadByte(ACK);
  // checksum=I2c_ReadByte(NO_ACK);
  // error |= SHT2x_CheckCrc (pRegisterValue,1,checksum);
  
  return (uint8_t)error;
}

//===========================================================================
uint8_t SHT2x_WriteUserRegister(uint8_t *pRegisterValue)
//===========================================================================
{
  int error=0;   //variable for error code

  error = i2c_write_reg(I2C_0, I2C_ADR_W, USER_REG_W, (char)pRegisterValue[0]);
  // error |= I2c_WriteByte (I2C_ADR_W);
  // error |= I2c_WriteByte (USER_REG_W);
  // error |= I2c_WriteByte (*pRegisterValue);
  
  return (uint8_t)error;
}

//===========================================================================
uint8_t SHT2x_MeasureHM(etSHT2xMeasureType eSHT2xMeasureType, uint8_t *pMeasurand)
//===========================================================================
{
  uint8_t  checksum = 0;   //checksum
  uint8_t  data[3];    //data array for checksum verification
  int  error=0;    //error variable
  uint16_t i;          //counting variable
  uint8_t cmd;
  //-- write I2C sensor address and command --
  
  //error |= I2c_WriteByte (I2C_ADR_W); // I2C Adr
  switch(eSHT2xMeasureType)
  { case HUMIDITY:
      //error |= i2c_write_byte(I2C_0, I2C_ADR_W, TRIG_RH_MEASUREMENT_HM);
      cmd =  TRIG_RH_MEASUREMENT_HM;
      DEBUG("SHT21 measure after write HUMI!\n" );
      //error |= I2c_WriteByte (TRIG_RH_MEASUREMENT_HM); 
      break;
    case TEMP:
      //error |= i2c_write_byte(I2C_0, I2C_ADR_W, TRIG_T_MEASUREMENT_HM); 
      cmd = TRIG_T_MEASUREMENT_HM;
      DEBUG("SHT21 measure after write TEMP!\n" );
      //error |= I2c_WriteByte (TRIG_T_MEASUREMENT_HM);  
      break;
    default: 
      return 0;
  }
  //-- wait until hold master is released --
  

  
  //i2c_write_byte(I2C_0, I2C_ADR_W, 0);

  //error |= I2c_WriteByte (I2C_ADR_R);
  //SCL=HIGH;                     // set SCL I/O port as input
  for(i=0; i<1000; i++)         // wait until master hold is released or
  { 
    hwtimer_wait(HWTIMER_TICKS_TO_US(1000));
    //DelayMicroSeconds(1000);    // a timeout (~1s) is reached    
  }
  DEBUG("SHT21 measure after wait!\n" );
  //-- check for timeout --
  //if(SCL_CONF==0) error |= TIME_OUT_ERROR;
  data[0] = 0;
  data[1] = 0;
  data[2] = 0;
  //-- read two data bytes and one checksum byte --
  error |= i2c_read_regs(I2C_0, I2C_ADR_R, cmd, (char*)data, 3);
  //error |= i2c_read_bytes(I2C_0, I2C_ADR_R, (char*)data, 3);
  DEBUG("SHT21 measure after read!\n" );
  pMeasurand[0] = (uint8_t)data[0];
  pMeasurand[1] = (uint8_t)data[1];
  DEBUG("SHT21 measure after data: [0] %d and [1] %d !\n", data[0], data[1] );
  checksum = data[3];//i2c_read_byte(I2C_0, I2C_ADR_R, (char*)(&checksum));

  //-- verify checksum --
  error |= SHT2x_CheckCrc(data,2,checksum);
  
  return (uint8_t)error;
}

//===========================================================================
uint8_t SHT2x_MeasurePoll(etSHT2xMeasureType eSHT2xMeasureType, uint8_t *pMeasurand)
//===========================================================================
{
  uint8_t  checksum;   //checksum
  uint8_t  data[3];    //data array for checksum verification
  int  error=0;    //error variable
  uint16_t i=0;        //counting variable
  uint8_t cmd;
  //-- write I2C sensor address and command --
  
  //error |= I2c_WriteByte (I2C_ADR_W); // I2C Adr
    switch(eSHT2xMeasureType)
  { case HUMIDITY:
      //error |= i2c_write_byte(I2C_0, I2C_ADR_W, TRIG_RH_MEASUREMENT_POLL); 
      cmd =  TRIG_RH_MEASUREMENT_POLL;
      break;
    case TEMP:
      //error |= i2c_write_byte(I2C_0, I2C_ADR_W, TRIG_T_MEASUREMENT_POLL);  
      cmd =  TRIG_T_MEASUREMENT_POLL;
      break;
    default: 
      return 0;
  }
  // switch(eSHT2xMeasureType)
  // { case HUMIDITY: error |= I2c_WriteByte (TRIG_RH_MEASUREMENT_POLL); break;
  //   case TEMP    : error |= I2c_WriteByte (TRIG_T_MEASUREMENT_POLL);  break;
  //   default: assert(0);
  // }
  //-- poll every 10ms for measurement ready. Timeout after 20 retries (200ms)--
 for(i=0; i<20; i++)         // wait until master hold is released or
  { 
    hwtimer_wait(HWTIMER_TICKS_TO_US(10000));   
  }
  data[0] = 0;
  data[1] = 0;
  DEBUG("sht21 measure poll after wait");
  //-- read two data bytes and one checksum byte --
  error |= i2c_read_regs(I2C_0, I2C_ADR_R, cmd, (char*)data, 3);
  //error |= i2c_read_bytes(I2C_0, I2C_ADR_R, (char*)data, 2);
  pMeasurand[0] = (uint8_t)data[0];
  pMeasurand[1] = (uint8_t)data[1];
  checksum = (uint8_t)data[2];//i2c_read_byte(I2C_0, I2C_ADR_R, (char*)(&checksum));

  //-- verify checksum --
  error |= SHT2x_CheckCrc(data,2,checksum);
  

  return error;
}

//===========================================================================
uint8_t SHT2x_SoftReset(void)
//===========================================================================
{
  int  error=0;           //error variable

  
  error |= i2c_write_byte(I2C_0, I2C_ADR_W, SOFT_RESET); 
  //error |= I2c_WriteByte (I2C_ADR_W); // I2C Adr
  //error |= I2c_WriteByte (SOFT_RESET);                            // Command
  

  hwtimer_wait(HWTIMER_TICKS_TO_US(15000)); // wait till sensor has restarted

  return (uint8_t)error;
}

//==============================================================================
float SHT2x_CalcRH(uint16_t calcH)
//==============================================================================
{
  float humidityRH;              // variable for result

  calcH &= ~0x0003;          // clear bits [1..0] (status bits)
  //-- calculate relative humidity [%RH] --

  //-- calculate relative humidity [%RH] --
  humidityRH = -6.0 + 125.0/65536 * (float)calcH; // RH= -6 + 125 * SRH/2^16
  return humidityRH;
}

//==============================================================================
float SHT2x_CalcTemperatureC(uint16_t calcT)
//==============================================================================
{
  float temperatureC;            // variable for result

  calcT &= ~0x0003;           // clear bits [1..0] (status bits)

  //-- calculate temperature [°C] --
    temperatureC = -46.85 + 175.72/65536 *(float)calcT; //T= -46.85 + 175.72 * ST/2^16
  return temperatureC;
}

//==============================================================================
uint8_t SHT2x_GetSerialNumber(uint8_t u8SerialNumber[])
//==============================================================================
{
  uint8_t  error=0;                          //error variable

  //Read from memory location 1
  
  // error |= I2c_WriteByte (I2C_ADR_W);    //I2C address
  // error |= I2c_WriteByte (0xFA);         //Command for readout on-chip memory
  // error |= I2c_WriteByte (0x0F);         //on-chip memory address
  
  // error |= I2c_WriteByte (I2C_ADR_R);    //I2C address
  // u8SerialNumber[5] = I2c_ReadByte(ACK); //Read SNB_3
  // I2c_ReadByte(ACK);                     //Read CRC SNB_3 (CRC is not analyzed)
  // u8SerialNumber[4] = I2c_ReadByte(ACK); //Read SNB_2
  // I2c_ReadByte(ACK);                     //Read CRC SNB_2 (CRC is not analyzed)
  // u8SerialNumber[3] = I2c_ReadByte(ACK); //Read SNB_1
  // I2c_ReadByte(ACK);                     //Read CRC SNB_1 (CRC is not analyzed)
  // u8SerialNumber[2] = I2c_ReadByte(ACK); //Read SNB_0
  // I2c_ReadByte(NO_ACK);                  //Read CRC SNB_0 (CRC is not analyzed)
  

  // //Read from memory location 2
  
  // error |= I2c_WriteByte (I2C_ADR_W);    //I2C address
  // error |= I2c_WriteByte (0xFC);         //Command for readout on-chip memory
  // error |= I2c_WriteByte (0xC9);         //on-chip memory address
  
  // error |= I2c_WriteByte (I2C_ADR_R);    //I2C address
  // u8SerialNumber[1] = I2c_ReadByte(ACK); //Read SNC_1
  // u8SerialNumber[0] = I2c_ReadByte(ACK); //Read SNC_0
  // I2c_ReadByte(ACK);                     //Read CRC SNC0/1 (CRC is not analyzed)
  // u8SerialNumber[7] = I2c_ReadByte(ACK); //Read SNA_1
  // u8SerialNumber[6] = I2c_ReadByte(ACK); //Read SNA_0
  // I2c_ReadByte(NO_ACK);                  //Read CRC SNA0/1 (CRC is not analyzed)
  

  return error;
}