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
 * Orrorin_main.cpp
 * 
 * "the main" code
 */
#include "Configuration.h"

Servo hServo[SERVO_COUNT];
int servoPort[SERVO_COUNT] = SERVO_PORT;
int servoMinPos[SERVO_COUNT] = SERVO_MIN_POS;
int servoMaxPos[SERVO_COUNT] = SERVO_MAX_POS;
int servoDefaultPos[SERVO_COUNT] = SERVO_DEFAULT_POS;
int servoCurrentPos[SERVO_COUNT] = SERVO_DEFAULT_POS;
int servoRequestedPos[SERVO_COUNT] = SERVO_DEFAULT_POS;
int servoTicks[SERVO_COUNT] = { 0 };

int savedMoves[100][4];
int savedMovesCount = 0;
int savedMovesCurrentStep = 0;
unsigned long savedMovesLastRun = 0;
unsigned long savedMovesDelay = 1000;
bool playSaved = false;

void servoTick() {
  for (int i=0;i<SERVO_COUNT;i++) {
    if (servoRequestedPos[i] == servoCurrentPos[i]) continue; // nie ma po co liczyc
    if (++servoTicks[i] == SERVO_REACTION_TIME) {
      servoTicks[i] = 0;
      int dir = 1;
      if ( servoRequestedPos[i] < servoCurrentPos[i] ) dir = -1;
      servoCurrentPos[i] = servoCurrentPos[i] + dir;
      hServo[i].write(servoCurrentPos[i]);
    }
  }
}

bool servoRequestPos(int servo, int pos) {
  if (pos < servoMinPos[servo] || pos > servoMaxPos[servo]) return false;
  servoRequestedPos[servo] = pos;
  return true;
}

void setup() {
  Serial.begin(SERIAL_SPEED);
  Serial.println("Orrorin Robotic Arm Controller");
  Serial.setTimeout(100); /* to avoid lags in communication */
  
  for (int i=0;i<SERVO_COUNT;i++) {
    hServo[i].attach( servoPort[i] );
    hServo[i].write( servoDefaultPos[i] );
    delay(30); /* wait for reach */
  }

  Serial.println("READY");
}

void loop() {
  if ( Serial.available() > 0 ) {
    String serialData = Serial.readStringUntil('\n');
    Command* cmd = new Command(serialData);
    if (cmd->getInstruction() == "G0") { /* set positions for all servos */
      for (int i=0;i<cmd->getParamCount();i++) {
        if (i > SERVO_COUNT) break; /* ??? */
        if (!servoRequestPos(i, (cmd->getParam(i+1)).toInt() )) {
          Serial.print("G0: Could not set servo ");
          Serial.print(i);
          Serial.print(" to angle ");
          Serial.println(cmd->getParam(i+1));
        }
      }
      Serial.println("ok");
    } else
    if (cmd->getInstruction() == "G1") { /* set position for specific servo */
      if (cmd->getParamCount() != 2) {
        Serial.println("G1: Wrong number of parameters");
      } else {
        if (!servoRequestPos( (cmd->getParam(1)).toInt(), (cmd->getParam(2)).toInt() ) ) {
          Serial.println("G1: This move is not allowed");
        } else {
          Serial.println("ok");
        }
      }
    } else
    if (cmd->getInstruction() == "G28") { /*  G28: Move to Origin (Home) */
      for (int i=0;i<SERVO_COUNT;i++) {
        servoRequestPos(i,servoDefaultPos[i]);
      }
      Serial.println("ok");
    } else
    if (cmd->getInstruction() == "M0") { /* force stop, disable servo control, restart needed */
      for ( int i=0; i<SERVO_COUNT; i++ ) {
        hServo[i].detach();
      }
      Serial.println("ok HALTED");
      for (;;) {}
    } else
    if (cmd->getInstruction() == "M10") { /* open gripper */
      servoRequestPos(SERVO_GRIPPER_NO, servoMaxPos[SERVO_GRIPPER_NO]);
      Serial.println("ok");
    } else
    if (cmd->getInstruction() == "M11") { /* close gripper */
      servoRequestPos(SERVO_GRIPPER_NO, servoMinPos[SERVO_GRIPPER_NO]);
      Serial.println("ok");
    } else
    if (cmd->getInstruction() == "M114") { /* M114: Get Current Position */
      Serial.print("ok");
      for (int i=0; i<SERVO_COUNT; i++) {
        Serial.print(" ");
        Serial.print(servoCurrentPos[i]);
      }
      Serial.println();
    } else
    if (cmd->getInstruction() == "M115") { /* M115: Get Firmware Version and Capabilities */
      Serial.print("ok ");
      Serial.print("PROTOCOL_VERSION:0.1 FIRMWARE_NAME:Orrorin FIRMWARE_VERSION:");
      Serial.print(ORRORIN_VERSION);
      Serial.println();
    } else
    if (cmd->getInstruction() == "S0") { /* save position as next move in sequence */
      if (savedMovesCount == 1000) {
        Serial.println("S0: Maximum number of moves already saved");
      } else {
        for (int i=0;i<SERVO_COUNT;i++) {
          savedMoves[savedMovesCount][i] = servoCurrentPos[i];
        }
        savedMovesCount++;
        Serial.println("ok");
      }
    } else
    if (cmd->getInstruction() == "S1") { /* display all saved positions from sequence (with G0) */
      for (int i=0;i<savedMovesCount;i++) {
        Serial.print("G0");
        for (int j=0;j<SERVO_COUNT;j++) {
          Serial.print(" ");
          Serial.print(savedMoves[i][j]);
        }
        Serial.println();
      }
      Serial.println("ok");
    } else
    if (cmd->getInstruction() == "S2") { /* remove last saved instruction */
      if (--savedMovesCount<0) savedMovesCount=0;
      Serial.println("ok");
    } else
    if (cmd->getInstruction() == "S9") { /* play/pause played moves */
      playSaved=!playSaved;
      Serial.println("ok");
    }
    if (SERIAL_ECHO) Serial.println(cmd->getInstruction());
    Serial.flush();
  }

  if (playSaved) {
    unsigned long currentMs = millis();
    if (savedMovesLastRun + savedMovesDelay < currentMs) {
      /* next run should be done, the time has already gone */
      for (int i=0;i<SERVO_COUNT;i++) {
        servoRequestPos(i, savedMoves[savedMovesCurrentStep][i]);
      }
      if (++savedMovesCurrentStep >= savedMovesCount) savedMovesCurrentStep=0;
      savedMovesLastRun = currentMs;
    }
  }
  
  servoTick();

}
