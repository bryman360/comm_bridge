#include "SerialCommunicator.hpp"

SerialCommunicator::SerialCommunicator(unsigned char id, std::string port, int baud_rate) : Communicator(id), port(port), baud_rate(baud_rate) {
    openSerialPort();
};

SerialCommunicator::~SerialCommunicator() {};

int SerialCommunicator::send(OutboundMessage msg_to_send) {
    if (!isConnected()) {
        std::cerr << "Serial port must be open to send messages. DevID " << comm_id << " to " << port << std::endl;
        return -1;
    }
    const char* msg_chars = msg_to_send.chars();
    std::cout << "Sending: " << msg_to_send.hex(0, 0) << ", size: " << msg_to_send.getLength() << std::endl;
    write(file_descriptor, msg_chars, msg_to_send.getLength());
    return 0;
}

int SerialCommunicator::recv(char (&buf)[MAX_MESSAGE_SIZE], int& n) {
    if (!isConnected()) {
        std::cerr << "Serial port must be open to read messages. DevID " << comm_id << " to " << port << std::endl;
        return 1;
    }
    int ret = poll(&pfd, 1, 1000);
    if (ret == -1) {
        std::cerr << "Error polling from serial port." << std::endl;;
        return -1;
    } else if (ret == 0) {
        return 0;
    } else {
        if (pfd.revents & POLLIN) {
            // std::cout << "RECVING: ";
            n = read(file_descriptor, buf, sizeof(buf));
            // for (int i = 0; i < n; i++) {std::cout << std::setfill('0') << std::setw(2) << std::hex << int(buf[i]);}
            // std::cout << std::endl;
            if (n < 0) {
                std::cerr << "Error reading from serial port." << std::endl;
                return -1;
            }
        }
        return 0;
    }
}

int SerialCommunicator::openSerialPort() {
    int fd = open(port.c_str(), O_RDWR | O_NOCTTY | O_SYNC);
    if (fd < 0) {
        std::cerr << "Error opening " << port << ": " << strerror(errno) << std::endl;
        return -1;
    }
    file_descriptor = fd;
    try {
        struct termios tty;
        if (tcgetattr(fd, &tty) != 0) {
            throw "Error from tcsetattr: " + std::to_string(errno);
        }

        cfsetospeed(&tty, baud_rate);

        tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;
        tty.c_iflag &= ~IGNBRK;
        tty.c_oflag = 0;
        tty.c_cc[VMIN] = 0;
        tty.c_cc[VTIME] = 2;

        tty.c_iflag &= ~(IXON | IXOFF | IXANY);
        tty.c_cflag |= (CLOCAL | CREAD);
        tty.c_cflag &= ~(PARENB | PARODD);
        tty.c_cflag &= ~CSTOPB;
        tty.c_cflag &= ~CRTSCTS;

        if (tcsetattr(fd, TCSANOW, &tty) != 0) {
            throw "Error from tcsetattr: " + std::to_string(errno);
        }

        if (tcflush(fd, TCIOFLUSH) == -1) {
            throw "Error from tcflush: " + std::to_string(errno);
        }
        pfd.fd = fd;
        pfd.events = POLLIN;

    } catch (...) {
        std::cerr << "Couldn't open serial port" << std::endl;
        closeSerialPort();
        return -1;
    }
    std::cout << "Serial port open: " << port << std::endl;
    open_connection = true;
    return 0;
}

void SerialCommunicator::closeSerialPort() {
    close(file_descriptor);
    open_connection = false;
}

bool SerialCommunicator::dataAvailable() {
    if (!isConnected()) return false;
    int bytesAvailable;
    if (ioctl(file_descriptor, FIONREAD, &bytesAvailable) == 0){
        if (bytesAvailable > 0) return true;
    }
    return false;
}