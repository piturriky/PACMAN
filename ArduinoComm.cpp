/*#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <sys/ioctl.h>*/

class ArduinoComm{

  public:
    int fd, n, i;
    
    struct termios toptions;
    bool running;
    

    ArduinoComm(){

      running = false;

      /* open serial port */
      fd = open("/dev/ttyACM2", O_RDWR | O_NOCTTY);
      printf("fd opened as %i\n", fd);

       //usleep(3500000);

      /* get current serial port settings */
      tcgetattr(fd, &toptions);
      /* set 9600 baud both ways */
      cfsetispeed(&toptions, B9600);
      cfsetospeed(&toptions, B9600);
      /* 8 bits, no parity, no stop bits */
      toptions.c_cflag &= ~PARENB;
      toptions.c_cflag &= ~CSTOPB;
      toptions.c_cflag &= ~CSIZE;
      toptions.c_cflag |= CS8;
      /* Canonical mode */
      toptions.c_lflag |= ICANON;
      /* commit the serial port settings */
      tcsetattr(fd, TCSANOW, &toptions);
    }

    void startCallibration(){
      char buf[64];
      write(fd, "2", 1);
      /*n = read(fd, buf, 64);
      printf("%i bytes read, buffer contains: %s\n\n", n, buf);
      if(buf[0] == '0') return true;
      return false;*/
    }

    void readDataTask(){
      char buf[64];
      printf("Getting values......\n");
      write(fd, "1", 1);
      n = read(fd, buf, 64);
      // insert terminating zero in the string 
      buf[n] = 0;

      printf("%i bytes read, buffer contains: %s\n\n", n, buf);
     // usleep(100000);

      if(n != 4){
        return;
      }

      
      aProtocol.pacmanDirection = buf[0] - '0';
      aProtocol.pacmanVision = buf[6] - '0';
      aProtocol.pacmanVelocity = buf[2] - '0';
      aProtocol.pacmanAmbient = buf[4] - '0';
      aProtocol.state = true;
    }

    void readData(){
      
      //if(!running){
        //printf("Send thread\n");
        //running = true;
      while(true){
        printf("Srart thread\n");
        thread t1(&ArduinoComm::readDataTask,this);
        t1.join();
        printf("End thread\n");
      }
        
      //}
      
      //t1.join();

      /*arduinoProtocol aP;

      printf("Getting values......\n");
      write(fd, "1", 1);
      n = read(fd, buf, 64);
      // insert terminating zero in the string 
      buf[n] = 0;

      printf("%i bytes read, buffer contains: %s\n\n", n, buf);
     // usleep(100000);

      if(n != 11){
        aP.state = false;
        return aP;
      }

      aP.state = true;
      aP.pacmanDirection = buf[0] - '0';
      aP.pacmanVision = buf[6] - '0';
      aP.pacmanVelocity = buf[2] - '0';
      aP.pacmanAmbient = buf[4] - '0';

      return aP;*/
    }

    void startProcess(){
      thread t1(&ArduinoComm::readData,this);
      t1.detach();
    }

};
