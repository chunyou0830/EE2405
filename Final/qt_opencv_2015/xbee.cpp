
#include "xbee.h"


Xbee::Xbee (char *devPath) {

    serial_d = openserial("/dev/ttyUSB0");

    // ----- Begin of setup serial ports
    tcgetattr(serial_d, &soptions_org);
    tcgetattr(serial_d, &soptions);

    speed = B9600; // Speed options: B19200, B38400, B57600, B115200
    cfsetispeed(&soptions, speed);
    cfsetospeed(&soptions, speed);

    // Enable the reciver and set local mode...
    soptions.c_cflag |= ( CLOCAL | CREAD );
    // Setting Parity Checking (8N1)
    soptions.c_cflag &= ~PARENB;
    soptions.c_cflag &= ~CSTOPB;
    soptions.c_cflag &= ~CSIZE;
    soptions.c_cflag |= CS8;

    // Local setting
    //soptions.c_lflag = (ICANON | ECHO | ECHOE); //canonical
    soptions.c_lflag =  ~(ICANON | ECHO | ECHOE | ISIG); //noncanonical

    // Input setting
    //soptions.c_iflag |= (IXON | IXOFF | IXANY); //software flow control
    soptions.c_iflag |= (INPCK | ISTRIP);
    soptions.c_iflag = IGNPAR;

    // Output setting
    soptions.c_oflag = 0;
    soptions.c_oflag &= ~OPOST;

    // Read options
    soptions.c_cc[VTIME] = 0;
    soptions.c_cc[VMIN] = 1; //transfer at least 1 character or block

    // Apply setting
    tcsetattr(serial_d, TCSANOW, &soptions);
    // ----- End of setup serial ports


    // restore setting and close
    //tcsetattr(serial_d, TCSANOW, &soptions_org);
    //close(serial_d);
    //return 0;

}
Xbee::~Xbee(){
    delete this;
}

void Xbee::clean_buffer () {
    fflush(stdin);
    while (getchar() != '\n');
}

int Xbee::openserial (char *sdevfile) {
    int _serial_d = open(sdevfile, O_RDWR | O_NOCTTY);
    if (_serial_d == -1) perror("Unable to open device\n");
        return _serial_d;
}

void Xbee::sending(){
    sent_c = write(serial_d, &command, 1); // Send command
    tcdrain(serial_d);
    usleep(2000000); // Wait for response
    memset(recv_buf, '\0', BUFLEN);
    recv_c = read(serial_d, recv_buf, BUFLEN); // Get response message
    tcdrain(serial_d);
}

void Xbee::run(){
     sent_c = write(serial_d, &command, 1); // Send command
     tcdrain(serial_d);
     //usleep(2000000); // Wait for response
     //memset(recv_buf, '\0', BUFLEN);
     //recv_c = read(serial_d, recv_buf, BUFLEN); // Get response message
     //tcdrain(serial_d);
}

void Xbee::setDir(char in){
    command = in;
}
