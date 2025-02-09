/*
 *
 * 3-digit counter with increment and decrement buttons
 */

#include <Arduino.h>
#include <EEPROM.h>

// Pin definitions
// The 7-segment displays are connected to the Arduino as follows:
int segPins[] = {9, 8, 7, 6, 5, 4, 3, 2};   // { a b c d e f g)
// These are the pins that power the individual displays, through a transistor
// to allow multiplexing
int displayPins[] = {12, 11, 10};   
// The display buffer contains the digits to be displayed.
int displayBuf[3]; 

// Input pins for the increment and decrement buttons
const int btnInc = 14; 
const int btnDec = 15; 
// Map of the segments to the pins on the 7-segment display
byte segCode[11][8] = {
//  a  b  c  d  e  f  g  .
  { 1, 1, 1, 1, 1, 1, 0, 0},  // 0
  { 0, 1, 1, 0, 0, 0, 0, 0},  // 1
  { 1, 1, 0, 1, 1, 0, 1, 0},  // 2
  { 1, 1, 1, 1, 0, 0, 1, 0},  // 3
  { 0, 1, 1, 0, 0, 1, 1, 0},  // 4
  { 1, 0, 1, 1, 0, 1, 1, 0},  // 5
  { 1, 0, 1, 1, 1, 1, 1, 0},  // 6
  { 1, 1, 1, 0, 0, 0, 0, 0},  // 7
  { 1, 1, 1, 1, 1, 1, 1, 0},  // 8
  { 1, 1, 1, 1, 0, 1, 1, 0},  // 9
  { 0, 0, 0, 0, 0, 0, 0, 1}   // .
};

/* 
 * Function to set the segments of the 7-segment display to display the number n
 */
void setSegments(int n)
{
  for (int i=0; i < 8; i++)
  {
    digitalWrite(segPins[i], segCode[n][i]);
  }
}

/*
 * Function to refresh the display with the digits digit2, digit1, digit0
 * This is the multiplexing part
 */
void refreshDisplay(int digit2, int digit1, int digit0)
{
  digitalWrite(displayPins[0], HIGH);  // displays digit 0 (least significant)
  digitalWrite(displayPins[1], LOW);
  digitalWrite(displayPins[2], LOW);
  setSegments(digit0);
  delay(5);
  digitalWrite(displayPins[0], LOW);    // then displays digit 1
  digitalWrite(displayPins[1], HIGH);
  digitalWrite(displayPins[2], LOW);
  setSegments(digit1);
  delay(5);
  digitalWrite(displayPins[0], LOW);    // then displays digit 2
  digitalWrite(displayPins[1], LOW);
  digitalWrite(displayPins[2], HIGH);
  setSegments(digit2);
  delay(5);
}

/*
 * Function to save the display buffer to EEPROM
 * This allows to save the current value of the counter even when the power is turned off
 */
void saveDisplayBuf()
{
  EEPROM.write(0, displayBuf[2]);
  EEPROM.write(1, displayBuf[1]);
  EEPROM.write(2, displayBuf[0]);
}

/*
 * Setup function
 * This function is called once when the Arduino is powered on
 * Defines the pin modes and reads the saved values from EEPROM
 * 
 * Note: The EEPROM may have been written with random values, so it is important to
 * initialize it once with the correct values
 */
void setup()
{
  for (int i=0; i < 8; i++)
  {
    pinMode(segPins[i], OUTPUT);
  }
  pinMode(displayPins[0], OUTPUT);
  pinMode(displayPins[1], OUTPUT);
  pinMode(displayPins[2], OUTPUT);

  pinMode(btnInc, INPUT_PULLUP); // Set increment button pin as input
  pinMode(btnDec, INPUT_PULLUP); // Set decrement button pin as input 

  // Read saved values from EEPROM
  displayBuf[2] = EEPROM.read(0);
  displayBuf[1] = EEPROM.read(1);
  displayBuf[0] = EEPROM.read(2);
}

/*
  * Main loop function
  * This function is called repeatedly after the setup function
  * It checks if the increment or decrement buttons are pressed and updates the display buffer
*/
void loop()
{
  // Refresh Display each iteration
  refreshDisplay(displayBuf[2], displayBuf[1],displayBuf[0]);

  // Check if increment button is pressed
  if (digitalRead(btnInc) == LOW)
  {
    displayBuf[0]++;
    if (displayBuf[0] > 9)
    {
      displayBuf[0] = 0;
      displayBuf[1]++;
      if (displayBuf[1] > 9)
      {
        displayBuf[1] = 0;
        displayBuf[2]++;
        if (displayBuf[2] > 9)
        {
          displayBuf[2] = 0;
        }
      }
    }
    saveDisplayBuf();
    // Debounce delay, otherwise the counter will increment too fast
    delay(200); 
  }

  // check if decrement button is pressed
  if (digitalRead(btnDec) == LOW) 
  {
    displayBuf[0]--;
    if (displayBuf[0] < 0)
    {
      displayBuf[0] = 9;
      displayBuf[1]--;
      if (displayBuf[1] < 0)
      {
        displayBuf[1] = 9;
        displayBuf[2]--;
        if (displayBuf[2] < 0)
        {
          displayBuf[2] = 9;
        }
      }
    }
    saveDisplayBuf();
    // Debounce delay, otherwise the counter will increment too fast
    delay(200);
  }
}