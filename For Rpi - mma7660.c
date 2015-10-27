/* compile with cc -o mma7660 mma7660.c -lncurses */

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

typedef unsigned char byte;

int deviceDescriptor;

void I2cSendData(byte addr,byte *data,int len)
{
        if(ioctl(deviceDescriptor,I2C_SLAVE, addr))
                printw("I2cSendData_device : IOCTL Problem\n");

        write(deviceDescriptor,data,len);
}

void I2cReadData(byte addr,byte *data,int len)
{
        if(ioctl(deviceDescriptor,I2C_SLAVE, addr))
                printw("I2cReadData_device : IOCTL Problem\n");

        read(deviceDescriptor,data,len);
}

void init_i2c(char *DeviceName)
{
        deviceDescriptor=open(DeviceName, O_RDWR);
        if (deviceDescriptor == -1)
        {
                printw("Error opening device '%s'\n",DeviceName);
                exit(-1);
        }
}

int main(int argc, char **argv)
{
        int i, v;
        byte data[20];

        initscr();
        timeout(1);

        init_i2c("/dev/i2c-0");

        // set mode register to 1 to activate accelerometer
        data[0] = 7;
        data[1] = 1;
        I2cSendData(MMA7660_ADDR,data,2);

        printw("Hit any key to quit\n\n");
        while (getch()<0) {
                I2cReadData(MMA7660_ADDR,data,11);
                for (i=0; i<3; i++) {
                        v = data[i];
                        if (v>=0x20) v = -(0x40 - v);  //sign extend negatives
                        printw("%c:%3d  ",i+'X',v);
                }
                for (i=3; i<11; i++)
                        printw("%x: %02x  ",i,data[i]);
                printw("\r");
        }
        printw("\n");

        close(deviceDescriptor);

        endwin();

        return 0;
}
