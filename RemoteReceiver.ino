/**************private includes***********/
//#include <SPI.h>
#include "LoRa.h"
#include "stdio.h"
#include "stdint.h"
#include "string.h"
#include "pins_arduino.h"

/*************Deffinitions***************/
//LoRa
#define pin_CE        53
#define pin_RST       49  
#define pin_D0        2

//define some output

                      
#define pin_out1      28
#define pin_out2      26
#define pin_out3      24
#define pin_out4      22
#define pin_out5      21
#define pin_out6      20
#define pin_out7      19
#define pin_out8      18
#define pin_out9      17
#define pin_out10     16
#define pin_out11     15
#define pin_outANL    44


//declare outputs
#define motor1_l_u            pin_out1        //motor1 left up 
#define motor1_l_d            pin_out2        //motor1 left down
#define motor1_r_u            pin_out3        //motor1 right up 
#define motor1_r_d            pin_out4        //motor1 right down

#define motor2_l_u            pin_out5        //motor2 left up 
#define motor2_l_d            pin_out6        //motor2 left down
#define motor2_r_u            pin_out7        //motor2 right up 
#define motor2_r_d            pin_out8        //motor2 right down

#define motor_fork_up         pin_out9        //motor2 right up 
#define motor_fork_down       pin_out10       //motor2 right down

#define horn_pin              pin_out11       //horn

#define CHECK_IF(str)         strstr(received_packet, str)

/***************Variables****************/
int Xaxis = A0;    // select the input pin for the Xaxis Joystick
int Yaxis = A1;    // select the input pin for the Yazis Joystick
uint32_t XValue = 0, YValue = 0;    // analog values of axis accordingly 
char  Buff[20];
char  received_packet[50];
char print_pkt[100];
uint32_t to_print_cntr = 0;
uint32_t time_out = 0;

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
    char * packet_counter = received_packet;
    // read packet
    while (LoRa.available()) {
      *received_packet_ptr++ = (char)LoRa.read();
      
      //to prevent writing data out of defined space
      if(received_packet_ptr - packet_counter > sizeof(received_packet))
        break;  
    }
    received_packet_ptr = strchr(received_packet, '\n');

    if(received_packet_ptr != 0)
      *++received_packet_ptr = '\0'; //terminator


    LoRa.read(); //flush 

    // print RSSI of packet
    Serial.print(received_packet);
        
    sprintf(print_pkt, "signal strength is: %d dbm", LoRa.packetRssi());
    Serial.print(print_pkt);
    Serial.println();
    to_print_cntr = 0;
    stat_flag = LED_on;
    time_out = 0; // reset time_out to prevent stop the whole functions
    LoRa.receive(25);
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

static inline void set_motor1_forward(void)
{
  digitalWrite(motor1_l_u, HIGH);
  digitalWrite(motor1_r_d, HIGH);

  digitalWrite(motor1_l_d, LOW);
  digitalWrite(motor1_r_u, LOW);
}

static inline void set_motor2_forward(void)
{
  digitalWrite(motor2_l_d, HIGH);
  digitalWrite(motor2_r_u, HIGH);
  
  digitalWrite(motor2_r_d, LOW);
  digitalWrite(motor2_l_u, LOW);
}


static inline void set_motor1_backward(void)
{
  digitalWrite(motor1_l_d, HIGH);
  digitalWrite(motor1_r_u, HIGH);
  
  digitalWrite(motor1_r_d, LOW);
  digitalWrite(motor1_l_u, LOW);
}

static inline void set_motor2_backward(void)
{
  digitalWrite(motor2_l_u, HIGH);
  digitalWrite(motor2_r_d, HIGH);

  digitalWrite(motor2_l_d, LOW);
  digitalWrite(motor2_r_u, LOW);
}


static inline void set_fork_up(void)
{
  digitalWrite(motor_fork_up, LOW);
}

static inline void set_fork_down(void)
{
  digitalWrite(motor_fork_down, LOW);
}


static inline void reset_fork(void)
{
  digitalWrite(motor_fork_up, HIGH);
  digitalWrite(motor_fork_down, HIGH);
}

static inline void reset_motor1(void)
{
  digitalWrite(motor1_l_u, HIGH);
  digitalWrite(motor1_r_d, HIGH);

  digitalWrite(motor1_l_d, HIGH);
  digitalWrite(motor1_r_u, HIGH);
}

static inline void reset_motor2(void)
{
  digitalWrite(motor2_l_d, HIGH);
  digitalWrite(motor2_r_u, HIGH);
  
  digitalWrite(motor2_r_d, HIGH);
  digitalWrite(motor2_l_u, HIGH);
}

void reset_motors(void)
{
  // digitalWrite(motor2_l_d, HIGH);
  // digitalWrite(motor2_l_u, HIGH);
  // digitalWrite(motor2_r_d, HIGH);
  // digitalWrite(motor2_r_u, HIGH);

  // digitalWrite(motor1_l_d, HIGH);
  // digitalWrite(motor1_l_u, HIGH);
  // digitalWrite(motor1_r_d, HIGH);
  // digitalWrite(motor1_r_u, HIGH);

  digitalWrite(pin_out1, HIGH);
  digitalWrite(pin_out2, HIGH);
  digitalWrite(pin_out3, HIGH);
  digitalWrite(pin_out4, HIGH);
  digitalWrite(pin_out5, HIGH);
  digitalWrite(pin_out6, HIGH);
  digitalWrite(pin_out7, HIGH);
  digitalWrite(pin_out8, HIGH);
}


void move_forward(uint8_t analoge_value)
{
  digitalWrite(pin_out1, LOW); // activation coil enable
  analogWrite(pin_outANL, analoge_value);
  // set_motor1_forward();
  // set_motor2_forward();
}

void reset_move_forward(void)
{
  digitalWrite(pin_out1, HIGH); // activation coil disable
  analogWrite(pin_outANL, 0);
  // set_motor1_forward();
  // set_motor2_forward();
}

void move_backward(uint8_t analoge_value)
{
  digitalWrite(pin_out2, LOW); // safety pin activation
  digitalWrite(pin_out1, LOW); // activation coil enable
  analogWrite(pin_outANL, analoge_value);
  // set_motor1_backward();
  // set_motor2_backward();
}

void reset_move_backward(void)
{
  digitalWrite(pin_out2, HIGH); // safety pin deactivation
  digitalWrite(pin_out1, HIGH); // activation coil disable
  analogWrite(pin_outANL, 0);
  // set_motor1_backward();
  // set_motor2_backward();
}

void move_right(void)
{
  digitalWrite(pin_out3, LOW);
  // reset_motor2();
  // set_motor1_forward();
}

void reset_move_right(void)
{
  digitalWrite(pin_out3, HIGH);
}


void reset_move_left(void)
{
  digitalWrite(pin_out4, HIGH);
}


void move_clockwaise(void)
{
  digitalWrite(pin_out5, LOW);
  // set_motor1_forward();
  // set_motor2_backward();
}

void move_counter_clockwaise(void)
{
  digitalWrite(pin_out6, LOW);
  // set_motor1_backward();
  // set_motor2_forward();
}

void move_fork_up(void)
{
  digitalWrite(pin_out7, LOW);
  // set_fork_up();
}

void move_fork_down(void)
{
  digitalWrite(pin_out8, LOW);
  // set_fork_down();
}

void move_fork_stop(void)
{
  digitalWrite(pin_out7, HIGH);
  digitalWrite(pin_out8, HIGH);
  // reset_fork();
}

void honk_horn(void)
{
  digitalWrite(pin_out10, LOW);
  // digitalWrite(horn_pin, LOW);
} 

void honk_NOT_horn(void)
{
  digitalWrite(pin_out10, HIGH);
  // digitalWrite(horn_pin, HIGH);
} 

void move_stop_all(void)
{
  honk_NOT_horn();
  reset_motors();
  reset_fork();
} 







/*************Initializations*************/
void setup() 
{
  pinMode(LED_BUILTIN, OUTPUT);

  pinMode(pin_out1, OUTPUT);
  pinMode(pin_out2, OUTPUT);
  pinMode(pin_out3, OUTPUT);
  pinMode(pin_out4, OUTPUT);
  pinMode(pin_out5, OUTPUT);
  pinMode(pin_out6, OUTPUT);
  pinMode(pin_out7, OUTPUT);
  pinMode(pin_out8, OUTPUT);
  pinMode(pin_out9, OUTPUT);
  pinMode(pin_out10, OUTPUT);
  pinMode(pin_out11, OUTPUT);




  digitalWrite(pin_out1, HIGH);
  digitalWrite(pin_out2, HIGH);
  digitalWrite(pin_out3, HIGH);
  digitalWrite(pin_out4, HIGH);
  digitalWrite(pin_out5, HIGH);
  digitalWrite(pin_out6, HIGH);
  digitalWrite(pin_out7, HIGH);
  digitalWrite(pin_out8, HIGH);
  digitalWrite(pin_out9, HIGH);
  digitalWrite(pin_out10, HIGH);
  digitalWrite(pin_out11, HIGH);

  
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
  LoRa.setSignalBandwidth(100000);
  LoRa.setSpreadingFactor(8);

  LoRa.setGain(6);
  LoRa.receive(25);
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
  //stop every ongoing commands if no command received 
  if(time_out++ > 500)
  {
    time_out = 0;
    //Serial.println("_STOP");
    move_stop_all();
  }
  if(stat_flag == LED_on)
  {
    //LED status
    stat_flag = LED_off;
    blink(20, 2);
    //pars data and perform the corresponding command
    if(CHECK_IF("_FORWARD"))
    {
      Serial.println("_FORWARD");
      reset_move_forward();
    }
    else if(CHECK_IF("FORWARD"))
    {
      Serial.println("FORWARD");
      char *ptr = strchr(received_packet,'='); ptr++; // find numbers
      int value = 255 - (atoi(ptr) - 20) * 2; // to obtain 0 to 255
      move_forward(value);
    }
    
    if(CHECK_IF("_BACKWARD"))
    {
      Serial.println("_BACKWARD");
      reset_move_backward();
    }
    else if(CHECK_IF("BACKWARD"))
    {
      Serial.println("BACKWARD");
      char *ptr = strchr(received_packet,'='); ptr++; // find numbers
      int value = (atoi(ptr) - 127 + 20) * 2; // to obtain 0 to 255
      move_backward(value);
    }

    if(CHECK_IF("_UP"))
    {
      Serial.println("_UP");
      reset_fork();
    }
    else if(CHECK_IF("UP"))
    {
      Serial.println("UP");
      move_fork_up();
    }

    if(CHECK_IF("_DOWN"))
    {
      
      Serial.println("_DOWN");
      reset_fork();
    }
    else if(CHECK_IF("DOWN"))
    {
      Serial.println("DOWN");
      move_fork_down();
    }

    if(CHECK_IF("_RIGHT"))
    {
      Serial.println("_RIGHT");
      reset_move_right();
    }
    else if(CHECK_IF("RIGHT"))
    {
      Serial.println("RIGHT");
      move_right();
    }


    if(CHECK_IF("_LEFT"))
    {
      Serial.println("_LEFT");
      reset_move_left();
    }
    else if(CHECK_IF("LEFT"))
    {
      Serial.println("LEFT");
      move_left();
    }

    //horn
    if(CHECK_IF("_HORN"))
    {
      Serial.println("_HORN");
      honk_NOT_horn();
    }
    else if(CHECK_IF("HORN"))
    {
      Serial.println("HORN");
      honk_horn();
    }

    //clockwise rotation
    if(CHECK_IF("_ROT_CLW"))
    {
      Serial.println("_ROT_CLW");
      reset_motors();
    }
    else if(CHECK_IF("ROT_CLW"))
    {
      Serial.println("ROT_CLW");
      move_clockwaise();
    }

    //counter clockwise rotation
    if(CHECK_IF("_ROT_CCW"))
    {
      Serial.println("_ROT_CCW");
      reset_motors();
    }
    else if(CHECK_IF("ROT_CCW"))
    {
      Serial.println("ROT_CCW");
      move_counter_clockwaise();
    }

    //stop button
    if(CHECK_IF("_STOP"))
    {
      Serial.println("_STOP");
      move_stop_all();
    }
    else if(CHECK_IF("STOP"))
    {
      Serial.println("STOP");
      move_stop_all();
    }
  }
}






