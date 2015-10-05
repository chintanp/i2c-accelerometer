// Original instructions - /* compile with cc -o mma7660 mma7660.c -lncurses */
// My changes -- //gcc -o mma mma.c -lncurses -l wiringPi

#define MMA7660_ADDR  0x4c

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <math.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <time.h>
#include <sys/time.h>
#include <curses.h>
#include <wiringPi.h>

typedef unsigned char byte;

int deviceDescriptor;

void I2cSendData(byte addr,byte *data,int len)
{
        if(ioctl(deviceDescriptor,I2C_SLAVE, addr))
                printf("I2cSendData_device : IOCTL Problem\n");

        write(deviceDescriptor,data,len);
}

void I2cReadData(byte addr,byte *data,int len)
{
        if(ioctl(deviceDescriptor,I2C_SLAVE, addr))
                printf("I2cReadData_device : IOCTL Problem\n");

        read(deviceDescriptor,data,len);
}

void init_i2c(char *DeviceName)
{	
	printf("Initialising i2c device");

        deviceDescriptor=open(DeviceName, O_RDWR);

        if (deviceDescriptor == -1)
        {
                printf("Error opening device '%s'\n",DeviceName);
                exit(-1);
        }
}

int main(int argc, char **argv)
{
        int i, v;
        byte data[20];
	
	fflush(stdout);

	printf("Inside main");

	//initscr();
        timeout(1);
	
	//printw("Inside main");

        init_i2c("/dev/i2c-1");

        // set mode register to 1 to activate accelerometer
        data[0] = 7;
        data[1] = 1;

        I2cSendData(MMA7660_ADDR,data,2);

        printf("Hit any key to quit\n\n");

        while (getch()<0) {

                I2cReadData(MMA7660_ADDR,data,11);

                for (i=0; i<3; i++) {
                        v = data[i];
                        if (v>=0x20) v = -(0x40 - v);  //sign extend negatives
                        printf("%c:%3d  ",i+'X',v);
                }
                for (i=3; i<11; i++)
                        printf("%x: %02x  ",i,data[i]);
                printf("\r");
		//delay(10);	
        }
        printf("\n");

        close(deviceDescriptor);

        endwin();

        return 0;
}
