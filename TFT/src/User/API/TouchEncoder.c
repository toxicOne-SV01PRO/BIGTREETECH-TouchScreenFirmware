#include "TouchEncoder.h"
#include "GPIO_Init.h"
#include "includes.h"

#if TOUCH_ENCODER_SUPPORT


const GUI_RECT encoderRect[3]={
  //Left icon
  {(LCD_WIDTH-E_ICON_WIDTH)/2 - E_ICON_WIDTH*1 - START_X , SIMULATOR_YEND + (LCD_HEIGHT - SIMULATOR_YEND - E_ICON_WIDTH)/2,
   (LCD_WIDTH-E_ICON_WIDTH)/2 - E_ICON_WIDTH*0 - START_X , SIMULATOR_YEND + (LCD_HEIGHT - SIMULATOR_YEND + E_ICON_WIDTH)/2},
  //select icon
  {(LCD_WIDTH-E_ICON_WIDTH)/2,  SIMULATOR_YEND + (LCD_HEIGHT - SIMULATOR_YEND - E_ICON_WIDTH)/2,
   (LCD_WIDTH+E_ICON_WIDTH)/2,  SIMULATOR_YEND + (LCD_HEIGHT - SIMULATOR_YEND + E_ICON_WIDTH)/2} ,
  //Right Icon
  {(LCD_WIDTH+E_ICON_WIDTH)/2 + E_ICON_WIDTH*0 + START_X ,   SIMULATOR_YEND + (LCD_HEIGHT - SIMULATOR_YEND - E_ICON_WIDTH)/2,
   (LCD_WIDTH+E_ICON_WIDTH)/2 + E_ICON_WIDTH*1 + START_X ,   SIMULATOR_YEND + (LCD_HEIGHT - SIMULATOR_YEND + E_ICON_WIDTH)/2}
};

const   uint16_t encPin[3]  = { TOUCH_BTN_PIN, TOUCH_ENCA_PIN,  TOUCH_ENCB_PIN};


int8_t encoderDirection = 1;
volatile int8_t encoderDiff; // Updated in update_buttons, added to encoderPosition every LCD update
int16_t encoderPosition = 0;
uint8_t buttons = 0;
uint8_t _encLastBtn =0;

void touchEncoderInit(void)
{
  GPIO_InitSet(encPin[0], MGPIO_MODE_IPU, 0);
  GPIO_InitSet(encPin[1], MGPIO_MODE_OUT_PP, 0);
  GPIO_InitSet(encPin[2], MGPIO_MODE_OUT_PP, 0);
  GUI_RestoreColorDefault();
  GUI_ClearRect(0, SIMULATOR_YEND, LCD_WIDTH, LCD_HEIGHT);
  GUI_DrawRect(0, SIMULATOR_YEND, LCD_WIDTH, SIMULATOR_YEND+2);
  ICON_CustomReadDisplay(encoderRect[0].x0,encoderRect[0].y0, E_ICON_WIDTH, E_ICON_WIDTH, ICON_ADDR(ICON_ENC_LEFT));
  ICON_CustomReadDisplay(encoderRect[1].x0,encoderRect[1].y0, E_ICON_WIDTH, E_ICON_WIDTH, ICON_ADDR(ICON_ENC_SEL));
  ICON_CustomReadDisplay(encoderRect[2].x0,encoderRect[2].y0, E_ICON_WIDTH, E_ICON_WIDTH, ICON_ADDR(ICON_ENC_RIGHT));
  sendPulse(ENC_LEFT);
  sendPulse(ENC_SEL);
}

void checkTouchButton(void)
{
  static ENC_VALUES lastbtn = ENC_IDLE;
  static u32 lastTime = 0;
  u32 nowtime;
  static u32 lastRepeat = 0;
  static u8 repeatcount = 0;
  //u32 slowDelay = 250;
  //u32 fastDelay = 2000;
  u32 slow_Repeat_time = 250;
  u32 fast_Repeat_time = 100;

  u16 tx, ty;

  // for (int i = 0; i < 3; i++)
  // {
  //   GUI_DrawPrect(&encoderRect[i]);
  // }
  if (!XPT2046_Read_Pen())
  {
    nowtime = OS_GetTimeMs();
    TS_Get_Coordinates(&tx, &ty);
    ENC_VALUES curbtn = ENC_IDLE;
    if (tx > encoderRect[0].x0 && tx < encoderRect[0].x1 && ty > encoderRect[0].y0 && ty < encoderRect[0].y1)
      curbtn = ENC_LEFT;
    else if (tx > encoderRect[1].x0 && tx < encoderRect[1].x1 && ty > encoderRect[1].y0 && ty < encoderRect[1].y1)
      curbtn = ENC_SEL;
    else if (tx > encoderRect[2].x0 && tx < encoderRect[2].x1 && ty > encoderRect[2].y0 && ty < encoderRect[2].y1)
      curbtn = ENC_RIGHT;
    else
      return;

    skipMode = true;
    if (lastbtn != curbtn)
    {
      lastbtn = curbtn;
      repeatcount = 0;
      lastRepeat = nowtime;
      Buzzer_play(sound_keypress);
      sendPulse(curbtn);
    }
    else
    {
      if (nowtime - lastTime < PULSE_DELAY*5)
      {
        if (repeatcount >= 10 && (nowtime - lastRepeat) > fast_Repeat_time)
        {
          lastRepeat = nowtime;
          sendPulse(curbtn);
        }
        else if (nowtime - lastRepeat > slow_Repeat_time)
        {
          lastRepeat = nowtime;
          repeatcount++;
          sendPulse(curbtn);
        }
      }
      else
      {
      lastbtn = curbtn;
      repeatcount = 0;
      lastRepeat = nowtime;
      Buzzer_play(sound_keypress);
      sendPulse(curbtn);
      }
    }
    lastTime = nowtime;
  }
  else
  {
    skipMode = false;
  }
}

void sendPulse(ENC_VALUES num)
{
  GPIO_InitSet(encPin[0], MGPIO_MODE_OUT_PP, 0);
	switch(num)
	{
		case ENC_SEL:
			GPIO_SetLevel(TOUCH_BTN_PIN, 0);
			Delay_us(PULSE_DELAY);
			GPIO_SetLevel(TOUCH_BTN_PIN, 1);
			break;

		case ENC_LEFT:
			GPIO_SetLevel(TOUCH_ENCA_PIN, 1);
			GPIO_SetLevel(TOUCH_ENCB_PIN, 1);
			Delay_us(PULSE_DELAY);
			GPIO_SetLevel(TOUCH_ENCA_PIN, 0);
			GPIO_SetLevel(TOUCH_ENCB_PIN, 1);
			Delay_us(PULSE_DELAY);
			GPIO_SetLevel(TOUCH_ENCA_PIN, 0);
			GPIO_SetLevel(TOUCH_ENCB_PIN, 0);
			Delay_us(PULSE_DELAY);
			GPIO_SetLevel(TOUCH_ENCA_PIN, 1);
			GPIO_SetLevel(TOUCH_ENCB_PIN, 0);
			Delay_us(PULSE_DELAY);
			GPIO_SetLevel(TOUCH_ENCA_PIN, 1);
			GPIO_SetLevel(TOUCH_ENCB_PIN, 1);
			break;

		case ENC_RIGHT:
			GPIO_SetLevel(TOUCH_ENCA_PIN, 1);
			GPIO_SetLevel(TOUCH_ENCB_PIN, 1);
			Delay_us(PULSE_DELAY);
			GPIO_SetLevel(TOUCH_ENCA_PIN, 1);
			GPIO_SetLevel(TOUCH_ENCB_PIN, 0);
			Delay_us(PULSE_DELAY);
			GPIO_SetLevel(TOUCH_ENCA_PIN, 0);
			GPIO_SetLevel(TOUCH_ENCB_PIN, 0);
			Delay_us(PULSE_DELAY);
			GPIO_SetLevel(TOUCH_ENCA_PIN, 0);
			GPIO_SetLevel(TOUCH_ENCB_PIN, 1);
			Delay_us(PULSE_DELAY);
			GPIO_SetLevel(TOUCH_ENCA_PIN, 1);
			GPIO_SetLevel(TOUCH_ENCB_PIN, 1);
			break;
    default:
      break;
	}
  GPIO_InitSet(encPin[0], MGPIO_MODE_IPU, 0);
  //GPIO_InitSet(encPin[1], MGPIO_MODE_IPU, 0);
  //GPIO_InitSet(encPin[2], MGPIO_MODE_IPU, 0);
}



#endif




