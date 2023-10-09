/*
 * _servoControl.h
 *
 *  Created on: Oct 6, 2023
 *      Author: mrahm
 */

#ifndef INC__SERVOCONTROL_H_
#define INC__SERVOCONTROL_H_

#include <iostream>


using namespace std;


void _servoLock(void);
void _servoUnlock(void);

void _servoUnlockTimeout(int seconds);


#endif /* INC__SERVOCONTROL_H_ */
