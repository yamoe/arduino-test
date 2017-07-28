/*
 * Smart Robot Car V3 
 * - Motor test code
 */

// 차량 운행 방향 상태 전역 변수.
int carDirection = 1; // 1 - forward, 2 - backward

int carSpeed = 200; // 최대 속도의  50 % for testing.

#define ENA   6
#define EN1   7
#define EN2   3

#define EN3   4
#define EN4   2
#define ENB   5


//////////////////////////////////
// Setup to run once
//
void setup()
{
    pinMode(ENA, OUTPUT);  // ENA
    pinMode(EN1, OUTPUT);  // EN1
    pinMode(EN2, OUTPUT);  // EN2

    pinMode(ENB, OUTPUT);  // ENB
    pinMode(EN3, OUTPUT);  // EN3
    pinMode(EN4, OUTPUT);  // EN4

    delay(3000);

    if( carDirection == 1 ) {  // forward
      digitalWrite(EN1, HIGH);
      digitalWrite(EN2, LOW);
      analogWrite(ENA, carSpeed);
        
      digitalWrite(EN3, HIGH);
      digitalWrite(EN4, LOW);
      analogWrite(ENB, carSpeed);
    } else if( carDirection == 2 ) {  // backward
      digitalWrite(EN1, LOW);
      digitalWrite(EN2, HIGH);
      analogWrite(ENA, carSpeed);
        
      digitalWrite(EN3, LOW);
      digitalWrite(EN4, HIGH);
      analogWrite(ENB, carSpeed);
    } else
       ;
    
    delay(2000);    

    // stop
    analogWrite(ENA, 0);
    analogWrite(ENB, 0);    
}

//////////////////////////////////
// main code to run repeatedly
//
void loop()
{

}
