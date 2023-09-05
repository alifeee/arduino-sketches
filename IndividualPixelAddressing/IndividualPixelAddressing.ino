
/*
 * Test program for bus sign library 
 * 
 */
#include <Adafruit_GFX.h>
#include <CH_AS1100.h>


// just 3 digital ports to control. Any you see fit will do - bit banged
#define DATA_PIN 5
#define CLK_PIN 9
#define LOAD_PIN 10

#define NUM_CHIPS 16


int ledState=LOW;

Panel panel=Panel(DATA_PIN,CLK_PIN,LOAD_PIN,NUM_CHIPS);

int col = 0;

void flashLED(int delay_ms,int numTimes){
// flashes the onboard built in led
// an indicator
 bool on=true;
  for (int t=0;t<numTimes;t++)
    {
      digitalWrite(LED_BUILTIN,on);
      delay(delay_ms);
	    on=!on;
    }
}


void showText(Panel & p,char * msg)
{       
     p.setCursor(0, 0);
     p.setTextColor(1);  
     p.setTextSize(1); // pixel size multiplier
     p.println(msg);
     p.display();
}

void setup(){

  Serial.begin(115200);
  while(!Serial) yield();

  pinMode(LED_BUILTIN,OUTPUT);
  digitalWrite(LED_BUILTIN,LOW); // off

  // begin panel
  // panel.begin();
  // initialise pin states etc
  // some delays needed (why??)

  // panel.setClockMode(2); // reset the clock to internal
  // panel.setClockMode(0);
  // panel.setBinaryMode();
  // panel.setScan(8);
  // panel.setIntensity(2, -1); // start low.
  // panel.clearDisplay();

  // panel.sendPixels();

  // panel.displayOn(1);
    
  flashLED(200,3);

  showText(panel,(char *)"Connected");
  // panel.fillDisplay(1);
  // panel.display();
}

void loop()
{
  //  panel.scrollRows(1,true);     // wrap scroll left to right
  panel.dumpPixels();
  panel.setClockMode(2); // reset the clock to internal
  // panel.display();
  
  //  panel.dumpPixels();
  delay(100);
  //  col++;
}
