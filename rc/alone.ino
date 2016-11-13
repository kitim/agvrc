/*
Line Tracer basic code.
*/

//
// 자동자 진행 방향 정의
//
#define CAR_DIR_FW  0   // 전진.
#define CAR_DIR_BK  1   // 후진.
#define CAR_DIR_LF  2   // 좌회전.
#define CAR_DIR_RF  3   // 우회전
#define CAR_DIR_ST  4   // 정지.

// 
// 차량 운행 방향 상태 전역 변수. // 정지 상태.
int g_carDirection = CAR_DIR_ST; // 

int g_carSpeed = 110; // 220 ~ 255 사이의 값을 넣어주도록 합니다.
//
// 주의:  ENA, ENB 는 PWM 지원 포트에 연결한다.
// 다음 업데이트시 변경합니다.
#define ENA   6
#define EN1   7
#define EN2   3

#define EN3   4
#define EN4   2
#define ENB   5


void init_car_controller_board()
{
  pinMode(ENA, OUTPUT);  // ENA
  pinMode(EN1, OUTPUT);  // EN1
  pinMode(EN2, OUTPUT);  // EN2

  pinMode(ENB, OUTPUT);  // ENB
  pinMode(EN3, OUTPUT);  // EN3
  pinMode(EN4, OUTPUT);  // EN4
}

//
// 전후좌우 4개의 함수는 테스트시
// DC 모터 연결에 맞게 고쳐서 정정해야 합니다.
// DC 모터 연결 (+)(-) 연결 변경하거나 코드를 변경합니다.
//


void car_forward()
{
  digitalWrite(EN1, LOW);     
  digitalWrite(EN2, HIGH);      
  analogWrite(ENA, g_carSpeed);

  digitalWrite(EN3, HIGH);       
  digitalWrite(EN4, LOW);         
  analogWrite(ENB, g_carSpeed);
}

void car_backward()
{
  digitalWrite(EN1, HIGH);     
  digitalWrite(EN2, LOW);    
  analogWrite(ENA, g_carSpeed);

  digitalWrite(EN3, LOW);     
  digitalWrite(EN4, HIGH);    
  analogWrite(ENB, g_carSpeed);
}

//
void car_left()
{
  digitalWrite(EN1, HIGH);       
  digitalWrite(EN2, LOW);     
  analogWrite(ENA, g_carSpeed);

  digitalWrite(EN3, HIGH);      
  digitalWrite(EN4, LOW);      
  analogWrite(ENB, g_carSpeed);
}

//
void car_right()
{
  digitalWrite(EN1, LOW);      
  digitalWrite(EN2, HIGH);    
  analogWrite(ENA, g_carSpeed);

  digitalWrite(EN3, LOW);
  digitalWrite(EN4, HIGH);
  analogWrite(ENB, g_carSpeed);
}

//
void car_stop()
{
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
}

//
// 방향 전환값에 의해 차량 운행.
//
void car_update()
{
  // %%
  // 현재는 매번 loop 에서 호출되지만 차후에 커맨드가 있을 경우만 호출될 수 있도록
  // 변경해야 합니다.
  if (g_carDirection == CAR_DIR_FW)  // 전진
  {
    car_forward();
  }
  else
  if (g_carDirection == CAR_DIR_BK) // 후진.
  {
    car_backward();
  }
  else
  if (g_carDirection == CAR_DIR_LF) // 좌회전
  {
    car_left();
  }
  else
  if (g_carDirection == CAR_DIR_RF) // 우회전
  {
    car_right();
  }
  else
  if (g_carDirection == CAR_DIR_ST) // 정지.
  {
    car_stop();
  }
}

void print_car_info()
{
  Serial.println("direction value " + g_carDirection);
  Serial.println("speed pwm value " + g_carSpeed);
}


////
// 라인트레이서 모듈 핀맵
#define LT_MODULE_A A3  //  17
#define LT_MODULE_F A1  //  15

void init_line_tracer_modules()
{
  pinMode(LT_MODULE_A, INPUT);
  pinMode(LT_MODULE_F, INPUT);
}

bool lt_isAlone()
{
  int ret = digitalRead(LT_MODULE_A);
  return ret == 1 ? true : false;
}
bool lt_isForward()
{
  int ret = digitalRead(LT_MODULE_F);
  return ret == 1 ? true : false;
}
// 
void lt_mode_update()
{
  int aa = lt_isAlone();
  int ff = lt_isForward();
  if (aa)
  {
    g_carDirection = CAR_DIR_ST;
    delay(5000);
  }
  else
  if (ff)    //ff
  {
    g_carDirection = CAR_DIR_FW;
  }
}
// 부팅 후 1회 실행되는 함수. 초기화 함수. Setup()
void setup()
{
  //
  Serial.begin(9600);
  //
  init_car_controller_board();

  print_car_info();
}

// 계속 실행되는 함수. Loop()
void loop()
{
  car_update();
  lt_mode_update();
}
