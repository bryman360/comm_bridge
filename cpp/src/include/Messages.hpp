#pragma once
#ifndef ARDUINO
#include <string>
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include <iostream>
#else

#ifndef SMALL_MESSAGING
#define SMALL_MESSAGING 1
#endif //SMALL_MESSAGING

#endif //ARDUINO

#include <string.h>

#ifndef MESSAGES_H
#define MESSAGES_H 
#define START_FLAG '~'
#define END_FLAG '\n'
#ifndef SMALL_MESSAGING
#define MAX_MESSAGE_SIZE 256
#else
#define MAX_MESSAGE_SIZE 64
#endif //SMALL_MESSAGING
#define MSG_HEADER_BYTES 4
#define MSG_FOOTER_BYTES 2
#define MAX_PAYLOAD_SIZE MAX_MESSAGE_SIZE - MSG_HEADER_BYTES - MSG_FOOTER_BYTES

class  InboundMessage {
    private:
        bool void_msg;
        char start_flag;
        char sender_and_recvr;
        char cmd_and_cmd_flags;
        char payload_length = 0;
        char payload[MAX_PAYLOAD_SIZE];
        char checksum;
        char end_flag;

        bool validateChecksum();
    public:
        InboundMessage() {void_msg = true;};
        InboundMessage(const char* message);
        ~InboundMessage();

#ifndef ARDUINO
        std::string print(bool pretty);
        std::string hex(bool pretty, bool include_opener);
#endif //ARDUINO

        char getSenderAndReceiver() {return sender_and_recvr;};
        char getCmdAndCmdFlags() {return cmd_and_cmd_flags;};
        char getPayloadLength() {return payload_length;};
        const char* getPayload() {return payload;};
        char getChecksum() {return checksum;};
        bool isMsgVoid() {return void_msg;};
};


class OutboundMessage {
    private:
        unsigned char sender_id;
        unsigned char recvr_id;
        unsigned char cmd_id;
        unsigned char cmd_flags;
        unsigned char payload_length;
        char buf[MAX_MESSAGE_SIZE];

        int calculateChecksum();
    public:
        OutboundMessage() {};
        ~OutboundMessage() {};

#ifndef ARDUINO
        std::string hex(bool pretty, bool include_opener);
#endif //ARDUINO
        const char* chars();

        void setSenderID(unsigned char id);
        void setRecvrID(unsigned char id);
        void setCmdID(unsigned char id);
        void setCmdFlags(unsigned char flag_bits);
        void setPayload(const char* message);

        int getLength();
};
#endif // MESSAGES_H
