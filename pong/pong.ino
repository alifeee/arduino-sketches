#include <Zforce.h>
#include <CH_AS1100.h>

// neonode
#define NEONODE_DATA_READY_PIN 2
#define TOUCH_MIN_X 36
#define TOUCH_MAX_X 3132
#define TOUCH_MIN_Y 0
#define TOUCH_MAX_Y 2085

// panel
#define TOP_LOAD_PIN 7
#define BOTTOM_LOAD_PIN 5
#define NUM_CHIPS 32
#define PIXELS_LEFT 50
#define PIXELS_RIGHT 150 // max 192
#define PIXELS_TOP 8 // max 8
#define PIXELS_BOTTOM 0
const int PIXELS_HEIGHT = PIXELS_TOP - PIXELS_BOTTOM;
const int PIXELS_WIDTH = PIXELS_RIGHT - PIXELS_LEFT;

// pong
// constants
#define PADDLE_X 2
#define PADDLE_Y 3 // must be odd
#define BALL_INIT_X 96
#define BALL_INIT_Y 4
#define BALL_INIT_V_X 20
#define BALL_INIT_V_Y 10
// variables
unsigned long time_of_last_frame = 0;
unsigned long time_of_current_frame = 0;
unsigned short delta_us;
float delta_s;
float player_left_paddle_y = 0.5;
float player_right_paddle_y = 0.5;
float ball_x = BALL_INIT_X;
float ball_y = BALL_INIT_Y;
float ball_vx = BALL_INIT_V_X;
float ball_vy = BALL_INIT_V_Y;
int player_left_score = 0;
int player_right_score = 0;

// neonode
Message *touch = nullptr;

Panel topRow = Panel(TOP_LOAD_PIN, NUM_CHIPS);
Panel bottomRow = Panel(BOTTOM_LOAD_PIN, NUM_CHIPS);

void setup()
{
  Serial.begin(115200);
  while (!Serial)
  {
  };

  Serial.println("init screen");
  topRow.begin();
  bottomRow.begin();

  Serial.println("init sensor");
  zforce.Start(NEONODE_DATA_READY_PIN);
  init_sensor();

  Serial.println("show text");
  showText(bottomRow, (char *)"Pong! v0.1.0");
}

void loop()
{
  // compute delta
  time_of_current_frame = micros();
  if (time_of_last_frame == 0)
  {
    time_of_last_frame = micros();
    return;
  }
  delta_us = time_of_current_frame - time_of_last_frame;
  delta_s = (float)delta_us / 1000000;
  time_of_last_frame = micros();

  // get touch
  touch = zforce.GetMessage();
  if (touch != nullptr)
  {
    if (touch->type == MessageType::TOUCHTYPE)
    {
      // for touch in touches
      for (uint8_t i = 0; i < ((TouchMessage *)touch)->touchCount; i++)
      {
        TouchData touchData = ((TouchMessage *)touch)->touchData[i];
        // printTouchInfo(touchData);
        int player = getPlayer(touchData);
        float y_fraction = getYFraction(touchData);
        if (false)
        {
          Serial.print("player ");
          Serial.print(player);
          Serial.print(" at ");
          Serial.println(y_fraction);
        }
        if (player == 0)
        {
          player_left_paddle_y = y_fraction;
        }
        else if (player == 1)
        {
          player_right_paddle_y = y_fraction;
        }
      }
    }
    else if (touch->type == MessageType::BOOTCOMPLETETYPE)
    {
      // reinitialize on boot complete message
      init_sensor();
    }
    zforce.DestroyMessage(touch);
  }

  // ball logic
  float ball_next_x = ball_x + (ball_vx * delta_s);
  float ball_next_y = ball_y + (ball_vy * delta_s);
  // off top
  if (ball_next_y > PIXELS_TOP)
  {
    ball_vy *= -1;
    ball_y = PIXELS_TOP;
  }
  // off bottom
  else if (ball_next_y < PIXELS_BOTTOM)
  {
    ball_vy *= -1;
    ball_y = PIXELS_BOTTOM;
  }
  // off left edge
  else if (ball_next_x < PIXELS_LEFT + PADDLE_X)
  {
    if (inRange(player_left_paddle_y * PIXELS_HEIGHT - PADDLE_Y, player_left_paddle_y * PIXELS_HEIGHT + PADDLE_Y, ball_next_y))
    {
      ball_vx *= -1;
      ball_x = PIXELS_LEFT + PADDLE_X;
    }
    else
    {
      ball_x = BALL_INIT_X;
      ball_y = BALL_INIT_X;
      ball_vx = BALL_INIT_V_X;
      ball_vy = BALL_INIT_V_Y;
    }
  }
  // off right edge
  else if (ball_next_x > PIXELS_RIGHT - PADDLE_X)
  {
    if (inRange(player_right_paddle_y * PIXELS_HEIGHT - PADDLE_Y, player_right_paddle_y * PIXELS_HEIGHT + PADDLE_Y, ball_next_y))
    {
      ball_vx *= -1;
      ball_x = PIXELS_RIGHT - PADDLE_X;
    }
    else
    {
      ball_x = BALL_INIT_X;
      ball_y = BALL_INIT_X;
      ball_vx = BALL_INIT_V_X;
      ball_vy = BALL_INIT_V_Y;
    }
  }
  else
  {
    ball_x += ball_vx * delta_s;
    ball_y += ball_vy * delta_s;
  }

  topRow.clearDisplay();
  drawPaddles(topRow, player_left_paddle_y, player_right_paddle_y);
  drawBall(topRow, ball_x, ball_y);
  topRow.display();
}

bool inRange(int min, int max, float val)
{
  // returns true if val is within min < val < max
  return val >= min && val <= max;
}

void showText(Panel &p, char *msg)
{
  p.setCursor(0, 0);
  p.setTextColor(1);
  p.setTextSize(1); // pixel size multiplier
  p.println(msg);
  p.display();
}

void drawPaddles(Panel &p, float left_y, float right_y)
{
  int pixel_left = left_y * PIXELS_HEIGHT;
  int pixel_right = right_y * PIXELS_HEIGHT;
  int panel_start = (PADDLE_Y - 1) / 2;
  for (int t = 0; t < PADDLE_X; t++)
  {
    for (int i = 0 - panel_start; i <= panel_start; i++)
    {
      p.setPixel(PIXELS_LEFT + t, pixel_left + i, 1);
      p.setPixel(PIXELS_RIGHT - 1 - t, pixel_right + i, 1);
    }
  }
}

void drawBall(Panel &p, float x, float y)
{
  p.setPixel(x, y, 1);
}

void init_sensor()
{
  Message *msg = nullptr;

  // Send and read ReverseX
  zforce.ReverseX(false);
  do
  {
    msg = zforce.GetMessage();
  } while (msg == nullptr);
  if (msg->type == MessageType::REVERSEXTYPE)
  {
    Serial.println("Received ReverseX Response");
    Serial.print("Message type is: ");
    Serial.println((int)msg->type);
  }
  zforce.DestroyMessage(msg);

  // Send and read ReverseY
  zforce.ReverseY(false);
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

void printTouchInfo(TouchData touch)
{
  Serial.print("Touch event ");
  Serial.print(touch.event);
  Serial.print(" with ID ");
  Serial.print(touch.id);
  Serial.print(" at (x,y) (");
  Serial.print(touch.x);
  Serial.print(", ");
  Serial.print(touch.y);
  Serial.println(")");
}

int getPlayer(TouchData touch)
{
  int midpoint = (TOUCH_MAX_X - TOUCH_MIN_X) / 2;
  if (touch.x < midpoint)
  {
    return 0;
  }
  else
  {
    return 1;
  }
}

float getYFraction(TouchData touch)
{
  return (TOUCH_MAX_Y - (float)touch.y) / (TOUCH_MAX_Y - TOUCH_MIN_Y);
}
