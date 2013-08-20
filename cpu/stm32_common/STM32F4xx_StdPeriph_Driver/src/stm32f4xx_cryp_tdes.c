/**
  ******************************************************************************
  * @file    stm32f4xx_cryp_tdes.c
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    11-January-2013
  * @brief   This file provides high level functions to encrypt and decrypt an 
  *          input message using TDES in ECB/CBC modes .
  *          It uses the stm32f4xx_cryp.c/.h drivers to access the STM32F4xx CRYP
  *          peripheral.
  *
@verbatim

 ===================================================================
                  ##### How to use this driver #####
 ===================================================================
 [..]
   (#) Enable The CRYP controller clock using 
       RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_CRYP, ENABLE); function.
  
   (#) Encrypt and decrypt using TDES in ECB Mode using CRYP_TDES_ECB() function.
  
   (#) Encrypt and decrypt using TDES in CBC Mode using CRYP_TDES_CBC() function.
  
@endverbatim
  *
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_cryp.h"


/** @addtogroup STM32F4xx_StdPeriph_Driver
  * @{
  */

/** @defgroup CRYP 
  * @brief CRYP driver modules
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define TDESBUSY_TIMEOUT    ((uint32_t) 0x00010000)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


/** @defgroup CRYP_Private_Functions
  * @{
  */ 

/** @defgroup CRYP_Group7 High Level TDES functions
 *  @brief   High Level TDES functions 
 *
@verbatim   
 ===============================================================================
                      ##### High Level TDES functions #####
 ===============================================================================

@endverbatim
  * @{
  */

/**
  * @brief  Encrypt and decrypt using TDES in ECB Mode
  * @param  Mode: encryption or decryption Mode.
  *           This parameter can be one of the following values:
  *            @arg MODE_ENCRYPT: Encryption
  *            @arg MODE_DECRYPT: Decryption
  * @param  Key: Key used for TDES algorithm.
  * @param  Ilength: length of the Input buffer, must be a multiple of 8.
  * @param  Input: pointer to the Input buffer.
  * @param  Output: pointer to the returned buffer.
  * @retval An ErrorStatus enumeration value:
  *          - SUCCESS: Operation done
  *          - ERROR: Operation failed
  */
ErrorStatus CRYP_TDES_ECB(uint8_t Mode, uint8_t Key[24], uint8_t *Input, 
                          uint32_t Ilength, uint8_t *Output)
{
  CRYP_InitTypeDef TDES_CRYP_InitStructure;
  CRYP_KeyInitTypeDef TDES_CRYP_KeyInitStructure;
  __IO uint32_t counter = 0;
  uint32_t busystatus = 0;
  ErrorStatus status = SUCCESS;
  uint32_t keyaddr    = (uint32_t)Key;
  uint32_t inputaddr  = (uint32_t)Input;
  uint32_t outputaddr = (uint32_t)Output;
  uint32_t i = 0;

  /* Crypto structures initialisation*/
  CRYP_KeyStructInit(&TDES_CRYP_KeyInitStructure);

  /* Crypto Init for Encryption process */
  if(Mode == MODE_ENCRYPT) /* TDES encryption */
  {
     TDES_CRYP_InitStructure.CRYP_AlgoDir = CRYP_AlgoDir_Encrypt;
  }
  else /*if(Mode == MODE_DECRYPT)*/ /* TDES decryption */
  {
     TDES_CRYP_InitStructure.CRYP_AlgoDir = CRYP_AlgoDir_Decrypt;
  }

  TDES_CRYP_InitStructure.CRYP_AlgoMode = CRYP_AlgoMode_TDES_ECB;
  TDES_CRYP_InitStructure.CRYP_DataType = CRYP_DataType_8b;
  CRYP_Init(&TDES_CRYP_InitStructure);

  /* Key Initialisation */
  TDES_CRYP_KeyInitStructure.CRYP_Key1Left = __REV(*(uint32_t*)(keyaddr));
  keyaddr+=4;
  TDES_CRYP_KeyInitStructure.CRYP_Key1Right= __REV(*(uint32_t*)(keyaddr));
  keyaddr+=4;
  TDES_CRYP_KeyInitStructure.CRYP_Key2Left = __REV(*(uint32_t*)(keyaddr));
  keyaddr+=4;
  TDES_CRYP_KeyInitStructure.CRYP_Key2Right= __REV(*(uint32_t*)(keyaddr));
  keyaddr+=4;
  TDES_CRYP_KeyInitStructure.CRYP_Key3Left = __REV(*(uint32_t*)(keyaddr));
  keyaddr+=4;
  TDES_CRYP_KeyInitStructure.CRYP_Key3Right= __REV(*(uint32_t*)(keyaddr));
  CRYP_KeyInit(& TDES_CRYP_KeyInitStructure);

  /* Flush IN/OUT FIFO */
  CRYP_FIFOFlush();

  /* Enable Crypto processor */
  CRYP_Cmd(ENABLE);

  if(CRYP_GetCmdStatus() == DISABLE)
  {
    /* The CRYP peripheral clock is not enabled or the device doesn't embedd 
       the CRYP peripheral (please check the device sales type. */
    return(ERROR);
  }
  for(i=0; ((i<Ilength) && (status != ERROR)); i+=8)
  {
    /* Write the Input block in the Input FIFO */
    CRYP_DataIn(*(uint32_t*)(inputaddr));
    inputaddr+=4;
    CRYP_DataIn(*(uint32_t*)(inputaddr));
    inputaddr+=4;

    /* Wait until the complete message has been processed */
    counter = 0;
    do
    {
      busystatus = CRYP_GetFlagStatus(CRYP_FLAG_BUSY);
      counter++;
    }while ((counter != TDESBUSY_TIMEOUT) && (busystatus != RESET));

    if (busystatus != RESET)
    {
       status = ERROR;
    }
    else
    {

      /* Read the Output block from the Output FIFO */
      *(uint32_t*)(outputaddr) = CRYP_DataOut();
      outputaddr+=4;
      *(uint32_t*)(outputaddr) = CRYP_DataOut();
      outputaddr+=4;
    }
  }

  /* Disable Crypto */
  CRYP_Cmd(DISABLE);

  return status; 
}

/**
  * @brief  Encrypt and decrypt using TDES in CBC Mode
  * @param  Mode: encryption or decryption Mode.
  *           This parameter can be one of the following values:
  *            @arg MODE_ENCRYPT: Encryption
  *            @arg MODE_DECRYPT: Decryption
  * @param  Key: Key used for TDES algorithm.
  * @param  InitVectors: Initialisation Vectors used for TDES algorithm.
  * @param  Input: pointer to the Input buffer.
  * @param  Ilength: length of the Input buffer, must be a multiple of 8.
  * @param  Output: pointer to the returned buffer.
  * @retval An ErrorStatus enumeration value:
  *          - SUCCESS: Operation done
  *          - ERROR: Operation failed
  */
ErrorStatus CRYP_TDES_CBC(uint8_t Mode, uint8_t Key[24], uint8_t InitVectors[8],
                          uint8_t *Input, uint32_t Ilength, uint8_t *Output)
{
  CRYP_InitTypeDef TDES_CRYP_InitStructure;
  CRYP_KeyInitTypeDef TDES_CRYP_KeyInitStructure;
  CRYP_IVInitTypeDef TDES_CRYP_IVInitStructure;
  __IO uint32_t counter = 0;
  uint32_t busystatus = 0;
  ErrorStatus status = SUCCESS;
  uint32_t keyaddr    = (uint32_t)Key;
  uint32_t inputaddr  = (uint32_t)Input;
  uint32_t outputaddr = (uint32_t)Output;
  uint32_t ivaddr     = (uint32_t)InitVectors;
  uint32_t i = 0;

  /* Crypto structures initialisation*/
  CRYP_KeyStructInit(&TDES_CRYP_KeyInitStructure);

  /* Crypto Init for Encryption process */
  if(Mode == MODE_ENCRYPT) /* TDES encryption */
  {
    TDES_CRYP_InitStructure.CRYP_AlgoDir = CRYP_AlgoDir_Encrypt;
  }
  else
  {
    TDES_CRYP_InitStructure.CRYP_AlgoDir = CRYP_AlgoDir_Decrypt;
  }
  TDES_CRYP_InitStructure.CRYP_AlgoMode = CRYP_AlgoMode_TDES_CBC;
  TDES_CRYP_InitStructure.CRYP_DataType = CRYP_DataType_8b;

  CRYP_Init(&TDES_CRYP_InitStructure);

  /* Key Initialisation */
  TDES_CRYP_KeyInitStructure.CRYP_Key1Left = __REV(*(uint32_t*)(keyaddr));
  keyaddr+=4;
  TDES_CRYP_KeyInitStructure.CRYP_Key1Right= __REV(*(uint32_t*)(keyaddr));
  keyaddr+=4;
  TDES_CRYP_KeyInitStructure.CRYP_Key2Left = __REV(*(uint32_t*)(keyaddr));
  keyaddr+=4;
  TDES_CRYP_KeyInitStructure.CRYP_Key2Right= __REV(*(uint32_t*)(keyaddr));
  keyaddr+=4;
  TDES_CRYP_KeyInitStructure.CRYP_Key3Left = __REV(*(uint32_t*)(keyaddr));
  keyaddr+=4;
  TDES_CRYP_KeyInitStructure.CRYP_Key3Right= __REV(*(uint32_t*)(keyaddr));
  CRYP_KeyInit(& TDES_CRYP_KeyInitStructure);

  /* Initialization Vectors */
  TDES_CRYP_IVInitStructure.CRYP_IV0Left = __REV(*(uint32_t*)(ivaddr));
  ivaddr+=4;
  TDES_CRYP_IVInitStructure.CRYP_IV0Right= __REV(*(uint32_t*)(ivaddr));
  CRYP_IVInit(&TDES_CRYP_IVInitStructure);

  /* Flush IN/OUT FIFO */
  CRYP_FIFOFlush();

  /* Enable Crypto processor */
  CRYP_Cmd(ENABLE);

  if(CRYP_GetCmdStatus() == DISABLE)
  {
    /* The CRYP peripheral clock is not enabled or the device doesn't embedd 
       the CRYP peripheral (please check the device sales type. */
    return(ERROR);
  }
  
  for(i=0; ((i<Ilength) && (status != ERROR)); i+=8)
  {
    /* Write the Input block in the Input FIFO */
    CRYP_DataIn(*(uint32_t*)(inputaddr));
    inputaddr+=4;
    CRYP_DataIn(*(uint32_t*)(inputaddr));
    inputaddr+=4;

    /* Wait until the complete message has been processed */
    counter = 0;
    do
    {
      busystatus = CRYP_GetFlagStatus(CRYP_FLAG_BUSY);
      counter++;
    }while ((counter != TDESBUSY_TIMEOUT) && (busystatus != RESET));

    if (busystatus != RESET)
   {
       status = ERROR;
    }
    else
    {

      /* Read the Output block from the Output FIFO */
      *(uint32_t*)(outputaddr) = CRYP_DataOut();
      outputaddr+=4;
      *(uint32_t*)(outputaddr) = CRYP_DataOut();
      outputaddr+=4;
    }
  }

  /* Disable Crypto */
  CRYP_Cmd(DISABLE);

  return status; 
}
/**
  * @}
  */ 

/**
  * @}
  */ 

/**
  * @}
  */ 

/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
