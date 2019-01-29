/*
 *  message.h
 */

typedef struct UDP_Control_MSG_Type
{
  int port;
  int numsamples;
  int interval;
  
}UDP_Cntr_MSG;

typedef struct Sensor_MSG_Type
{
  float value;
  int sequence;
  
}Sensor_MSG;
