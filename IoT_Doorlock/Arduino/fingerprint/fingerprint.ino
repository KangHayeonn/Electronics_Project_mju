#include <Adafruit_Fingerprint.h>
#include <Servo.h>

#if (defined(__AVR__) || defined(ESP8266)) && !defined(__AVR_ATmega2560__)
SoftwareSerial mySerial(2,3);
#else
#define mySerial Serial1
#endif


Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

uint8_t id;
int redpin = 11; // select the pin for the red LED
int greenpin =10; // select the pin for the green LED
int bluepin =9; // select the pin for the  blue LED

void setColor(int red, int green, int blue)
{
  analogWrite(redpin, red);
  analogWrite(greenpin, green);
  analogWrite(bluepin, blue);
  delay(1000);
}
const int SERVO = 5;
Servo servo;
///////////////////////////////////////////////////////////////////////////////////////////////
void setup()
{
  //서브모터 제어
  servo.attach(SERVO);
  
  //LED포트 제어
  pinMode(redpin, OUTPUT);
  pinMode(bluepin, OUTPUT);
  pinMode(greenpin, OUTPUT);

  //지문인식
  Serial.begin(115200); //보드 속도 초기화
  while (!Serial);  // For Yun/Leo/Micro/Zero/... 시리얼 통신이 시작 된다면!
  delay(100); 
  // set the data rate for the sensor serial port
  finger.begin(57600);// 인식 시작

  if (finger.verifyPassword()) {//센서 연결 되어있는지 확인
    Serial.println(F("센서를 찾았습니다"));
    Serial.println(F("\n\n도어락 지문 인식 센서 설정"));
    Serial.println(F("\nA: 신규 지문 저장\nB: 기존 지문 삭제")); 
  } else {
    Serial.println("센서가 없습니다\n");
    while (1) { delay(1); }
  }

  finger.getParameters();
  finger.getTemplateCount(); //템플릿 수를 센서에 문의 (templateCount=지문데이터=템플릿수)
  
  if (finger.templateCount == 0) {// 동일한 지문 데이터 있는지 확인 아무것도 없을 경우 지문을 먼저 등록한다
    Serial.println(F("\n저장된 지문이 없습니다."));
    Serial.println(F("A를 눌러 지문을 먼저 등록해주세요.\n"));
  }
  else {
    Serial.println(F("유효한 지문을 찾고있습니다. 기다려주세요.\n"));
  }
}
//숫자입력함수
uint8_t readnumber(void) {
  uint8_t num = 0;

  while (num == 0) {
    while (! Serial.available());
    num = Serial.parseInt();
  }
  return num;
}
////////////////////////////////////////////////////////////////////////////////////////////////
void loop()                    
{


      setColor(0, 0, 0);  
      getFingerprintID();
      delay(100); 
      
   if(Serial.available())
   {
     char cTemp = Serial.read();  
     switch(cTemp)
     {
       case 'A':
          Serial.println(F("\n< 지문데이터 신규 설정창 >\n"));
          Serial.println(F("1부터 127까지의 ID코드를 입력해주세요"));
          id = readnumber();
          if (id == 0) { //ID #0 not allowed, try again!
            return;
          }
          Serial.print("등록할 ID ");
          Serial.println(id);
          while (! getFingerprintEnroll() ); //getFingerprintEnroll함수에 들어가서 등록하고 값이 저장되면 함수를 나와라.
          break;       
        case 'B':
          Serial.println(F("\n< 지문데이터 삭제 설정창 >\n"));
          Serial.println(F("삭제할 ID코드를 입력해주세요(1~127)"));
          id = readnumber();
          if (id == 0) { //ID #0 not allowed, try again!
            return;
          }
          Serial.print("삭제할 ID ");
          Serial.println(id);
          while (! deleteFingerprint(id) ); //getFingerprintEnroll함수에 들어가서 등록하고 값이 저장되면 함수를 나와라.
          break;       
        default:
          break;       
     } 
   }
}

/////////////////////////////////////////////////////////////////////////////////

uint8_t getFingerprintID() {
  uint8_t p = finger.getImage(); //센서에서 이미지를 받음
  switch (p) {
    case FINGERPRINT_OK:
      break;
    case FINGERPRINT_NOFINGER:
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      return p;
    case FINGERPRINT_IMAGEFAIL:
      return p;
    default:
 
      return p;
  }

  // OK success!

  p = finger.image2Tz(); //getimage에서 받은 센서값을 템플릿 값으로 바꿔줌 (전환)
  switch (p) {
    case FINGERPRINT_OK:
      break;
    case FINGERPRINT_IMAGEMESS:
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      return p;
    case FINGERPRINT_FEATUREFAIL:
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      return p;
    default:
      return p;
  }

  // OK converted!
  
  p = finger.fingerSearch(); //센서에서 받아 저장된 이미지를 템플릿으로 변환시킨것을 기존 센서에 저장된 템플릿과 일치하는지 확인( 일치한다면 fingerID에 들어감)
  if (p == FINGERPRINT_OK) {
    Serial.println(F("\n지문이 일치합니다."));
    servo.attach(SERVO);
    setColor(0, 0, 255); // green
    servo.write(0);
    delay(500);
    servo.write(180);
    delay(500);
    servo.detach();   
    delay(50);  
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    setColor(255, 0, 0); // RED
    Serial.println(F("\n입력받은 지문템플릿이 저장되어있지 않습니다."));
    Serial.println(F("(A를 눌러 지문을 등록하세요."));
    delay(50);  
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println(F("\n입력받은 지문템플릿이 저장되어있지 않습니다."));
    Serial.println(F("A를 눌러 지문을 등록하세요."));
    setColor(255, 0, 0); // RED
    return p;
  } else {
    setColor(255, 0, 0); // RED
    return p;
  }

  // found a match!
  Serial.print("ID "); Serial.print(finger.fingerID); //match가 안되면 figerID와 confidence에 들어갈 값이 없음
  Serial.print(F(" with confidence of ")); Serial.println(finger.confidence); Serial.print("\n");
  
  return finger.fingerID;
}

// returns -1 if failed, otherwise returns ID #
/*int getFingerprintIDez() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;

  // found a match!
  Serial.print("ID #"); Serial.print(finger.fingerID);
  //Serial.print(" with confidence of "); Serial.println(finger.confidence);
  return finger.fingerID;
}*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//지문등록 함수

uint8_t getFingerprintEnroll() {

  int p = -1;
  Serial.print("ID:"); Serial.print(id); Serial.println("에 저장됩니다.");
  Serial.println(F("손가락을 센서에 대주세요."));
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println(F("\n지문이 인식되었습니다."));
      setColor(0, 255, 0); // green
      delay(500);      
      break;
    case FINGERPRINT_NOFINGER:
      delay(500);
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println(F("떼고 다시 대주세요"));
      setColor(255, 0, 0); // red
      delay(500);
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println(F("지문인식오류"));
      setColor(255, 0, 0); // red
      delay(500);
      break;
    default:
      Serial.println("에러");
      setColor(255, 0, 0); // red
      delay(500);
      break;
    }
  }

  // OK success!
//출력시 
  p = finger.image2Tz(1);  //검증1
  switch (p) {
    case FINGERPRINT_OK:
      break;
    case FINGERPRINT_IMAGEMESS:
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      return p;
    case FINGERPRINT_FEATUREFAIL:
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      return p;
    default:
      return p;
  }

  Serial.println(F("손가락을 떼어주세요\n"));
  delay(1000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  
  p = -1;
  Serial.println(F("지문을 다시 확인 합니다."));
  Serial.println(F("손가락을 다시 입력해주세요."));
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println(F("지문이 저장되었습니다.\n"));
      setColor(0, 255, 0); // green
      delay(500);
      break;
    case FINGERPRINT_NOFINGER:
     delay(500);
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("가까이 대주세요");
      setColor(255, 0, 0);
      delay(500);
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("지문인식오류");
      setColor(255, 0, 0); // red
      delay(500);
      break;
    default:
      Serial.println("에러");
      setColor(255, 0, 0); // red
      delay(500);
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(2); //검증2
  switch (p) {
    case FINGERPRINT_OK:
      break;
    case FINGERPRINT_IMAGEMESS:
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      return p;
    case FINGERPRINT_FEATUREFAIL:
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      return p;
    default:
      return p;
  }

  // OK converted!

  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    Serial.println("지문이 일치합니다");
    delay(500);
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("가까이 대주세요");
     delay(500);
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    Serial.println(F("지문이 일치하지 않습니다"));
    delay(500);
    return p;
  } else {
    Serial.println("에러입니다");
    delay(500);
    return p;
  }

  Serial.print("ID "); Serial.print(id);
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    Serial.print(": 지문이 저장되었습니다.\n");
    setColor(0, 255, 0); // green
    delay(500);
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    delay(500);
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    return p;
  } else {
    return p;
  }

  return true;
}

//////////////////////////////////////////////////////////////////////////////////
//지문데이터 삭제함수
uint8_t deleteFingerprint(uint8_t id) {
  uint8_t p = -1;

  p = finger.deleteModel(id);

  if (p == FINGERPRINT_OK) {
    Serial.println("삭제완료!\n");
    setColor(0, 255, 0);
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    setColor(255, 0, 0);
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Could not delete in that location");
    setColor(255, 0, 0);
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to flash");
    setColor(255, 0, 0);
    return p;
  } else {
    Serial.print("Unknown error: 0x"); Serial.println(p, HEX);
    setColor(255, 0, 0);
    return p;
  }
}
