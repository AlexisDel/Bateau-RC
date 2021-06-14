//Bibliothèques:

#include <Wire.h> //I2C
#include <SoftwareSerial.h> //BLE
#include <Servo.h> //ServoMoteur


//Constantes:

#define US_FRONT 7
#define US_REAR 6

#define MOTOR_LEFT  0
#define MOTOR_RIGHT 1

#define SERVO_FRONT 5

#define BLE_RX 2
#define BLE_TX 3

#define I2C_MOTORDRIVER_ADDR            0x14
#define I2C_MOTORDRIVER_CMD_STOP        0x01
#define I2C_MOTORDRIVER_CMD_FORWARD     0x02
#define I2C_MOTORDRIVER_CMD_REVERSE     0x03

#define SOUND_SPEED 0.034029
#define DISTANCE_AVOID 50


//Objets:

class Ultrasonic //Capteur Ultrason
{
  private:
    //Varibales :
    int pin;
  public:
    //Fonctions :
    Ultrasonic(int select_pin);
    int Measure();
};

Ultrasonic::Ultrasonic(int select_pin) //Constructeur
{
  pin = select_pin;
}

int Ultrasonic::Measure() // Mesure de la distance
{
  //Trigger
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);
  delayMicroseconds(2);
  digitalWrite(pin, HIGH);
  delayMicroseconds(10);
  digitalWrite(pin,LOW);

  //Mesure
  pinMode(pin,INPUT);
  return ((pulseIn(pin,HIGH)*SOUND_SPEED)/2);
}


class Motor //Moteur
{
  private:
    //Varibales
    int channel;
  public:
    //Fonctions
    Motor(int set_channel);
    stop();
    run(int speed);
};

Motor::Motor(int set_channel) //Constructeur
{
  channel = set_channel;
}

Motor::stop() //Arrêt
{
  Wire.beginTransmission(I2C_MOTORDRIVER_ADDR);
  Wire.write(byte(I2C_MOTORDRIVER_CMD_STOP));
  Wire.write(byte(channel));
  Wire.endTransmission();
  delay(1);
}


Motor::run(int speed) //Activation
{
  if (speed > 255) speed = 255;
  else if (speed < -255) speed = -255;

  Wire.beginTransmission(I2C_MOTORDRIVER_ADDR);
  if (speed >=0) 
    {
      Wire.write(byte(I2C_MOTORDRIVER_CMD_FORWARD));
      Wire.write(byte(channel));
      Wire.write(byte(speed));
    }
    else 
    {
      Wire.write(byte(I2C_MOTORDRIVER_CMD_REVERSE));
      Wire.write(byte(channel));
      Wire.write(byte(-speed));
    }
  
  Wire.endTransmission();
  delay(1);
}


//Création des objets :

Ultrasonic ultrasonic_front(US_FRONT);
Ultrasonic ultrasonic_rear(US_REAR);
Motor motor_left(MOTOR_LEFT);
Motor motor_right(MOTOR_RIGHT);
Servo servo_front;
SoftwareSerial BLE(BLE_RX,BLE_TX);

//Variables: 

int range_front_0;
int range_front_45;
int range_front_90;
int range_front_135;
int range_front_180;
int range_rear;
char recvChar;
char last_recvChar;


//Fonctions (Anti-collision):

void avoid_forward_0()
{
  delay(200);
  motor_left.run(-255);
  motor_right.run(-255);
  delay(2000);
  motor_left.stop();
  motor_right.stop();
  delay(100);
  motor_left.run(255);
  motor_right.run(-255);
  delay(1500);
  motor_right.stop();
}

void avoid_forward_45()
{
  delay(200);
  motor_left.run(-255);
  motor_right.run(-255);
  delay(5000);
  motor_left.stop();
  motor_right.stop();
  delay(250);
  motor_left.run(255);
  motor_right.run(-255);
  delay(500);
  motor_left.stop();
}

void avoid_forward_90()
{
  delay(200);
  motor_left.run(-255);
  motor_right.run(-255);
  delay(2000);
  motor_left.stop();
  motor_right.stop();
}

void avoid_forward_135()
{
  delay(200);
  motor_left.run(-255);
  motor_right.run(-255);
  delay(2000);
  motor_left.stop();
  motor_right.stop();
  delay(100);
  motor_right.run(255);
  motor_left.run(-255);
  delay(1500);
  motor_right.stop();
}

void avoid_forward_180()
{
  delay(200);
  motor_left.run(-255);
  motor_right.run(-255);
  delay(2000);
  motor_left.stop();
  motor_right.stop();
  delay(100);
  motor_right.run(255);
  motor_left.run(-255);
  delay(500);
  motor_right.stop();
}

void avoid_backward()
{
  delay(200);
  motor_left.run(-255);
  motor_right.run(-255);
  delay(2000);
  motor_left.stop();
  motor_right.stop();
}


//Fonctions Contrôle:

void go_forward()
{
  motor_left.run(255);
  motor_right.run(255);
}

void idle()
{
  motor_left.stop();
  motor_right.stop();
}

void go_backward()
{
  motor_left.run(-255);
  motor_right.run(-255);
}

void go_left()
{
  motor_left.run(-255);
  motor_right.run(255);
}

void go_right()
{
  motor_left.run(255);
  motor_right.run(-255);
}


//Fonction Bluetooth:

void setupBleConnection()
{
  BLE.begin(9600);
  delay(100);
  BLE.print("AT+CLEAR");
  delay(100);
  BLE.print("AT+ROLE0");
  delay(100);
  BLE.print("AT+SAVE0");
  delay(100);
  BLE.print("AT+NAMEBATEAU");
}


//Tâches :

void task_SM0()
{
  range_front_0 = ultrasonic_front.Measure();
  delay(25);
  if (range_front_0 < DISTANCE_AVOID){
     avoid_forward_0();
  }
  servo_front.write(45);
  delay(200);
}

void task_SM45() 
{  
  range_front_45 = ultrasonic_front.Measure();
  delay(25);
  if (range_front_45 < DISTANCE_AVOID){
     avoid_forward_45();
  }
  servo_front.write(90);
  delay(200);
}

void task_SM90()
{
  range_front_90 = ultrasonic_front.Measure();
  delay(25);
  if (range_front_90 < 50){
     avoid_forward_90();
  }
  servo_front.write(135);
  delay(200);
}

void task_SM135()
{
  range_front_135 = ultrasonic_front.Measure();
  delay(25);
  if (range_front_135 < DISTANCE_AVOID){
     avoid_forward_135();
  }
  servo_front.write(180);
  delay(250);
}

void task_SM180()
{
  range_front_180 = ultrasonic_front.Measure();
  delay(25);
  if (range_front_180 < DISTANCE_AVOID){
     avoid_forward_180();
  }
  servo_front.write(0);
  delay(500);
}

void task_SMrear()
{
  range_rear = ultrasonic_rear.Measure();
  delay(25);
   if (range_rear < DISTANCE_AVOID){
     avoid_backward();
  }
 }

void task_BLE()
{
  if(BLE.available()){
    
    recvChar = BLE.read();
    if (recvChar != last_recvChar){
      
      if(recvChar == 'z'){
       last_recvChar = 'z';
       go_forward();
      }
  
      if(recvChar == 's'){
       last_recvChar = 's';
       idle();
      }

      if(recvChar == 'w'){
       last_recvChar = 'w';
       go_backward();
      }

      if(recvChar == 'q'){
       last_recvChar = 'q';
       go_left();
      }

      if(recvChar == 'd'){
       last_recvChar = 'd';
       go_right();
      }      
    }
  }  
}


void setup()
{
  //Moniteur Série
  Serial.begin(9600);
  
  //I2C
  Wire.begin();

  //BLE
  pinMode(BLE_RX, INPUT);
  pinMode(BLE_TX, OUTPUT);
  setupBleConnection();
  
  //Servo
  servo_front.attach(SERVO_FRONT);
  servo_front.write(0);
  
  delay(5000);
}

void loop()
{ 
  task_SM0();
  task_BLE();
  task_SM45();
  task_BLE();
  task_SM90();
  task_BLE();
  task_SM135();
  task_BLE();
  task_SM180();
  task_BLE();
  task_SMrear();
  task_BLE();
}
