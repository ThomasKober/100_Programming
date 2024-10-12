/*
 * my_controls.h
 *
 *  Created on: Oct 12, 2024
 *      Author: Thomas Kobers
 */

#ifndef INC_MY_CONTROLS_H_
#define INC_MY_CONTROLS_H_

#include "stdbool.h"


typedef struct{
	bool status_lt_bea;	//beacon light
	bool status_lt_pos;	//position light
	bool status_lt_ac;	//anti collision light
	bool status_lt_lan;	//landing light
	bool status_lt_sea;	//search light
} Status;

void init_status(Status *s, bool stat_bea, bool stat_pos, bool stat_ac, bool stat_lan, bool stat_sea);
void get_status(const Status *s);

void lt_beacon(Status *s);
void lt_position(Status *s);
void lt_anticollition(Status *s);
void lt_landing(Status *s);
void lt_search(Status *s);

#endif /* INC_MY_CONTROLS_H_ */
