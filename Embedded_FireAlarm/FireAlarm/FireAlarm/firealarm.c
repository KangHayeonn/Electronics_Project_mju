/////////최종 코드//////
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/input.h>
#include <string.h>
#include <sys/types.h>
#include <pthread.h>
#include <sys/times.h>
#include <signal.h>
#include <sys/ioctl.h> // for ioctl
#include <fcntl.h> // for O_RDWR
#include <ctype.h> //led에서 추가
#include <sys/ipc.h> //led에서 추가
#include <sys/stat.h> //led에서 추가

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include <sys/kd.h> //touchscreen
#include <sys/ioctl.h> //touchscreen

#ifndef _AMG_H_
#define _AMG_H_
int AMG(int a);

#endif

/////////////////////////////////////////////////////////////////////button
//#define  INPUT_DEVICE_LIST   "/dev/input/event"  //실제 디바이스 드라이버 노드파일: 뒤에 숫자가 붙음., ex)/dev/input/event5
//#define PROBE_FILE   "/proc/bus/input/devices" //PPT에 제시된 "이 파일을 까보면 event? 의 숫자를 알수 있다"는 바로 그 파일

static pthread_t buttonTh_id;
static int fdB = 0;
static void* buttonThFunc();
static int msgID = 0;
char inputDevPath[200] = { 0, };

static pthread_t buttonThread_ID;

#ifndef _BUTTON_H_
#define _BUTTON_H_

int buttonLibInit(void);
int buttonLibExit(void);
//#define BUTTON_DRIVER_NAME "/dev/input/event1"
#define MESSAGE_ID 1122

typedef struct
{
    long int messageNum;
    int keyInput;
}BUTTON_MSG_T;
#endif
/////////////////////////////////////////////////////////////////////////



////////Touch.c//////////////////////////////////////////////
#define EVENT_TYPE EV_ABS
#define EVENT_CODE_X    ABS_X
#define EVENT_CODE_Y    ABS_Y
#define  INPUT_DEVICE_LIST   "/dev/input/event"  //실제 디바이스 드라이버 노드파일: 뒤에 숫자가 붙음., ex)/dev/input/event5
#define PROBE_FILE   "/proc/bus/input/devices" //PPT에 제시된 "이 파일을 까보면 event? 의 숫자를 알수 있다"는 바로 그 파일
static void* TouchThFunc();
static pthread_t TouchTh_id;
char inputDevPatht[200] = { 0, };

/////////Touch.h///////////////////////////////////////////
#ifndef _TOUCH_H_
#define _TOUCH_H_

int TouchLibInit(void);
int TouchLibExit(void);
#define MESSAGE 1100

typedef struct
{
    long int messageNum;
    int Xvalue;
    int Yvalue;
}TOUCH_MSG_T;
#endif
/////////////////////////////////////////////////////////////




////////////////////////////call-fnd
int fnda = 0, fndb = 0, a = 0;
int fndNum[6] =

{

   0,0,0,0,0,0

};
int q = 0;


///home button
static pthread_t displayid;
int play, check;
void* Firealram();

/////////////////////touch
static pthread_t state_ID;
static pthread_t test_ID;
static int msgIDTouch = 0;
int Messageclean = 0;
void* testthFunc();
void* statethFunc();
int x, y;
int state;





////////////////////////////////////////////////////////////////////////led
#ifndef _LED_H_
#define _LED_H_

#define LED_DRIVER_NAME "/dev/periled"
#endif


//#define  SYS_PATH   "S: Sysfs="
//#define  BUTTON_NAME   "ecube-button"

//#define  LINE_FEED   0x0A

//#define  MAX_BUFF   200

#define ACCELPATH "/sys/class/misc/FreescaleAccelerometer/"



int value, valuex;//temperature_number
char bufx[128];//temperature_srting
char buf[128];

void* AccelThreadFunc();
static int ledvalue;
static unsigned int ledValue;
static int fd = 0;

int ledLibInit(void);
int ledOnOff(int ledNum, int onoff);

int val;


char strCmd[80];
int result;


int AMG(int a)
{
    int fd = 0;
    FILE* fp = NULL;
    if (a == 0) {
        //Accelerometer
        fd = open(ACCELPATH "enable", O_WRONLY);
        dprintf(fd, "1");
        close(fd);

        fp = fopen(ACCELPATH "data", "rt");
        int i, a1 = 0, a2 = 0, a3 = 0;
        int accel_1[1000];
        int accel_2[1000];
        int accel_3[1000];
        for (i = 0; i < 1000; i++)
        {
            fscanf(fp, "%d, %d, %d", &accel_1[i], &accel_2[i], &accel_3[i]);
            a1 += accel_1[i];
            a2 += accel_2[i];
            a3 += accel_3[i];
        }
        // printf ("I read Accel %4d, %4d, %4d\r\n\n\n",(a1/1000)%10000,(a2/1000)%10000,(a3/1000)%10000); //왼쪽으로 기울이면 첫번째 값이 양수 오른쪽은 음수
     //위로 기울이면 -1910~-1920  아래로 기울이면 -1890~-1900
        fclose(fp);
        return (-1) * (a1 / 1000) % 10000;//나중에 쓰기위해서 중간 값을 받음

    }

    return 0;
}


int ledOnOff(int ledNum, int onoff)
{
    int i = 1;
    i = i << ledNum;
    if (onoff == 1)
        ledValue |= i;

    else if (onoff == 0)
        ledValue = ledValue & (~i);

    else printf("wrong number\n");
    write(fd, &ledValue, 4);
}

void alarmLedOn(int bEnable_a)
{

    if (bEnable_a == 1)
    {
        sprintf(strCmd, "./ledtest 0x01");
        result = system(strCmd);
    }
    else if (bEnable_a == 2)
    {
        sprintf(strCmd, "./ledtest 0x03");
        result = system(strCmd);
    }
    else if (bEnable_a == 4)
    {
        sprintf(strCmd, "./ledtest 0x07");
        result = system(strCmd);
    }
    else if (bEnable_a == 8)
    {
        sprintf(strCmd, "./ledtest 0x0f");
        result = system(strCmd);
    }
    else if (bEnable_a == 16)
    {
        sprintf(strCmd, "./ledtest 0x1f");
        result = system(strCmd);
    }
    else if (bEnable_a == 32)
    {
        sprintf(strCmd, "./ledtest 0x3f");
        result = system(strCmd);
    }
    else if (bEnable_a == 64)
    {
        sprintf(strCmd, "./ledtest 0x7f");
        result = system(strCmd);
    }
    else if (bEnable_a == 128)
    {
        sprintf(strCmd, "./ledtest 0xff");
        result = system(strCmd);
    }
    else
    {
        sprintf(strCmd, "./ledtest 0x00");
        result = system(strCmd);
    }

}
void* AccelThreadFunc()
{
    printf("\n<-----------------inter_AccelThread------------------->\n");

    while (1)
    {
        int a;//오른쪽으로 기울이면 +++ 왼쪽은 ---출력
        a = AMG(0);
        usleep(500000);
        if (a < 0)
        {
            printf("\n---\n");
            printf("output:%d", a);
            if (ledvalue > 0) { ledvalue--;  ledOnOff(ledvalue, 0); }
        }
        else if (a > 0)
        {
            printf("\n+++\n");
            printf("output:%d", a);
            if (ledvalue <= 7) { ledOnOff(ledvalue, 1); ledvalue++; }
        }
        else break;
        switch (ledvalue) {
        case 0:  alarmLedOn(0); val = 0; printf("power: 0\n"); break;
        case 1:  alarmLedOn(1); val = 3; printf("power: 10\n"); break;
        case 2:  alarmLedOn(2); val = 6; printf("power: 20\n"); break;
        case 3:  alarmLedOn(4); val = 9; printf("power: 30\n"); break;
        case 4:  alarmLedOn(8); val = 12; printf("power: 40\n"); break;
        case 5:  alarmLedOn(16); val = 15; printf("power: 50\n"); break;
        case 6:  alarmLedOn(32); val = 18; printf("power: 60\n"); break;
        case 7:  alarmLedOn(64); val = 21; printf("power: 70\n"); break;
        case 8:  alarmLedOn(128); val = 24; printf("power: 80\n"); break;
        default: break;
        }

        usleep(10000);
        a = 0;
    }
    return 0;
}

/////////////sehyeon code////////////////////////////////////
void setStrTextLCD(int line, char* str)
{
    sprintf(strCmd, "./textlcdtest %d '%s'", line, str);
    result = system(strCmd);

}

void updateFnd(void)
{
    sprintf(strCmd, "./fndtest s '%s'", bufx);
    result = system(strCmd);
}

void buzzerOn(int bEnable)
{
    if (bEnable)
    {
        sprintf(strCmd, "./buzzertest 5");
        result = system(strCmd);

    }
    else
    {
        sprintf(strCmd, "./buzzertest 0");
        result = system(strCmd);
    }
}


void setColorLED(int red, int green, int blue)
{

    sprintf(strCmd, "./colorledtest %d %d %d", red, green, blue);
    result = system(strCmd);
}

int getTemperature(void)
{
    FILE* stream = popen("./temperaturetest", "r");


    int bytesRead;
    int value = 0;
    if (ferror(stream))
    {
        printf("temperaturetest open fail.\n");
        return 0;
    }
    while (!feof(stream))
    {
        bytesRead = fread(buf, 1, 128, stream);
        buf[bytesRead] = 0;

        value = atoi(buf);

        printf("temperature : %d\n", value);
    }
    pclose(stream);
    return value;
}



void springcooler(void)
{
    int value = getTemperature();

    if (value > 28)
    {
        //setStrTextLCD(1, "            ");
        setStrTextLCD(2, "   Emergency");
        sprintf(bufx, "%d", value * 5);
        updateFnd();
        setColorLED(0, 100, 100);
        valuex = value * 5 - 5;
        while (valuex > 28)
        {
            valuex -= val;
            if (valuex >= value)
            {
                sprintf(bufx, "%d", valuex);
                updateFnd();
            }
            AccelThreadFunc();

            sleep(1);
            buzzerOn(1);
        }
    }

    setColorLED(0, 100, 0);
    value = getTemperature();

    if (value <= 28)
    {

        setStrTextLCD(2, "room temperature");
        sprintf(bufx, "%d", value);
        updateFnd();
        setColorLED(0, 100, 0);
    }

    buzzerOn(0);

}

void* set()
{
    setStrTextLCD(1, "      TRY");
    setStrTextLCD(2, "Smart Firealarm");
    sprintf(strCmd, "./bitmap Initial.bmp");
    result = system(strCmd);
    setColorLED(100, 100, 100);
    sprintf(strCmd, "./fndtest s 000000");
    result = system(strCmd);
    sprintf(strCmd, "./ledtest 0");
    result = system(strCmd);
}

//////////////////////////////////////////////////////////////////////////////////////////button
int probeButtonPath(char* newPathb)
{
    int returnValue = 0;   //button에 해당하는 event#을 찾았나?
    int number = 0;            //찾았다면 여기에 집어넣자
    FILE* fp = fopen(PROBE_FILE, "rt");   //파일을 열고

#define HAVE_TO_FIND_1    "N: Name=\"ecube-button\"\n"
#define HAVE_TO_FIND_2      "H: Handlers=kbd event"
    while (!feof(fp))   //파일 끝까지 읽어들인다.
    {
        char tmpStr[200];   //200자를 읽을 수 있게 버퍼
        fgets(tmpStr, 200, fp);   //최대 200자를 읽어봄
        //printf ("%s",tmpStr);
        if (strcmp(tmpStr, HAVE_TO_FIND_1) == 0)
        {

            returnValue = 1;   //찾음
        }
        if (
            (returnValue == 1) &&    //찾은 상태에서
            (strncasecmp(tmpStr, HAVE_TO_FIND_2, strlen(HAVE_TO_FIND_2)) == 0)
            //Event??을 찾았으면
            )
        {

            number = tmpStr[strlen(tmpStr) - 3] - '0';
            break;
        }
    }
    //이 상황에서 number에는 event? 중 ? 에 해당하는 숫자가 들어가 있다.
    fclose(fp);

    if (returnValue == 1)
        sprintf(newPathb, "%s%d", INPUT_DEVICE_LIST, number);
    //인자로 들어온 newPath 포인터에 /dev/input/event? 의 스트링을 채움
    return returnValue;
}
/*====================================================================================================================*/
int buttonLibInit(void)
{
    printf("\n ===========< button_init >============ \n");
    if (probeButtonPath(inputDevPath) == 0)
    {
        printf("ERROR! File Not Found!\r\n");
        printf("Did you insmod?\r\n");
        return 0;
    }
    printf("--->inputDevPath: %s\r\n", inputDevPath);
    fdB = open(inputDevPath, O_RDONLY);
    msgID = msgget(MESSAGE_ID, IPC_CREAT | 0666);
    pthread_create(&buttonTh_id, NULL, &buttonThFunc, NULL);
    return msgID;
}
int buttonLibExit(void)
{
    pthread_cancel(buttonTh_id);
    close(fdB);
    printf("===========< button_Exit >============\n");
}


void updateFndnum(void) //ok

{

    sprintf(strCmd, "./fndtest s '%d%d%d%d%d%d'", fndNum[0], fndNum[1], fndNum[2], fndNum[3], fndNum[4], fndNum[5]);

    result = system(strCmd);

}

void setnumberFnd(void)

{

    if (a == 0 && fnda != 0) { fndb = 0;   fnda = --fnda; }

    else if (a == 0 && fnda == 0) { fndb = 0;   fnda = 5; }

    if (a == 1 && fnda != 5) { fndb = 0;   fnda = ++fnda; }

    else if (a == 1 && fnda == 5) { fndb = 0;   fnda = 0; }

    if (a == 2 && fndb != 9)      fndb = ++fndb;

    else if (a == 2 && fndb == 9)      fndb = 0;

    if (a == 3 && fndb != 0)      fndb = --fndb;

    else if (a == 3 && fndb == 0)      fndb = 9;

    fndNum[fnda] = fndb;



    updateFndnum();

}


static void* buttonThFunc()
{
    BUTTON_MSG_T msgTxB;
    msgTxB.messageNum = 1;
    struct input_event stEventB;
    while (1)
    {
        read(fdB, &stEventB, sizeof(stEventB));
        if ((stEventB.type == EV_KEY) && (stEventB.value == 0))
        {
            msgTxB.keyInput = stEventB.code;
            msgsnd(msgID, &msgTxB, sizeof(long int), 0);
            printf("button on\r\n");
        }
    }
}

static void* emergency_call()
{
    q++;

    if ((q % 2) == 1)
    {
        setStrTextLCD(1, "Enter callnumber");
        setStrTextLCD(2, "                 ");
        sprintf(strCmd, "./bitmap call.bmp");
        result = system(strCmd);


    }
    else if ((q % 2) == 0)
    {
        setStrTextLCD(1, "Emergency call");
        setStrTextLCD(2, " in progress ");
        sprintf(strCmd, "./bitmap calling.bmp");
        result = system(strCmd);
        setColorLED(100, 0, 100);

    }
}

void* buttonTheadFunc()
{
    int returnValue = 0;
    int temperature;
    BUTTON_MSG_T msgRxB;
    printf("\n<-----------------inter_ButtonThread------------------->\n");
    while (1) {
        returnValue = msgrcv(msgID, &msgRxB, sizeof(long int), 0, 0);
        printf("\n");
        check = 0;
        switch (msgRxB.keyInput)
        {

        case KEY_HOME:
            sprintf(strCmd, "./bitmap House.bmp");
            result = system(strCmd);
            check = 1;
            pthread_create(&displayid, NULL, &Firealram, NULL); //main home
            play = pthread_detach(displayid);
            if (play == 0) printf("detach clear\n");

            printf("\nHome key):\n");

            break;

        case KEY_BACK:
            printf("\nBack):\n");  emergency_call();
            break;

        case KEY_SEARCH:
            printf("\nSearch):\n"); a = 0; setnumberFnd();
            break;

        case KEY_MENU:
            printf("Menu\n):\n");  a = 1; setnumberFnd();
            break;

        case KEY_VOLUMEUP:
            printf("\nVolume up key):\n");  a = 2; setnumberFnd();
            break;

        case KEY_VOLUMEDOWN:
            printf("\nVolume down key):\n");  a = 3; setnumberFnd();
            break;
        }
    }
}

void* Firealram()
{
    printf("home thread\n");
    while (check == 1)
    {

        if ((x > 0) && (x < 340) && (y > 0) && (y < 300))
        {
            sprintf(strCmd, "./bitmap livingroom.bmp");
            result = system(strCmd);
            setStrTextLCD(1, "  living room    ");
            printf("\nliving room\n");
            springcooler();
        }
        else if ((x > 341) && (x < 680) && (y > 0) && (y < 300))
        {
            sprintf(strCmd, "./bitmap kitchen.bmp");
            result = system(strCmd);
            setStrTextLCD(1, "   kitchen    ");
            printf("\nkitchen\n");
            springcooler();
        }
        else if ((x > 681) && (x < 1024) && (y > 0) && (y < 300))
        {
            sprintf(strCmd, "./bitmap bathroom.bmp");
            result = system(strCmd);
            setStrTextLCD(1, "   bathroom    ");
            printf("\nbathroom\n");
            springcooler();
        }
        else if ((x > 0) && (x < 340) && (y > 300) && (y < 600))
        {
            sprintf(strCmd, "./bitmap room1.bmp");
            result = system(strCmd);
            setStrTextLCD(1, "   room1    ");
            printf("\nroom1\n");
            springcooler();
        }
        else if ((x > 341) && (x < 680) && (y > 300) && (y < 600))
        {
            sprintf(strCmd, "./bitmap room2.bmp");
            result = system(strCmd);
            setStrTextLCD(1, "   room2    ");
            printf("\nroom2\n");
            springcooler();
        }
        else if ((x > 681) && (x < 1024) && (y > 300) && (y < 600))
        {
            sprintf(strCmd, "./bitmap room3.bmp");
            result = system(strCmd);
            setStrTextLCD(1, "   room3    ");
            printf("\nroom3\n");
            springcooler();
        }

        usleep(1000);
    }
}






////////////////////////////////////touch
void* statethFunc()
{
    while (1)//터치가 현재 눌려있는 상태인지 확인하는 코드
    {
        if (state == 0) { x = 0; y = 0; }
        //      printf("state : %d, x = %d y = %d statethFunc\n",state,x,y);
        usleep(10000);
    }
}
void* testthFunc()
{
    int returnValue = 0;
    TOUCH_MSG_T msgRx;

    while (Messageclean != 1) //메세지비우기 
    {

        returnValue = msgrcv(msgIDTouch, &msgRx, sizeof(msgRx) - sizeof(long int), 0, IPC_NOWAIT);
        if (returnValue == -1) Messageclean = 1;
    }

    while (1) {
        usleep(1000);
        returnValue = msgrcv(msgIDTouch, &msgRx, sizeof(msgRx) - sizeof(long int), 0, 0);
        if (msgRx.messageNum == 1)
        {
            printf("\nx :%d \n", msgRx.Xvalue);
            x = msgRx.Xvalue; state = 1;
        }
        else if (msgRx.messageNum == 2)
        {
            printf("\ny :%d \n", msgRx.Yvalue);
            y = msgRx.Yvalue; state = 1;
        }
        else printf("other");
        state = 0;
    }
}

int probetouchPath(char* newPath)
{
    int returnValue = 0;   //button에 해당하는 event#을 찾았나?
    int number = 0;            //찾았다면 여기에 집어넣자
    FILE* fp = fopen(PROBE_FILE, "rt");   //파일을 열고

#define HAVE_TO_FIND_3    "N: Name=\"WaveShare WaveShare Touchscreen\"\n"
#define HAVE_TO_FIND_4    "H: Handlers=mouse0 event"

    while (!feof(fp))   //파일 끝까지 읽어들인다.
    {
        char tmpStr[200];   //200자를 읽을 수 있게 버퍼
        fgets(tmpStr, 200, fp);   //최대 200자를 읽어봄
        //printf ("%s",tmpStr);
        if (strcmp(tmpStr, HAVE_TO_FIND_3) == 0)
        {
            printf("YES! I found!: %s\r\n", tmpStr);
            returnValue = 1;   //찾음
        }
        if (
            (returnValue == 1) &&    //찾은 상태에서
            (strncasecmp(tmpStr, HAVE_TO_FIND_4, strlen(HAVE_TO_FIND_4)) == 0))
        {
            printf("-->%s", tmpStr);
            printf("\t%c\r\n", tmpStr[strlen(tmpStr) - 3]);
            number = tmpStr[strlen(tmpStr) - 3] - '0';   //Ascii character '0'-'9' (0x30-0x39) to interger(0)
            break;
        }
    }
    //이 상황에서 number에는 event? 중 ? 에 해당하는 숫자가 들어가 있다.
    fclose(fp);

    if (returnValue == 1)
        sprintf(newPath, "%s%d", INPUT_DEVICE_LIST, number);
    //인자로 들어온 newPath 포인터에 /dev/input/event? 의 스트링을 채움
    return returnValue;
}

int TouchLibInit(void)
{
    if (probetouchPath(inputDevPatht) == 0)
    {
        printf("ERROR! File Not Found!\r\n");
        return 0;
    }

    fd = open(inputDevPatht, O_RDONLY);
    msgIDTouch = msgget(MESSAGE, IPC_CREAT | 0666);
    pthread_create(&TouchTh_id, NULL, &TouchThFunc, NULL);
    return msgIDTouch;
}
int TouchLibExit(void)
{
    printf("Finish");
    pthread_cancel(TouchTh_id);
    close(fd);
}
static void* TouchThFunc()
{
    TOUCH_MSG_T msgTx;
    struct input_event stEvent;

    while (1)
    {
        read(fd, &stEvent, sizeof(stEvent));
        if ((stEvent.type == EV_ABS) && (stEvent.code == EVENT_CODE_X || stEvent.code == EVENT_CODE_Y)) {
            //  printf("%s=%d\n",stEvent.code==EVENT_CODE_X ? "X" : "Y",stEvent.value);

           //if ( ( stEvent.type == EV_ABS))
       //   {
            if (stEvent.code == EVENT_CODE_X)
            {
                msgTx.messageNum = 1;
                msgTx.Xvalue = stEvent.value;
                // printf("\n %d",msgTx.Xvalue);

                msgsnd(msgIDTouch, &msgTx, sizeof(msgTx) - sizeof(long int), 0);
            }
            else //if(stEvent.code == EVENT_CODE_Y)   
            {
                msgTx.messageNum = 2;
                msgTx.Yvalue = stEvent.value;
                //  printf("\n %d",msgTx.Yvalue);
                msgsnd(msgIDTouch, &msgTx, sizeof(msgTx) - sizeof(long int), 0);
                //    msgctl( msgID, IPC_RMID, 0);
            }
        }
    }
}

int stopEvent = 0;
void handle_kill_signal() {
    stopEvent = 1;
    signal(SIGINT, SIG_DFL); // SIG_DFL => do the default action of this signal 
    signal(SIGTERM, SIG_DFL);  //when terminal is killed  
    signal(SIGHUP, SIG_DFL); // hangup signal , notify the connected to the control terminmal . 
}

int main(void)
{
    set();

    /*===========터치스크린 깨짐방지=====================*/
    int conFD = open("/dev/tty0", O_RDWR);
    ioctl(conFD, KDSETMODE, KD_GRAPHICS);
    close(conFD);
    /*============init=====================*/
    struct sigaction action;
    // Add signal handler to terminate server
    action.sa_handler = handle_kill_signal;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    sigaction(SIGINT, &action, NULL);  // ctrl+c
    sigaction(SIGTERM, &action, NULL);
    sigaction(SIGHUP, &action, NULL);


    msgID = buttonLibInit();
    msgIDTouch = TouchLibInit();

    pthread_create(&buttonTh_id, NULL, &buttonTheadFunc, NULL); //버튼 계속확인
    pthread_create(&test_ID, NULL, &testthFunc, NULL);
    pthread_create(&state_ID, NULL, &statethFunc, NULL);

    pthread_join(state_ID, NULL); //touch가 눌렸는지확인
    pthread_join(buttonThread_ID, NULL); //버튼쓰레드
    pthread_join(test_ID, NULL);

    TouchLibExit();
    buttonLibExit();
    return 0;
}