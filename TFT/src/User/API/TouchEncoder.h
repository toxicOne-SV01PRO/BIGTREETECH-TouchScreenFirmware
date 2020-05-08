#ifndef _TOUCHENCODER_H_
#define _TOUCHENCODER_H_

#include "GUI.h"
#include "stdbool.h"
#include "stdint.h"
#include "ST7920_Simulator.h"

#define SIMULATOR_YEND   (SIMULATOR_YSTART + PIXEL_YSIZE*LCD_YROWS + 6)
#define E_ICON_WIDTH     (ICON_WIDTH - 15)
#define NAV_AREA_HEIGHT  (SPACE_X*2 + E_ICON_WIDTH)

#define  TOUCH_ENCODER_SUPPORT (defined(TOUCH_ENCA_PIN) && defined(TOUCH_ENCB_PIN) && defined(TOUCH_BTN_PIN))
#define PULSE_DELAY 20

#if TOUCH_ENCODER_SUPPORT && LCD_ENCODER_SUPPORT
  #error "TOUCH_ENCODER_SUPPORT cannot be enabled with LCD_ENCODER_SUPPORT"
#endif


#define ENCODER_PULSES_PER_STEP   4
#define LCD_CHANGE_MODE_INTERVALS 1500 //long pressed 1.5s
#define LCD_BUTTON_INTERVALS      20   //20ms
#define ENCODER_STEP_DISTANCE     50

#define EN_A (1<<0)
#define EN_B (1<<1)
#define EN_C (1<<2)
#define B01  1
#define B10  2

typedef enum
{
  ENC_LEFT = 0,
  ENC_SEL,
  ENC_RIGHT,
  ENC_IDLE = 0xFFFF,
}ENC_VALUES;

void touchEncoderInit(void);
void sendPulse(ENC_VALUES num);
void checkTouchButton(void);

#endif