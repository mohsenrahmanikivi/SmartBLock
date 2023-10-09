/*
 * _servoControl.cpp
 *
 *  Created on: Oct 6, 2023
 *      Author: mrahm
 */

#include "stm32f4xx_hal.h"
#include "_servoControl.h"

extern TIM_HandleTypeDef htim1;


void _servoLock(void){
	if (HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1) != HAL_OK) 	printf("\n_servoLock--<error> PWM START function error \r");

	htim1.Instance->CCR1 = 25;  // Lock (duty cycle is 2.5 ms) meaning set position to 180 degree
	HAL_Delay(1000);

	//print LOCK
		printf("\n     .--------.\n"
				"    / .------. \\\n"
				"   / /        \\ \\\n"
				"   | |        | |\n"
				"  _| |________| |_\n"
				".' |_|        |_| '.\n"
				"'._____ ____ _____.'\n"
				"|     .'____'.     |\n"
				"'.__.'.'    '.'.__.'\n"
				"'.__  | LOCK |  __.'\n"
				"|   '.'.____.'.'   |\n"
				"'.____'.____.'____.'\n"
				"'.________________.'\n");

	if (HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1) != HAL_OK) 		printf("\n_servoLock--<error> PWM STOP function error \r");
}

void _servoUnlock(void){
	if (HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1) != HAL_OK) 	printf("\n_servoUnlock--<error> PWM START function error \r");

	  htim1.Instance->CCR1 = 125;  //  Open-Unlock (duty cycle is .5 ms) meaning set position to 0 degree
	  HAL_Delay(1000);
	  printf("\n     .--------.\n"
	  				  "    / .------. \\\n"
	  				  "   / /        \\ \\\n"
	  				  "   | |        | |\n"
	  				  "   | |        |_|\n"
	  				  "   | |\n"
	  				  "  _| |____________\n"
	  				  ".' |_|         _  '.\n"
	  				  "'._____ ____ _____.'\n"
	  				  "|     .'____'.     |\n"
	  				  "'.__.'.'    '.'.__.'\n"
	  				  "'.__  |UNLOCK|  __.'\n"
	  				  "|   '.'.____.'.'   |\n"
	  				  "'.____'.____.'____.'\n"
	  				  "'.________________.'\n");
	if (HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1) != HAL_OK) 		printf("\n_servoUnlock--<error> PWM STOP function error \r");

}


void _servoUnlockTimeout(int seconds){

	if (seconds > 999){
		printf("\n__servoUnlockTimeout--<error> TIMEOUT can NOT be more than 999, TIMEOUT= %d \r", seconds);
		while(1);
	}

		if (HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1) != HAL_OK) 	printf("\n__servoUnlockTimeout--<error> PWM START function error \r");

		htim1.Instance->CCR1 = 125;  //  Open-Unlock (duty cycle is .5 ms) meaning set position to 0 degree
	  printf("\n     .--------.\n"
				  "    / .------. \\\n"
				  "   / /        \\ \\\n"
				  "   | |        | |\n"
				  "   | |        |_|\n"
				  "   | |\n"
				  "  _| |____________\n"
				  ".' |_|         _  '.\n"
				  "'._____ ____ _____.'\n"
				  "|     .'____'.     |\n"
				  "'.__.'.'    '.'.__.'\n"
				  "'.__  |UNLOCK|  __.'\n"
				  "|   '.'.____.'.'   |\n"
				  "'.____'.____.'____.'\n"
				  "'.________________.'\n");
	  printf("\n__servoUnlockTimeout--<info> Unlock TimeOut= ");
		while( seconds >= 0){ // waiting point
			printf("%03d",seconds);
			fflush(stdout);
			HAL_Delay(1000);
			printf("\b\b\b");
			fflush(stdout);
			seconds--;
			}


		htim1.Instance->CCR1 = 25;  // Lock (duty cycle is 2.5 ms) meaning set position to 180 degree
		HAL_Delay(500);
		if (HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1) != HAL_OK) 		printf("\n_servoUnlock--<error> PWM STOP function error \r");

		//print LOCK
			printf("\n     .--------.\n"
					"    / .------. \\\n"
					"   / /        \\ \\\n"
					"   | |        | |\n"
					"  _| |________| |_\n"
					".' |_|        |_| '.\n"
					"'._____ ____ _____.'\n"
					"|     .'____'.     |\n"
					"'.__.'.'    '.'.__.'\n"
					"'.__  | LOCK |  __.'\n"
					"|   '.'.____.'.'   |\n"
					"'.____'.____.'____.'\n"
					"'.________________.'\n");

}



