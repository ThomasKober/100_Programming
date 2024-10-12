/*
 * my_controls.c
 *
 *  Created on: Oct 12, 2024
 *      Author: Thomas Kober
 */

#include "my_controls.h"


void init_status(Status *s, bool stat_bea, bool stat_pos, bool stat_ac, bool stat_lan, bool stat_sea)
{
	s->status_lt_bea = stat_bea;
	s->status_lt_pos = stat_pos;
	s->status_lt_ac = stat_ac;
	s->status_lt_lan = stat_lan;
	s->status_lt_bea = stat_bea;
}


void get_status(const Status *s)
{
	//Send status over UART
}


void lt_beacon(Status *s)
{

}


void lt_position(Status *s)
{
//	HAL_GPIO_WritePin(lt_pos_GPIO_Port, lt_pos_Pin, GPIO_PIN_SET);
}


void lt_anticollition(Status *s)
{
//	//ON | OFF | ON | OFF | ON | OFF (50 ms ON 100ms OFF)
//	//if( ((cnt>=0)&&(cnt<=1)) || ((cnt>=3)&&(cnt<=4)) || ((cnt>=6)&&(cnt<=7)) || ((cnt>=50)&&(cnt<=51)) || ((cnt>=53)&&(cnt<=54)) || ((cnt>=56)&&(cnt<=57)))
//	if( (cnt==0) || (cnt==3) || (cnt==6) || (cnt==50) || (cnt==53) || (cnt==56) )
//	{
//		HAL_GPIO_WritePin(lt_ac_GPIO_Port, lt_ac_Pin, GPIO_PIN_SET);
//	}
//	else
//	{
//		HAL_GPIO_WritePin(lt_ac_GPIO_Port, lt_ac_Pin, GPIO_PIN_RESET);
//	}
}


void lt_landing(Status *s)
{
//	HAL_GPIO_WritePin(lt_lan_GPIO_Port, lt_lan_Pin, GPIO_PIN_SET);
}


void lt_search(Status *s)
{
//	HAL_GPIO_WritePin(lt_sea_GPIO_Port, lt_sea_Pin, GPIO_PIN_SET);
}
