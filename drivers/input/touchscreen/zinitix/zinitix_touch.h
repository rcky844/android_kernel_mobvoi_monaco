/*
 *
 * Zinitix touch IC driver
 *
 * Copyright (C) 2013 Samsung Electronics Co.Ltd
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 */


#ifndef _LINUX_BT541_TS_H
#define _LINUX_BT541_TS_H

//#include "zinitix_touch_zxt_firmware.h"
#include "zinitix_touch_zxt_firmware_v08030A.h"
/*used for fw compatible*/
#define TSP_TYPE_COUNT	1


/* Upgrade Method*/
#define TOUCH_ONESHOT_UPGRADE      1
#define TOUCH_FORCE_UPGRADE        1
#define USE_CHECKSUM				0

/*Support Gesture Wakeup & Palm Sleep */
#define USE_WAKEUP_GESTURE         1
#define SUPPORTED_PALM_TOUCH       1

/*Support Finger Number*/
#define MAX_SUPPORTED_FINGER_NUM	2

/*Touch Mode Select*/
//#define TOUCH_POINT_FLAG
#ifdef TOUCH_POINT_FLAG
#define TOUCH_POINT_MODE		1
#else
#define TOUCH_POINT_MODE		0
#endif


#define CHIP_OFF_DELAY			70 /*ms*/
#define CHIP_ON_DELAY			200//100 /*ms*/
#define FIRMWARE_ON_DELAY		150//40 /*ms*/

#define DELAY_FOR_SIGNAL_DELAY		30 /*us*/
#define DELAY_FOR_TRANSCATION		50
#define DELAY_FOR_POST_TRANSCATION	10


/* ESD Protection */
/*second : if 0, no use. if you have to use, 3 is recommended*/
#define ESD_TIMER_INTERVAL         0
#define SCAN_RATE_HZ			100
#define CHECK_ESD_TIMER			3

//#ifdef CONFIG_OF
//#define CONFIG_HAS_EARLYSUSPEND
//#endif

#define BT541_TS_DEVICE		"bt541_ts_device"


 /*Test Mode (Monitoring Raw Data) */
#define SEC_DND_N_COUNT			10
#define SEC_DND_U_COUNT			2
#define SEC_DND_FREQUENCY		99 /* 200khz */
#define SEC_PDND_N_COUNT          27
#define SEC_PDND_U_COUNT          3
#define SEC_PDND_FREQUENCY        240

#define SEC_SELF_N_COUNT          15 /* >12 */
#define SEC_SELF_U_COUNT          2  /* 6~12 */
#define SEC_SELF_FREQUENCY        60 /* 79 */
#define ZINITIX_DEBUG               1
#define TSP_VERBOSE_DEBUG

#define SEC_FACTORY_TEST


#define zinitix_debug_msg(fmt, args...) \
	do { \
		if (m_ts_debug_mode) \
			printk(KERN_ERR "bt541_ts[%-18s:%5d] " fmt, \
					__func__, __LINE__, ## args); \
	} while (0);

#define zinitix_printk(fmt, args...) \
	do { \
		printk(KERN_ERR "bt541_ts_device[%-18s:%5d] " fmt, \
				__func__, __LINE__, ## args); \
	} while (0);

#define bt541_err(fmt) \
	do { \
		pr_err("bt541_ts_device : %s " fmt, __func__); \
	} while (0);

struct bt541_ts_platform_data {
	int 	gpio_reset;
	int		gpio_int;

	u32		tsp_irq;

	int		tsp_vendor1;
	int		tsp_vendor2;
	int		tsp_en_gpio;
	u32		tsp_supply_type;
	//int (*tsp_power)(int on);
	u16		x_resolution;
	u16		y_resolution;
	u16		page_size;
	u8		orientation;
	const char      *pname;
#ifdef USE_TSP_TA_CALLBACKS
	void (*register_cb) (struct tsp_callbacks *);
	struct tsp_callbacks callbacks;
#endif

};

 struct class *sec_class;

#endif /* LINUX_BT541_TS_H */

