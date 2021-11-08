/*
 * BUTTON GPIO functions
 *
 * Writen by T&W Sudh
 *
 * $Id: btn_gpio.c 2012-12-22 10:31:40Z $
 */

//#include <typedefs.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <bcmgpio.h>
#include "btn_gpio.h"

#include <sys/select.h>
#include <sys/time.h>
#define CLOCK_REALTIME	0

extern int clock_gettime();

#define RESET_RELAY_TIME				3

#define WL_GPIO_WIFI_BUTTON_VALUE		"wifi_button"
//#define WL_GPIO_2G_WIFI_EN_VALUE		"wifi4331_en"
//#define WL_GPIO_5G_WIFI_EN_VALUE		"wifi4360_en"
#define RESET_GPIO_BUTTON_VALUE			"reset_button"


typedef struct btn_s {
	int gpio;
	int assertlvl;
	struct timespec start_ts;
	unsigned long prev_assertval;
	btn_press_t status;
	char name[16];
} btn_t;

btn_t *btn_chain[BCMGPIO_MAXPINS] = {NULL};
btn_t wl_btn;
btn_t rs_btn;


#if 0
typedef struct wl_wifi_en_s {
	int gpio_2g;
	int gpio_5g;
	int assertlvl_active;
} wl_wifi_en_t;
wl_wifi_en_t wl_wifi_en;

static void
wl_gpio_wifi_on(int gpio_active, int assertlvl_active)
{
	unsigned long led_mask;
	unsigned long value;

	if (gpio_active == BCMGPIO_UNDEFINED)
		return;

	led_mask = ((unsigned long)1 << gpio_active);
	value = ((unsigned long) assertlvl_active << gpio_active);
	bcmgpio_out(led_mask, value);	
}

static void
wl_gpio_wifi_off(int gpio_active, int assertlvl_active)
{
	unsigned long led_mask;
	unsigned long value;

	if (gpio_active == BCMGPIO_UNDEFINED)
		return;

	led_mask = ((unsigned long)1 <<  gpio_active);
	value = ((unsigned long) ~assertlvl_active <<  gpio_active);
	value &= led_mask;	
	bcmgpio_out(led_mask, value);
}

void 
wl_btn_dualband_wifi_on()
{
	wl_gpio_wifi_on(wl_wifi_en.gpio_2g, wl_wifi_en.assertlvl_active);
	wl_gpio_wifi_on(wl_wifi_en.gpio_5g, wl_wifi_en.assertlvl_active);
}

void 
wl_btn_dualband_wifi_off()
{
	wl_gpio_wifi_off(wl_wifi_en.gpio_2g, wl_wifi_en.assertlvl_active);
	wl_gpio_wifi_off(wl_wifi_en.gpio_5g, wl_wifi_en.assertlvl_active);
}

static int
wl_gpio_wifi_control_init()
{
	char *wifi_enable = NULL;
	
	/* init wifi enable key structure */
	memset(&wl_wifi_en, 0, sizeof(wl_wifi_en));
	wl_wifi_en.gpio_2g = BCMGPIO_UNDEFINED;
	wl_wifi_en.gpio_5g = BCMGPIO_UNDEFINED;
	wl_wifi_en.assertlvl_active = WL_WIFI_ENABLE_ASSERTLVL;

	wl_wifi_en.gpio_2g = bcmgpio_getpin(WL_GPIO_2G_WIFI_EN_VALUE, BCMGPIO_UNDEFINED);
	wl_wifi_en.gpio_5g = bcmgpio_getpin(WL_GPIO_5G_WIFI_EN_VALUE, BCMGPIO_UNDEFINED);

	if (wl_wifi_en.gpio_2g == BCMGPIO_UNDEFINED
			&& wl_wifi_en.gpio_5g == BCMGPIO_UNDEFINED) {
		BUTTON_GPIO("No default wifi4331 or wifi4360 gpio configuration\n");
		return -1;
	} 
	else 
	{
		if(bcmgpio_connect(wl_wifi_en.gpio_2g, BCMGPIO_DIRN_OUT) != 0)
		{
			BUTTON_GPIO("Error connecting GPIO %d to %s\n", wl_wifi_en.gpio_2g, WL_GPIO_2G_WIFI_EN_VALUE);
			return -1;
		}
		else if(bcmgpio_connect(wl_wifi_en.gpio_5g, BCMGPIO_DIRN_OUT) != 0)
		{
			bcmgpio_disconnect(wl_wifi_en.gpio_2g);
			BUTTON_GPIO("Error connecting GPIO %d to %s\n", wl_wifi_en.gpio_5g, WL_GPIO_5G_WIFI_EN_VALUE);
			return -1;
		}

		/*TODO, by sdh 2012-12-25, when init the wifi control gpio, should synchronous the gpio status as same as saved status*/ 
		wifi_enable = nvram_get("wifi_hardware_radio_enable");
		if(wifi_enable && !strcmp(wifi_enable, "0"))
		{
			wl_btn_dualband_wifi_off();
		}
		else if(wifi_enable && !strcmp(wifi_enable, "1"))
		{ 
			wl_btn_dualband_wifi_on();
		}
	}
	
	return 0;
}

static void
wl_gpio_wifi_control_cleanup()
{
	if (wl_wifi_en.gpio_2g != BCMGPIO_UNDEFINED)
	{
		bcmgpio_disconnect(wl_wifi_en.gpio_2g);
		wl_wifi_en.gpio_2g = BCMGPIO_UNDEFINED;
	}
	
	if (wl_wifi_en.gpio_5g != BCMGPIO_UNDEFINED)
	{
		bcmgpio_disconnect(wl_wifi_en.gpio_5g);
		wl_wifi_en.gpio_5g = BCMGPIO_UNDEFINED;
	}
}
#endif

static int
btn_gpio_init(btn_t *btn, char *btn_value)
{
#if 0
	/*Reset btn_t structure*/
	memset(btn, 0, sizeof(btn_t));
	btn->gpio = BCMGPIO_UNDEFINED;
	btn->assertlvl = BTN_ASSERTLVL;
	btn->status = BTN_NO_PRESS;	
	btn->prev_assertval = btn->assertlvl ? 0 : 1;
	strcpy(btn->name, btn_value);

	/*Determine the GPIO pins for the Button*/
	btn->gpio = bcmgpio_getpin(btn_value, BCMGPIO_UNDEFINED);

	if (btn->gpio != BCMGPIO_UNDEFINED) {
		BUTTON_GPIO("Using pin %d for wL button input\n", btn->gpio);

		if (bcmgpio_connect(btn->gpio, BCMGPIO_DIRN_IN) != 0) {
			BUTTON_ERROR("Error connecting GPIO %d to wps button\n", btn->gpio);
			return -1;
		}
	}
#endif
	return 0;
}

static void 
btn_gpio_cleanup(btn_t *btn)
{
#if 0
	if (btn->gpio != BCMGPIO_UNDEFINED) {
		bcmgpio_disconnect(btn->gpio);
		btn->gpio = BCMGPIO_UNDEFINED;
	}
#endif	
}

int
btn_init()
{
	if(btn_gpio_init(&rs_btn, RESET_GPIO_BUTTON_VALUE)<0)
	{
		BUTTON_ERROR("reset button initial failed.\n");
		//return -1;
	}

	#if 0
	if(wl_gpio_wifi_control_init()<0)
	{
		BUTTON_ERROR("WIFI enable control work initial failed.\n");
		//return -1;
	}
	else if(btn_gpio_init(&wl_btn, WL_GPIO_WIFI_BUTTON_VALUE)<0)
	
	{
		wl_gpio_wifi_control_cleanup();
		BUTTON_ERROR("WIFI button initial failed.\n");
		//return -1;
	}
	#endif

	if(btn_gpio_init(&wl_btn, WL_GPIO_WIFI_BUTTON_VALUE)<0)
	{
		BUTTON_ERROR("WIFI button initial failed.\n");
	}

	btn_chain[rs_btn.gpio] = &rs_btn;
	btn_chain[wl_btn.gpio] = &wl_btn;
	
	return 0;
}

void
btn_clean()
{
	int i;
	#if 0
	wl_gpio_wifi_control_cleanup();
	#endif
	btn_gpio_cleanup(&wl_btn);
	btn_gpio_cleanup(&rs_btn);
#if 0	
	for(i=0; i <BCMGPIO_MAXPINS; i++)
		btn_chain[i]=NULL;
#endif	
}

static btn_press_t
btn_comfirn(btn_t *btn, int value)
{
	struct timespec cur_ts;
	
	/* PUSH, we detect button push down here */
	if (value == btn->assertlvl) {
		/* Remember push start time */
		if (btn->prev_assertval != value) {
			btn->prev_assertval = value;
			clock_gettime(CLOCK_REALTIME, &(btn->start_ts));
		}

		return BTN_NO_PRESS;
	}
	/* !PUSH */
	else {
		/*Push done, we detect button push up here*/
		if (btn->prev_assertval != value) {
			btn->prev_assertval = value;

			if(strcmp(btn->name, RESET_GPIO_BUTTON_VALUE)==0)
			{
				clock_gettime(CLOCK_REALTIME, &cur_ts);
				if ((cur_ts.tv_sec - btn->start_ts.tv_sec) >= RESET_RELAY_TIME) 
				{
					return BTN_PRESS;
				}
			}
			else
			{
				return BTN_PRESS;
			}
		}

		return BTN_NO_PRESS;
	}
}

static int flag = 0;
unsigned long
btn_check(void)
{
	unsigned long tmpmask, mask;
	unsigned long tmpvalue, value;
	int i;
	bool check = FALSE;
#if 0	
	flag++;
	tmpmask = 0;
	for(i=0; i < BCMGPIO_MAXPINS; i++)
	{
		if(btn_chain[i] && (btn_chain[i]->gpio != BCMGPIO_UNDEFINED))
		{
			btn_chain[i]->status = BTN_NO_PRESS;
			tmpmask |= (unsigned long)1 << btn_chain[i]->gpio;
		}
	}
	
	bcmgpio_in(tmpmask, &tmpvalue);
	for(i=0; i < BCMGPIO_MAXPINS; i++)
	{
		if(btn_chain[i] && (btn_chain[i]->gpio != BCMGPIO_UNDEFINED))
		{
			value = tmpvalue & ((unsigned long)1 << btn_chain[i]->gpio);
			value >>= btn_chain[i]->gpio;
			btn_chain[i]->status = btn_comfirn(btn_chain[i], value);

			if(BTN_PRESS == btn_chain[i]->status)
				check = TRUE;
		}
	}
#endif
	return check;
}

btn_press_t
rs_btn_pressed()
{
	return rs_btn.status;
}

btn_press_t
wl_btn_pressed()
{
	return wl_btn.status;
}

