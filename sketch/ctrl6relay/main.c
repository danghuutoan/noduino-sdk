/*
 *  Copyright (c) 2015 - 2025 MaiKe Labs
 *
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
*/
#include "user_config.h"
#include "compile.h"

#define	DEBUG	1

static void mjyun_stated_cb(mjyun_state_t state)
{
    if (mjyun_state() != state)
        INFO("Platform: mjyun_state error \r\n");

    switch (state)
    {
        case WIFI_IDLE:
            INFO("Platform: WIFI_IDLE\r\n");
            break;
		case WIFI_SMARTLINK_START:
			INFO("Platform: WIFI_SMARTLINK_START\r\n");
			led_set_effect(0);
			break;
        case WIFI_SMARTLINK_LINKING:
            INFO("Platform: WIFI_SMARTLINK_LINKING\r\n");
			led_set_effect(1);
            break;
        case WIFI_SMARTLINK_FINDING:
            INFO("Platform: WIFI_SMARTLINK_FINDING\r\n");
			led_set_effect(0);
            break;
        case WIFI_SMARTLINK_TIMEOUT:
            INFO("Platform: WIFI_SMARTLINK_TIMEOUT\r\n");
            break;
        case WIFI_SMARTLINK_GETTING:
            INFO("Platform: WIFI_SMARTLINK_GETTING\r\n");
			led_set_effect(1);
            break;
        case WIFI_SMARTLINK_OK:
            INFO("Platform: WIFI_SMARTLINK_OK\r\n");
			led_set_effect(1);
            break;
        case WIFI_AP_OK:
            INFO("Platform: WIFI_AP_OK\r\n");
            break;
        case WIFI_AP_ERROR:
            INFO("Platform: WIFI_AP_ERROR\r\n");
            break;
        case WIFI_AP_STATION_OK:
            INFO("Platform: WIFI_AP_STATION_OK\r\n");
            break;
        case WIFI_AP_STATION_ERROR:
            INFO("Platform: WIFI_AP_STATION_ERROR\r\n");
            break;
        case WIFI_STATION_OK:
            INFO("Platform: WIFI_STATION_OK\r\n");
			led_set_effect(1);
            break;
        case WIFI_STATION_ERROR:
            INFO("Platform: WIFI_STATION_ERROR\r\n");
            break;
        case MJYUN_CONNECTING:
            INFO("Platform: MJYUN_CONNECTING\r\n");
            break;
        case MJYUN_CONNECTING_ERROR:
            INFO("Platform: MJYUN_CONNECTING_ERROR\r\n");
            break;
        case MJYUN_CONNECTED:
            INFO("Platform: MJYUN_CONNECTED \r\n");
            break;
        case MJYUN_DISCONNECTED:
            INFO("Platform: MJYUN_DISCONNECTED\r\n");
            break;
        default:
            break;
	}
}

void mjyun_receive(const char *event_name, const char *event_data)
{
	INFO("RECEIVED: key:value [%s]:[%s]", event_name, event_data);

	if (0 == os_strcmp(event_name, "set")) {
		cJSON * pD = cJSON_Parse(event_data);
		if ((NULL != pD) && (cJSON_Object == pD->type)) {
			cJSON * pR1 = cJSON_GetObjectItem(pD, "r1");
			cJSON * pR2 = cJSON_GetObjectItem(pD, "r2");
			cJSON * pR3 = cJSON_GetObjectItem(pD, "r3");
			cJSON * pR4 = cJSON_GetObjectItem(pD, "r4");
			cJSON * pR5 = cJSON_GetObjectItem(pD, "r5");
			cJSON * pR6 = cJSON_GetObjectItem(pD, "r6");

			relay_status_t mst;
			extern ctrl_status_t ctrl_st;
			mst.r1 = ctrl_st.relay_status.r1;
			mst.r2 = ctrl_st.relay_status.r2;
			mst.r3 = ctrl_st.relay_status.r3;
			mst.r4 = ctrl_st.relay_status.r4;
			mst.r5 = ctrl_st.relay_status.r5;
			mst.r6 = ctrl_st.relay_status.r6;

			if ((NULL != pR1) && (cJSON_Number == pR1->type)) {
				if (mst.r1 != pR1->valueint) {
					mst.r1 = pR1->valueint;
				}
			}
			if ((NULL != pR2) && (cJSON_Number == pR2->type)) {
				if (mst.r2 != pR2->valueint) {
					mst.r2 = pR2->valueint;
				}
			}
			if ((NULL != pR3) && (cJSON_Number == pR3->type)) {
				if (mst.r3 != pR3->valueint) {
					mst.r3 = pR3->valueint;
				}
			}
			if ((NULL != pR4) && (cJSON_Number == pR4->type)) {
				if (mst.r4 != pR4->valueint) {
					mst.r4 = pR4->valueint;
				}
			}
			if ((NULL != pR5) && (cJSON_Number == pR5->type)) {
				if (mst.r5 != pR5->valueint) {
					mst.r5 = pR5->valueint;
				}
			}
			if ((NULL != pR6) && (cJSON_Number == pR6->type)) {
				if (mst.r6 != pR6->valueint) {
					mst.r6 = pR6->valueint;
				}
			}

			app_check_set_push_save(&mst);
		} else {
			INFO("%s: Error when parse JSON\r\n", __func__);
		}

		cJSON_Delete(pD);
	}

	if(os_strncmp(event_data, "ota", 3) == 0)
	{
		INFO("OTA: upgrade the firmware!\r\n");
		mjyun_mini_ota_start("ota/dev/nodec/files");
	}
	if(os_strncmp(event_data, "reset", 5) == 0)
	{
		INFO("Reset the system!\r\n");
		system_restart();
	}
}

void mjyun_connected()
{
	// need to update the status into cloud
	app_push_status(NULL);

	// stop to show the wifi status
	wifi_led_disable();
}

void mjyun_disconnected()
{
	// show the wifi status
	wifi_led_enable();
}

const mjyun_config_t mjyun_conf = {
	"MJP1032524580",		/* MK Ctrl6relay */
	//"MJP1923825656",		/* MK Noduino NodeC */
	HW_VERSION,
	FW_VERSION,
	FW_VERSION,				/* 设备上线时，给app发送 online 消息中的附加数据，[选填] */
	"Device Offline",		/* 设备掉线时，给app发送 offline 消息中的附加数据，[选填] */
#ifdef LOW_POWER
	0,
#else
	WITH_MQTT,
#endif
};

void init_yun()
{
	mjyun_statechanged(mjyun_stated_cb);
	mjyun_ondata(mjyun_receive);
	mjyun_onconnected(mjyun_connected);
	mjyun_ondisconnected(mjyun_disconnected);

	mjyun_run(&mjyun_conf);
	wifi_set_sleep_type(MODEM_SLEEP_T);
}

irom void setup()
{
#ifdef DEBUG
	uart_init(115200, 115200);
#endif
	os_delay_us(100);
	INFO("\r\n\r\n\r\n\r\n\r\n\r\n");
	INFO("\r\nWelcom to Noduino Open Ctrl6relay!\r\n");
	INFO("Current firmware is user%d.bin\r\n", system_upgrade_userbin_check()+1);
	INFO("%s", noduino_banner);

	param_load();
	relay_init();

	led_init();
	xkey_init();

	init_yun();

	wifi_set_sleep_type(MODEM_SLEEP_T);
}

void loop()
{
	delay(10*1000);
}
