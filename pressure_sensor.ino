#include <ros.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085_U.h>
#include <std_msgs/Bool.h>
#include <std_msgs/Empty.h>

#define THRESHOLD_LOWER 870
#define THRESHOLD_UPPER 960
#define N_READINGS 10
#define VACUUM_PIN 12


Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085);
bool vacuum_flag = false;
bool grip_flag = false;
long pressure_sum = 0;
int pressure_avg;
int counter = 0;

ros::NodeHandle  nh;
std_msgs::Bool b_msg_Held;
std_msgs::Bool b_msg_Status;
ros::Publisher ItemHeld("ItemHeld", &b_msg_Held);
ros::Publisher VacuumStatus("VacuumStatus", &b_msg_Status);


void messageCb( const std_msgs::Bool& toggle_msg)
{
  if (toggle_msg.data == true)
    digitalWrite(VACUUM_PIN, LOW);
  else
    digitalWrite(VACUUM_PIN, HIGH);
}

ros::Subscriber<std_msgs::Bool> GripItem("GripItem", messageCb );



void setup(void)
{
  //  Serial.begin(9600);

  /* Initialise the sensor */
  if (!bmp.begin())
  {
    //If error, what to do? Send error message via ROS?
  }
  pinMode(VACUUM_PIN, OUTPUT);
  digitalWrite(VACUUM_PIN, HIGH);

  nh.initNode();
  nh.advertise(ItemHeld);
  nh.advertise(VacuumStatus);
  nh.subscribe(GripItem);
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

    if (pressure_avg < THRESHOLD_LOWER)
      grip_flag = true;

    else if (pressure_avg > THRESHOLD_LOWER)
      grip_flag = false;

    if (pressure_avg < THRESHOLD_UPPER)
      vacuum_flag = true;
    else if (pressure_avg > THRESHOLD_UPPER)
      vacuum_flag = false;
  }
  else
  {
    pressure_sum += event.pressure;
    counter++;
  }

  b_msg_Held.data = grip_flag;
  b_msg_Status.data = vacuum_flag;
  ItemHeld.publish(&b_msg_Held);
  VacuumStatus.publish(&b_msg_Status);
  nh.spinOnce();

  delay(10);
}
