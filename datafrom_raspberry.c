#include <string.h>
#include <errno.h>
#include <wiringSerial.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>   
#include <math.h>    //Compile : gcc -o a a.c -l wiringPi -lm

#define Sample_rate 25
#define Config      26
#define Gyro_config 27
#define Acc_config  28 
#define Interrupt   56
#define PWR_managment 107
#define Acc_X       59
#define Acc_Y       61
#define Acc_Z       63

#define BT_LG           25      // landing gear
#define BT_FU           20      // flap up
#define BT_FD           16      // flap down
#define BT_STOP         26      // tam dung
#define BT_UG           19      // up gas
#define BT_DG           13      // down gas
#define BT_EXIT         5       // thoat game
#define BT_AUTO         6       // tu dong lai
#define BT_R            8      // quay phai
#define BT_L            21       // quay trai

unsigned int a,b,c,d,e,f,g,td,i,k = 0;
int mpu;
float data;

void Init_6050(void){
        // register 25->28,56,107
        // sample_rate 500hz
        wiringPiI2CWriteReg8(mpu,Sample_rate,15);
        //ko sd nguồn xung ngoài, tắt DLFP
        wiringPiI2CWriteReg8(mpu,Config,0);
        //gyro FS: +-500 o/s
        wiringPiI2CWriteReg8(mpu,Gyro_config,0x08);
        //acc FS: +- 8g
        wiringPiI2CWriteReg8(mpu,Acc_config,0x10);
        // mở interrupt của data ready
        wiringPiI2CWriteReg8(mpu,Interrupt,1);
        // chọn nguồn xung Gyro X
        wiringPiI2CWriteReg8(mpu,PWR_managment,0x01);
}

int16_t read_sensor(unsigned char sensor)
{
    int16_t high,low,data;
    high = wiringPiI2CReadReg8(mpu,sensor);
    low = wiringPiI2CReadReg8(mpu,sensor+1);
    data = (high<<8) | low;
    return data;
}

void read_button(){
        // nut nhan landing gear
        if(!digitalRead(BT_LG)){
                a = 1;              
        }
        else{   a = 0;}
        // nut nhan flap up
        if(!digitalRead(BT_FU)){
                b = 1;              
        }
        else{   b = 0;}
        // nut nhan flap down
        if(!digitalRead(BT_FD)){
                c = 1;              
        }
        else{   c = 0;}
        // nut nhan stop
        if(!digitalRead(BT_STOP)){
                d = 1;              
        }
        else{   d = 0;}
        // nut nhan up gas
        if(!digitalRead(BT_UG)){
                e = 1;              
        }
        else{   e = 0;}
        // nut nhan down gas
        if(!digitalRead(BT_DG)){
                f = 1;              
        }
        else{   f = 0;}
        // nut nhan exit 
        if(!digitalRead(BT_EXIT)){
                g = 1;              
        }
        else{   g = 0;}
        // nut nhan che do lai auto 
        if(!digitalRead(BT_AUTO)){
                td = 1;              
        }
        else{   td = 0;}
        // quay phai
        if(!digitalRead(BT_R)){
                i = 1;              
        }
        else{   i = 0;}
        // quay trai
        if(!digitalRead(BT_L)){
                k = 1;              
        }
        else{   k = 0;}

        delay(1);
}

int main(void)
{
    mpu = wiringPiI2CSetup(0x68);
    //setup gieo tiep i2c
    wiringPiSetupPhys();

    wiringPiSetupGpio();
    pinMode(BT_LG, INPUT);
    pullUpDnControl (BT_LG, PUD_UP);
    pinMode(BT_FU, INPUT);
    pullUpDnControl (BT_FU, PUD_UP);
    pinMode(BT_FD, INPUT);
    pullUpDnControl (BT_FD, PUD_UP);
    pinMode(BT_STOP, INPUT);
    pullUpDnControl (BT_STOP, PUD_UP);
    pinMode(BT_UG, INPUT);
    pullUpDnControl (BT_UG, PUD_UP);
    pinMode(BT_DG, INPUT);
    pullUpDnControl (BT_DG, PUD_UP);
    pinMode(BT_EXIT, INPUT);
    pullUpDnControl (BT_EXIT, PUD_UP);
    pinMode(BT_AUTO, INPUT);
    pullUpDnControl (BT_AUTO, PUD_UP);
    pinMode(BT_R, INPUT);
    pullUpDnControl (BT_R, PUD_UP);
    pinMode(BT_L, INPUT);
    pullUpDnControl (BT_L, PUD_UP);

    //thiet lap che do cho MPU
    Init_6050();
    
    //đọc các giá trị đo
    while (1)
    {
        float Ax = (float)read_sensor(Acc_X)/4096.0;
        float Ay = (float)read_sensor(Acc_Y)/4096.0;
        float Az = (float)read_sensor(Acc_Z)/4096.0;
        
        float pitch = atan2(Ax,sqrt(pow(Ay,2)+pow(Az,2)))*180/M_PI;
        float roll = atan2(Ay,sqrt(pow(Ax,2)+pow(Az,2)))*180/M_PI;
        //float yaw = atan2(Az,sqrt(pow(Ax,2)+pow(Ay,2)))*180/M_PI;

	//printf("Gia tri Ax : %f \n",Ax);
	//printf("Gia tri Ay : %f \n",Ay);
	//printf("Gia tri Az : %f \n",Az);

        //printf("Gia tri roll :%.2f\n",roll);
        //printf("Gia tri pitch :%.2f\n",pitch);
        //printf("Gia tri yaw :%.2f\n",yaw);

        read_button();

	int fd;
	//printf("Raspberry's sending : \n");
	if((fd = serialOpen ("/dev/ttyS0", 9600)) < 0 ){
		fprintf (stderr, "Unable to open serial device: %s\n", strerror (errno));
	}

        char cmd[200];
        sprintf(cmd,"%d %d %d %d %d %d %d %.2f %.2f %d %d %d ",a,b,c,d,e,f,g,pitch,roll,td,i,k);
        printf("%s\n", cmd);
	serialPuts(fd, cmd);
        delay(100);
	serialFlush(fd);
	fflush(stdout);

	delay(100);
	}
    delay(1000);    
    return 0;
}