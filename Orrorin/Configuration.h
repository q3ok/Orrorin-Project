/**
 * Orrorin Robotic Arm Controller
 * Copyright (C) 2017 q3ok [https://github.com/MarlinFirmware/Marlin]
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

/**
 * Configuration.h
 * 
 * Settings file, self-explainable
 * 
 */
#ifndef CONFIGURATION_H
#define CONFIGURATION_H

/* serial port configuration */
#define SERIAL_SPEED 115200 /*bps*/
#define SERIAL_ECHO false /* not working EXACTLY as expected */

/* connected servos configuration */
#define SERVO_REACTION_TIME 800 /* loop ticks as the pwm interrupt is taken by servo...*/
#define SERVO_COUNT 4 /* amount of servos used in the arm */
#define SERVO_PORT { 11, 9, 10, 6 } /* arduino ports with PWM */
#define SERVO_MIN_POS { 10, 40, 50, 10 } /* minimal angles for servos */
#define SERVO_MAX_POS { 170, 130, 130, 170 } /* maximum angles for servos */
#define SERVO_DEFAULT_POS { 115, 50, 110, 10 } /* default angles for servos (eg. default position, at start) */
#define SERVO_GRIPPER_NO 3 /* first servo is 0, second 1, etc */

/* misc */
#define ORRORIN_VERSION 0.1

#include "Servo.h"
#include "Command.h"

#endif
