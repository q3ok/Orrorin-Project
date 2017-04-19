# Orrorin Robotic Arm Controller

This software are described as "robotic arm controller", but it just can drive anything with servos.
Tested on Arduino Nano with four servos.
It should fit on every arduino board, or even on ESP8266 (like WeMos D1 Mini) or similar.

To control the servos, use serial communication port.

### Implemented GCodes:
- G0 A (B C...) - Set angles of servos (0 - A, 1 - B, 2 - C, ...)
- G1 X Y - Set angle to Y of servo X
- G28 - Go to home position
- M0 - Force stop
- M10 - Open gripper
- M11 - Close gripper
- M114 - Get current position
- M115 - Get firmware version
- S0 - save current position as next step in sequence
- S1 - display all moves in sequence
- S2 - remove last move from sequence
- S9 - play/payse sequence
