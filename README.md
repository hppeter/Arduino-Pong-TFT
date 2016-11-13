# Arduino-Pong-TFT
Arduino Pong game for TFT with additional single player mode



 Arduino Pong
 By Pete Lamonica
 modified by duboisvb
 updated by James Bruce 
 modified for tft, add menu and squash for single player, by Hans-Peter Dietrich
 A simple implementation of Pong on the Arduino using a tft for output.
  
  I used Arduino IDE 1.6.12 from Arduino.CC
  
  Testet with 2.4" TFT 320x240 LCD Shield (8-bit parallel, ST7783) and SWTFT library 
  from http://www.smokeandwires.co.nz on Arduino UNO.  Should work with any other display which can work with 
  the Adafruit GFX Master library.
  
  Uno modified with additional soldered A6 and A7 lines, because all other pins are connected to TFT. If you can, you can use a Arduino MEGA, there are enough free IO lines.
  
  2 Paddle (10K) connected to 5V, GND and A6 (A7)
  
  1 Button connected to 5V and A5 (A5 pulldown with 10K to GND)
  
  1 optional Buzzer connected to GND and D11
 
  
  In Menu select with Paddle 1 and Button. To start place paddle on "Start".
  In Game press Button for exit
  
 
