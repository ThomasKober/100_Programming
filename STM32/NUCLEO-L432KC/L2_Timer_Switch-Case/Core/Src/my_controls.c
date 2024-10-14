/*
 * my_controls.c
 *
 *  Created on: Oct 12, 2024
 *      Author: Thomas Kober
 */

#include "main.h"
#include "tim.h"
#include "stdbool.h"

#include "my_controls.h"
#include <my_signalshapes.h>



/*
 * ----------------------------------------------------------------------
 * initialization of counter variable
 * ----------------------------------------------------------------------
 */
uint16_t cnt = 0;

/*
 * ----------------------------------------------------------------------
 * counter function for strobe light
 * ----------------------------------------------------------------------
 */
void counter(void)
{
	cnt++;
	if(cnt >= 100)
	{
		cnt = 0;
	}
}


/*
 * ----------------------------------------------------------------------
 * initialization of status struct
 * ----------------------------------------------------------------------
*/
static LtStatus status;




/*
 * ----------------------------------------------------------------------
 * struct init function
 * ----------------------------------------------------------------------
 */
void init_status(bool stat_bea, bool stat_pos, bool stat_ac, bool stat_lan, bool stat_sea)
{
	status.status_lt_bea = stat_bea;
	status.status_lt_pos = stat_pos;
	status.status_lt_ac = stat_ac;
	status.status_lt_lan = stat_lan;
	status.status_lt_sea = stat_sea;
}


/*
 * ----------------------------------------------------------------------
 * get-status functions
 * ----------------------------------------------------------------------
 */
//void get_status(LtStatus* s)
//{
//	if (s != NULL)
//	{
//		s->status_lt_bea = status.status_lt_bea;
//		s->status_lt_pos = status.status_lt_pos;
//		s->status_lt_ac = status.status_lt_ac;
//		s->status_lt_lan = status.status_lt_lan;
//		s->status_lt_sea = status.status_lt_sea;
//	}
//}


//returns status of beacon light
bool get_status_lt_bea(void)
{
	return status.status_lt_bea;
}


//returns status of position light
bool get_status_lt_pos(void)
{
	return status.status_lt_pos;
}


//returns status of anti collision light
bool get_status_lt_ac(void)
{
	return status.status_lt_ac;
}


//returns status of landing light
bool get_status_lt_lan(void)
{
	return status.status_lt_lan;
}


//returns status of search light
bool get_status_lt_sea(void)
{
	return status.status_lt_sea;
}


/*
 * ----------------------------------------------------------------------
 * set-status functions
 * ----------------------------------------------------------------------
 */
//sets status of beacon light
void set_status_lt_bea(bool stat)
{
	status.status_lt_bea = stat;
}


//sets status of position light
void set_status_lt_pos(bool stat)
{
	status.status_lt_pos = stat;
}


//sets status of anti collision light
void set_status_lt_ac(bool stat)
{
	status.status_lt_ac = stat;
}


//sets status of landing light
void set_status_lt_lan(bool stat)
{
	status.status_lt_lan = stat;
}


//sets status of search light
void set_status_lt_sea(bool stat)
{
	status.status_lt_sea = stat;
}


/*
 * ----------------------------------------------------------------------
 * light functions
 * ----------------------------------------------------------------------
 */
//beacon light
void lt_beacon(void)
{
	if (get_status_lt_bea())
	{
		HAL_TIM_PWM_Start_DMA(&htim2, TIM_CHANNEL_1, data, 1024);
	}
	else
	{
		HAL_TIM_PWM_Stop_DMA(&htim2, TIM_CHANNEL_1);
	}
}


//position light
void lt_position(void)
{
	if (get_status_lt_pos())
	{
		HAL_GPIO_WritePin(lt_pos_GPIO_Port, lt_pos_Pin, GPIO_PIN_SET);
	}
	else
	{
		HAL_GPIO_WritePin(lt_pos_GPIO_Port, lt_pos_Pin, GPIO_PIN_RESET);
	}
}


//anti collision light
void lt_anticollition(void)
{
	if (get_status_lt_ac())
	{
		//ON | OFF | ON | OFF | ON | OFF (50 ms ON 100ms OFF)
		//if( ((cnt>=0)&&(cnt<=1)) || ((cnt>=3)&&(cnt<=4)) || ((cnt>=6)&&(cnt<=7)) || ((cnt>=50)&&(cnt<=51)) || ((cnt>=53)&&(cnt<=54)) || ((cnt>=56)&&(cnt<=57)))
		if( (cnt==0) || (cnt==3) || (cnt==6) || (cnt==50) || (cnt==53) || (cnt==56) )
		{
			HAL_GPIO_WritePin(lt_ac_GPIO_Port, lt_ac_Pin, GPIO_PIN_SET);
		}
		else
		{
			HAL_GPIO_WritePin(lt_ac_GPIO_Port, lt_ac_Pin, GPIO_PIN_RESET);
		}
	}
	else
	{
		HAL_GPIO_WritePin(lt_ac_GPIO_Port, lt_ac_Pin, GPIO_PIN_RESET);
	}
}


//landing light
void lt_landing(void)
{
	if (get_status_lt_lan())
	{
		HAL_GPIO_WritePin(lt_lan_GPIO_Port, lt_lan_Pin, GPIO_PIN_SET);
	}
	else
	{
		HAL_GPIO_WritePin(lt_lan_GPIO_Port, lt_lan_Pin, GPIO_PIN_RESET);
	}
}


//search light
void lt_search(void)
{
	if (get_status_lt_sea())
	{
		HAL_GPIO_WritePin(lt_sea_GPIO_Port, lt_sea_Pin, GPIO_PIN_SET);
	}
	else
	{
		HAL_GPIO_WritePin(lt_sea_GPIO_Port, lt_sea_Pin, GPIO_PIN_RESET);
	}
}
