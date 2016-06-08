#ifndef XBEE_H
#define XBEE_H

#include <QThread>

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <limits.h>
#include <unistd.h>
#include <sys/select.h>


#define BUFLEN (255)
class Xbee : public QThread
{
    Q_OBJECT

public:
    Xbee(char *devPath);
    ~Xbee();

    void setDir(char in);
    void sending();
protected:
    void run();

private:
    int serial_d;
    speed_t speed;
    struct termios soptions, soptions_org;
    char command;
    //unsigned char send_buf[BUFLEN];
    //unsigned char recv_buf[BUFLEN];
    unsigned char send_buf[BUFLEN];
    unsigned char recv_buf[BUFLEN];
    int sent_c, recv_c;

    int openserial(char *sdevfile);
    void clean_buffer();


};

#endif // XBEE_H
