#include <DFRobot_TFmini.h>

// 단자번호 설정
#define PIN_MOTOR_A_SPD 3
#define PIN_MOTOR_A_EN 9
#define PIN_MOTOR_A_DIR 12

#define PIN_MOTOR_B_SPD 11
#define PIN_MOTOR_B_EN 8
#define PIN_MOTOR_B_DIR 13

SoftwareSerial mySerial(10, 7); // RX, TX 우노에 꼽는 단자번호 설정 Lidar 센서

void set_motor(char motor, int speed, bool dir, bool break_ = false) {
  analogWrite(motor == 'A' ? PIN_MOTOR_A_SPD : PIN_MOTOR_B_SPD, break_ == true ? 255 : speed);
  digitalWrite(motor == 'A' ? PIN_MOTOR_A_EN : PIN_MOTOR_B_EN, break_ == true ? HIGH : LOW);
  digitalWrite(motor == 'A' ? PIN_MOTOR_A_DIR : PIN_MOTOR_B_DIR, dir == true ? HIGH : LOW);
}


DFRobot_TFmini  TFmini;
uint16_t distance, strength;   // 거리와 강도를 담는 변수


void setup() {

  Serial.begin(115200);
  Serial.print("asdf");
  
  TFmini.begin(mySerial);
  pinMode(PIN_MOTOR_A_SPD, OUTPUT);
  pinMode(PIN_MOTOR_A_EN, OUTPUT);
  pinMode(PIN_MOTOR_A_DIR, OUTPUT);
  pinMode(PIN_MOTOR_B_SPD, OUTPUT);
  pinMode(PIN_MOTOR_B_EN, OUTPUT);
  pinMode(PIN_MOTOR_B_DIR, OUTPUT);
}

void loop() {
  if (TFmini.measure()) {                  // 거리와 신호의 강도를 측정합니다. 성공하면 을 반환하여 if문이 작동합니다.
    distance = TFmini.getDistance();       // 거리값을 cm단위로 불러옵니다.
    strength = TFmini.getStrength();       // 신호의 강도를 불러옵니다. 측정 대상이 넓으면 강도가 커집니다.
    Serial.print("Distance = ");
    Serial.print(distance);
    Serial.println("cm");
    Serial.print("Strength = ");
    Serial.println(strength);
  
    if (distance > 100 && strength < 100)  // 거리에 관측되는 거리 범위 지정 및 로봇 강도 설정 해야된다
    {
    set_motor('A',64,true);
    set_motor('B',64,false);
  
  
    }
    else
    {
    set_motor('A',0,true,true);
    set_motor('B',0,true,true);

    delay(100);
    set_motor('A',255,true);
    set_motor('B',255,true);
    }
  }
  delay(100);
// 계속 추적이 가능 하게끔 코딩 해야됨 일정 강도 이하로 떨어지면 재추적을 한다던가 그런식? break 이용해서 나가던가 아니면 특정지점으로 회귀하도록 하던가
}
