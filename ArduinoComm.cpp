class ArduinoComm{

  public:
    int fd, n, i, c = 0;
    
    struct termios toptions;
    

    ArduinoComm(){
    }

    bool init(){
      /* open serial port */
      //fd = open("/dev/ttySIM0", O_RDWR | O_NOCTTY);
      fd = open( "/dev/ttyACM0", O_RDWR| O_NOCTTY );
      if(DEBUG) printf("fd opened as %i\n", fd);

      usleep(3500000);

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

      if(fd > 0)return true;
      return false;
    }

    void startCallibration(){
      write(fd, "1", 1);
    }

    void readDataTask(){
      int n = 0,
          spot = 0;
      char buf = '\0';

      /* Whole response*/
      char response[1024];
      memset(response, '\0', sizeof response);

      if(DEBUG)cout << "Start read" << std::endl;

      do {
          n = read( fd, &buf, 1 );
          sprintf( &response[spot], "%c", buf );
          spot += n;
          if(DEBUG) cout << "Spot: " << spot << std::endl;
      } while( buf != '\n' && n > 0);

      if(DEBUG){
        if (n < 0) {
          std::cout << "Error reading: " << strerror(errno) << std::endl;
        }
        else if (n == 0) {
            std::cout << "Read nothing!" << std::endl;
        }
        else {
            std::cout << c << " --> Response: " << response << std::endl;
        }
      }
      c++;

      if(spot != 10){
        if(DEBUG)printf("BAD BYTEES!!!!!!!\n");
        return;
      }

      
      aProtocol.pacmanDirection = response[0] - '0';
      aProtocol.pacmanVision = response[6] - '0';
      aProtocol.pacmanVelocity = response[2] - '0';
      aProtocol.pacmanAmbient = response[4] - '0';
      aProtocol.state = true;
    }

    void readData(){

      //write(fd, "1", 1);
      while(true){
        readDataTask();
        /*printf("Srart thread\n");
        thread t1(&ArduinoComm::readDataTask,this);
        t1.join();
        printf("End thread\n");*/
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
