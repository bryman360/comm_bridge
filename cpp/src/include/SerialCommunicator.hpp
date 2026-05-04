#pragma once
#ifndef SERIAL_COMMUNICATOR_H
#define SERIAL_COMMUNICATOR_H
#include "Communicator.hpp"
#include <termios.h>
#include <iostream>
#include <iomanip>
#include <cstring>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>

class SerialCommunicator : public Communicator {
    private:
        std::string port;
        bool open_connection = false;
        int baud_rate;
        int file_descriptor = -1;
        struct pollfd pfd;

        int send(OutboundMessage msg_to_send) override;
        int recv(char (&buf)[MAX_MESSAGE_SIZE], int& n) override;

    public:
        SerialCommunicator(unsigned char id, std::string port, int baud_rate = B9600);
        ~SerialCommunicator();

        int openSerialPort();
        void closeSerialPort();
        bool isConnected() {return open_connection && file_descriptor >= 0;};
        bool dataAvailable();
};
#endif //SERIAL_COMMUNICATOR_H