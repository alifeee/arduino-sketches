
/*
 * Test program for bus sign library
 *
 */
#include <Adafruit_GFX.h>
#include <CH_AS1100.h>
#include <SPI.h>

// just 3 digital ports to control
#define DATA_PIN 11
#define CLK_PIN 13
#define LOAD_PIN 7

// bottom panel
#define DATA_PIN_2 11
#define CLK_PIN_2 13
#define LOAD_PIN_2 5

#define NUM_CHIPS 32

Panel topRow = Panel(DATA_PIN, CLK_PIN, LOAD_PIN, NUM_CHIPS);
Panel bottomRow = Panel(DATA_PIN_2, CLK_PIN_2, LOAD_PIN_2, NUM_CHIPS);

void showText(Panel &p, char *msg)
{
  p.setCursor(0, 0);
  p.setTextColor(1);
  p.setTextSize(1); // pixel size multiplier
  p.println(msg);
  p.display();
}

void setup()
{
  SPI.begin();
  topRow.begin();
  bottomRow.begin();

  showText(topRow, (char *)"Hull Makerspace");
  showText(bottomRow, (char *)"@alifeee");
}

void loop()
{
  topRow.scrollRows(1, true); // wrap scroll left to right
  topRow.display();
  bottomRow.scrollRows(-1, true); // wrap scroll right to left
  bottomRow.display();
  // delay(100);
}
