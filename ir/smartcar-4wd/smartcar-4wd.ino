#include <IRremoteModule.h>

/*
 - : 속도 감소
 + : 속도 증가
 2 : 전진
 4 : 좌회전, 5 : 정지, 6 : 우회전
 8 : 후진
 */

#define CAR_DIR_FW  0   // forward
#define CAR_DIR_BK  1   // backward
#define CAR_DIR_LT  2   // left turn
#define CAR_DIR_RT  3   // right turn
#define CAR_DIR_ST  4   // stop

#define ENA   6    // PWD
#define EN1   7
#define EN2   3
#define EN3   4
#define EN4   2
#define ENB   5    // PWD

int g_carDirection = CAR_DIR_ST;
int g_carSpeed = 153; // 60% of max speed for testing

const int irReceiverPin = A3;

IRrecv irrecv(irReceiverPin);
decode_results decodedSignal;

void init_IRreceiverModule()
{
  pinMode(irReceiverPin, INPUT);
  irrecv.enableIRIn();
}

struct IRvalueData
{
  String name;
  unsigned long value;
};

IRvalueData irData[21] = 
{
  {"0", 0xFF6897},
  {"1", 0xFF30CF},
  {"2", 0xFF18E7},
  {"3", 0xFF7A85},
  {"4", 0xFF10EF},
  {"5", 0xFF38C7},
  {"6", 0xFF5AA5},
  {"7", 0xFF42BD},
  {"8", 0xFF4AB5},
  {"9", 0xFF52AD},
  {"100+", 0xFF9867},
  {"200+", 0xFF804F},
  {"-", 0xFFE01F},
  {"+", 0xFFA857},
  {"EQ", 0xFF906F},
  {"<<", 0xFF22DD},
  {">>", 0xFF02FD},
  {">|", 0xFFC23D},
  {"CH-", 0xFFA25D},
  {"CH", 0xFF629D},
  {"CH+", 0xFFE21D},
};

String decode_IRvalue(unsigned long irValue)
{
  for (int i=0; i<21; i++) {
    if (irData[i].value == irValue) {
      return irData[i].name;
    }
  }
  Serial.println("Not Defined.");
  return String("key Value None");
}

void controllerByIRCommand(String& szIRCmd)
{
  if (szIRCmd == "2") { // forward
    g_carDirection = CAR_DIR_FW;
  } else if (szIRCmd == "5") { // stop
    g_carDirection = CAR_DIR_ST;
  } else if (szIRCmd == "8") { // backward
    g_carDirection = CAR_DIR_BK;
  } else if (szIRCmd == "4") { // left
    g_carDirection = CAR_DIR_LT;
  } else if (szIRCmd == "6") { // right
    g_carDirection = CAR_DIR_RT;
  } else if (szIRCmd == "+") { // speed up
    g_carSpeed += 10;
    if (g_carSpeed > 200) {
      g_carSpeed = 200;
    }
  } else if (szIRCmd == "+") { // speed down
     g_carSpeed -= 10;
     if (g_carSpeed < 0) {
      g_carSpeed = 0;
     }
  }
}

void update_IRreceiverModule()
{
  // this is true if a message has been received
  if (irrecv.decode(&decodedSignal) == true) {
    if (decodedSignal.bits > 0) {
      String szRecvCmd = decode_IRvalue(decodedSignal.value);
      controllerByIRCommand(szRecvCmd);
    }
    irrecv.resume(); // watch out for another message
  }
}

void init_car_controller_board()
{
  pinMode(ENA, OUTPUT);  // ENA
  pinMode(EN1, OUTPUT);  // EN1
  pinMode(EN2, OUTPUT);  // EN2

  pinMode(ENB, OUTPUT);  // ENB
  pinMode(EN3, OUTPUT);  // EN3
  pinMode(EN4, OUTPUT);  // EN4
}

void car_forward()
{
  digitalWrite(EN1, HIGH);
  digitalWrite(EN2, LOW);
  analogWrite(ENA, g_carSpeed);

  digitalWrite(EN3, HIGH);
  digitalWrite(EN4, LOW);
  analogWrite(ENB, g_carSpeed);
}

void car_backward()
{
  digitalWrite(EN1, LOW);
  digitalWrite(EN2, HIGH);
  analogWrite(ENA, g_carSpeed);

  digitalWrite(EN3, LOW);
  digitalWrite(EN4, HIGH);
  analogWrite(ENB, g_carSpeed);
}

//void car_left()
void car_right()
{
  digitalWrite(EN1, LOW);
  digitalWrite(EN2, HIGH);
  analogWrite(ENA, g_carSpeed);

  digitalWrite(EN3, HIGH);
  digitalWrite(EN4, LOW);
  analogWrite(ENB, g_carSpeed);
}

//void car_right()
void car_left()
{
  digitalWrite(EN1, HIGH);
  digitalWrite(EN2, LOW);
  analogWrite(ENA, g_carSpeed);

  digitalWrite(EN3, LOW);
  digitalWrite(EN4, HIGH);
  analogWrite(ENB, g_carSpeed);
}

void car_stop()
{
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
}


void car_update()
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
  Serial.begin(9600);
  init_IRreceiverModule();
  init_car_controller_board();
}

void loop() {
  car_update();
  update_IRreceiverModule();
}


