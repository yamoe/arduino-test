/*
 * 
 * Smart Robot Car V3 
 * - Auto driving
 * - Obstacle avoidance(장애물 회피)
 */

//////////////////////////////////////////////////
// Ultrasonic sensor
// 
#define Sonic_TRIG    12      // Ultrasonic Trigger
#define Sonic_ECHO    13      // Ultrasonic Echo

#define MAX_DISTANCE  200

int cmFront = 0;
int cmF_old = 0;
int cmF_avg = 0;

//////////////////////////////////////////////////
// Infrared sensor(FC-51)
//////////////////////////////////////////////////
// Obstacle = LOW,  Nothing  = HIGH
//
#define pin_InfraR    8      // Right Infrared sensor
#define pin_InfraL    9      // Left Infrared sensor

int InfraL = HIGH;
int InfraR = HIGH;
int InfraL_Old = HIGH;
int InfraR_Old = HIGH;

int obstacle_cnt = 0;
int obstacle_try = 0;
long duration, cm, avg_cm;

///////////////////////////////////////////////////////////////
// Note:  ENA and ENB must be connected to PWD supported pins
//
#define ENA   6    // PWD
#define EN1   7
#define EN2   3

#define EN3   4
#define EN4   2
#define ENB   5    // PWD

////////////////////////////////////
// Car direction
//
#define CAR_DIR_FW  0   // forward
#define CAR_DIR_BK  1   // backward
#define CAR_DIR_LT  2   // left turn
#define CAR_DIR_RT  3   // right turn
#define CAR_DIR_ST  4   // stop

///////////////////////////////////
// Car Speed : 0 ~ 255
//
#define CAR_SPEED_DEFAULT 150

///////////////////////////////////
// Default direction and speed
//
int g_carDirection = CAR_DIR_ST;
int g_carSpeed_L; 
int g_carSpeed_R; 

////////////////////////////////////////////////
// Note : confirm HIGH/LOW for correct movement
//
void car_forward()
{
  digitalWrite(EN1, HIGH);
  digitalWrite(EN2, LOW);
  analogWrite(ENA, g_carSpeed_R);

  digitalWrite(EN3, HIGH);
  digitalWrite(EN4, LOW);
  analogWrite(ENB, g_carSpeed_L);
}

void car_backward()
{
  digitalWrite(EN1, LOW);
  digitalWrite(EN2, HIGH);
  analogWrite(ENA, g_carSpeed_R);

  digitalWrite(EN3, LOW);
  digitalWrite(EN4, HIGH);
  analogWrite(ENB, g_carSpeed_L);
}

void car_left()
{
  digitalWrite(EN1, LOW);
  digitalWrite(EN2, HIGH);
  analogWrite(ENA, g_carSpeed_R);

  digitalWrite(EN3, HIGH);
  digitalWrite(EN4, LOW);
  analogWrite(ENB, g_carSpeed_L);
}

void car_right()
{
  digitalWrite(EN1, HIGH);
  digitalWrite(EN2, LOW);
  analogWrite(ENA, g_carSpeed_R);

  digitalWrite(EN3, LOW);
  digitalWrite(EN4, HIGH);
  analogWrite(ENB, g_carSpeed_L);
}

void car_stop()
{
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
}

////////////////////////////
// Execute car moving
//
void update_Car()
{
  switch ( g_carDirection ) {
    case CAR_DIR_FW:
        car_forward();
        break;
    case CAR_DIR_BK:
        car_backward();
        break;
    case CAR_DIR_LT:
        car_left();
        break;
    case CAR_DIR_RT:
        car_right();
        break;
    case CAR_DIR_ST:
        car_stop();
        break;
    default : 
        ;
  }
  return;
}

void setup() {
  
  // for debug
  Serial.begin(9600);     
  delay(100);
  Serial.println("Auto Driving(Obstacle avoidance) >> Start");
  
  //
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(EN1, OUTPUT);
  pinMode(EN2, OUTPUT);
  pinMode(EN3, OUTPUT);
  pinMode(EN4, OUTPUT);
  
  pinMode(Sonic_TRIG,OUTPUT);  
  pinMode(Sonic_ECHO,INPUT);    
  pinMode(pin_InfraR,INPUT);
  pinMode(pin_InfraL,INPUT);
  
}

void loop() {

  //    
  GetDistance_UltraSonic();
  DetectObstacle_InfraRed();
  
  g_carSpeed_L = CAR_SPEED_DEFAULT;
  g_carSpeed_R = CAR_SPEED_DEFAULT;

  Serial.print("cmFront -> ");
  Serial.println(cmFront);
    
  if ( (InfraL == LOW)||(InfraR == LOW) )     //좌측이나 우측에 장애물이 있다면   
  {

    Serial.println("Left or Right : obstacle");
    //Serial.print("cmFront -> ");
    //Serial.println(cmFront);
    if( cmFront < 30 )        //전방센서 근접에 장애물이 있다면 약간 후진
    {
      Serial.println("Front(LR) : obstacle -> BACK");
      g_carDirection = CAR_DIR_BK;
      update_Car();
      delay(100);
    } 
    else {
      if( InfraL < InfraR )        //좌측에 장애물이 있다면 우측으로 회피
      {        
        Serial.println("LEFT : obstacle -> Turn Right");
        g_carSpeed_R = CAR_SPEED_DEFAULT;
        g_carDirection = CAR_DIR_RT;  //우측으로 선회 회피
        update_Car(); 
        delay(10);
        obstacle_cnt += 1;      
      } 
      else if( InfraL > InfraR ){   //우측에 장애물이 있다면 좌측으로 회피
        Serial.println("RIGHT : obstacle -> Turn Left");
        g_carSpeed_L = CAR_SPEED_DEFAULT;
        g_carDirection = CAR_DIR_LT;  //좌측으로 선회 회피
        update_Car(); 
        delay(10);  
        obstacle_cnt += 1;
      } 
      else {
        Serial.println("No : obstacle - ERROR?");
        g_carDirection = CAR_DIR_FW;
        update_Car(); 
        delay(100);  
      }
    }
    
    //g_carSpeed_L = CAR_SPEED_DEFAULT;
    //g_carSpeed_R = CAR_SPEED_DEFAULT;
  
    //좌우 측면에 장애물이 계속 감지 된다면 코너에 있는 것으로 간주
    if (obstacle_cnt>5) {
                      
      do{
        
        if( InfraL < InfraR ) {      //좌측에 장애물이 있다면
          g_carDirection = CAR_DIR_RT;  //우측으로 회피
        } else {              //우측에 장애물이 있다면
          g_carDirection = CAR_DIR_LT;  //좌측으로 회피
        }
        update_Car(); 
        delay(100);
        
        GetDistance_UltraSonic();   //전방(Ultrasonice) 장애물 감지
        
        if ( cmFront < 15) { //전방에 장애물이 있으면 후진.
          g_carDirection = CAR_DIR_BK;
          update_Car(); 
          delay(100);
        }
                
        if ( cmFront > 40 ){  //트인공간이 있으면 탈출
          g_carDirection = CAR_DIR_FW;
          update_Car(); 
          delay(100);
          obstacle_cnt = 0;
          break;
        }       
        
        if (obstacle_try > 30) {    //탈출하지 못하면 약간 후진.
          g_carDirection = CAR_DIR_BK;
          update_Car(); 
          delay(50);
          obstacle_cnt = 0;
          obstacle_try = 0;
          break;
        }
        obstacle_try+=1;

        Serial.println("obstacle count : Corner??");
        
      }while(true);
          
    }     

  } 
  else {        //좌우 측면에 장애물이 없다면

    obstacle_try = 0;
    obstacle_cnt = 0;
    g_carSpeed_L = CAR_SPEED_DEFAULT;
    g_carSpeed_R = CAR_SPEED_DEFAULT;
    
    if( cmFront >= 30 ) {
      
      g_carDirection = CAR_DIR_FW;    

//      if( cmFront >= 70 ) { //장애물이 70cm 밖에 있으면 직진
//        g_carSpeed_L = CAR_SPEED_DEFAULT; 
//        g_carSpeed_R = CAR_SPEED_DEFAULT;
//      }     
      update_Car();    

      Serial.println("Nothing at front -> go FW");
    } 
    else if ( cmFront < 30 ) {
        Serial.println("Front 30 obstacle -> turn RIGHT(15<>30)");
        g_carDirection = CAR_DIR_RT;  //우측으로 선회 회피
        update_Car(); 
        delay(10);
    }
    else if ( cmFront < 15 ) {  //장애물이 15cm 이내이면 후진
      Serial.println("Front 15 obstacle -> BACK");
      g_carDirection = CAR_DIR_BK;      
      update_Car(); 
      delay(100);       
    } 
    else {
      // nothing
      ;
    } 
    
  } // no Left/Right obstacle

  //
  // For debug : possible to check out direction with delay()
  //
  //
  
  delay(2000);
  
}  // loop

long microsecondsToCentimeters(long microseconds)
{
  return microseconds/29/2;
} 

void GetDistance_UltraSonic(){

  //HC-SR04 Ultrasonic sensor(0~400cm)
  digitalWrite(Sonic_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(Sonic_TRIG,LOW); 
  duration = pulseIn(Sonic_ECHO, HIGH);  
  cmFront = microsecondsToCentimeters(duration);
  
  if ( cmFront > MAX_DISTANCE ) { cmFront = MAX_DISTANCE; }
  
  cmF_avg = (cmF_old + cmFront) / 2;
  cmF_old = cmFront;
  cmFront = cmF_avg;
  delay(10);
  
  Serial.println( cmFront );
    
}


void DetectObstacle_InfraRed()
{
  //FC-51 Infrared sensor
  InfraL = digitalRead( pin_InfraL );
  InfraR = digitalRead( pin_InfraR );

  Serial.print(InfraL);
  Serial.print(" -- I -- ");
  Serial.println(InfraR);
}

