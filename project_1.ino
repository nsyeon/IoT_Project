#include <MFRC522.h>  // RFID
#include <SPI.h>
#include <Servo.h>

#define   SS_PIN  10
#define   RST_PIN 9
#define   SERVO_PIN 6
#define   LED_PIN 7

//RFID 초음파
#define TRIG 9  // 초음파 보내는 핀
#define ECHO 8  // 초음파 받는 핀

//보내고 받는 거
#define TRIG_1 5 //TRIG_1 핀 설정 (초음파 보내는 핀)
#define ECHO_1 4 //ECHO_1 핀 설정 (초음파 받는 핀)

MFRC522 mfrc522(SS_PIN, RST_PIN);
Servo servo;
int angle = 0;
long duration, distance;
int check = 1;
const int g_Card_UID[4] = {0xC3, 0xBC, 0xF5, 0x6};

boolean CheckCardUID(){
  if (mfrc522.uid.uidByte[0] == g_Card_UID[0] && 
      mfrc522.uid.uidByte[1] == g_Card_UID[1] && 
      mfrc522.uid.uidByte[2] == g_Card_UID[2] &&
      mfrc522.uid.uidByte[3] == g_Card_UID[3])
      return true;

  return false;
}

void open_b(void){
  for(angle = 0; angle < 90; angle++){
    servo.write(angle);
    delay(2);
  }
}

void close_b(void){
  for(angle = 90; angle > 0; angle--){
    servo.write(angle);
    delay(2);  
  }
}

void setup() {
  Serial.begin(9600);
  
  SPI.begin();
  mfrc522.PCD_Init();
  mfrc522.PCD_DumpVersionToSerial();

  servo.attach(SERVO_PIN);
  servo.write(0);
  
  pinMode(TRIG_1, OUTPUT);
  pinMode(ECHO_1, INPUT);

  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  if(!mfrc522.PICC_IsNewCardPresent())
    return;

  if(!mfrc522.PICC_ReadCardSerial())
    return;

  if(CheckCardUID()){
    open_b();

    while(true){
      digitalWrite(TRIG_1, LOW);
      delayMicroseconds(2); 
      digitalWrite(TRIG_1, HIGH); 
      delayMicroseconds(10);
      digitalWrite(TRIG_1, LOW);
      duration = pulseIn(ECHO_1, HIGH); 
    
      distance = duration * 17 / 1000;

      delay(100);

      if(distance < 5){
        digitalWrite(LED_PIN, HIGH);
        close_b();
        break;
      }
    } // end while()

    // 주차가 되어 있고, 문이 닫겨있을 때
    while(true){
      digitalWrite(TRIG_1, LOW);
      delayMicroseconds(2);
      digitalWrite(TRIG_1, HIGH);
      delayMicroseconds(10);
      digitalWrite(TRIG_1, LOW);
      duration = pulseIn(ECHO_1, HIGH);

      distance = duration * 17 / 1000;

      delay(100);

      if(check == 1 && distance > 5){
        digitalWrite(LED_PIN, LOW);
        open_b();
        check++;
      }

      if(check == 2 && distance > 35){
        close_b();
        check--;
        break;
      }
      
    } // end while()
    delay(100);
  } // end if(CheckCardUID)
  
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
}
