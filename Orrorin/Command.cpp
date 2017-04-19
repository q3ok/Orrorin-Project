/**
 * Orrorin Robotic Arm Controller
 * Copyright (C) 2017 q3ok [https://github.com/q3ok/Orrorin-Project]
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

#include "Command.h"

Command::Command() {
  this->cmd = "";
}

Command::Command( String cmd ) {
  /* remove all unnecessary elements of pure command */
  cmd.replace('\r',' ');
  cmd.replace('\n',' ');
  cmd.trim();
  this->cmd = cmd;
}

String Command::getInstruction() {
  int de = this->cmd.indexOf(' ');
  if (de == -1) return this->cmd; /* command without parameters */
  return this->cmd.substring(0, de);
}

int Command::getParamCount() {
  int spaceCount = 0;
  for (int i=0;i<=this->cmd.length();i++) {
    if (this->cmd.substring(i,i+1) == " ") spaceCount++;
  }
  return spaceCount; /* spaceCount == paramCount */
}

String Command::getParam(int nr) {
  int paramCount = this->getParamCount();
  if ( nr < 1 || nr > paramCount ) return ""; /* param out of bounds */
  String params[paramCount];
  int posDelimiter = this->cmd.indexOf(' ');
  int posStart = 0;
  for ( int i=0; i < nr; i++ ) {
    posStart = posDelimiter + 1;
    posDelimiter = this->cmd.indexOf(' ', posStart);
  }
  return this->cmd.substring(posStart, posDelimiter);
}
