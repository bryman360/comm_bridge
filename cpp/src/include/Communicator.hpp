#pragma once
#ifndef COMMUNICATOR_H
#define COMMUNICATOR_H
#include "Messages.hpp"

#ifdef ARDUINO
#ifndef SMALL_MESSAGING
#define SMALL_MESSAGING 1
#endif //SMALL_MESSAGING
#endif //ARDUINO


#define MAX_ID 16
#ifndef SMALL_MESSAGING
#define DATA_BUFFER_SIZE 512
#else
#define DATA_BUFFER_SIZE 256
#endif //SMALL_MESSAGING
#define MSG_HISTORY_SIZE 3

enum CommunicatorState {
    WAITING_FOR_START,
    COLLECTING_HEADER,
    COLLECTING_MSG,
    PARSING_MSG
};

class Communicator {
    protected:
        unsigned char comm_id;
        InboundMessage in_msg_history[MSG_HISTORY_SIZE];
        bool is_new_msg[MSG_HISTORY_SIZE] = {false};

        int msg_start_idx = 0;
        int msg_end_idx = 0;
        int in_msg_history_idx = 0;
        int next_msg_to_view_idx = 0;

        CommunicatorState state = WAITING_FOR_START;
        void parseMessage();
        void addInboundMessageToHistory(InboundMessage &recvd_msg);

        virtual int send(OutboundMessage msg_to_send);
        virtual int recv(char (&buf)[MAX_MESSAGE_SIZE], int& n);


        int data_buffer_idx = 0;
        char data_buffer[DATA_BUFFER_SIZE] = {'b'}; 

    public:
        Communicator(unsigned char id);
        ~Communicator();

        int sendMessage(unsigned char recvr_id, unsigned char cmd_id, unsigned char cmd_flags, const char* msg = "");
        int recvData();

        bool newMessageAvailable();
        InboundMessage getNewMessage();
};
#endif //COMMUNICATOR_H
