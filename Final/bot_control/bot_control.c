#include "simpletools.h"                      // Include simple tools
#include "ping.h"
#include "servo.h"
#include "fdserial.h"

#define PIN_XBEE_RX     0
#define PIN_XBEE_TX     1
#define PIN_SERVO_LEFT  16
#define PIN_SERVO_RIGHT 17
#define PIN_SERVO_MID   18
#define PIN_PING        19

int distLeft, distCenter, distRight;
int * cog_ping, cog_motor, cog_xbee, cog_light;

int speedLeft = 30, speedRight = 30;
int sfixLeft = 0, sfixRight = 0;
int turnSigLeft = 0, turnSigRight = 0, turnSigStop = 0;
int lanNum = 2;
int turnSigIng = 0;
char manualSig = '0';

void zigbee(){
	fdserial *xbee;
	xbee = fdserial_open(0, 1, 0, 9600);
	putchar(CLS); // Clear terminal screen
	fdserial_rxFlush(xbee);
	pause(100);
    while(1){
		int ch = fdserial_rxChar(xbee);
		fdserial_rxFlush(xbee);
		manualSig = (char) ch;
		fdserial_txChar(xbee, ch);
		fdserial_txFlush(xbee);
		pause(100);
  
       if(manualSig == 'r' && turnSigLeft==0 && turnSigRight==0 && speedLeft != 0 && speedRight != 0){
        sfixLeft = 20;
        sfixRight = 0;
        high(27); low(26);
       }   
       else if(manualSig == 'l' && turnSigLeft==0 && turnSigRight==0 && speedLeft != 0 && speedRight != 0){
        sfixLeft = 20;
        sfixRight++;
        high(26); low(27);
       }   
       else{
        sfixLeft = 0;
        sfixRight = 0;
        low(27); low(26);
       }        
    }        
	
}    
void dist(){
    while(1){
        servo_angle(PIN_SERVO_MID, 1400);
        pause(300);
        distLeft = ping_cm(PIN_PING);
        servo_angle(PIN_SERVO_MID, 900);
        pause(300);
        distCenter = ping_cm(PIN_PING);
        servo_angle(PIN_SERVO_MID, 400);
        pause(300);
        distRight = ping_cm(PIN_PING);
        servo_angle(PIN_SERVO_MID, 900);
        pause(300);
        distCenter = ping_cm(PIN_PING);
    }    
}

void motor(){
    while(1){
        servo_speed(PIN_SERVO_LEFT,speedLeft+sfixLeft);
        servo_speed(PIN_SERVO_RIGHT,-speedRight-sfixRight);
    }        
}

void switch_left(){
    turnSigLeft = 1;
    speedLeft = 20; speedRight = 50;
    pause(1000);
    speedLeft = 30; speedRight = 30;
    pause(4000);
    speedLeft = 50; speedRight = 20;
    pause(1000);
    speedLeft = 50; speedRight = 50;
    lanNum--;
    low(27);
    turnSigLeft = 0;
    return;
}

void switch_right(){
    turnSigRight = 1;
    speedLeft = 50; speedRight = 20;
    pause(1000);
    speedLeft = 30; speedRight = 30;
    pause(4000);
    speedLeft = 20; speedRight = 50;
    pause(1000);
    speedLeft = 50; speedRight = 50;
    lanNum++;
    low(26);
    turnSigRight = 0;
    return;
}

void drive_stop(){
    speedLeft = 0; speedRight = 0;
    return;
}

void speed_up(){
    speedLeft += 5; speedRight += 5;
    return;
}

void speed_down(){
    speedLeft -= 5; speedRight -= 5;
    return;
}

int turning(){
    return (turnSigLeft!=0 && turnSigRight!=0);
} 

int chk_left(){
    if(lanNum == 1)
        return 0;
    else
        return (distLeft > 15) ? 1 : 0;
}    

int chk_right(){
    if(lanNum == 3)
        return 0;
    else
        return (distRight > 15) ? 1 : 0;
}  

void light(){
    while(1){
        if(turnSigLeft){
            low(9);
            high(10);
            pause(500);
            low(10);
            pause(500);
        }
        else if(turnSigRight){
            low(10);
            high(9);
            pause(500);
            low(9);
            pause(500);
        }
        else if(speedLeft == 0 && speedRight == 0){
            high(9); high(10);
        }
        else{
            low(9); low(10);
        }            
    }
}

int main(){
    cog_ping = cog_run(dist, 128);
    cog_motor = cog_run(motor, 64);
    cog_xbee = cog_run(zigbee, 128);
    cog_light = cog_run(light, 128);
    pause(1000);
    while(1)
    {
        if(!turning() && distCenter < 10 && speedLeft >= 20 && speedRight >= 20){
            speed_down();
        }            
        else if(!turning() && distCenter > 20 && speedLeft < 50 && speedRight < 50){
            speed_up();
        }
        else if(!turning() && distCenter < 10 && chk_left()){
            switch_left();
        }
        else if(!turning() && distCenter < 10 && chk_right()){
            switch_right();
        }
        else if(distCenter < 10 && !chk_right() && !chk_left()){
            drive_stop();
        }            
        print("Lan:%d  Turn:%d %d %d  Speed:%d %d  Dist:%d %d %d %c\n", lanNum,turnSigLeft, turnSigRight, turnSigStop, speedLeft+sfixLeft, speedRight+sfixRight, distLeft, distCenter, distRight, manualSig);
        pause(200);

    }
}
