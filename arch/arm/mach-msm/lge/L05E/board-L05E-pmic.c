/* Copyright (c) 2011-2012, The Linux Foundation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <linux/init.h>
#include <linux/ioport.h>
#include <linux/gpio.h>
#include <linux/platform_device.h>
#include <linux/bootmem.h>
#include <linux/mfd/pm8xxx/pm8921.h>
#include <linux/leds.h>
#include <linux/leds-pm8xxx.h>
#include <linux/mfd/pm8xxx/pm8xxx-adc.h>
#include <asm/mach-types.h>
#include <asm/mach/mmc.h>
#include <mach/msm_bus_board.h>
#include <mach/board.h>
#include <mach/gpio.h>
#include <mach/gpiomux.h>
#include <mach/restart.h>
#include <mach/socinfo.h>
#if defined(CONFIG_MACH_LGE)
#include <mach/board_lge.h>
#include <linux/i2c.h>
#include "devices.h"
#include "board-L05E.h"
#else
#include "board-8064.h"
#endif //#if defined(CONFIG_MACH_LGE)
#ifdef CONFIG_BATTERY_MAX17047
#include <linux/max17047_battery.h>
#endif
#ifdef CONFIG_BATTERY_MAX17048
#include <linux/max17048_fuelgauge.h>
#endif
#ifdef CONFIG_WIRELESS_CHARGER
#include <linux/power/bq51051b_charger.h>
#endif

struct pm8xxx_gpio_init {
	unsigned			gpio;
	struct pm_gpio			config;
};

struct pm8xxx_mpp_init {
	unsigned			mpp;
	struct pm8xxx_mpp_config_data	config;
};

#define PM8921_GPIO_INIT(_gpio, _dir, _buf, _val, _pull, _vin, _out_strength, \
			_func, _inv, _disable) \
{ \
	.gpio	= PM8921_GPIO_PM_TO_SYS(_gpio), \
	.config	= { \
		.direction	= _dir, \
		.output_buffer	= _buf, \
		.output_value	= _val, \
		.pull		= _pull, \
		.vin_sel	= _vin, \
		.out_strength	= _out_strength, \
		.function	= _func, \
		.inv_int_pol	= _inv, \
		.disable_pin	= _disable, \
	} \
}

#define PM8921_MPP_INIT(_mpp, _type, _level, _control) \
{ \
	.mpp	= PM8921_MPP_PM_TO_SYS(_mpp), \
	.config	= { \
		.type		= PM8XXX_MPP_TYPE_##_type, \
		.level		= _level, \
		.control	= PM8XXX_MPP_##_control, \
	} \
}

#define PM8821_MPP_INIT(_mpp, _type, _level, _control) \
{ \
	.mpp	= PM8821_MPP_PM_TO_SYS(_mpp), \
	.config	= { \
		.type		= PM8XXX_MPP_TYPE_##_type, \
		.level		= _level, \
		.control	= PM8XXX_MPP_##_control, \
	} \
}

#define PM8921_GPIO_DISABLE(_gpio) \
	PM8921_GPIO_INIT(_gpio, PM_GPIO_DIR_IN, 0, 0, 0, PM_GPIO_VIN_S4, \
			 0, 0, 0, 1)

#define PM8921_GPIO_OUTPUT(_gpio, _val, _strength) \
	PM8921_GPIO_INIT(_gpio, PM_GPIO_DIR_OUT, PM_GPIO_OUT_BUF_CMOS, _val, \
			PM_GPIO_PULL_NO, PM_GPIO_VIN_S4, \
			PM_GPIO_STRENGTH_##_strength, \
			PM_GPIO_FUNC_NORMAL, 0, 0)

#define PM8921_GPIO_OUTPUT_BUFCONF(_gpio, _val, _strength, _bufconf) \
	PM8921_GPIO_INIT(_gpio, PM_GPIO_DIR_OUT,\
			PM_GPIO_OUT_BUF_##_bufconf, _val, \
			PM_GPIO_PULL_NO, PM_GPIO_VIN_S4, \
			PM_GPIO_STRENGTH_##_strength, \
			PM_GPIO_FUNC_NORMAL, 0, 0)

#define PM8921_GPIO_INPUT(_gpio, _pull) \
	PM8921_GPIO_INIT(_gpio, PM_GPIO_DIR_IN, PM_GPIO_OUT_BUF_CMOS, 0, \
			_pull, PM_GPIO_VIN_S4, \
			PM_GPIO_STRENGTH_NO, \
			PM_GPIO_FUNC_NORMAL, 0, 0)

#define PM8921_GPIO_OUTPUT_FUNC(_gpio, _val, _func) \
	PM8921_GPIO_INIT(_gpio, PM_GPIO_DIR_OUT, PM_GPIO_OUT_BUF_CMOS, _val, \
			PM_GPIO_PULL_NO, PM_GPIO_VIN_S4, \
			PM_GPIO_STRENGTH_HIGH, \
			_func, 0, 0)

#define PM8921_GPIO_OUTPUT_VIN(_gpio, _val, _vin) \
	PM8921_GPIO_INIT(_gpio, PM_GPIO_DIR_OUT, PM_GPIO_OUT_BUF_CMOS, _val, \
			PM_GPIO_PULL_NO, _vin, \
			PM_GPIO_STRENGTH_HIGH, \
			PM_GPIO_FUNC_NORMAL, 0, 0)

#ifdef CONFIG_LGE_PM
/* Initial PM8921 GPIO configurations */
static struct pm8xxx_gpio_init pm8921_gpios[] __initdata = {
// LGE_BROADCAST_ONESEG {
// TDMBG, 1-Seg & MMBi different RF S/W problem
#if defined(CONFIG_LGE_BROADCAST_TDMB)
	PM8921_GPIO_OUTPUT(11, 0, HIGH), /* DMB Retractble Ant. Select */
	PM8921_GPIO_OUTPUT(12, 1, HIGH), /* Ear Retractble Ant. Select */
#endif

#if defined(CONFIG_LGE_BROADCAST_ONESEG) && defined(CONFIG_LGE_BROADCAST_ONESEG_MB86A35S) //GJ_DCM
	PM8921_GPIO_OUTPUT(11, 1, HIGH), /* DMB Retractble Ant. Select */
#endif //defined(CONFIG_LGE_BROADCAST_ONESEG) && defined(CONFIG_LGE_BROADCAST_ONESEG_MB86A35S)
// LGE_BROADCAST_ONESEG }

#if defined(CONFIG_LGE_IRDA)
	PM8921_GPIO_OUTPUT(20, 0, HIGH), /* APQ_IRDA_PWDN */
	PM8921_GPIO_OUTPUT(25, 0, HIGH), /* IRDA_SW_EN */
#endif
	PM8921_GPIO_OUTPUT(19, 0, HIGH), /* AMP_EN_AMP */
	PM8921_GPIO_OUTPUT(33, 0, HIGH), /* HAPTIC_EN */
	PM8921_GPIO_OUTPUT(34, 0, HIGH), /* WCD_RESET_N */
	PM8921_GPIO_OUTPUT(28, 0, HIGH), /* VTCAM_RESET */
	PM8921_GPIO_OUTPUT(23, 0, HIGH), /* REAR_CAM_RESET */

	#if defined(CONFIG_MACH_APQ8064_J1D) || defined(CONFIG_MACH_APQ8064_J1KD)
	PM8921_GPIO_OUTPUT(15, 0, HIGH),
	#endif

#if !defined( CONFIG_BCMDHD ) && !defined( CONFIG_BCMDHD_MODULE )
	PM8921_GPIO_OUTPUT_FUNC(44, 0, PM_GPIO_FUNC_2),
#else
	PM8921_GPIO_OUTPUT_FUNC(43, 0, PM_GPIO_FUNC_1),
#endif

#if defined ( CONFIG_BCMDHD ) || defined( CONFIG_BCMDHD_MODULE )
	// WLAN_POWER_ON
	PM8921_GPIO_OUTPUT(29, 0, HIGH),
#endif
//ADD: 0019632: [F200][BT] Bluetooth board bring-up
	PM8921_GPIO_OUTPUT(30, 0, HIGH),
//END: 0019632 chanha.park@lge.com 2012-05-31
};
#else //Qualcomm original
/* Initial PM8921 GPIO configurations */
static struct pm8xxx_gpio_init pm8921_gpios[] __initdata = {
	PM8921_GPIO_OUTPUT(14, 1, HIGH),	/* HDMI Mux Selector */
	PM8921_GPIO_OUTPUT_BUFCONF(25, 0, LOW, CMOS), /* DISP_RESET_N */
	PM8921_GPIO_OUTPUT_FUNC(26, 0, PM_GPIO_FUNC_2), /* Bl: Off, PWM mode */
	PM8921_GPIO_OUTPUT_VIN(30, 1, PM_GPIO_VIN_VPH), /* SMB349 susp line */
	PM8921_GPIO_OUTPUT_BUFCONF(36, 1, LOW, OPEN_DRAIN),
#if !defined( CONFIG_BCMDHD ) && !defined( CONFIG_BCMDHD_MODULE )
	PM8921_GPIO_OUTPUT_FUNC(44, 0, PM_GPIO_FUNC_2),
#else
	PM8921_GPIO_OUTPUT_FUNC(43, 0, PM_GPIO_FUNC_1),
#endif
	PM8921_GPIO_OUTPUT(33, 0, HIGH),
	PM8921_GPIO_OUTPUT(20, 0, HIGH),
	PM8921_GPIO_INPUT(35, PM_GPIO_PULL_UP_30),
	PM8921_GPIO_INPUT(38, PM_GPIO_PULL_UP_30),
	/* TABLA CODEC RESET */
	PM8921_GPIO_OUTPUT(34, 1, HIGH),
	PM8921_GPIO_OUTPUT(13, 0, HIGH),               /* PCIE_CLK_PWR_EN */
	PM8921_GPIO_INPUT(12, PM_GPIO_PULL_UP_30),     /* PCIE_WAKE_N */
#if defined ( CONFIG_BCMDHD ) || defined( CONFIG_BCMDHD_MODULE )
	// WLAN_POWER_ON
	PM8921_GPIO_OUTPUT(29, 0, HIGH),
#endif
};
#endif //#if defined(CONFIG_MACH_LGE)
#ifdef CONFIG_SWITCH_MAX1462X
static struct pm8xxx_gpio_init pm8921_max1462x_gpios[] __initdata = {
	PM8921_GPIO_OUTPUT(31, 0, HIGH), /* PMIC - MAX1462X_EAR_MIC_EN */
	PM8921_GPIO_OUTPUT(32, 0, LOW), /* PMIC - EXT_EAR_MIC_BIAS_EN */
};
#endif

static struct pm8xxx_gpio_init pm8921_mtp_kp_gpios[] __initdata = {
	PM8921_GPIO_INPUT(3, PM_GPIO_PULL_UP_30),
	PM8921_GPIO_INPUT(4, PM_GPIO_PULL_UP_30),
};

static struct pm8xxx_gpio_init pm8921_cdp_kp_gpios[] __initdata = {
	PM8921_GPIO_INPUT(27, PM_GPIO_PULL_UP_30),
	PM8921_GPIO_INPUT(42, PM_GPIO_PULL_UP_30),
	PM8921_GPIO_INPUT(17, PM_GPIO_PULL_UP_1P5),	/* SD_WP */
};

#if defined(CONFIG_MACH_LGE)&& defined (CONFIG_TOUCHSCREEN_S340010_SYNAPTICS_TK)
/* L05E HW Rev.C ~ */
static struct pm8xxx_gpio_init pm8921_touchkey_gpios[] __initdata = {
	PM8921_GPIO_OUTPUT(21, 0, HIGH), /* TOUCH_KEY_LDO_EN */
};
#endif

/* Initial PM8917 GPIO configurations */
static struct pm8xxx_gpio_init pm8917_gpios[] __initdata = {
	PM8921_GPIO_OUTPUT(14, 1, HIGH),	/* HDMI Mux Selector */
	PM8921_GPIO_OUTPUT_BUFCONF(25, 0, LOW, CMOS), /* DISP_RESET_N */
	PM8921_GPIO_OUTPUT(26, 1, HIGH), /* Backlight: on */
	PM8921_GPIO_OUTPUT_BUFCONF(36, 1, LOW, OPEN_DRAIN),
	PM8921_GPIO_OUTPUT_FUNC(38, 0, PM_GPIO_FUNC_2),
	PM8921_GPIO_OUTPUT(33, 0, HIGH),
	PM8921_GPIO_OUTPUT(20, 0, HIGH),
	PM8921_GPIO_INPUT(35, PM_GPIO_PULL_UP_30),
	PM8921_GPIO_INPUT(30, PM_GPIO_PULL_UP_30),
	/* TABLA CODEC RESET */
	PM8921_GPIO_OUTPUT(34, 1, MED),
	PM8921_GPIO_OUTPUT(13, 0, HIGH),               /* PCIE_CLK_PWR_EN */
	PM8921_GPIO_INPUT(12, PM_GPIO_PULL_UP_30),     /* PCIE_WAKE_N */
};

/* PM8921 GPIO 42 remaps to PM8917 GPIO 8 */
static struct pm8xxx_gpio_init pm8917_cdp_kp_gpios[] __initdata = {
	PM8921_GPIO_INPUT(27, PM_GPIO_PULL_UP_30),
	PM8921_GPIO_INPUT(8, PM_GPIO_PULL_UP_30),
	PM8921_GPIO_INPUT(17, PM_GPIO_PULL_UP_1P5),	/* SD_WP */
};

static struct pm8xxx_gpio_init pm8921_mpq_gpios[] __initdata = {
	PM8921_GPIO_INIT(27, PM_GPIO_DIR_IN, PM_GPIO_OUT_BUF_CMOS, 0,
			PM_GPIO_PULL_NO, PM_GPIO_VIN_VPH, PM_GPIO_STRENGTH_NO,
			PM_GPIO_FUNC_NORMAL, 0, 0),
};

/* Initial PM8XXX MPP configurations */
static struct pm8xxx_mpp_init pm8xxx_mpps[] __initdata = {
	PM8921_MPP_INIT(3, D_OUTPUT, PM8921_MPP_DIG_LEVEL_VPH, DOUT_CTRL_LOW),
	/* External 5V regulator enable; shared by HDMI and USB_OTG switches. */
	PM8921_MPP_INIT(7, D_OUTPUT, PM8921_MPP_DIG_LEVEL_VPH, DOUT_CTRL_LOW),
	PM8921_MPP_INIT(8, D_OUTPUT, PM8921_MPP_DIG_LEVEL_S4, DOUT_CTRL_LOW),
#ifndef CONFIG_SWITCH_MAX1462X
	/*MPP9 is used to detect docking station connection/removal on Liquid*/
	PM8921_MPP_INIT(9, D_INPUT, PM8921_MPP_DIG_LEVEL_S4, DIN_TO_INT),
	/* PCIE_RESET_N */
	PM8921_MPP_INIT(1, D_OUTPUT, PM8921_MPP_DIG_LEVEL_VPH, DOUT_CTRL_HIGH),
#endif
};

static struct pm8xxx_gpio_init pm8921_sglte2_gpios[] __initdata = {
	PM8921_GPIO_OUTPUT(23, 1, HIGH),		/* PM2QSC_SOFT_RESET */
	PM8921_GPIO_OUTPUT(21, 1, HIGH),		/* PM2QSC_KEYPADPWR */

};


#ifdef CONFIG_SWITCH_MAX1462X
static struct pm8xxx_mpp_init pm8xxx_max1462x_mpps[] __initdata = {
	/* add by ehgrace.kim@lge.com for headset */
	PM8921_MPP_INIT(9, D_INPUT, PM8921_MPP_DIG_LEVEL_S4, DIN_TO_INT),
	PM8921_MPP_INIT(10, D_INPUT, PM8921_MPP_DIG_LEVEL_S4, DIN_TO_INT),
	/*MPP1 is used to read ADC for headset 3 Button read key*/
	PM8921_MPP_INIT(1, D_INPUT, PM8921_MPP_DIG_LEVEL_S4, DIN_TO_INT),
};
#endif

#ifdef CONFIG_WIRELESS_CHARGER
static struct pm8xxx_gpio_init pm8921_gpios_wlc_rev_c[] __initdata = {
	PM8921_GPIO_INPUT(PM8921_GPIO_WLC_ACTIVE,PM_GPIO_PULL_UP_1P5_30),
	PM8921_GPIO_OUTPUT(PM8921_GPIO_WLC_STATE, 0, HIGH), /* WLC CHG_STAT */
	PM8921_GPIO_DISABLE(PM8921_GPIO_WLC_TS_CTRL),

};
static struct pm8xxx_gpio_init pm8921_gpios_wlc_rev_d[] __initdata = {
	PM8921_GPIO_INPUT(PM8921_GPIO_WLC_ACTIVE,PM_GPIO_PULL_UP_1P5_30),
	PM8921_GPIO_OUTPUT(PM8921_GPIO_WLC_STATE, 0, HIGH), /* WLC CHG_STAT */
	PM8921_GPIO_INPUT(PM8921_GPIO_WLC_TS_CTRL,PM_GPIO_PULL_NO),
};

static int wireless_charger_is_plugged(void);

static struct bq51051b_wlc_platform_data bq51051b_wlc_pmic_pdata = {
	.chg_state_gpio  = GPIO_WLC_STATE,
	.active_n_gpio   = GPIO_WLC_ACTIVE,
	.wc_ts_ctrl_gpio = GPIO_WLC_TS_CTRL,
#ifdef CONFIG_WIRELESS_CHARGER_TEMP_SCENARIO
	.discharging_temp = 500,
	.discharging_clear_temp = 490,
#endif
	.wlc_is_plugged  = wireless_charger_is_plugged,
};

struct platform_device wireless_charger = {
	.name		= "bq51051b_wlc",
	.id		= -1,
	.dev = {
		.platform_data = &bq51051b_wlc_pmic_pdata,
	},
};

static int wireless_charger_is_plugged(void)
{
	static bool initialized = false;
	unsigned int wlc_active_n = 0;
	int ret = 0;

	wlc_active_n = bq51051b_wlc_pmic_pdata.active_n_gpio;
	if (!wlc_active_n) {
		pr_warn("wlc : active_n gpio is not defined yet");
		return 0;
	}

	if (!initialized) {
		ret =  gpio_request_one(wlc_active_n, GPIOF_DIR_IN,
				"active_n_gpio");
		if (ret < 0) {
			pr_err("wlc: active_n gpio request failed\n");
			return 0;
		}
		initialized = true;
	}

	return !(gpio_get_value(wlc_active_n));
}
#endif

// LGE_BROADCAST_ONESEG {
/*[1seg] L05E_DCM Rev.C- Set gpio_38 input and pull-down 10uA register to prevent leakage power [start] */
#if defined(CONFIG_MACH_APQ8064_L05E)
static struct pm8xxx_gpio_init L05E_rev_c_pm8921_gpio_38[] __initdata = {
	/* Unused GPIO pins shoulbe be configured as input with 10ua pull-down */
	/* Refer to QCT 80-N4420-1 Rev.G p.113, 3.10 General-purpose input/output specification */
	PM8921_GPIO_INPUT(38, PM_GPIO_PULL_DN), /* 1SEG_LDO_EN(gpio_38) is not used in L05E rev.C. */
};
#endif
/*[1seg] L05E_DCM Rev.C- Set gpio_38 input and pull-down 10uA register to prevent leakage power [end] */
// LGE_BROADCAST_ONESEG }
void __init apq8064_configure_gpios(struct pm8xxx_gpio_init *data, int len)
{
	int i, rc;

	for (i = 0; i < len; i++) {
		rc = pm8xxx_gpio_config(data[i].gpio, &data[i].config);
		if (rc)
			pr_err("%s: pm8xxx_gpio_config(%u) failed: rc=%d\n",
				__func__, data[i].gpio, rc);
	}
}

void __init apq8064_pm8xxx_gpio_mpp_init(void)
{
	int i, rc;

	if (socinfo_get_pmic_model() != PMIC_MODEL_PM8917)
	{
		apq8064_configure_gpios(pm8921_gpios, ARRAY_SIZE(pm8921_gpios));
#if defined(CONFIG_MACH_LGE)&& defined (CONFIG_TOUCHSCREEN_S340010_SYNAPTICS_TK)
		if (lge_get_board_revno() >= HW_REV_C)
			apq8064_configure_gpios(pm8921_touchkey_gpios, ARRAY_SIZE(pm8921_touchkey_gpios));
#endif
	}
	else
		apq8064_configure_gpios(pm8917_gpios, ARRAY_SIZE(pm8917_gpios));

#ifdef CONFIG_SWITCH_MAX1462X
	printk("[%s] CONFIG_SWITCH_MAX1462X pm8921 init\n", __func__);
	apq8064_configure_gpios(pm8921_max1462x_gpios, ARRAY_SIZE(pm8921_max1462x_gpios));
#endif

// LGE_BROADCAST_ONESEG {
/*[1seg] L05E_DCM Rev.C- Set gpio_38 input and pull-down 10uA register to prevent leakage power [start] */
#if defined(CONFIG_MACH_APQ8064_L05E)
	if( lge_get_board_revno() >= HW_REV_C ) {
		apq8064_configure_gpios(L05E_rev_c_pm8921_gpio_38, ARRAY_SIZE(L05E_rev_c_pm8921_gpio_38));
	}
#endif
/*[1seg] L05E_DCM Rev.C- Set gpio_38 input and pull-down 10uA register to prevent leakage power [end] */
// LGE_BROADCAST_ONESEG }

	if (machine_is_apq8064_cdp() || machine_is_apq8064_liquid()) {
		if (socinfo_get_pmic_model() != PMIC_MODEL_PM8917)
			apq8064_configure_gpios(pm8921_cdp_kp_gpios,
					ARRAY_SIZE(pm8921_cdp_kp_gpios));
		else
			apq8064_configure_gpios(pm8917_cdp_kp_gpios,
					ARRAY_SIZE(pm8917_cdp_kp_gpios));
	}

	if (machine_is_apq8064_mtp()) {
		apq8064_configure_gpios(pm8921_mtp_kp_gpios,
					ARRAY_SIZE(pm8921_mtp_kp_gpios));
		if (socinfo_get_platform_subtype() ==
					PLATFORM_SUBTYPE_SGLTE2) {
			apq8064_configure_gpios(pm8921_sglte2_gpios,
					ARRAY_SIZE(pm8921_sglte2_gpios));
		}
	}

	if (machine_is_mpq8064_cdp() || machine_is_mpq8064_hrd()
	    || machine_is_mpq8064_dtv())
		apq8064_configure_gpios(pm8921_mpq_gpios,
					ARRAY_SIZE(pm8921_mpq_gpios));

	for (i = 0; i < ARRAY_SIZE(pm8xxx_mpps); i++) {
		rc = pm8xxx_mpp_config(pm8xxx_mpps[i].mpp,
					&pm8xxx_mpps[i].config);
		if (rc) {
			pr_err("%s: pm8xxx_mpp_config: rc=%d\n", __func__, rc);
			break;
		}
	}

#ifdef CONFIG_SWITCH_MAX1462X
	for (i = 0; i < ARRAY_SIZE(pm8xxx_max1462x_mpps); i++) {
		rc = pm8xxx_mpp_config(pm8xxx_max1462x_mpps[i].mpp,
					&pm8xxx_max1462x_mpps[i].config);
		if (rc) {
			pr_err("%s: pm8xxx_mpp_max1462x_config: rc=%d\n", __func__, rc);
			break;
		}
	}
#endif

#ifdef CONFIG_WIRELESS_CHARGER
	if (lge_get_board_revno() < HW_REV_C)
		apq8064_configure_gpios(pm8921_gpios_wlc_rev_c, ARRAY_SIZE(pm8921_gpios_wlc_rev_c));
	else
		apq8064_configure_gpios(pm8921_gpios_wlc_rev_d, ARRAY_SIZE(pm8921_gpios_wlc_rev_d));
#endif
}

static struct pm8xxx_pwrkey_platform_data apq8064_pm8921_pwrkey_pdata = {
	.pull_up		= 1,
	.kpd_trigger_delay_us	= 15625,
	.wakeup			= 1,
};

static struct pm8xxx_misc_platform_data apq8064_pm8921_misc_pdata = {
	.priority		= 0,
};

#define PM8921_LC_LED_MAX_CURRENT 4	/* I = 4mA */
#define PM8921_LC_LED_LOW_CURRENT 1	/* I = 1mA */
#define PM8XXX_LED_PWM_ADJUST_BRIGHTNESS_E 10	/* max duty percentage */
#define PM8XXX_LED_PWM_PERIOD     1000
#define PM8XXX_LED_PWM_DUTY_MS    50
#define PM8XXX_LED_PWM_DUTY_PCTS  16
#define PM8XXX_LED_PWM_START_IDX0 16
#define PM8XXX_LED_PWM_START_IDX1 32
#define PM8XXX_LED_PWM_START_IDX2 48

/**
 * PM8XXX_PWM_CHANNEL_NONE shall be used when LED shall not be
 * driven using PWM feature.
 */
#define PM8XXX_PWM_CHANNEL_NONE		-1

static struct led_info pm8921_led_info[] = {
	[0] = {
		.name = "red",
	},
	[1] = {
		.name = "blue",
	},
	[2] = {
		.name = "green",
	},
};

static struct led_platform_data pm8921_led_core_pdata = {
	.num_leds = ARRAY_SIZE(pm8921_led_info),
	.leds = pm8921_led_info,
};

static int pm8921_led0_pwm_duty_pcts[PM8XXX_LED_PWM_DUTY_PCTS] = {0,};
static int pm8921_led1_pwm_duty_pcts[PM8XXX_LED_PWM_DUTY_PCTS] = {0,};
static int pm8921_led2_pwm_duty_pcts[PM8XXX_LED_PWM_DUTY_PCTS] = {0,};

/*
 * Note: There is a bug in LPG module that results in incorrect
 * behavior of pattern when LUT index 0 is used. So effectively
 * there are 63 usable LUT entries.
 */
static struct pm8xxx_pwm_duty_cycles pm8921_led0_pwm_duty_cycles = {
	.duty_pcts = (int *)&pm8921_led0_pwm_duty_pcts,
	.num_duty_pcts = PM8XXX_LED_PWM_DUTY_PCTS,
	.duty_ms = PM8XXX_LED_PWM_DUTY_MS,
	.start_idx = PM8XXX_LED_PWM_START_IDX0,
};

static struct pm8xxx_pwm_duty_cycles pm8921_led1_pwm_duty_cycles = {
	.duty_pcts = (int *)&pm8921_led1_pwm_duty_pcts,
	.num_duty_pcts = PM8XXX_LED_PWM_DUTY_PCTS,
	.duty_ms = PM8XXX_LED_PWM_DUTY_MS,
	.start_idx = PM8XXX_LED_PWM_START_IDX1,
};

static struct pm8xxx_pwm_duty_cycles pm8921_led2_pwm_duty_cycles = {
	.duty_pcts = (int *)&pm8921_led2_pwm_duty_pcts,
	.num_duty_pcts = PM8XXX_LED_PWM_DUTY_PCTS,
	.duty_ms = PM8XXX_LED_PWM_DUTY_MS,
	.start_idx = PM8XXX_LED_PWM_START_IDX2,
};

static struct pm8xxx_led_config pm8921_led_configs[] = {
	[0] = {
		.id = PM8XXX_ID_LED_0,
		.mode = PM8XXX_LED_MODE_PWM3,
		.max_current = PM8921_LC_LED_MAX_CURRENT,
		.pwm_channel = 6,
		.pwm_period_us = PM8XXX_LED_PWM_PERIOD,
		.pwm_duty_cycles = &pm8921_led0_pwm_duty_cycles,
		.pwm_adjust_brightness = 52,
	},
	[1] = {
		.id = PM8XXX_ID_LED_1,
		.mode = PM8XXX_LED_MODE_PWM2,
		.max_current = PM8921_LC_LED_MAX_CURRENT,
		.pwm_channel = 5,
		.pwm_period_us = PM8XXX_LED_PWM_PERIOD,
		.pwm_duty_cycles = &pm8921_led1_pwm_duty_cycles,
		.pwm_adjust_brightness = 43,
	},
	[2] = {
		.id = PM8XXX_ID_LED_2,
		.mode = PM8XXX_LED_MODE_PWM1,
		.max_current = PM8921_LC_LED_MAX_CURRENT,
		.pwm_channel = 4,
		.pwm_period_us = PM8XXX_LED_PWM_PERIOD,
		.pwm_duty_cycles = &pm8921_led2_pwm_duty_cycles,
		.pwm_adjust_brightness = 75,
	},
};

static struct pm8xxx_led_platform_data apq8064_pm8921_leds_pdata = {
		.led_core = &pm8921_led_core_pdata,
		.configs = pm8921_led_configs,
		.num_configs = ARRAY_SIZE(pm8921_led_configs),
		.use_pwm = 1,
};

static struct pm8xxx_adc_amux apq8064_pm8921_adc_channels_data[] = {
	{"vcoin", CHANNEL_VCOIN, CHAN_PATH_SCALING2, AMUX_RSV1,
		ADC_DECIMATION_TYPE2, ADC_SCALE_DEFAULT},
	{"vbat", CHANNEL_VBAT, CHAN_PATH_SCALING2, AMUX_RSV1,
		ADC_DECIMATION_TYPE2, ADC_SCALE_DEFAULT},
	{"dcin", CHANNEL_DCIN, CHAN_PATH_SCALING4, AMUX_RSV1,
		ADC_DECIMATION_TYPE2, ADC_SCALE_DEFAULT},
	{"ichg", CHANNEL_ICHG, CHAN_PATH_SCALING1, AMUX_RSV1,
		ADC_DECIMATION_TYPE2, ADC_SCALE_DEFAULT},
	{"vph_pwr", CHANNEL_VPH_PWR, CHAN_PATH_SCALING2, AMUX_RSV1,
		ADC_DECIMATION_TYPE2, ADC_SCALE_DEFAULT},
	{"ibat", CHANNEL_IBAT, CHAN_PATH_SCALING1, AMUX_RSV1,
		ADC_DECIMATION_TYPE2, ADC_SCALE_DEFAULT},
	{"batt_therm", CHANNEL_BATT_THERM, CHAN_PATH_SCALING1, AMUX_RSV2,
		ADC_DECIMATION_TYPE2, ADC_SCALE_BATT_THERM},
	{"batt_id", CHANNEL_BATT_ID, CHAN_PATH_SCALING1, AMUX_RSV1,
		ADC_DECIMATION_TYPE2, ADC_SCALE_DEFAULT},
	{"usbin", CHANNEL_USBIN, CHAN_PATH_SCALING3, AMUX_RSV1,
		ADC_DECIMATION_TYPE2, ADC_SCALE_DEFAULT},
	{"pmic_therm", CHANNEL_DIE_TEMP, CHAN_PATH_SCALING1, AMUX_RSV1,
		ADC_DECIMATION_TYPE2, ADC_SCALE_PMIC_THERM},
	{"625mv", CHANNEL_625MV, CHAN_PATH_SCALING1, AMUX_RSV1,
		ADC_DECIMATION_TYPE2, ADC_SCALE_DEFAULT},
	{"125v", CHANNEL_125V, CHAN_PATH_SCALING1, AMUX_RSV1,
		ADC_DECIMATION_TYPE2, ADC_SCALE_DEFAULT},
	{"chg_temp", CHANNEL_CHG_TEMP, CHAN_PATH_SCALING1, AMUX_RSV1,
		ADC_DECIMATION_TYPE2, ADC_SCALE_DEFAULT},
	{"xo_therm", CHANNEL_MUXOFF, CHAN_PATH_SCALING1, AMUX_RSV0,
		ADC_DECIMATION_TYPE2, ADC_SCALE_XOTHERM},
//LGE_PM_L05E, samin,ryu, need check
/* 2012-06-05 cs.kim@lge.com implement Thermal Profile log. */
	{"pa_therm0", ADC_MPP_1_AMUX3, CHAN_PATH_SCALING1, AMUX_RSV1,
		ADC_DECIMATION_TYPE2, ADC_SCALE_APQ_THERM},
	{"usb_id_device", ADC_MPP_1_AMUX6, CHAN_PATH_SCALING1, AMUX_RSV1,
		ADC_DECIMATION_TYPE2, ADC_SCALE_DEFAULT},
};

static struct pm8xxx_adc_properties apq8064_pm8921_adc_data = {
	.adc_vdd_reference	= 1800, /* milli-voltage for this adc */
	.bitresolution		= 15,
	.bipolar                = 0,
};

static struct pm8xxx_adc_platform_data apq8064_pm8921_adc_pdata = {
	.adc_channel		= apq8064_pm8921_adc_channels_data,
	.adc_num_board_channel	= ARRAY_SIZE(apq8064_pm8921_adc_channels_data),
	.adc_prop		= &apq8064_pm8921_adc_data,
	.adc_mpp_base		= PM8921_MPP_PM_TO_SYS(1),
};

static struct pm8xxx_mpp_platform_data
apq8064_pm8921_mpp_pdata __devinitdata = {
	.mpp_base	= PM8921_MPP_PM_TO_SYS(1),
};

static struct pm8xxx_gpio_platform_data
apq8064_pm8921_gpio_pdata __devinitdata = {
	.gpio_base	= PM8921_GPIO_PM_TO_SYS(1),
};

static struct pm8xxx_irq_platform_data
apq8064_pm8921_irq_pdata __devinitdata = {
	.irq_base		= PM8921_IRQ_BASE,
	.devirq			= MSM_GPIO_TO_INT(74),
	.irq_trigger_flag	= IRQF_TRIGGER_LOW,
	.dev_id			= 0,
};

static struct pm8xxx_rtc_platform_data
apq8064_pm8921_rtc_pdata = {
	.rtc_write_enable       = false,
	.rtc_alarm_powerup      = false,
};

static int apq8064_pm8921_therm_mitigation[] = {
	1100,
	700,
	600,
	325,
};

#ifdef CONFIG_LGE_PM
/*
 * J1 battery characteristic
 * Typ.1900mAh capacity, Li-Ion Polymer 3.8V
 * Battery/VDD voltage programmable range, 20mV steps.
 * it will be changed in future
 */
#define MAX_VOLTAGE_MV		4360
#define CHG_TERM_MA		100
#ifdef CONFIG_LGE_PM
	/* MAKO patch */
#define EOC_CHECK_SOC	1
#endif

static struct pm8921_charger_platform_data apq8064_pm8921_chg_pdata __devinitdata = {

	/* max charging time in minutes incl. fast and trkl. it will be changed in future  */
	.update_time		= 60000,
	.max_voltage		= MAX_VOLTAGE_MV,

	/* the voltage (mV) where charging method switches from trickle to fast.
	 * This is also the minimum voltage the system operates at */
	.min_voltage		= 3200,
	.alarm_low_mv		= 3400,
	.alarm_high_mv		= 4000,
	.resume_voltage_delta	= 60,
	.resume_charge_percent	= 99,
	.term_current		= CHG_TERM_MA,

	/* the voltage of charger_gone_irq */
	.vin_min			= 4400,
#ifdef CONFIG_LGE_CHARGER_TEMP_SCENARIO
	/* Configuration of cool and warm thresholds (JEITA compliance only) */
/* CONFIG_LGE_PM Start Qulcomm charging scenario off kwangjae1.lee@lge.com */
	.cool_temp		= INT_MIN, /* 10 */	/* 10 degree celsius */
	.warm_temp		= INT_MIN, /* 40 */	/* 40 degree celsius */
	.cool_bat_chg_current	= 350,	/* 350 mA (max value = 2A) */
	.warm_bat_chg_current	= 350,
	.temp_level_1		= 550,
	.temp_level_2		= 450,
	.temp_level_3		= 420,
	.temp_level_4		= -50,
	.temp_level_5		= -100,
	/* Temperature Thresholds (JEITA compliance) (-10'C ~ 60'C) */
	.cold_thr		= 1,	/* 80% */
	.hot_thr		= 0,	/* 20% */
#else /* qualcomm original value */
	.cool_temp		= 10,
	.warm_temp		= 40,
	.cool_bat_chg_current	= 350,
	.warm_bat_chg_current	= 350,
#endif

	.temp_check_period	= 1,

#ifdef CONFIG_LGE_PM_LOW_BATT_CHG
	.weak_voltage = 3000,
	.trkl_voltage = 2800,	// trickle voltage 2800mV
	.trkl_current = 200,	// trickle current 200mA
#endif

	/*  Battery charge current programmable range 50mA steps */
	/*  max_bat_chg_current:
	 *    Max charge current of the battery in mA
	 *    Usually 70% of full charge capacity
	 */
#if defined(CONFIG_MACH_APQ8064_J1D)||defined(CONFIG_MACH_APQ8064_J1KD)
	.max_bat_chg_current	= 1500,
#elif defined(CONFIG_MACH_APQ8064_GKSK) || defined(CONFIG_MACH_APQ8064_GKKT) || defined(CONFIG_MACH_APQ8064_GKU) || defined(CONFIG_MACH_APQ8064_GKATT)
	.max_bat_chg_current	= 1800,
#else
	.max_bat_chg_current	= 1350,
#endif

	.cool_bat_voltage	= 4100,
	.warm_bat_voltage	= 4100,
	.thermal_mitigation	= apq8064_pm8921_therm_mitigation,
	.thermal_levels		= ARRAY_SIZE(apq8064_pm8921_therm_mitigation),
	/* for led on, off control */
	.led_src_config = LED_SRC_5V,
	/*LGE_CHANGE_E, jungwoo.yun@lge.com for led on, off control*/
/* Be omitted OCT code */
	//.rconn_mohm    = 18,

};
#else /* qualcomm original code */
#define MAX_VOLTAGE_MV          4200
#define CHG_TERM_MA		100
static struct pm8921_charger_platform_data
apq8064_pm8921_chg_pdata __devinitdata = {
	.update_time		= 60000,
	.max_voltage		= MAX_VOLTAGE_MV,
	.min_voltage		= 3200,
	.uvd_thresh_voltage	= 4050,
	.alarm_low_mv		= 3400,
	.alarm_high_mv		= 4000,
	.resume_voltage_delta	= 60,
	.resume_charge_percent	= 99,
	.term_current		= CHG_TERM_MA,
	.cool_temp		= 10,
	.warm_temp		= 45,
	.temp_check_period	= 1,
	.max_bat_chg_current	= 1100,
	.cool_bat_chg_current	= 350,
	.warm_bat_chg_current	= 350,
	.cool_bat_voltage	= 4100,
	.warm_bat_voltage	= 4100,
	.thermal_mitigation	= apq8064_pm8921_therm_mitigation,
	.thermal_levels		= ARRAY_SIZE(apq8064_pm8921_therm_mitigation),
};
#endif

static struct pm8xxx_ccadc_platform_data
apq8064_pm8xxx_ccadc_pdata = {
	.r_sense_uohm		= 10000,
	.calib_delay_ms		= 600000,
};

static struct pm8921_bms_platform_data
apq8064_pm8921_bms_pdata __devinitdata = {
	.battery_type			= BATT_2200_LGE,
	.r_sense_uohm			= 10000,
	.v_cutoff			= 3500,
	.max_voltage_uv			= MAX_VOLTAGE_MV * 1000,
	.rconn_mohm			= 18,
	.shutdown_soc_valid_limit	= 20,
	.adjust_soc_low_threshold	= 25,
	.chg_term_ua			= CHG_TERM_MA * 1000,
	.normal_voltage_calc_ms		= 20000,
	.low_voltage_calc_ms		= 1000,
	.alarm_low_mv			= 3400,
	.alarm_high_mv			= 4000,
	.high_ocv_correction_limit_uv	= 50,
	.low_ocv_correction_limit_uv	= 100,
	.hold_soc_est			= 3,
#ifdef CONFIG_WIRELESS_CHARGER
	.bms_support_wlc		= 1,
	.wlc_is_plugged			= wireless_charger_is_plugged,
#endif
};

static unsigned int keymap[] = {
	KEY(0, 0, KEY_VOLUMEUP),
	KEY(0, 1, KEY_VOLUMEDOWN),
	//KEY(0, 2, KEY_CAMERA_SNAPSHOT),
	//KEY(0, 3, KEY_CAMERA_FOCUS),
#if defined(CONFIG_MACH_APQ8064_GKKT)||defined(CONFIG_MACH_APQ8064_GKSK)||defined(CONFIG_MACH_APQ8064_GKU)||defined(CONFIG_MACH_APQ8064_GKATT)
	KEY(1, 1, KEY_HOMEPAGE),
	KEY(1, 0, KEY_BACK), //KEY_QUICK_CLIP - > 2012-07-16 temp code for touch fw
#endif
};

static struct matrix_keymap_data keymap_data = {
	.keymap_size    = ARRAY_SIZE(keymap),
	.keymap         = keymap,
};

static struct pm8xxx_keypad_platform_data keypad_data = {
	.input_name             = "L05E-keypad-8064",
	.input_phys_device      = "L05E-keypad-8064/input0",
#if defined(CONFIG_MACH_APQ8064_GKKT)||defined(CONFIG_MACH_APQ8064_GKSK)||defined(CONFIG_MACH_APQ8064_GKU)||defined(CONFIG_MACH_APQ8064_GKATT)
	.num_rows				= 2,
	.num_cols				= 5,
#else
	.num_rows               = 1,
	.num_cols               = 5,
#endif
	.rows_gpio_start	= PM8921_GPIO_PM_TO_SYS(9),
	.cols_gpio_start	= PM8921_GPIO_PM_TO_SYS(1),
	.debounce_ms            = 15,
	.scan_delay_ms          = 32,
	.row_hold_ns            = 91500,
	.wakeup                 = 1,
	.keymap_data            = &keymap_data,
};

static struct pm8921_platform_data
apq8064_pm8921_platform_data __devinitdata = {
	.irq_pdata		= &apq8064_pm8921_irq_pdata,
	.gpio_pdata		= &apq8064_pm8921_gpio_pdata,
	.mpp_pdata		= &apq8064_pm8921_mpp_pdata,
	.rtc_pdata		= &apq8064_pm8921_rtc_pdata,
	.pwrkey_pdata		= &apq8064_pm8921_pwrkey_pdata,
	.keypad_pdata		= &keypad_data,
	.misc_pdata		= &apq8064_pm8921_misc_pdata,
	.leds_pdata		= &apq8064_pm8921_leds_pdata,
	.adc_pdata		= &apq8064_pm8921_adc_pdata,
	.charger_pdata		= &apq8064_pm8921_chg_pdata,
	.bms_pdata		= &apq8064_pm8921_bms_pdata,
	.ccadc_pdata		= &apq8064_pm8xxx_ccadc_pdata,
};

static struct pm8xxx_irq_platform_data
apq8064_pm8821_irq_pdata __devinitdata = {
	.irq_base		= PM8821_IRQ_BASE,
	.devirq			= PM8821_SEC_IRQ_N,
	.irq_trigger_flag	= IRQF_TRIGGER_HIGH,
	.dev_id			= 1,
};

static struct pm8xxx_mpp_platform_data
apq8064_pm8821_mpp_pdata __devinitdata = {
	.mpp_base	= PM8821_MPP_PM_TO_SYS(1),
};

static struct pm8821_platform_data
apq8064_pm8821_platform_data __devinitdata = {
	.irq_pdata	= &apq8064_pm8821_irq_pdata,
	.mpp_pdata	= &apq8064_pm8821_mpp_pdata,
};

static struct msm_ssbi_platform_data apq8064_ssbi_pm8921_pdata __devinitdata = {
	.controller_type = MSM_SBI_CTRL_PMIC_ARBITER,
	.slave	= {
		.name		= "pm8921-core",
		.platform_data	= &apq8064_pm8921_platform_data,
	},
};

static struct msm_ssbi_platform_data apq8064_ssbi_pm8821_pdata __devinitdata = {
	.controller_type = MSM_SBI_CTRL_PMIC_ARBITER,
	.slave	= {
		.name		= "pm8821-core",
		.platform_data	= &apq8064_pm8821_platform_data,
	},
};

#ifdef CONFIG_BATTERY_MAX17048
#define FUEL_GAUGE_INT_N        36

#define I2C_SURF 1
#define I2C_FFA  (1 << 1)
#define I2C_RUMI (1 << 2)
#define I2C_SIM  (1 << 3)
#define I2C_LIQUID (1 << 4)

struct i2c_registry {
	u8                     machs;
	int                    bus;
	struct i2c_board_info *info;
	int                    len;
};

/* BEGIN: hiro.kwon@lge.com 2011-12-22 RCOMP update when the temperature of the cell changes */

// From GK
struct max17048_platform_data L05E_max17048_pdata = {
	.starting_rcomp = 0x5A,
	.temp_co_hot = -775,
	.temp_co_cold = -710,
	.soc_cal_data = NULL,
};
/*
struct max17048_platform_data L05E_max17048_pdata = {
	.starting_rcomp = 0x0,
	.temp_co_hot = 0,
	.temp_co_cold = 0,
	.soc_cal_data = NULL,
};
*/
/* END: hiro.kwon@lge.com 2011-12-22 */

static struct i2c_board_info max17048_i2c_info[] = {
	{
		I2C_BOARD_INFO("max17048", MAX17048_FUELGAUGE_I2C_ADDR),
		.irq = FUEL_GAUGE_INT_N,
		.platform_data = (void *)&L05E_max17048_pdata,
	}
};

static struct i2c_registry L05E_i2c_pm_subsystem __initdata = {
		I2C_SURF | I2C_FFA | I2C_LIQUID | I2C_RUMI,
		APQ_8064_GSBI1_QUP_I2C_BUS_ID,
		max17048_i2c_info,
		ARRAY_SIZE(max17048_i2c_info),
};

void __init lge_add_i2c_pm_subsystem_devices(void)
{
	/* LGE_CHANGE
	 * 2011-12-03, hyuncheol0.kim@lge.com
	 * Work-around code to support old H/W revision.
	 */
	/* Run the array and install devices as appropriate */
	i2c_register_board_info(L05E_i2c_pm_subsystem.bus,
				L05E_i2c_pm_subsystem.info,
				L05E_i2c_pm_subsystem.len);
}

#endif // CONFIG_BATTERY_MAX17048

void __init apq8064_init_pmic(void)
{
	pmic_reset_irq = PM8921_IRQ_BASE + PM8921_RESOUT_IRQ;

	apq8064_device_ssbi_pmic1.dev.platform_data =
						&apq8064_ssbi_pm8921_pdata;
	apq8064_device_ssbi_pmic2.dev.platform_data =
				&apq8064_ssbi_pm8821_pdata;
	if (socinfo_get_pmic_model() != PMIC_MODEL_PM8917) {
		apq8064_pm8921_platform_data.regulator_pdatas
			= msm8064_pm8921_regulator_pdata;
		apq8064_pm8921_platform_data.num_regulators
			= msm8064_pm8921_regulator_pdata_len;
	} else {
		apq8064_pm8921_platform_data.regulator_pdatas
			= msm8064_pm8917_regulator_pdata;
		apq8064_pm8921_platform_data.num_regulators
			= msm8064_pm8917_regulator_pdata_len;
	}

#ifndef CONFIG_MACH_LGE
/* LGE_S jungshik.park@lge.com 2012-04-18 for lge battery type */
	if (machine_is_apq8064_mtp()) {
		apq8064_pm8921_bms_pdata.battery_type = BATT_PALLADIUM;
	} else if (machine_is_apq8064_liquid()) {
		apq8064_pm8921_bms_pdata.battery_type = BATT_DESAY;
	} else if (machine_is_apq8064_cdp()) {
		apq8064_pm8921_chg_pdata.has_dc_supply = true;
	}

	if (!machine_is_apq8064_mtp() && !machine_is_apq8064_liquid())
		apq8064_pm8921_chg_pdata.battery_less_hardware = 1;
/* LGE_E jungshik.park@lge.com 2012-04-18 for lge battery type */
#endif

	if (machine_is_mpq8064_hrd())
		apq8064_pm8921_chg_pdata.disable_chg_rmvl_wrkarnd = 1;

	apq8064_pm8921_adc_pdata.apq_therm = true;
}
