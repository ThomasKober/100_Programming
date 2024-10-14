/*
 * my_controls.h
 *
 *  Created on: Oct 12, 2024
 *      Author: Thomas Kobers
 */

#ifndef INC_MY_CONTROLS_H_
#define INC_MY_CONTROLS_H_

#include "stdbool.h"


/*
 * ----------------------------------------------------------------------
 * ON/OFF status struct
 * ----------------------------------------------------------------------
 */
typedef struct{
	bool status_lt_bea;	//beacon light
	bool status_lt_pos;	//position light
	bool status_lt_ac;	//anti collision light
	bool status_lt_lan;	//landing light
	bool status_lt_sea;	//search light
} LtStatus;


/*
 * ----------------------------------------------------------------------
 * struct init function prototypes
 * ----------------------------------------------------------------------
 */
void init_status(bool stat_bea, bool stat_pos, bool stat_ac, bool stat_lan, bool stat_sea);


/*
 * ----------------------------------------------------------------------
 * get-status functions prototypes
 * ----------------------------------------------------------------------
 */
//void get_status(LtStatus* s);
bool get_status_lt_bea(void);
bool get_status_lt_pos(void);
bool get_status_lt_ac(void);
bool get_status_lt_lan(void);
bool get_status_lt_sea(void);


/*
 * ----------------------------------------------------------------------
 * set-status functions prototypes
 * ----------------------------------------------------------------------
 */
void set_status_lt_bea(bool stat);
void set_status_lt_pos(bool stat);
void set_status_lt_ac(bool stat);
void set_status_lt_lan(bool stat);
void set_status_lt_sea(bool stat);


/*
 * ----------------------------------------------------------------------
 * light functions prototypes
 * ----------------------------------------------------------------------
 */
void lt_beacon(void);
void lt_position(void);
void lt_anticollition(void);
void lt_landing(void);
void lt_search(void);

#endif /* INC_MY_CONTROLS_H_ */
