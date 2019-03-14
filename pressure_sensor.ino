#include <ros.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085_U.h>

#define THRESHOLD 800
   
Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085);
bool vacuum_flag;

ros::NodeHandle  nh;
std_msgs::bool b_msg;
ros::Publisher vacuum_on("vacuum_on", &b_msg);


void setup(void) 
{
  Serial.begin(9600);
  
  /* Initialise the sensor */
  if(!bmp.begin())
  {
    //If error, what to do? Send error message via ROS?
  }

  nh.initNode();
  nh.advertise(vacuum_on);
}

void loop(void) 
{
  /* Get a new sensor event */ 
  sensors_event_t event;
  bmp.getEvent(&event);

  if (event.pressure > THRESHOLD)
  {
    vacuum_flag = true;
  }
  else if (event.pressure < THRESHOLD)
  {
    vacuum_flag = false;
  }
  
  str_msg.data = vacuum_flag;
  vacuum_on.publish(&b_msg );
  nh.spinOnce();
  
  delay(100);
}
