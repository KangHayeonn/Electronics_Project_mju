//컨트롤러 측 아두이노 코드
//작성자 강하연 박정준 박지훈 이서영

#include <Adafruit_Fingerprint.h>
#include <Servo.h>
#include "WiFiEsp.h"
#include "SoftwareSerial.h"

#include <NfcAdapter.h>
#include <PN532/PN532/PN532.h>

#if 1
    #include <SPI.h>
    #include <PN532/PN532_SPI/PN532_SPI.h>


    PN532_SPI pn532spi(SPI, 10);
    NfcAdapter nfc = NfcAdapter(pn532spi);
#else

    #include <Wire.h>
    #include <PN532/PN532_I2C/PN532_I2C.h>


    PN532_I2C pn532_i2c(Wire);
    NfcAdapter nfc = NfcAdapter(pn532_i2c);
#endif


//아두이노 끼리 통신
SoftwareSerial BTSerial(12,13);
//지문인식센서 하드웨어 시리얼 통신으로 연결

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&Serial2);

//LED 핀번호
uint8_t id;
int stock;
int redpin = 11; // select the pin for the red LED
int greenpin =10; // select the pin for the green LED
int bluepin =9; // select the pin for the  blue LED
//LED 셋팅 함수
void setColor(int red, int green, int blue)
{
  analogWrite(redpin, red);
  analogWrite(greenpin, green);
  analogWrite(bluepin, blue);
  delay(1000);
}
//WIFI 연결 (수정해야 할 부분)
char ssid[] = "LG Q7_9079";     // your network SSID (name)
char pwd[] = "11111111";  // your network password
char pwdErr[] = "xxxx";   // wrong password
int status = WL_IDLE_STATUS;     // the Wifi radio's status

int GP0val=0;
int GP2val =0;
int enter=0;
int kenter=0;
int state=0;

const int SERVO = 5;
Servo servo;

WiFiEspServer server(80);

//셋업 함수
void setup()
{
  //아두이노끼리의 보레이트 맞추기
  BTSerial.begin(9600); // BTSerial을 9600으로 통신 할거다.
  
  //LED포트 제어
  pinMode(redpin, OUTPUT);
  pinMode(bluepin, OUTPUT);
  pinMode(greenpin, OUTPUT);
    
  Serial.begin(115200); //시리얼모니터의 보레이트(PIN 18,19) 
  Serial1.begin(9600); //와이파이 통신의 보레이트
  Serial2.begin(57600);// 지문인식센서 통신의 보레이트


  //센서가 존재여부를 판단(PIN10, PIN11)
    if (finger.verifyPassword()) {//센서 연결 되어있는지 확인
    Serial.println(F("센서를 찾았습니다"));
    Serial.println(F("\n\n도어락 지문 인식 센서 설정"));
    Serial.println(F("\nA: 신규 지문 저장\nB: 기존 지문 삭제")); 
    } 
    else {
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


  // GPIO 포트들을 초기화 합니다
  pinMode(9, OUTPUT);
  digitalWrite(9, 0);
  pinMode(11, OUTPUT);
  digitalWrite(11, 0); //LED

  //WIFI를 셋업하는 함수
  WiFi.init(&Serial1); //WiFi.begin(ssid, password);
    // check for the presence of the shield
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue
    while (true);
  }
  
  // attempt to connect to WiFi network
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network
    status = WiFi.begin(ssid, pwd);
  }
  Serial.println("You're connected to the network");
  printWifiStatus();
  // start the web server on port 80
  server.begin(); //WiFi 동작 시작
  nfc.begin();  //NFC 동작 시작
}

///////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
//루프 함수

void loop()
{
   setColor(0, 0, 0);  //LED 끄기
   id = getFingerprintID(); //
   //delay(100); 
       if (nfc.tagPresent()) {
        NfcTag tag = nfc.read();

        if (tag.hasNdefMessage()) { // every tag won't have a message

            NdefMessage message = tag.getNdefMessage();
            
            int recordCount = message.getRecordCount();
            for (int i = 0; i < recordCount; i++) {
                NdefRecord record = message.getRecord(i);
               
                int payloadLength = record.getPayloadLength();
                byte payload[payloadLength];
                record.getPayload(payload);

                String payloadAsString = "";
                //String payload = "";
                for (int c = 3; c < payloadLength; c++) {
                    payloadAsString += (char)payload[c];
                }
                
                SERIAL.print("");
                SERIAL.println(payloadAsString);
                //SERIAL.println();

                //payload = payloadAsString;

                if (payloadAsString == "3928813"){
                  SERIAL.println("만국통운");
                  SERIAL.println();
                  stock = 1;
                }
                else if (payloadAsString == "5491334"){
                  SERIAL.println("롯돼택배1");
                  SERIAL.println();
                  stock = 2;
                }
                else if (payloadAsString == "2111398"){
                  SERIAL.println("루젠택배");
                  SERIAL.println();
                  stock = 3;
                }
                
                else if (payloadAsString == "6152273"){
                  SERIAL.println("롯돼택배2");
                  SERIAL.println();
                  stock = 4;
                }

               
                String uid = record.getId();
                if (uid != "") {
                    SERIAL.print("  ID: "); SERIAL.println(uid);
                }
            }
        }
    }


  // 클라이언트가 접속하는지 체크
   WiFiEspClient client = server.available();
   
   if (!client) {
   return;
  }

  // 클라이언트가 데이터를 보낼 때까지 기다립니다.
  
  Serial.println("new client");
  while(!client.available()){
    delay(1);
  }
  
  // 요청 첫 줄을 읽어 옵니다.
  String req = client.readStringUntil('\r');
  Serial.println(req);
  client.flush();

    if (req.indexOf("/A/1") != -1)
        {
            id = 1;
            Serial.print("등록할 ID ");
            Serial.println(id);
            while (! getFingerprintEnroll() ); //getFingerprintEnroll함수에 들어가서 등록하고 값이 저장되면 함수를 나와라.
         }    
    else if (req.indexOf("/B/1") != -1) 
        {
            id = 2;
            Serial.print("등록할 ID ");
            Serial.println(id);
            while (! getFingerprintEnroll() ); //getFingerprintEnroll함수에 들어가서 등록하고 값이 저장되면 함수를 나와라.
         }   
    else if (req.indexOf("/C/1") != -1) 
        {
            id = 3;
            Serial.print("등록할 ID ");
            Serial.println(id);
            while (! getFingerprintEnroll() ); //getFingerprintEnroll함수에 들어가서 등록하고 값이 저장되면 함수를 나와라.
         }
   else if (req.indexOf("/D/1") != -1) 
        {
            id = 4;
            Serial.print("등록할 ID ");
            Serial.println(id);
            while (! getFingerprintEnroll() ); //getFingerprintEnroll함수에 들어가서 등록하고 값이 저장되면 함수를 나와라.
         }
    else if (req.indexOf("/E/1") != -1) 
        {
            id = 5;
            Serial.print("등록할 ID ");
            Serial.println(id);
            while (! getFingerprintEnroll() ); //getFingerprintEnroll함수에 들어가서 등록하고 값이 저장되면 함수를 나와라.
         }
    else if (req.indexOf("/A/0") != -1)
        {
            id = 1;
            Serial.print("삭제할 ID ");
            Serial.println(id);
            while (! deleteFingerprint(id) ); //getFingerprintEnroll함수에 들어가서 등록하고 값이 저장되면 함수를 나와라.
         }    
    else if (req.indexOf("/B/0") != -1) 
        {
            id = 2;
            Serial.print("삭제할 ID ");
            Serial.println(id);
            while (! deleteFingerprint(id) ); //getFingerprintEnroll함수에 들어가서 등록하고 값이 저장되면 함수를 나와라.
         }   
    else if (req.indexOf("/C/0") != -1) 
        {
            id = 3;
            Serial.print("삭제할 ID ");
            Serial.println(id);
            while (! deleteFingerprint(id) ); //getFingerprintEnroll함수에 들어가서 등록하고 값이 저장되면 함수를 나와라.
         }
   else if (req.indexOf("/D/0") != -1) 
        {
            id = 4;
            Serial.print("삭제할 ID ");
            Serial.println(id);
            while (! deleteFingerprint(id) ); //getFingerprintEnroll함수에 들어가서 등록하고 값이 저장되면 함수를 나와라.
         }
    else if (req.indexOf("/E/0") != -1) 
        {
            id = 5;
            Serial.print("삭제할 ID ");
            Serial.println(id);
            while (! deleteFingerprint(id) ); //getFingerprintEnroll함수에 들어가서 등록하고 값이 저장되면 함수를 나와라.
         }


    else if (req.indexOf("/aa") != -1) 
        {
  Serial.print("stock1 ID Code Get! ");
          }
    else if (req.indexOf("/bb") != -1) 
        {
  Serial.print("stock2 ID Code Get! ");
         }
    else if (req.indexOf("/cc") != -1) 
        {
  Serial.print("stock3 ID Code Get! ");
         }
    else if (req.indexOf("/dd") != -1) 
        {
  Serial.print("stock4 ID Code Get! ");
         }           


else if (req.indexOf("/arrive") != -1)
         {
 //배송도착
                  client.flush();
                  String s3 = "HTTP/1.1 200\r\n\r\n";
                  switch(stock)
                  {
                     case 1:
                      s3 += "1"; 
                      break;
                     case 2:
                      s3 += "2"; 
                      break;
                     case 3:
                      s3 += "3"; 
                      break;
                     case 4:
                      s3 += "4"; 
                      break;
                     case 5:
                      s3 += "5"; 
                      break;
                     default:
                      s3 = "0";
                   }

                  client.print(s3);
                  delay(1);
                  client.stop();
                 s3 ="0";
                 stock = 0;
         }






         
    else if (req.indexOf("/enter") != -1)
         {

               if(GP0val==1){ //키패드
                   client.flush();
                   Serial.println("키패드으으으");
                   String s1 = "HTTP/1.1 200\r\n\r\n";
                   s1 += "open"; 
                   client.print(s1);
                   delay(1);
                   client.stop();
                   s1="0";
                   GP0val=0;
                }  
               else if(GP2val ==1){ //지문
                 //for(int i=0;i<enter;i++){
                  client.flush();
                  //Serial.println("지무우우우운");
                  String s2 = "HTTP/1.1 200\r\n\r\n";
                  switch(id)
                  {
                     case 1:
                      s2 += "1"; 
                      break;
                     case 2:
                      s2 += "2"; 
                      break;
                     case 3:
                      s2 += "3"; 
                      break;
                     case 4:
                      s2 += "4"; 
                      break;
                     case 5:
                      s2 += "5"; 
                      break;
                     default:
                      s2 = "0";
                   }
                  //s2 += "1"; 
                  client.print(s2);
                  delay(1);
                  client.stop();
                 //delay(1000);
                 s2 ="0";
                  //
                //}  
                //enter=0;
                //kenter=0; 
                 GP2val=0;
            
                }

         }
    else {
             Serial.println("invalid request");
             client.stop();
             return;
          } 
   
  client.stop(); //이게 이렇게 중요해요 ~ 이거없으면 client 4까지밖에안됨
  Serial.println("Client disonnected");  // 응답완료 후에는 클라이언트를 닫습니다.
  
 }



////////////////////////////////////////////////////////////////////////////////////

void printWifiStatus()
{
  // print the SSID of the network you're attached to
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address
  IPAddress ip = WiFi.localIP(); //선언
  Serial.print("IP Address: ");
  Serial.println(ip);
  
  // print where to go in the browser
  Serial.println();
  Serial.print("To see this page in action, open a browser to http://");
  Serial.println(ip);
  Serial.println();
}

uint8_t getFingerprintID() {

    if(BTSerial.available()){ // btserial 통해 받은 데이터가 있는 지 여부
      //Serial.println("비티씨리ㅣ어어어얼");
      state=BTSerial.read(); // 있으면 state에 수신받아 저장해라
      //Serial.println(BTSerial.read());
      if(BTSerial.read()== -1){         
        Serial.println("\nKeypad Open"); //비밀번호 일치 시 입장
        servo.attach(SERVO);
        servo.write(160);
        delay(5000);
        servo.write(60);
        delay(500);
        servo.detach();
        GP0val=1;
        }  // 그 데이터가 y이면 모터를 90회전 하고 1초쉬고 -90회전해라
    }
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
    //BTSerial.write("y"); //센서 아두이노에 보내는 함수
    servo.attach(SERVO);
    servo.write(160);
    delay(5000);
    servo.write(60);
    delay(500);
    servo.detach();
    GP2val = 1;
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    //setColor(255, 0, 0); // RED
    Serial.println(F("\n입력받은 지문템플릿이 저장되어있지 않습니다."));
    Serial.println(F("(A를 눌러 지문을 등록하세요."));
    delay(50);  
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println(F("\n입력받은 지문템플릿이 저장되어있지 않습니다."));
    Serial.println(F("A를 눌러 지문을 등록하세요."));
    //setColor(255, 0, 0); // RED
    return p;
  } else {
    //setColor(255, 0, 0); // RED
    return p;
  }

  // found a match!
  Serial.print("ID "); Serial.print(finger.fingerID); //match가 안되면 figerID와 confidence에 들어갈 값이 없음
  Serial.print(F(" with confidence of ")); Serial.println(finger.confidence); Serial.print("\n");
  
  return finger.fingerID;
}

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
      //setColor(0, 255, 0); // green
      delay(500);      
      break;
    case FINGERPRINT_NOFINGER:
      delay(500);
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println(F("떼고 다시 대주세요"));
      //setColor(255, 0, 0); // red
      delay(500);
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println(F("지문인식오류"));
      //setColor(255, 0, 0); // red
      delay(500);
      break;
    default:
      Serial.println("에러");
      //setColor(255, 0, 0); // red
      delay(500);
      break;
    }
  }
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
      //setColor(0, 255, 0); // green
      delay(500);
      break;
    case FINGERPRINT_NOFINGER:
     delay(500);
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("가까이 대주세요");
      //setColor(255, 0, 0);
      delay(500);
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("지문인식오류");
      //setColor(255, 0, 0); // red
      delay(500);
      break;
    default:
      Serial.println("에러");
      //setColor(255, 0, 0); // red
      delay(500);
      break;
    }
  }
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
    //setColor(0, 255, 0); // green
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
//지문데이터 삭제함수
uint8_t deleteFingerprint(uint8_t id) {
  uint8_t p = -1;

  p = finger.deleteModel(id);

  if (p == FINGERPRINT_OK) {
    Serial.println("삭제완료!\n");
    //setColor(0, 255, 0);
    return 1;
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    //setColor(255, 0, 0);
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Could not delete in that location");
    //setColor(255, 0, 0);
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to flash");
    //setColor(255, 0, 0);
    return p;
  } else {
    Serial.print("Unknown error: 0x"); Serial.println(p, HEX);
    //setColor(255, 0, 0);
    return p;
  }
}
//지문인식센서 ID등록하기 위한 숫자입력함수
uint8_t readnumber(void) {
  uint8_t num = 0;

  while (num == 0) {
    while (! Serial.available());
    num = Serial.parseInt();
  }
  return num;
}
