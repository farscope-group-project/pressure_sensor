#include <ros.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085_U.h>
#include <std_msgs/Bool.h>
#include <std_msgs/Empty.h>

#define THRESHOLD 870
#define N_READINGS 10
#define VACUUM_PIN 12


Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085);
bool vacuum_flag = false;
long pressure_sum = 0;
int pressure_avg;
int counter = 0;

ros::NodeHandle  nh;
std_msgs::Bool b_msg;
ros::Publisher vacuum_on("vacuum_on", &b_msg);


void messageCb( const std_msgs::Empty& toggle_msg)
{
  if (digitalRead(VACUUM_PIN) == HIGH)
    digitalWrite(VACUUM_PIN, LOW);
  else
    digitalWrite(VACUUM_PIN, HIGH);
}

ros::Subscriber<std_msgs::Empty> vacuum_toggle("toggle_vacuum", messageCb );



void setup(void)
{
  //  Serial.begin(9600);

  /* Initialise the sensor */
  if (!bmp.begin())
  {
    //If error, what to do? Send error message via ROS?
  }
  pinMode(VACUUM_PIN, OUTPUT);

  nh.initNode();
  nh.advertise(vacuum_on);
  nh.subscribe(vacuum_toggle);
}

void loop(void)
{
  /* Get a new sensor event */
  sensors_event_t event;
  bmp.getEvent(&event);

  if (counter == N_READINGS)
  {
    pressure_avg = (pressure_sum / N_READINGS);
    pressure_sum = 0;
    counter = 0;

    if (pressure_avg < THRESHOLD)
      vacuum_flag = true;

    else if (pressure_avg > THRESHOLD)
      vacuum_flag = false;
  }
  else
  {
    pressure_sum += event.pressure;
    counter++;
  }

  b_msg.data = vacuum_flag;
  vacuum_on.publish(&b_msg );
  nh.spinOnce();

  delay(10);
}
