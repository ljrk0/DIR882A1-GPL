/*
 * BUTTON GPIO Header file
 *
 * Writen by T&W Sudh
 * 
 * $Id: btn_gpio.h 2012-12-22 10:31:40Z $
 */

#ifndef _BUTTON_GPIO_H_
#define _BUTTON_GPIO_H_

#define _BUTTON_DEBUG 1

#ifdef _BUTTON_DEBUG
#define BUTTON_GPIO(fmt, arg...)		printf(fmt, ##arg)
#define BUTTON_ERROR(fmt, arg...) 		printf(fmt, ##arg)
#else
#define BUTTON_GPIO(fmt, arg...)
#define BUTTON_ERROR(fmt, arg...)
#endif

#define BTN_ASSERTLVL					0
#define WL_WIFI_ENABLE_ASSERTLVL		1


typedef enum btn_press {
	BTN_NO_PRESS = 0,
	BTN_PRESS,
} btn_press_t;


typedef int bool;

#define TRUE (1)
#define FALSE (0)
#define BCMGPIO_MAXPINS 64

int btn_init(void);
void btn_clean(void);
unsigned long btn_check(void);
btn_press_t rs_btn_pressed(void);
btn_press_t wl_btn_pressed(void);
#if 0
void wl_btn_dualband_wifi_on(void);
void wl_btn_dualband_wifi_off(void);
#endif

#endif /* _BUTTION_GPIO_H_ */
