/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : lcd.c
* Author             : MCD Application Team
* Version            : V1.1.1
* Date               : 06/13/2008
* Description        : This file includes the LCD driver for AM-240320L8TNQW00H 
*                     (LCD_ILI9320) Liquid Crystal Display Module of STM3210E-EVAL
*                      board.
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/

#include "lcd_low.h"
#include "aux_lib.h"

#define Delay(x)    sleep_us((x) * 10000)
/* Private typedef -----------------------------------------------------------*/
typedef struct
{
  vu16 LCD_REG;
  vu16 LCD_RAM;
} LCD_TypeDef;

/* Note: LCD /CS is CE4 - Bank 4 of NOR/SRAM Bank 1~4 */
#define LCD_BASE        ((u32)(0x60000000 | 0x0C000000))
#define LCD             ((LCD_TypeDef *) LCD_BASE)


#define HSCREEN1    1  /* 1:定义为竖屛显示方式1 */
#define HSCREEN2    2
#define VSCREEN1    4
#define VSCREEN2    8

#define SCREENTYPE  VSCREEN1

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/                              
/* Private variables ---------------------------------------------------------*/
  
/* Private function prototypes -----------------------------------------------*/
static void LCD_CtrlLinesConfig(void);
static void LCD_FSMCConfig(void);
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : STM3210E_LCD_Init
* Description    : Initializes the LCD.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void STM3210E_LCD_Init(void)
{ 
    vu16 dummy = 0;
/* Configure the LCD Control pins --------------------------------------------*/
    LCD_CtrlLinesConfig();

/* Configure the FSMC Parallel interface -------------------------------------*/
    LCD_FSMCConfig();
    Delay(5); /* delay 50 ms */

//  LCD_WriteReg(R227, 0x3008);   // Set internal timing
//  LCD_WriteReg(R231, 0x0012); // Set internal timing
//  LCD_WriteReg(R239, 0x1231);   // Set internal timing
    LCD_WriteReg(R1  , 0x0000); // set SS and SM bit
//  LCD_WriteReg(R2  , 0x0700); // set 1 line inversion
    LCD_WriteReg(R2  , 0x0000);
    /* Set GRAM write direction and BGR = 1 */
    /* I/D=01 (Horizontal : increment, Vertical : decrement) */
    /* AM=1 (address is updated in vertical writing direction) */
    LCD_WriteReg(R3  , 0x0030);//0x1018);
//  LCD_WriteReg(R3  , 0x1030);   // set GRAM write direction and BGR=1.
    LCD_WriteReg(R4  , 0x0000);   // Resize register
    LCD_WriteReg(R8  , 0x0307);   // set the back porch and front porch
    LCD_WriteReg(R9  , 0x002F);   // set non-display area refresh cycle ISC[3:0]
    LCD_WriteReg(R10 , 0x0000);   // FMARK function
    LCD_WriteReg(R12 , 0x0000); // RGB interface setting
    LCD_WriteReg(R13 , 0x0001);   // Frame marker Position
    LCD_WriteReg(R15 , 0x0000); // RGB interface polarity
    /**************Power On sequence ****************/
    LCD_WriteReg(R16 , 0x0000);   // SAP, BT[3:0], AP, DSTB, SLP, STB
    LCD_WriteReg(R17 , 0x0007);   // DC1[2:0], DC0[2:0], VC[2:0]
    LCD_WriteReg(R18 , 0x0000); // VREG1OUT voltage
    LCD_WriteReg(R19 , 0x0000);   // VDV[4:0] for VCOM amplitude
    Delay(20);                    // Delay 200 MS , Dis-charge capacitor power voltage
    LCD_WriteReg(R16 , 0x1690);   // SAP, BT[3:0], AP, DSTB, SLP, STB
    LCD_WriteReg(R17 , 0x0227); // R11H=0x0221 at VCI=3.3V, DC1[2:0], DC0[2:0], VC[2:0]
    Delay(5);      // Delay 50ms
    LCD_WriteReg(R18 , 0x001D); // External reference voltage= Vci;
    Delay(5);      // Delay 50ms
    LCD_WriteReg(R19 , 0x0800); // R13H=1D00 when R12H=009D;VDV[4:0] for VCOM amplitude
    LCD_WriteReg(R41 , 0x0014); // R29H=0013 when R12H=009D;VCM[5:0] for VCOMH
//    LCD_WriteReg(R43 , 0x000B);   // Frame Rate = 96Hz
    LCD_WriteReg(R43 , 0x000d);

    Delay(5);      // Delay 50ms
    LCD_WriteReg(R32 , 0x0000); // GRAM horizontal Address
    LCD_WriteReg(R33 , 0x0000); // GRAM Vertical Address
    /* ----------- Adjust the Gamma Curve ---------- */
    LCD_WriteReg(R48 , 0x0007);
    LCD_WriteReg(R49 , 0x0707);
    LCD_WriteReg(R50 , 0x0006);
    LCD_WriteReg(R53 , 0x0704);
    LCD_WriteReg(R54 , 0x1F04);
    LCD_WriteReg(R55 , 0x0004);
    LCD_WriteReg(R56 , 0x0000);
    LCD_WriteReg(R57 , 0x0706);
    LCD_WriteReg(R60 , 0x0701);
    LCD_WriteReg(R61 , 0x000F);
    /* ------------------ Set GRAM area --------------- */
    LCD_WriteReg(R80 , 0x0000);   // Horizontal GRAM Start Address
    // LCD_WriteReg(R81 , 0x00EF);   // Horizontal GRAM End Address
    LCD_WriteReg(R81, 0x00EF);
    LCD_WriteReg(R82 , 0x0000); // Vertical GRAM Start Address
    LCD_WriteReg(R83 , 0x013F); // Vertical GRAM end Address
    LCD_WriteReg(R96 , 0x2700); // Gate Scan Line
    LCD_WriteReg(R97 , 0x0001); // NDL,VLE, REV
    LCD_WriteReg(R106, 0x0000); // set scrolling line
    /* -------------- Partial Display Control --------- */
    LCD_WriteReg(R128, 0x0000);   
    LCD_WriteReg(R129, 0x0000);
    LCD_WriteReg(R130, 0x0000);
    LCD_WriteReg(R131, 0x0000);
    LCD_WriteReg(R132, 0x0000);
    LCD_WriteReg(R133, 0x0000);
    /* -------------- Panel Control ------------------- */
    LCD_WriteReg(R144, 0x0010);
    LCD_WriteReg(R146, 0x0000);
    LCD_WriteReg(R147, 0x0003);
    LCD_WriteReg(R149, 0x0110);
    LCD_WriteReg(R151, 0x0000);
    LCD_WriteReg(R152, 0x0000);
  
//竖屛1
//#define HSCREEN 1


#if SCREENTYPE == VSCREEN1
 LCD_WriteReg(R1  , 0x0100); // set SS and SM bit
 LCD_WriteReg(R96 , 0xA700);
 LCD_WriteReg(R3  , 0x0030);//0x1018);
#elif SCREENTYPE == VSCREEN2
//竖屛2
 LCD_WriteReg(R1  , 0x0000); // set SS and SM bit
 LCD_WriteReg(R96 , 0x2700);
 LCD_WriteReg(R3  , 0x0030);//0x1018);
#elif SCREENTYPE == HSCREEN1
//横屛1
 LCD_WriteReg(R1  , 0x0000); // set SS and SM bit
 LCD_WriteReg(R96 , 0xA700);
 LCD_WriteReg(R3  , 0x0038); //0x1018);
#elif SCREENTYPE == HSCREEN2
//横屛2
 LCD_WriteReg(R1  , 0x0100); // set SS and SM bit
 LCD_WriteReg(R96 , 0x2700);
 LCD_WriteReg(R3  , 0x0038); //0x1018);
#else
#error "Err : SCREENTYPE"
#endif
  LCD_WriteReg(R7  , 0x0173); // 262K color and display ON

}


/*******************************************************************************
* Function Name  : LCD_Clear
* Description    : Clears the hole LCD.
* Input          : Color: the color of the background.
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_Clear(u16 Color)
{
  u32 index = 0;
  
  LCD_SetCursor(0x00, 0x0000); 

  LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */

  for(index = 0; index < 76800; index++)
  {
    LCD->LCD_RAM = Color;	
  }
}




/*******************************************************************************
* Function Name  : LCD_SetCursor
* Description    : Sets the cursor position.
* Input          : - Xpos: specifies the X position.
*                  - Ypos: specifies the Y position. 
* Output         : None
* Return         : None
*******************************************************************************/
__inline void LCD_SetCursor(u16 Xpos, u16 Ypos)
{ 
#if (SCREENTYPE & (VSCREEN1 | VSCREEN2)) 
  LCD_WriteReg(R32, Xpos);
  LCD_WriteReg(R33, Ypos);
#elif (SCREENTYPE & (HSCREEN1 | HSCREEN2))
  LCD_WriteReg(R32, Ypos);
  LCD_WriteReg(R33, Xpos);
#else
#error "Err : SCREENTYPE"
#endif
}


/*******************************************************************************
* Function Name  : LCD_SetDisplayWindow
* Description    : Sets a display window
* Input          : - Xpos: specifies the X buttom left position.
*                  - Ypos: specifies the Y buttom left position.
*                  - Height: display window height.
*                  - Width: display window width.
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_SetDisplayWindow(u8 Xpos, u16 Ypos, u8 Height, u16 Width)
{
  /* Horizontal GRAM Start Address */
  if(Xpos >= Height)
  {
    LCD_WriteReg(R80, (Xpos - Height + 1));
  }
  else
  {
    LCD_WriteReg(R80, 0);
  }
  /* Horizontal GRAM End Address */
  LCD_WriteReg(R81, Xpos);
  /* Vertical GRAM Start Address */
  if(Ypos >= Width)
  {
    LCD_WriteReg(R82, (Ypos - Width + 1));
  }  
  else
  {
    LCD_WriteReg(R82, 0);
  }
  /* Vertical GRAM End Address */
  LCD_WriteReg(R83, Ypos);

  LCD_SetCursor(Xpos, Ypos);
}

/*******************************************************************************
* Function Name  : LCD_WindowModeDisable
* Description    : Disables LCD Window mode.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_WindowModeDisable(void)
{
  LCD_SetDisplayWindow(239, 0x13F, 240, 320);
  LCD_WriteReg(R3, 0x1018);    
}




/*******************************************************************************
* Function Name  : LCD_PowerOn
* Description    : Power on the LCD.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_PowerOn(void)
{
/* Power On sequence ---------------------------------------------------------*/
  LCD_WriteReg(R16, 0x0000); /* SAP, BT[3:0], AP, DSTB, SLP, STB */
  LCD_WriteReg(R17, 0x0000); /* DC1[2:0], DC0[2:0], VC[2:0] */
  LCD_WriteReg(R18, 0x0000); /* VREG1OUT voltage */
  LCD_WriteReg(R19, 0x0000); /* VDV[4:0] for VCOM amplitude*/
  Delay(20);             /* Dis-charge capacitor power voltage (200ms) */
  LCD_WriteReg(R16, 0x17B0); /* SAP, BT[3:0], AP, DSTB, SLP, STB */
  LCD_WriteReg(R17, 0x0137); /* DC1[2:0], DC0[2:0], VC[2:0] */
  Delay(5);              /* Delay 50 ms */
  LCD_WriteReg(R18, 0x0139); /* VREG1OUT voltage */
  Delay(5);              /* Delay 50 ms */
  LCD_WriteReg(R19, 0x1d00); /* VDV[4:0] for VCOM amplitude */
  LCD_WriteReg(R41, 0x0013); /* VCM[4:0] for VCOMH */
  Delay(5);              /* Delay 50 ms */
  LCD_WriteReg(R7, 0x0173);  /* 262K color and display ON */
}

/*******************************************************************************
* Function Name  : LCD_DisplayOn
* Description    : Enables the Display.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_DisplayOn(void)
{
  /* Display On */
  LCD_WriteReg(R7, 0x0173); /* 262K color and display ON */
}

/*******************************************************************************
* Function Name  : LCD_DisplayOff
* Description    : Disables the Display.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_DisplayOff(void)
{
  /* Display Off */
  LCD_WriteReg(R7, 0x0); 
}

/*******************************************************************************
* Function Name  : LCD_WriteReg
* Description    : Writes to the selected LCD register.
* Input          : - LCD_Reg: address of the selected register.
*                  - LCD_RegValue: value to write to the selected register.
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_WriteReg(u8 LCD_Reg, u16 LCD_RegValue)
{
  /* Write 16-bit Index, then Write Reg */
  LCD->LCD_REG = LCD_Reg;
  /* Write 16-bit Reg */
  LCD->LCD_RAM = LCD_RegValue;
}

/*******************************************************************************
* Function Name  : LCD_ReadReg
* Description    : Reads the selected LCD Register.
* Input          : None
* Output         : None
* Return         : LCD Register Value.
*******************************************************************************/
u16 LCD_ReadReg(u8 LCD_Reg)
{
  /* Write 16-bit Index (then Read Reg) */
  LCD->LCD_REG = LCD_Reg;
  /* Read 16-bit Reg */
  return (LCD->LCD_RAM);
}

/*******************************************************************************
* Function Name  : LCD_WriteRAM_Prepare
* Description    : Prepare to write to the LCD RAM.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_WriteRAM_Prepare(void)
{ 
  LCD->LCD_REG = R34; 
}
/*******************************************************************************
* Function Name  : LCD_WriteRAM
* Description    : Writes to the LCD RAM.
* Input          : - RGB_Code: the pixel color in RGB mode (5-6-5).
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_WriteRAM(u16 RGB_Code)
{
  /* Write 16-bit GRAM Reg */
  LCD->LCD_RAM = RGB_Code;
}

/*******************************************************************************
* Function Name  : LCD_ReadRAM
* Description    : Reads the LCD RAM.
* Input          : None
* Output         : None
* Return         : LCD RAM Value.
*******************************************************************************/
u16 LCD_ReadRAM(void)
{
  /* Write 16-bit Index (then Read Reg) */
  LCD->LCD_REG = R34; /* Select GRAM Reg */
  /* Read 16-bit Reg */
  return LCD->LCD_RAM;
}

/*******************************************************************************
* Function Name  : LCD_CtrlLinesConfig
* Description    : Configures LCD Control lines (FSMC Pins) in alternate function
                   Push-Pull mode.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void LCD_CtrlLinesConfig(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* Enable FSMC, GPIOD, GPIOE, GPIOF, GPIOG and AFIO clocks */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE);

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE |
                         RCC_APB2Periph_GPIOF | RCC_APB2Periph_GPIOG, ENABLE);

  /* Set PD.00(D2), PD.01(D3), PD.04(NOE), PD.05(NWE), PD.08(D13), PD.09(D14),
     PD.10(D15), PD.14(D0), PD.15(D1) as alternate 
     function push pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5 |
                                GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_14 | 
                                GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOD, &GPIO_InitStructure);

  /* Set PE.07(D4), PE.08(D5), PE.09(D6), PE.10(D7), PE.11(D8), PE.12(D9), PE.13(D10),
     PE.14(D11), PE.15(D12) as alternate function push pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | 
                                GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | 
                                GPIO_Pin_15;
  GPIO_Init(GPIOE, &GPIO_InitStructure);

  /* Set PF.00(A0 (RS)) as alternate function push pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_Init(GPIOF, &GPIO_InitStructure);

  /* Set PG.12(NE4 (LCD/CS)) as alternate function push pull - CE3(LCD /CS) */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
  GPIO_Init(GPIOG, &GPIO_InitStructure);
}

/*******************************************************************************
* Function Name  : LCD_FSMCConfig
* Description    : Configures the Parallel interface (FSMC) for LCD(Parallel mode)
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void LCD_FSMCConfig(void)
{
  FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
  FSMC_NORSRAMTimingInitTypeDef  p;

/*-- FSMC Configuration ------------------------------------------------------*/
/*----------------------- SRAM Bank 4 ----------------------------------------*/
  /* FSMC_Bank1_NORSRAM4 configuration */
  p.FSMC_AddressSetupTime = 0;
  p.FSMC_AddressHoldTime = 0;
  p.FSMC_DataSetupTime = 2;
  p.FSMC_BusTurnAroundDuration = 0;
  p.FSMC_CLKDivision = 0;
  p.FSMC_DataLatency = 0;
  p.FSMC_AccessMode = FSMC_AccessMode_A;

  /* Color LCD configuration ------------------------------------
     LCD configured as follow:
        - Data/Address MUX = Disable
        - Memory Type = SRAM
        - Data Width = 16bit
        - Write Operation = Enable
        - Extended Mode = Enable
        - Asynchronous Wait = Disable */
  FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM4;
  FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
  FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;
  FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
  FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
  FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
  FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
  FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
//  FSMC_NORSRAMInitStructure.FSMC_AsyncWait = FSMC_AsyncWait_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
  FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &p;
  FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &p;

  FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);  

  /* BANK 4 (of NOR/SRAM Bank 1~4) is enabled */
  FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM4, ENABLE);
}

/******************  for ucgui use ************************/
unsigned int LCD_ReadP(int x, int y)
{
    LCD_SetCursor(x,y);
    LCD_ReadRAM();
    return LCD_ReadRAM();

}
void LCD_WriteP(int x, int y, int c)
{
    LCD_SetCursor(x, y);
    LCD_WriteRAM_Prepare(); 
    LCD_WriteRAM(c);
}

/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
