/* Theramin
 */

#include <Zforce.h>
#include <CH_AS1100.h>

// panels
#define PANEL1_LOAD_PIN 7
#define PANEL2_LOAD_PIN 5
#define NUM_CHIPS 32

// IMPORTANT: change "1" to assigned GPIO digital pin for dataReady signal in your setup:
#define DATA_READY_PIN 2
#define SPEAKER_PIN 3

#define MIN_X 36
#define MAX_X 3132
#define MIN_Y 0
#define MAX_Y 2085

#define MIN_TONE 50   // hz
#define MAX_TONE 5000 // hz

int x;
int y;
float fraction;
int note;

unsigned long time_of_tone_start;
unsigned long time_now;

Panel topRow = Panel(PANEL1_LOAD_PIN, NUM_CHIPS);
Panel bottomRow = Panel(PANEL2_LOAD_PIN, NUM_CHIPS);

void setup()
{
  Serial.begin(115200);
  while (!Serial)
  {
  };

  topRow.begin();
  bottomRow.begin();

  Serial.println("zforce start");
  zforce.Start(DATA_READY_PIN);
  init_sensor();
}

void loop()
{
  // Continuously read any messages available from the sensor and print
  // touch data to Serial interface. Normally there should be only
  // touch notifications as long as no request messages are sent to the sensor.
  Message *touch = zforce.GetMessage();
  if (touch != nullptr)
  {
    if (touch->type == MessageType::TOUCHTYPE)
    {
      for (uint8_t i = 0; i < ((TouchMessage *)touch)->touchCount; i++)
      {
        Serial.print("Touch event ");
        Serial.print(((TouchMessage *)touch)->touchData[i].event);
        Serial.print(" with ID ");
        Serial.print(((TouchMessage *)touch)->touchData[i].id);
        Serial.print(" at (x,y) (");
        Serial.print(((TouchMessage *)touch)->touchData[i].x);
        Serial.print(", ");
        Serial.print(((TouchMessage *)touch)->touchData[i].y);
        Serial.println(")");
      }

      time_of_tone_start = millis();

      x = ((TouchMessage *)touch)->touchData[0].x;
      y = ((TouchMessage *)touch)->touchData[0].y;

      fraction = (MAX_Y - (float)y) / (MAX_Y - MIN_Y);
      note = MIN_TONE + fraction * (MAX_TONE - MIN_TONE);

      tone(SPEAKER_PIN, note);
      fillFraction(topRow, fraction);
      fillFraction(bottomRow, fraction);
    }
    else if (touch->type == MessageType::BOOTCOMPLETETYPE)
    {
      // If we for some reason receive a boot complete
      // message, the sensor needs to be reinitialized:
      init_sensor();
    }

    zforce.DestroyMessage(touch);
  }
  time_now = millis();

  if ((time_now - time_of_tone_start) > 50)
  {
    noTone(SPEAKER_PIN);
    // topRow.clearDisplay();
    // topRow.display();
    // bottomRow.clearDisplay();
    // bottomRow.display();
  }
}

void fillFraction(Panel &p, float fraction) {
  int max = NUM_CHIPS * 6;
  for (int i = 0; i < max; i++) {
    for (int row = 0; row < 9; row++) {
      if (((float)i / (float)max) < fraction) {
        p.setPixel(i, row, 1);
      }
      else {
        p.setPixel(i, row, 0);
      }
    }
  }
  p.display();
}

// Write some configuration parameters to sensor and enable sensor.
// The choice of parameters to configure here are just examples to show how to
// use the library.
// NOTE: Sensor firmware versions 2.xx has persistent storage of configuration
// parameters while versions 1.xx does not. See the library documentation for
//  further info.
void init_sensor()
{
  Message *msg = nullptr;

  // Send and read ReverseY
  zforce.ReverseY(true);

  do
  {
    msg = zforce.GetMessage();
  } while (msg == nullptr);

  if (msg->type == MessageType::REVERSEYTYPE)
  {
    Serial.println("Received ReverseY Response");
    Serial.print("Message type is: ");
    Serial.println((int)msg->type);
  }

  zforce.DestroyMessage(msg);

  // Send and read Enable

  zforce.Enable(true);

  do
  {
    msg = zforce.GetMessage();
  } while (msg == nullptr);

  if (msg->type == MessageType::ENABLETYPE)
  {
    Serial.print("Message type is: ");
    Serial.println((int)msg->type);
    Serial.println("Sensor is now enabled and will report touches.");
  }

  zforce.DestroyMessage(msg);
}
