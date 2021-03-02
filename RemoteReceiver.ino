/**************private includes***********/
//#include <SPI.h>
#include <LoRa.h>
#include "stdio.h"
#include "stdint.h"
#include "string.h"
#include "pins_arduino.h"


/***************Variables****************/
int Xaxis = A0;    // select the input pin for the Xaxis Joystick
int Yaxis = A1;    // select the input pin for the Yazis Joystick
uint32_t XValue = 0, YValue = 0;    // analog values of axis accordingly 
char  Buff[20];
char  received_packet[50];
char print_pkt[100];
uint32_t to_print_cntr = 0;

enum 
{
  LED_on,
  LED_off
}stat_flag;

/*************Functions*************/
void rx_int(int data_size)
{
  // try to parse packet
  //int packetSize = LoRa.parsePacket();
  if (data_size) 
  {
    // received a packet
    Serial.println("Received packet: ");

    char * received_packet_ptr = received_packet;
    // read packet
    while (LoRa.available()) {
      *received_packet_ptr++ = (char)LoRa.read();
    }
    *received_packet_ptr = '\0'; //terminator

    LoRa.read(); //flush 

    // print RSSI of packet
    Serial.print(received_packet);
        
    sprintf(print_pkt, "signal strength is: %d dbm", LoRa.packetRssi());
    Serial.print(print_pkt);
    Serial.println();
    to_print_cntr = 0;
    stat_flag = LED_on;
  }
  else
  {
   
    LoRa.read(); //flush 
    Serial.println("no data in int");

  }
}

void blink (uint32_t del, uint32_t times)
{
  for(uint32_t i = 0; i < times; i++)
  {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(del);
    digitalWrite(LED_BUILTIN, LOW);
    delay(del);
  }
}

/*************Deffinitions***************/
#define avr_dem   10
#define pin_CE        53
#define pin_RST       49  
#define pin_D0        2



/*************Initializations*************/
void setup() 
{
  pinMode(LED_BUILTIN, OUTPUT);
  
  //it's mandatory to configure pins before start
  LoRa.setPins(pin_CE, pin_RST, pin_D0);

  Serial.begin(9600);
  while (!Serial);

  Serial.println("LoRa receiver");


 while(!LoRa.begin(433000000))
  {
    Serial.println("initialization FAILED");
    blink(50, 2);

    delay(500);
  }
  
  LoRa.setFrequency(433000000);
  LoRa.setSignalBandwidth(500000);
  LoRa.setSpreadingFactor(8);

  LoRa.setGain(6);
  LoRa.receive();
  LoRa.onReceive(rx_int);
  

  
  Serial.println("initialization SUCCESSFUL");
  blink(300, 2);

  delay(300);
}


void loop()
{
  delay(1);
  if(to_print_cntr++ > 5000)
  {
    to_print_cntr = 0;
    LoRa.read(); //flush 
    Serial.println("no data"); 
  }
  if(stat_flag == LED_on)
  {
    stat_flag = LED_off;
    blink(20, 5);
  }
}




