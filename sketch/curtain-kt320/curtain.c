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

/*
 * pos = 0 is closed
 * pos = 100 is opened
 *
 */
static os_timer_t check_pos_timer;

void curtain_open()
{
	INFO("curtain open switch\n");
	digitalWrite(D3, HIGH);
	delayMicroseconds(100000);
	digitalWrite(D3, LOW);
}

void curtain_close()
{
	INFO("curtain close switch\n");
	digitalWrite(D4, HIGH);
	delayMicroseconds(100000);
	digitalWrite(D4, LOW);
}

void curtain_stop()
{
	INFO("curtain stop\n");
	switch (param_get_status())
	{
		case 0: 
			//re-enter to stop the close process
			// set the runing state in encoder interrupt!
			curtain_close();
			break;
		case 2:
			// re-enter to stop the open process
			// set the runing state in encoder interrupt!
			curtain_open();
			break;
	}
}

irom void curtain_init()
{
	/* Init the ctrl pin */
	pinMode(D3, OUTPUT);
	pinMode(D4, OUTPUT);

	//digitalWrite(D3, HIGH);
	//digitalWrite(D4, HIGH);
}

void check_encoder_pos(void *parm)
{
	uint32_t target_pos = *((uint32_t *) parm);

	INFO("check encoder pos timer: target_pos = %d\r\n", target_pos);
	INFO("current pos = %d\r\n", param_get_position());

	int epos = encoder_pos();

	if (encoder_direction()) {
		if (epos >= target_pos) {
			os_timer_disarm(&check_pos_timer);
			param_set_position(epos);
			curtain_stop();
		}
	} else {
		if (epos <= target_pos) {
			os_timer_disarm(&check_pos_timer);
			param_set_position(epos);
			curtain_stop();
		}
	}
}

static uint32_t tt_pos; 

void curtain_set_status(int status, int pos)
{
	tt_pos = (uint32_t) pos;

	int delta;

	// only need to process the stop status
	if(status == 1) {
		INFO("set curtain stop\r\n");
		curtain_stop();
		param_set_status(1);
	} else {
		delta = pos - param_get_position();
		if(delta > 0) {
			// open
			curtain_open();
			param_set_status(2);
			INFO("set curtain open\r\n");
			// need to start a timer to check the encoder
			// when meetting the pos, call the curtain_stop() 
			os_timer_disarm(&check_pos_timer);
			os_timer_setfn(&check_pos_timer, (os_timer_func_t *)check_encoder_pos, &tt_pos);
			os_timer_arm(&check_pos_timer, 6 * encoder_circle(), 1);

		} else if (delta < 0) {
			// close
			curtain_close();
			param_set_status(0);
			INFO("set curtain close\r\n");
			// need to start a timer to check the encoder
			os_timer_disarm(&check_pos_timer);
			os_timer_setfn(&check_pos_timer, (os_timer_func_t *)check_encoder_pos, &tt_pos);
			os_timer_arm(&check_pos_timer, 6 * encoder_circle(), 1);
		}
	}

	param_save();
}

void curtain_set_status_and_publish(int status, int pos)
{
	char msg[32];
	os_memset(msg, 0, 32);

	curtain_set_status(status, pos);

	os_sprintf(msg, "{\"status\":%d,\"position\":%d}", param_get_status(), param_get_position());
	mjyun_publishstatus(msg);
}

void curtain_publish_status()
{
	char msg[32];
	os_memset(msg, 0, 32);

	int status = param_get_status();
	int pos = param_get_position();

	os_sprintf(msg, "{\"status\":%d,\"position\":%d}", status, pos);
	mjyun_publishstatus(msg);
}