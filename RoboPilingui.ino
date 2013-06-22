#include <Adafruit_VC0706.h>
#include <SoftwareSerial.h>
#include <Servo.h> 

#define MOTOR1_CTL1  6 // I1  
#define MOTOR1_CTL2  7 // I2  
#define MOTOR1_PWM   5 // EA  

#define MOTOR2_CTL1  11  // I3  
#define MOTOR2_CTL2  12 // I4  
#define MOTOR2_PWM   10 // EB

#define PITO 34
#define LUCES 35

#define AVANCE 0  
#define RETROCESO 1

#define CAMARA 3


char cadena[64];
int dato = 50;
int camaraDisponible = 0;
uint16_t jpglen;
int apagar = 0;

uint8_t *buffer;
int aux = 0;
int video = 0;

Adafruit_VC0706 cam = Adafruit_VC0706(&Serial1);

Servo servoHorizontal;
Servo servoVertical;

void setup()
{

  Serial.begin(115200);

  servoHorizontal.attach(43);
  servoVertical.attach(42);

  servoVertical.write(40);
  servoHorizontal.write(73);

  // Setup pins for motor 1  
  pinMode(MOTOR1_CTL1,OUTPUT);  
  pinMode(MOTOR1_CTL2,OUTPUT);  
  pinMode(MOTOR1_PWM,OUTPUT);  

  // Setup pins for motor 2  
  pinMode(MOTOR2_CTL1,OUTPUT);  
  pinMode(MOTOR2_CTL2,OUTPUT);  
  pinMode(MOTOR2_PWM,OUTPUT); 

  pinMode(PITO,OUTPUT);
  pinMode(LUCES,OUTPUT);
  pinMode(CAMARA,OUTPUT);


  digitalWrite(LUCES, LOW);
  digitalWrite(CAMARA, LOW);

}

void loop()
{   

  aux++;

  if(Serial.available()> 0)
  { 

    dato = (int) Serial.read();
    delay (10);

    switch (dato)  
    { 
    case 1:
      motorStart(1, AVANCE);  
      motorStart(2, AVANCE);
      dato = (int) Serial.read();
      delay (10); 
      setSpeed(1, dato*2); 
      dato = (int) Serial.read();
      delay (10);
      setSpeed(2, dato*2);
      break;

    case 2:
      motorStart(1, RETROCESO);  
      motorStart(2, RETROCESO);
      dato = (int) Serial.read();
      delay (10); 
      setSpeed(1, dato*2); 
      dato = (int) Serial.read();
      delay (10);
      setSpeed(2, dato*2);
      break;

    case 3:
      motorStart(1, AVANCE);  
      motorStart(2, RETROCESO);

      dato = (int) Serial.read();
      delay (10); 
      setSpeed(1, dato*2); 
      dato = (int) Serial.read();
      delay (10);
      setSpeed(2, dato*2);
      break;

    case 4:
      motorStart(1, RETROCESO);  
      motorStart(2, AVANCE);
      dato = (int) Serial.read();
      delay (10); 
      setSpeed(1, dato*2); 
      dato = (int) Serial.read();
      delay (10);
      setSpeed(2, dato*2);
      break;

    case 5:
      motorStart(1, AVANCE);  
      motorStop(2);
      dato = (int) Serial.read();
      delay (10); 
      setSpeed(1, dato*2);
      break;

    case 6:
      motorStart(1, RETROCESO);  
      motorStop(2);
      dato = (int) Serial.read();
      delay (10); 
      setSpeed(1, dato*2); 
      break;

    case 7:
      motorStart(2, AVANCE);  
      motorStop(1);
      dato = (int) Serial.read();
      delay (10); 
      setSpeed(2, dato*2); 
      break;

    case 8:
      motorStart(2, RETROCESO);  
      motorStop(1);
      dato = (int) Serial.read();
      delay (10); 
      setSpeed(2, dato*2); 
      break;

    case 9:
      motorStop(1);
      motorStop(2);
      break;


    case 11:
      digitalWrite(PITO,HIGH);
      break;

    case 12:
      digitalWrite(PITO,LOW);
      break;

    case 15:

      digitalWrite(LUCES,HIGH);

      break;

    case 16:

      digitalWrite(LUCES,LOW);

      break;


    case 20:
      dato = (int) Serial.read();
      delay (10); 
      servoHorizontal.write(dato);

      dato = (int) Serial.read();
      delay (10); 
      servoVertical.write(dato);
      break;

    case 30:

      /*
      sprintf(cadena, "TELE -s1 %d -s2 %d -s3 %d -s4 %d",(int) temp1, (int) temp2, luz, obstaculo);
       
       Serial.write((uint8_t*)cadena,64);
       */

      break;

    case 40:
      apagar = 1;

      break;

    case 41:
    
      digitalWrite(CAMARA, LOW);
      delay(500);
      digitalWrite(CAMARA, HIGH);
      delay(500);
      cam.begin();
      delay(2000);
      cam.setImageSize(VC0706_160x120);
      cam.begin();
      delay(2000);
      cam.setBaudMax();
      delay(100);


      video = 1;
      camaraDisponible = 0;

      break;

    case 42:


      digitalWrite(CAMARA, LOW);
      delay(500);
      digitalWrite(CAMARA, HIGH);
      delay(500);
      cam.begin();
      delay(2000);
      cam.setImageSize(VC0706_320x240);
      cam.begin();
      delay(2000);
      cam.setBaudMax();
      delay(100);

      video = 0;
      camaraDisponible = 1;
      
      break;


    case 65:

      if(camaraDisponible)
      {
        cam.takePicture();

        jpglen = cam.frameLength();

        sprintf(cadena,"PHOTO -len %d", jpglen);

        Serial.write((uint8_t*)cadena,64);

        while (jpglen > 0) {
          // read 32 bytes at a time;

          uint8_t bytesToRead = min(64, jpglen); // change 32 to 64 for a speedup but may not work with all setups!

          buffer = cam.readPicture(bytesToRead);

          Serial.write(buffer, bytesToRead);

          jpglen -= bytesToRead;
        }  

        cam.resumeVideo();

      }

      break;

    default:
      motorStop(1);
      motorStop(2);
      break;
    }

  }

  else if(aux > 1000)
  {

    aux = 0;

    if(video == 1)
    {

      cam.takePicture();

      jpglen = cam.frameLength();

      sprintf(cadena,"PHOTO -len %d", jpglen);

      Serial.write((uint8_t*)cadena,64);

      while (jpglen > 0) {
        // read 32 bytes at a time;

        uint8_t bytesToRead = min(64, jpglen); // change 32 to 64 for a speedup but may not work with all setups!

        buffer = cam.readPicture(bytesToRead);

        Serial.write(buffer, bytesToRead);

        jpglen -= bytesToRead;
      }  

      cam.resumeVideo();

    }


  } 
  
  else if(apagar)
  {
    
      video = 0;
      camaraDisponible = 0;
      apagar = 0;

      Serial1.flush();
      Serial1.end();

      digitalWrite(CAMARA, LOW);
   
  }

}

void setSpeed(char motor_num, char motor_speed)  
{  
  if (motor_num == 1)  
  {  
    analogWrite(MOTOR1_PWM, motor_speed);  
  }     
  else  
  {  
    analogWrite(MOTOR2_PWM, motor_speed);  
  }  
}  

void motorStart(char motor_num, byte direction)  
{  

  char pin_ctl1;  
  char pin_ctl2;  

  if (motor_num == 1)  
  {  
    pin_ctl1 = MOTOR1_CTL1;  
    pin_ctl2 = MOTOR1_CTL2;  
  }     
  else  
  {  
    pin_ctl1 = MOTOR2_CTL1;  
    pin_ctl2 = MOTOR2_CTL2;       
  }  

  switch (direction)  
  {  
  case AVANCE:  
    {  
      digitalWrite(pin_ctl1,LOW);  
      digitalWrite(pin_ctl2,HIGH);            
    }  
    break;   

  case RETROCESO:  
    {  
      digitalWrite(pin_ctl1,HIGH);  
      digitalWrite(pin_ctl2,LOW);            
    }  
    break;           
  }  
}  

void motorStop(char motor_num)  
{  
  setSpeed(motor_num, 0);  
  if (motor_num == 1)  
  {  
    digitalWrite(MOTOR1_CTL1,HIGH);  
    digitalWrite(MOTOR1_CTL2,HIGH);       
  }  
  else  
  {  
    digitalWrite(MOTOR2_CTL1,HIGH);  
    digitalWrite(MOTOR2_CTL2,HIGH);       
  }  
}  







































