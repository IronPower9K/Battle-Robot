//2022.11.16 기준
// **현재 남은 문제점**
// 1. 레이저 timeout 최소화 하기 and 언제 timeout뜨는지 찾아내기
// 2. turnspeed , 돌진 스피드 , 최대속도 돌진 거리값 최적화
// 3. 센서가 오류값을 표출시 특히, 레이저 센서 어떻게 반응 할 것인지
//    아마 하나라도 살아 있으면 그 하나만 가지고도 운영 할 수 있겠끔
//    거리 이내 전진, 거리 초과시 빙글빙글 돌게 하기 (오류값일 경우 거리 0으로 표기됨)
// 4. 초반 180도 모드일 때 180도가 나오는 timeout값 찾아보기

// 각종 라이브러리에서 불러오기 이거는 컴퓨터에 설치 해야됨
#include <Wire.h>
#include <VL53L1X.h>
#include "CytronMotorDriver.h"

#define TRIG2 11 //TRIG 핀 설정 (후방 초음파 2내는 핀)
#define ECHO2 10 //ECHO 핀 설정 (후방 초음파 받는 핀)
#define TRIG1 9 //TRIG 핀 설정 (전방 초음파 보내는 핀)
#define ECHO1 8 //ECHO 핀 설정 (전방 초음파 받는 핀)

// 1번 초음파 센서는 전방에 달거임 7CM초과 떨어진경우 후진하기
//2번 초음파 센서는 후방에 달거임 7CM초과 떨어진경우 전진하기

// 전방 초음파 센서갑이 갑자기 7로 뜨는 상황 발생 --조치중--





// Configure the motor driver. 모터1 좌측 모터2 우측
CytronMD motor1(PWM_DIR, 5, 4);  // PWM 1 = Pin 5, DIR 1 = Pin 4.
CytronMD motor2(PWM_DIR, 6, 7); // PWM 2 = Pin 6, DIR 2 = Pin 7.

const uint8_t sensorCount = 2;

// The Arduino pin connected to the XSHUT pin of each sensor.
const uint8_t xshutPins[sensorCount] = { 2, 3};

// 0번 좌측 1번 우측

VL53L1X sensors[sensorCount];

 int timecount = 160; 
 int timecount2 = timecount/2;




void setup()
{ 
  Serial.begin(115200);
  pinMode(TRIG1, OUTPUT);
  pinMode(ECHO1, INPUT);
  pinMode(TRIG2, OUTPUT);
  pinMode(ECHO2, INPUT);
  
  while (!Serial) {}
  Serial.begin(115200);
  Wire.begin();
  Wire.setClock(400000); // use 400 kHz I2C

  for (uint8_t i = 0; i < sensorCount; i++)
  {
    pinMode(xshutPins[i], OUTPUT);
    digitalWrite(xshutPins[i], LOW);
  }

  for (uint8_t i = 0; i < sensorCount; i++)
  {
    // Stop driving this sensor's XSHUT low. This should allow the carrier
    // board to pull it high. (We do NOT want to drive XSHUT high since it is
    // not level shifted.) Then wait a bit for the sensor to start up.
    pinMode(xshutPins[i], INPUT);
    delay(10);

    bool setDistanceMode(VL53L1X::Medium);
    
    sensors[i].setTimeout(5000);
    if (!sensors[i].init())
    {
      Serial.print("Failed to detect and initialize sensor ");
      Serial.println(i);
      while (1);
    }

    // Each sensor must have its address changed to a unique value other than
    // the default of 0x29 (except for the last one, which could be left at
    // the default). To make it simple, we'll just count up from 0x2A.
    sensors[i].setAddress(0x2A + i);

    sensors[i].startContinuous(50);

   
    
  }

  
   
}

// 코드 우선순위 낭떠러지 >> 180도 모드 >> 360도 모드

void loop()
{
  int count;
  for (uint8_t i = 0; i < sensorCount; i++)
  {
    Serial.print(sensors[i].read());
    if (sensors[i].timeoutOccurred()) { Serial.print(" TIMEOUT"); }
    Serial.print('\t');
  }
  Serial.println();
  

   long duration1, distance1, duration2, distance2;
   
 
//  motor1.setSpeed(0);
//  motor2.setSpeed(0);

  
  // 전방 초음파 센서값 읽고 쓰기
  digitalWrite(TRIG1, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG1, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG1, LOW);
  


  // 전방 초음파 센서값을 가지고 cm단위로 환산
  duration1 = pulseIn (ECHO1, HIGH);
  distance1 = duration1 * 17 / 1000;

  // 후방 초음파 센서값 읽고 쓰기
  digitalWrite(TRIG2, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG2, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG2, LOW);

  // 후방 초음파 센서값을 가지고 cm단위로 환산
  duration2 = pulseIn (ECHO2, HIGH);
  distance2 = duration2 * 17 / 1000;

  
  // 각종 센서값 출력
  Serial.print("\nDIstance1 : ");
  Serial.print(distance1);
  Serial.println(" Cm");
  
  Serial.print(" DIstance2 : ");
  Serial.print(distance2);
  Serial.println(" Cm \n");

  Serial.print(timecount);
  Serial.println("\n");

  
 

  
  
  // 낭떠러지를 마주했을 때 어떻게 할지
  if(distance1>7)
  {
    Serial.println("정지!정지! 후진");
    count=500;
      //모터출력 최대로 후진
      // 완전하게 뒤로 이동할때 까지 계속 후진하기
    
        //motor1.setSpeed(-255)
       while(count>=0){
        motor1.setSpeed(-254);//최대출력 뒤로 후진
        motor2.setSpeed(-254);//최대출력 뒤로 후진
        delay(1);
        count--;
       }
  

       
  }
  else if(distance2>7)
  {
    Serial.println("정지!정지! 전진");
      count=500;
       //motor2.setSpeed(255)
       while(count>=0){
        motor1.setSpeed(254);//최대출력 뒤로 후진
        motor2.setSpeed(254);//최대출력 뒤로 후진
        delay(1);
        count--;
       }
    
  }

  else
  {
    // 레이저 센서에 필요한 각종 값
    // lim = 인식거리 lim2 = 돌진거리 
    int lim = 2100;
    int lim2 = 300;

    // turnspeed = 좌회전 우회전 속도 findspeed = 찾을 때 속도
    int turnspeed=60;
    int findspeed=50;
  
  //diff = 두개의 레이저 센서 절댓값
  
 
   // 최초 시작시 180도 범위 내에서만 찾다가 못찾으면 360도 범위에서 찾기 (1번 왕복) -> 모든 찾는 방향은 시계방향
   // 단, 180도 범위내에서 찾은 경우 바로 360도 범위내에서 찾는 코드로 변경(else 안에 있는 코드로 변경됨)
    if(timecount > timecount2 && sensors[0].read()>=lim && sensors[1].read()>=lim)
    {
        Serial.println("시작 우회전");
        motor1.setSpeed(findspeed);
        motor2.setSpeed(-findspeed);
        delay(1);
        timecount--;
        
    }
    else if(timecount>0 && sensors[0].read()>=lim && sensors[1].read()>=lim)
    {
        Serial.println("시작 좌회전");
        motor1.setSpeed(-findspeed);
        motor2.setSpeed(findspeed);
        delay(1);
        timecount--;

        
       
    }
  // 1회 왕복 후 180도 범위내에서 못찾은 경우 or 180도 범위내에서 한번이라도 찾은 경우
    else
    {
        timecount=-1;

        
        
        
        int dif = sensors[0].read() - sensors[1].read();
        int diff = abs(dif);
        Serial.print(diff);
        Serial.println("\n");

       
        
       if(sensors[0].read()>=lim && sensors[1].read()>=lim)
      {
        Serial.println("찾는중");
        
        motor1.setSpeed(findspeed);
        motor2.setSpeed(-findspeed);
        delay(1);
        
        
      }
    // 오른쪽 센서 물체 인식, 왼쪽 센서 오른쪽 센서와 500mm이상 차이날 경우 성립(오작동을 방지하기 위함)
      
       
       
        else if(sensors[0].read() > lim && sensors[1].read() < lim)
      {
          
             if(sensors[0].read()>sensors[1].read()+500 && sensors[1].read()<lim)
             {
              
             
              Serial.println("우회전");

            
              motor1.setSpeed(turnspeed);
              motor2.setSpeed(-turnspeed);
              delay(1);
             }

             else
             {
              Serial.println("스무스 우회전");

            
              motor1.setSpeed(turnspeed*0.6);
              motor2.setSpeed(-turnspeed*0.6);
              delay(1);
             }
        
      }
    // 왼쪽 센서 물체 인식, 오른쪽 센서 오른쪽 센서와 500mm이상 차이날 경우 성립(오작동을 방지하기 위함)
        else if(sensors[0].read() < lim && sensors[1].read() > lim)
      {
           if(sensors[0].read()<lim && sensors[1].read()>sensors[0].read()+500)
           {
               Serial.println("좌회전");
            
               motor1.setSpeed(-turnspeed);
               motor2.setSpeed(turnspeed);
               delay(1);
           }

            else
            {
              Serial.println("스무스 좌회전");
            
               motor1.setSpeed(-turnspeed*0.6);
               motor2.setSpeed(turnspeed*0.6);
               delay(1);
            }
               
           
       }
       

    // 물체를 일직선으로 인식 시 실행 두개의 센서값 차이 100mm이내 and 인식거리내 물체 인식
        else if(sensors[0].read() < lim && sensors[1].read() < lim)
      {
    
     
    // 일직선 상에서 인식 중 일정거리 이내 가까워 진 경우 최대 속도로 돌진 (중간에 물체가 사라진 경우 즉각적인 반응이 느려 무지성 돌진하다가 떨어지는 것을 방지하기 위함) 
          if(sensors[0].read()<lim2 || sensors[1].read()<lim2 && ( sensors[0].read() < sensors[1].read()+500 || sensors[1].read() < sensors[0].read()+500))
          {
    
            Serial.println("담구자!!");
            motor1.setSpeed(254);
            motor2.setSpeed(254);
            delay(1);
          }
        
          else
          {
            
          
            Serial.println("돌진");
            motor1.setSpeed(60);
            motor2.setSpeed(60);
            delay(1);
          }
      
      }
      //  else
      //{
        // else 추가로 오류값이 뜰경우 빙빙 돌기라도 하라고 추가할 예정
        //Serial.println("찾는중(예외)");
        //motor1.setSpeed(findspeed);
        //motor2.setSpeed(-findspeed);
        //delay(1);
       //}
      
    }  
    
  }
  
}
