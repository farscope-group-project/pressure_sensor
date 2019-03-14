#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085_U.h>

#define N_READINGS 100
   
Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085);
int cal_counter = 0;
bool vacuum_flag;
float baseline;

void setup(void) 
{
  Serial.begin(9600);
  
  /* Initialise the sensor */
  if(!bmp.begin())
  {
    //If error, what to do? Send error message via ROS?
  }
}

void baseline_value()
{
  if (cal_counter = N_READINGS)
  {
    STATE = 1;
    baseline = (pressure_sum / N_READINGS);
  }
  else
    pressure_sum += event.pressure;
}

void empty_state()
{
  if (counter == 10)
    vacuum_flag = true;
    STATE = 2;

  if (event.pressure > baseline + THRESHOLD )
    counter++;
}

void holding_state()
{
  if (counter == 10)
    vacuum_flag = false;

  if (event.pressure < baseline - THRESHOLD)
    counter++;
}

void loop(void) 
{
  /* Get a new sensor event */ 
  sensors_event_t event;
  bmp.getEvent(&event);

  switch(STATE)
  {
    case 0: 
      baseline_value();
      break;
    
    case 1:
      empty_state();
      break;
    
    case 2:
      holding_state();
      break;
  }
  
  Serial.println(vacuum_flag);
  
  delay(10);
}
