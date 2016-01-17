class ArduinoComm2{

  public:
    int USB;
    int i = 0;

    ArduinoComm2(){
      USB = open( "/dev/ttyACM0", O_RDWR| O_NOCTTY );
      struct termios tty;
      struct termios tty_old;
      memset (&tty, 0, sizeof tty);

      /* Error Handling */
      if ( tcgetattr ( USB, &tty ) != 0 ) {
         std::cout << "Error " << errno << " from tcgetattr: " << strerror(errno) << std::endl;
      }

      /* Save old tty parameters */
      tty_old = tty;

      /* Set Baud Rate */
      cfsetospeed (&tty, (speed_t)B9600);
      cfsetispeed (&tty, (speed_t)B9600);

      /* Setting other Port Stuff */
      tty.c_cflag     &=  ~PARENB;            // Make 8n1
      tty.c_cflag     &=  ~CSTOPB;
      tty.c_cflag     &=  ~CSIZE;
      tty.c_cflag     |=  CS8;

      /* Canonical mode */
      //tty.c_lflag |= ICANON;

      tty.c_cflag     &=  ~CRTSCTS;           // no flow control
      tty.c_cc[VMIN]   =  1;                  // read doesn't block
      tty.c_cc[VTIME]  =  5;                  // 0.5 seconds read timeout
      tty.c_cflag     |=  CREAD | CLOCAL;     // turn on READ & ignore ctrl lines

      /* Make raw */
      cfmakeraw(&tty);

      /* Flush Port, then applies attributes */
      tcflush( USB, TCIFLUSH );
      if ( tcsetattr ( USB, TCSANOW, &tty ) != 0) {
         std::cout << "Error " << errno << " from tcsetattr" << std::endl;
      }
    }

    void write_(){
      unsigned char cmd[] = "1\r";
      int n_written = 0,
          spot = 0;

      //n_written = write( USB, "1\n", 1 );
      //printf("WRITE %i \n", n_written);

          
      do {
          n_written = write( USB, &cmd[spot], 1 );
          printf("WRITE %i \n", n_written);
          spot += n_written;
      } while (cmd[spot-1] != '\r' && n_written > 0);
    }

    void read_(){
      int n = 0,
          spot = 0;
      char buf = '\0';

      /* Whole response*/
      char response[64];
      memset(response, '\0', sizeof response);

      //n = read(USB, response, 64);

      do {
          n = read( USB, &buf, 1 );
          
          sprintf( &response[spot], "%c", buf );
          spot += n;
      } while( buf != '\n' && n > 0);

      if (n < 0) {
          std::cout << "Error reading: " << strerror(errno) << std::endl;
      }
      else if (n == 0) {
          std::cout << "Read nothing!" << std::endl;
      }
      else {
          std::cout << i << " Response: " << response << std::endl;
      }
        //printf("%i ---> %i bytes read, buffer contains: %s\n\n", i, n, response);
      i++;

      /*aProtocol.pacmanDirection = response[0] - '0';
      aProtocol.pacmanVision = response[6] - '0';
      aProtocol.pacmanVelocity = response[2] - '0';
      aProtocol.pacmanAmbient = response[4] - '0';
      aProtocol.state = true;*/

    }

    void startCallibration(){
      
    }

    void readDataTask(){
      cout << "Start read: "<< time(0) << std::endl;
      read_();
      cout << "End read: "<< time(0) << std::endl;
    }

    void readData(){
      write_();
      while(true){
        thread t1(&ArduinoComm2::readDataTask,this);
        t1.join();
      }
    }

    void startProcess(){
      thread t1(&ArduinoComm2::readData,this);
      t1.detach();
    }

};