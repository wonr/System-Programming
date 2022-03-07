#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <stdint.h>
#include <sched.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <softPwm.h>
#include <fcntl.h>
#include <string.h>
#include <linux/kdev_t.h>
#include <linux/i2c-dev.h>
#include "ondo.h"

#define GPIO_DATA 7
#define SERVO 1
#define BUFFER_LENGTH 256

static char receive[BUFFER_LENGTH];
static int switchDrvFd, buzzerDrvFd, i2cDrvFd;

double* getGyro(double* result);
int servoControll(int waitTime);
int initGyro();

int main(int argc, char **argv) {
  int state = 1;
  int ret;
  char selectedSwitch;
  int waitTime = 0;
  const struct sched_param priority = {1};

  if((switchDrvFd = open("/dev/switch_driver", O_RDWR | O_NONBLOCK)) < 0) {
    perror("switch open failed");
    return 1;
  }
  if((buzzerDrvFd = open("/dev/buzzer_driver", O_RDWR | O_NONBLOCK)) < 0) {
    perror("buzzer open failed");
    return 1;
  }
  if((i2cDrvFd = open("/dev/i2c-1", O_RDWR | O_NONBLOCK)) < 0) {
    perror("i2c open failed");
    return 1;
  } else {
    initGyro();
  }

  while(1) {
    ret = read(switchDrvFd, receive, BUFFER_LENGTH);
    selectedSwitch = receive[0];

    if(selectedSwitch != '0' && selectedSwitch != 10) { //switch select
      printf("switch value: %c\n", receive[0]);
      
      if(selectedSwitch == '1') { // half boiled egg switch time set
        waitTime = 420;
        write(buzzerDrvFd, "1", BUFFER_LENGTH);
      } else if(selectedSwitch == '2') {  // hard boiled egg switch time set
        waitTime = 600;
        write(buzzerDrvFd, "101", BUFFER_LENGTH);
      }

      close(switchDrvFd);
      break;
    }
  }

  printf("thermometer measuring\n");
  sched_setscheduler(0, SCHED_FIFO, &priority); //set scheduling priority for the thread
  mlockall(MCL_CURRENT | MCL_FUTURE); //lock all of the calling process's virtual address space into RAM

  if (wiringPiSetup() == -1)
    return 1 ;

  pinMode(GPIO_DATA, INPUT);

  //wait until target temperature and then call motor rotation
  if(presence(GPIO_DATA) == 0) {
    double temp;
    while(state){
      temp = getTemperature(GPIO_DATA);
      printf("%.2f \n", temp);
      sleep(1);

      if(temp > 90) {
	printf("buzzer start\n");
	printf("survo motor start\n");
        state = servoControll(waitTime);
      }
    }
  }

  close(buzzerDrvFd);
}

double* getGyro(double* result) {

  // Read 6 bytes of data from GYRO_XOUT_H register.
  // Each value is XOUT H/L, YOUT H/L, ZOUT H/L.
  char reg[1] = {0x1D};
  char data[6] = {0};
  write(i2cDrvFd, reg, 1);
  if(read(i2cDrvFd, data, 6) != 6) {
    perror("i2c open failed");
  } else {
    result[0] = (data[0] * 256 + data[1] - 90);
    if(result[0] > 32767) result[0] -= 65536;
    // Correct the value for using this.
    result[0] /= 10.0;

    result[1] = (data[2] * 256 + data[3] + 26);
    if(result[1] > 32767) result[1] -= 65536;
    result[1] /= 10.0;

    result[2] = (data[4] * 256 + data[5] + 3);
    if(result[0] > 32767) result[2] -= 65536;
    result[2] /= 10.0;

    return result;
  }
}
//control servo motor for basket lift up and down work
int servoControll(int waitTime) {
  double gyroResult[3];
  int delayLoop = 6;
  int measureCount = 0;
  double sumX = 0, sumY = 0, sumZ = 0;
  if(wiringPiSetup() == -1)
    return 1;
    
  softPwmCreate(SERVO,0,200);

  printf("down\n");
    
  softPwmWrite(SERVO, 5); //'basket lift down' motor work
  
  // gyro sensor trace the position of basket  
  for(;delayLoop > 0;delayLoop--) {
    if(!getGyro(gyroResult)) {
      return 1;
    } else if(gyroResult[2] < 7000) {
      printf("x: %.2lf, y: %.2lf, z: %.2lf\n", gyroResult[0], gyroResult[1], gyroResult[2]);
      sumX += gyroResult[0];
      sumY += gyroResult[1];
      sumZ += gyroResult[2];
      measureCount++;
    }
    delay(100);
  }
  
  // detect basket lift working error and alarm buzzer alert
  if((sumX / (measureCount - 1)) < 30 && (sumY / (measureCount - 1)) > - 20 && (sumZ / (measureCount - 1)) > 6000) {
    delay(1000);
    softPwmWrite(SERVO, 24);
    write(buzzerDrvFd, "11111111111111111111111111", BUFFER_LENGTH);
    close(i2cDrvFd);
    close(switchDrvFd);
    close(buzzerDrvFd);
    printf("exit by abnormal operation\n");
    exit(0);
  }

  write(buzzerDrvFd, "10101", BUFFER_LENGTH);
  sleep(waitTime);

  printf("up\n");
  softPwmWrite(SERVO, 24);  //'basket lift up' motor work
  delay(600);

  printf("ave of x: %.2lf\n", sumX / (measureCount-1));
  printf("ave of y: %.2lf\n", sumY / (measureCount-1));
  printf("ave of z: %.2lf\n", sumZ / (measureCount-1));

  write(buzzerDrvFd, "1011011101111", BUFFER_LENGTH);
    
  printf("finish\n");
    
  return 0;
}

int initGyro() {
  char config[2] = {0};

  // Get I2C device from address 0x68 of gyro sensor.
  ioctl(i2cDrvFd, I2C_SLAVE, 0x68);

  // Set power up register and gyro value reference.
  config[0] = 0x3E;
  config[1] = 0x01;
  write(i2cDrvFd, config, 2);

  // Set full scale range of +/- 2000 deg/sec.
  config[0] = 0x16;
  config[0] = 0x18;
  write(i2cDrvFd, config, 2);
  sleep(1);
}


