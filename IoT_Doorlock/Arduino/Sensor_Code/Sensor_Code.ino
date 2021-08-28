///센서최종코드(LCD)

//센서 측 아두이노 코드
//작성자 강하연 박정준 박지훈 이서영

#include <Keypad_I2C.h>
#include <Keypad.h>       
#include <Wire.h>
#include <EEPROM.h>
#include <SoftwareSerial.h>
#include <LiquidCrystal.h>

#define I2CADDR 0x20

SoftwareSerial BTSerial(12,13);

/////////////////////////////////////////////////////11.17
LiquidCrystal lcd(8, 9, 4, 5, 6, 7); //mega:lcd(8, 9, 31, 33, 35, 37);

int lcd_key = 0;
int adc_key_in = 0;
#define btnRIGHT 0
#define btnUP 1
#define btnDOWN 2
#define btnLEFT 3
#define btnSELECT 4
#define btnNONE 5
int read_LCD_buttons()
{
adc_key_in = analogRead(0); // AO 핀으로부터 아날로그값을 읽어옴
// 읽어온 아날로그의 값에 따라 눌려진 버튼을 판단함
if (adc_key_in > 1000) return btnNONE;
if (adc_key_in < 50) return btnRIGHT;
if (adc_key_in < 250) return btnUP;
if (adc_key_in < 450) return btnDOWN;
if (adc_key_in < 650) return btnLEFT;
if (adc_key_in < 850) return btnSELECT;
return btnNONE; // when all others fail, return this...
}
////////////////////////////////////////////////////////////

//키패드 설정
int addr=0;
int count=0;
int tru=0;
char newpw[4]={0};
static char number[4]={'0','0','0','0'}; //비밀번호 초기값

const int buttonPin = 2;     // the number of the pushbutton pin
const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
//define the cymbols on the buttons of the keypads
char hexaKeys[ROWS][COLS] = {
  {'D','#','0','*'},
  {'C','9','8','7'},
  {'B','6','5','4'},
  {'A','3','2','1'}
};
byte rowPins[ROWS] = {3, 2, 1, 0}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {7, 6, 5, 4}; //connect to the column pinouts of the keypad

//initialize an instance of class NewKeypad
Keypad_I2C customKeypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS, I2CADDR); 
Keypad_I2C NEWKeypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS, I2CADDR); 

// variables will change:
//int buttonState = 0;         // variable for reading the pushbutton status

//시리얼 변수선언

void setup(){
     lcd.begin(16, 2); //LCD초기화
     lcd.setCursor(0,0);
     lcd.print("Password");
     lcd.setCursor(0,1);        
     lcd.print("Change Window!"); //비밀번호 변경
     delay(50);
  
     Wire.begin( );                // GDY200622
     customKeypad.begin( );        // GDY120705
     NEWKeypad.begin();
     Serial.begin(115200);
     //아두이노 끼리 연결된 시리얼
     BTSerial.begin(9600);
  
     Serial.println( "\n도어락 비밀번호" );
     Serial.println("도어락내부버튼 : 비밀번호 변경");
     Serial.println("C : 변경된 비밀번호 저장");
     Serial.println("D : 비밀번호 확인");
     Serial.println("* : 입장");
}

void loop()
{
     static char userinput[4];
     static int count = 0;
     static int count1 = 0;
     static int tru = 0;
 
     char key = customKeypad.getKey();
     char key1;
     //시리얼모니터에 state데이터값을 보내라.
  
     //buttonState = digitalRead(buttonPin);
     lcd.setCursor(0,1); // 2번째 줄(1) 1번째(0) 패널에 위치하고
     lcd_key = read_LCD_buttons(); // 버튼이 눌려진 이름 출력하는 함수 호출
     switch (lcd_key) // 버튼 판단
     {
        case btnRIGHT:
        {
            lcd.clear();
            delay(50);
            lcd.setCursor(0,0);
            lcd.print("Confirm Password");
            lcd.setCursor(0,1);
            lcd.print(number[0]); //비밀번호 변경
            lcd.setCursor(1,1);
            lcd.print(number[1]); //비밀번호 변경
            lcd.setCursor(2,1);
            lcd.print(number[2]); //비밀번호 변경
            lcd.setCursor(3,1);
            lcd.print(number[3]); //비밀번호 변경
            delay(1000);
            break;
         }
         case btnLEFT:
         {
             lcd.clear();
             delay(50);
             lcd.setCursor(0,0);
             lcd.print("Change Password"); //비밀번호 변경
             lcd.setCursor(0,1);
             lcd.print("Press a number"); //비밀번호 변경
             Serial.println(F("Press a number:"));
             memset(userinput, 0, sizeof(userinput));
             count = 0;
             delay(1000);
             break;
         }
         case btnSELECT: //비밀번호 저장
         {
             lcd.clear();
             delay(50);
             for( int i=0; i<count; i++)
             {
                 EEPROM.write(i, 0);
                 EEPROM.write(i, userinput[i]);
             }
             Serial.println(F("\nSaved"));
             lcd.setCursor(0,0);
             lcd.print("Save Password"); //비밀번호 저장
             lcd.setCursor(3,1);
             lcd.print("Yeah!"); //비밀번호 변경
             delay(100);
             for(int i=0; i<4; i++){
             number[i] = EEPROM.read(i);
             }
             count = 0;
             break;
          }
      } 
      if (key != NO_KEY)
      {
           Serial.print(key); //키패드에서 비밀번호 입력받음
      }
      switch (key)
      {
           case NO_KEY:   // nothing to do
               break;
               
           case '*': //입장
               for(int i=0; i<count; i++)
               {
                   if(number[i] == userinput[i])tru++;
               }
               if(tru==4){
               lcd.clear();
               delay(50);                
               Serial.println("\nOpen"); //비밀번호 일치 시 입장
               lcd.setCursor(0,0);
               lcd.print("Correct Password"); //비밀번호 저장
               lcd.setCursor(3,1);
               lcd.print("Open!"); //비밀번호 저장
               BTSerial.write("y"); //센서 아두이노에 보내는 함수
               tru = 0;
               delay(1000);
               }
               else{
               lcd.clear();
               delay(50);                
               Serial.println("\nWrong password"); //비밀번호 불일치
               lcd.setCursor(0,0);
               lcd.print("Wrong Password"); //비밀번호 저장
               lcd.setCursor(2,1);
               lcd.print("Closed"); //비밀번호 저장               
               tru = 0;
               delay(1000);
               }
               count=0;
               break;
          default:   // if not 10 characters yet
               if (count < 4)  // add key to userinput array and increment counter
               {              
                     userinput[count++] = key;
               }
               break;
        }
}
