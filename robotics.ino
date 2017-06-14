#include <Servo.h>
#include <math.h>
#include <Wire.h>

//*** BELOW CODE TAKEN FROM http://blog.solutions-cubed.com/sensing-color-with-the-arduino-and-the-tcs34725/
byte i2cWriteBuffer[10];                                          
byte i2cReadBuffer[10];

#define SensorAddressWrite 0x29 //
#define SensorAddressRead 0x29 // 
#define EnableAddress 0xa0 // register address + command bits
#define ATimeAddress 0xa1 // register address + command bits
#define WTimeAddress 0xa3 // register address + command bits
#define ConfigAddress 0xad // register address + command bits
#define ControlAddress 0xaf // register address + command bits
#define IDAddress 0xb2 // register address + command bits
#define ColorAddress 0xb4 // register address + command bits
void Writei2cRegisters(byte numberbytes, byte command)
{
    byte i = 0;

    Wire.beginTransmission(SensorAddressWrite);   // Send address with Write bit set
    Wire.write(command);                          // Send command, normally the register address 
    for (i=0;i<numberbytes;i++)                       // Send data 
      Wire.write(i2cWriteBuffer[i]);
    Wire.endTransmission();

    delayMicroseconds(100);      // allow some time for bus to settle      
}

byte Readi2cRegisters(int numberbytes, byte command)
{
   byte i = 0;

    Wire.beginTransmission(SensorAddressWrite);   // Write address of read to sensor
    Wire.write(command);
    Wire.endTransmission();

    delayMicroseconds(100);      // allow some time for bus to settle      

    Wire.requestFrom(SensorAddressRead,numberbytes);   // read data
    for(i=0;i<numberbytes;i++)
      i2cReadBuffer[i] = Wire.read();
    Wire.endTransmission();   

    delayMicroseconds(100);      // allow some time for bus to settle      
}  

void init_TCS34725(void)
{
  i2cWriteBuffer[0] = 0x10;
  Writei2cRegisters(1,ATimeAddress);    // RGBC timing is 256 - contents x 2.4mS =  
  i2cWriteBuffer[0] = 0x00;
  Writei2cRegisters(1,ConfigAddress);   // Can be used to change the wait time
  i2cWriteBuffer[0] = 0x00;
  Writei2cRegisters(1,ControlAddress);  // RGBC gain control
  i2cWriteBuffer[0] = 0x03;
  Writei2cRegisters(1,EnableAddress);    // enable ADs and oscillator for sensor  
}

void get_TCS34725ID(void)
{
  Readi2cRegisters(1,IDAddress);
  if (i2cReadBuffer[0] = 0x44)
    Serial.println("TCS34725 is present");    
  else
    Serial.println("TCS34725 not responding");    
}

// ******** END OF COPIED BLOCK   **************************************
char get_Colors(void)
{
  unsigned int clear_color = 0;
  unsigned int red_color = 0;
  unsigned int green_color = 0;
  unsigned int blue_color = 0;

  Readi2cRegisters(8,ColorAddress);
  clear_color = (unsigned int)(i2cReadBuffer[1]<<8) + (unsigned int)i2cReadBuffer[0];
  red_color = (unsigned int)(i2cReadBuffer[3]<<8) + (unsigned int)i2cReadBuffer[2];
  green_color = (unsigned int)(i2cReadBuffer[5]<<8) + (unsigned int)i2cReadBuffer[4];
  blue_color = (unsigned int)(i2cReadBuffer[7]<<8) + (unsigned int)i2cReadBuffer[6];

  // send register values to the serial monitor 


 // Basic RGB color differentiation can be accomplished by comparing the values and the largest reading will be 
 // the prominent color
  if((red_color>blue_color) && (red_color>green_color))             // if red value is dominant in RGB values, the object is red.
    return 'r';
  else if((green_color>blue_color) && (green_color>red_color))   // if green value is dominant in RGB values, the object is green.
   return 'g';
  else if((blue_color>red_color) && (blue_color>green_color))  //if blue value is dominant in RGB values, the object is blue.
    return 'b';
  else return 'e';            //else there is a error, returns e
}  

Servo right_keeper;
Servo left_keeper;
Servo goal_keeper;
Servo box;
int trigPin1 = 13; //mid bottom
int echoPin1 = 22; //mid bottom 
int trigPin2 = 9; // mid top 
int echoPin2 = 10; // mid top
int trigPin3 = 8; //right
int echoPin3 = 23; //right
int echoPin4 = 2; //left 
int trigPin4 = 3; //left
int back_left = 50; 
int forward_left = 51;
int back_right = 52; 
int enable_right = 12;
int enable_left = 11;
int forward_right = 53;
long timer;
bool checkflag = false;
long bottom_mid;
long top_mid;
long right_value;
long left_value;
long distance;

void setup(){
  pinMode(back_left, OUTPUT);         //we sets up pins.
  pinMode(forward_left , OUTPUT);
    pinMode(enable_left, OUTPUT);
  pinMode(enable_right, OUTPUT);
  pinMode(back_right,OUTPUT);
  pinMode(forward_right,OUTPUT);
  pinMode(trigPin1, OUTPUT); 
  pinMode(echoPin1,INPUT); 
  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2, INPUT);
  pinMode(trigPin3,OUTPUT);
  pinMode(echoPin3, INPUT);
  pinMode(trigPin4, OUTPUT);
  pinMode(echoPin4,INPUT);
  box.attach(7);
  goal_keeper.attach(4);  
  right_keeper.attach(5);
  left_keeper.attach(6);
  goal_keeper.write(15);  
  right_keeper.write(90);
  left_keeper.write(90);
  box.write(90);
   Wire.begin();
  Serial.begin(9600);  // start serial for output
  init_TCS34725();
  get_TCS34725ID();  
  Serial.begin(9600); 
}

int servo_right = 90;     //angles of start location
int servo_left = 90;        
int servo_mid = 10;
int servo_kapak = 90;


void grap(){                //compresses object by iron rods.
  for(; servo_right > 50 ; servo_right--, servo_left++){
    delay(20);
     right_keeper.write(servo_right);
     left_keeper.write(servo_left);
    }
}

void reset(){              //opens compressed rods.
  for(; servo_right <90 ; servo_right++, servo_left--){
    delay(20);
     right_keeper.write(servo_right);
     left_keeper.write(servo_left);
    }
}

void down(){                //makes down the system
   for(;servo_mid < 180  ; servo_mid++ ){ 
       delay(20);
       goal_keeper.write(servo_mid);
    }  
}

void up(){            //lifts the object
  for(;servo_mid > 10 ; servo_mid-- ){
       delay(20);
       goal_keeper.write(servo_mid);
    }  
}
long olc(int trigPin,int echoPin){
   digitalWrite(trigPin, LOW); 
  delayMicroseconds(5);
  digitalWrite(trigPin, HIGH); 
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);  
  timer = pulseIn(echoPin, HIGH); 
  distance= timer /29.1/2;     
  return distance;
}

bool is_wall(){
  
 bottom_mid = olc(trigPin1,echoPin1);
 top_mid = olc(trigPin2,echoPin2);
 if(bottom_mid< 20 && top_mid < 20){
 if (abs(bottom_mid-top_mid)<7)
    return true;
 }
 return false;
}

bool is_object(){
  bottom_mid = olc(trigPin1,echoPin1);
  if(bottom_mid < 50){	//if bottom_mid ultrasonic sensor measures smaller than 50cm, return true(inform that there is an object)
    return true;
  }
  return false;
}

//In order to turn right, right motors move backward and left motors move forward
void turn_right(){
      analogWrite(enable_right, 200); //it adjusts the velocity of the motors in pwm
      analogWrite(enable_left, 200);
      digitalWrite(forward_right,HIGH);  //corresponding pin which is used for right motors forward action, is adjested high boolean value
      digitalWrite(back_right, LOW);    //corresponding pin which is used for right motors backward action, is adjested low boolean value
      digitalWrite(forward_left, LOW);  //corresponding pin which is used for left motors forward action, is adjested low boolean value
       digitalWrite(back_left, HIGH);  //corresponding pin which is used for left motors backward action, is adjested high boolean value
 
      
}

//In order to turn left, left motors move backwards and right motors move forward
void turn_left(){
       analogWrite(enable_right, 200); //it adjusts the velocity of the motors in pwm
      analogWrite(enable_left, 200);
      digitalWrite(forward_right,LOW); //corresponding pin which is used for right motors forward action, is adjested low boolean value
      digitalWrite(back_right, HIGH);  //corresponding pin which is used for right motors backward action, is adjested high boolean value
      digitalWrite(forward_left, HIGH);  //corresponding pin which is used for left motors forward action, is adjested high boolean value
       digitalWrite(back_left, LOW);  //corresponding pin which is used for left motors backward action, is adjested low boolean value
  }

//In order to move forward both right and left motors move forward 
void forward(){
      analogWrite(enable_right, 70);  
      analogWrite(enable_left, 70);
      digitalWrite(forward_right,LOW);  //corresponding pin which is used for right motors forward action, is adjested low boolean value
      digitalWrite(back_right, HIGH);   //corresponding pin which is used for right motors back action, is adjested high boolean value
      digitalWrite(forward_left, LOW);   //corresponding pin which is used for left motors forward action, is adjested low boolean value
       digitalWrite(back_left, HIGH);    //corresponding pin which is used for left motors backward action, is adjested high boolean value
  }


//In order to stop robot, all pins are given as low value
void stop(){
  analogWrite(enable_right, 0); 
      analogWrite(enable_left, 0);
      digitalWrite(forward_right,LOW);   //corresponding pin which is used for right motors forward action, is adjested low boolean value
      digitalWrite(back_right, LOW);    //corresponding pin which is used for right motors backward action, is adjested low boolean value
      digitalWrite(forward_left, LOW);   //corresponding pin which is used for left motors forward action, is adjested low boolean value
       digitalWrite(back_left, LOW);    //corresponding pin which is used for left motors backward action, is adjested low boolean value
  }


//In order to turn back robot turns right till getting the desired position
void turn_back(){
    stop();
    delay(200);   //First robot stops for 0.2 seconds
    turn_right();
    delay(750);   //Then it turns right for 0.75 seconds
    stop();
    delay(1000); //After turning back it waits one more second
   
  }
  
void grap_all(){          
  if(is_wall()){        // if there is a wall in front of robot,it turns back
    turn_back();
    return;
  }
     down();              //else makes down the gripper 
    delay(1000);
      grap();         //grabs the object

    delay(1000);
   
     up();          //lifts the object
    delay(1000);
      reset();        //leaves the object to bucket
    delay(1000);
     for(int k=0; k < 3;  k++){             //color determination runs three times to get correct value. 
      
      if(get_Colors() == 'r'){          //if object is red
            servo_kapak = 16;
            box.write(servo_kapak);       //servo turns ninety degrees in counter clockwise
            delay(1000);
            servo_kapak = 96;
            box.write(servo_kapak);
            
          }else if(get_Colors() == 'b'){    // else if object is blue
            servo_kapak = 176;
            box.write(servo_kapak);        //servo turns ninety degrees in counter clockwise
            delay(1000);
            servo_kapak = 96;
            box.write(servo_kapak);
          }
          delay(1000);
          
    }
    
}



   

  

 


void check_left(){
  Serial.print("checkleft\n");
  bool flag=true;
  bool is_seen = false;
  int i=0;
  stop();
  delay(1000);
  for(int i = 0; i < 20 ; i++){
    turn_left();
    delay(50);
    stop();
    delay(100);
    if(is_object()){	//if an object closer than 50 cm
      Serial.print("left object");
      flag=false;
       turn_left();		//call turn_left func to rotate to left
      delay(100);
      stop();
      while( 20 < olc(trigPin1,echoPin1)){	//While bottom-mid ultrasonic sensor measures shorter than 20cm
      forward();	//Rotating to left is enough because middle sensor can see the object now. Go forward. 
       if(is_wall()){	//Check is it a wall. If it is,
        turn_back();	//call turn_back func to avoid having a crash
        is_seen = true;	//make is_seen true
        break;	//finish the while loop
        }
      Serial.println("gotoobj");	//If it is not a wall, inform user "I go to the object"
      stop();						//call stop func to stop robot. Because grappler operation will start.
      }
      if(is_seen){					
        break;
        }
       if(is_wall()){	//If it is wall,
        turn_back();	//call turn_back func to avoid having a crash
        break;			//Break the for loop
        }
       
      grap_all();	//call grap_all func for grappler object 
      break;
    }
  }
}

void check_right(){
  Serial.print("checkright\n");
  bool flag=true;
  int i=0;
  bool is_seen = false;
  stop();
  delay(1000);
  for(int i= 0 ; i < 20 ; i++){		
    turn_right();
    delay(50);
    stop();
    delay(100);
    if(is_object()){	//if an object closer than 50 cm
      Serial.print("right object");
      flag=false;
       turn_right();	//Rotate right 
      delay(100);
      stop();
      while( 20 < olc(trigPin1,echoPin1)){	//If bottom-mid sensor measures shorter than 20cm,
            forward();	//Go forward to object
       if(is_wall()){	//If it is a wall,
         is_seen = true;//make is_seen true
         turn_back();	//call turn_back func to avoid having crash
         break;			//break loop
        }
      stop();	//If it is not a wall, call stop func to stop robot. Because grappler operation will start.
      }
       if(is_seen){	//If it was wall, break loop
        break;
        }
       if(is_wall()){	
			turn_back();	//call turn_back func to avoid having crash
			break;			//break loop
        }

      grap_all();	//grappler operation performs
      break;		//break the loop
    }
  }
}

bool control(){
  int counter = 0;
  
  }



void loop()
{
 bottom_mid = olc(trigPin1,echoPin1);
 top_mid = olc(trigPin2,echoPin2);
 right_value = olc(trigPin3,echoPin3);
 left_value = olc(trigPin4,echoPin4);
 
    if (bottom_mid<20 && bottom_mid>7){   //if the middle low sensor measure between 20 and 7 there should be something
      int counter=0;
      stop();   //it stops for 0.3 second
      delay(300);
      while(is_object()){  //sometimes ultrasonic sensors give wrong results in order to avoid this situation robot checks 5 times if it is object or not
        counter++;
        if(counter==5){
          
         if(is_wall()){ //if it is wall it turns back
          turn_back();
          break;
          }
          grap_all();  //otherwise it grabs the object
          counter = 0;
          break;
          
        }
      }
      
    }

  else if (left_value < 40 && left_value > 7){ //if left sensor measures between 40 and 7 it checks the left
    check_left();
  }
   else if(right_value < 40 && right_value > 7){ //if left sensor measures between 40 and 7 it checks the right
    check_right();
   }
   else
    forward();  //if none of these sensors measure desired values robot moves forward untill see something.
}  


