#include "include/Communicator.hpp"

Communicator::Communicator(unsigned char id) {
    comm_id = id % MAX_ID;
};

Communicator::~Communicator() {};

int Communicator::sendMessage(unsigned char recvr_id, unsigned char cmd_id, unsigned char cmd_flags, const char* msg) {
    OutboundMessage msg_to_send;
    msg_to_send.setSenderID(comm_id);
    msg_to_send.setRecvrID(recvr_id);
    msg_to_send.setPayload(msg);
    msg_to_send.setCmdID(cmd_id);
    msg_to_send.setCmdFlags(cmd_flags);
    return send(msg_to_send);
}

int Communicator::recvData() {
    char buf[MAX_MESSAGE_SIZE];
    int n = -1;
    if (recv(buf, n) != 0) {
        return -1;
    }
    if (n >= MAX_MESSAGE_SIZE) {
        return -1;
    }
    for (int i = 0; i < n; i++) {
        data_buffer[data_buffer_idx] = buf[i];
        if (state == WAITING_FOR_START && data_buffer[data_buffer_idx] == START_FLAG) {
            msg_start_idx = data_buffer_idx;
            state = COLLECTING_HEADER;
        }
        else if (state == COLLECTING_HEADER) {
            if (data_buffer_idx >= (msg_start_idx + MSG_HEADER_BYTES) % DATA_BUFFER_SIZE) {
                state = COLLECTING_MSG;
            }
        }
        else if (state == COLLECTING_MSG && data_buffer[data_buffer_idx] == END_FLAG) {
            msg_end_idx = data_buffer_idx;
            state = PARSING_MSG;
            parseMessage();
        }
        else if (state == COLLECTING_MSG && ((msg_start_idx + MAX_MESSAGE_SIZE) % DATA_BUFFER_SIZE) == data_buffer_idx) {
            bool new_start_found = false;
            for (int j = 1; j < MAX_MESSAGE_SIZE; j++) {
                if (data_buffer[(msg_start_idx + j) % DATA_BUFFER_SIZE] == START_FLAG) {
                    new_start_found = true;
                    if ((msg_start_idx + j + MSG_HEADER_BYTES) % DATA_BUFFER_SIZE >= data_buffer_idx) {
                        state = COLLECTING_HEADER;
                    } else {
                        state = COLLECTING_MSG;
                    }
                    msg_start_idx = (msg_start_idx + j) % DATA_BUFFER_SIZE;
                    break;
                }
            }
            if (!new_start_found) {
                state = WAITING_FOR_START;
            }
        }
        data_buffer_idx = (data_buffer_idx + 1) % DATA_BUFFER_SIZE;
    }
    return 0;

}

// Function to be overwritten in child classes to send per the child's protocol
int Communicator::send(OutboundMessage msg_to_send) {
#ifndef ARDUINO
    std::cout << msg_to_send.chars() << std::endl;
#endif //ARDUINO
    return 0;
}


// Function to be overwritten in child classes to recv per the child's protocol
int Communicator::recv(char (&buf)[MAX_MESSAGE_SIZE], int& n) {
    int example_checksum = int('~') ^  0x12 ^ 0x11 ^ 0x00 ^ 0x02 ^ int('H') ^ int('I') ^ int('\n');
    example_checksum |= 0x01;
    buf[0] = '~';
    buf[1] = char((0x1 << 4) | comm_id);
    buf[2] = char(0x11);
    buf[3] = char(0x00);
    buf[4] = char(0x02);
    buf[5] = 'H';
    buf[6] = 'I';
    buf[7] = char(example_checksum);
    buf[8] = '\n';
    n = 9;
    return 0;
}

void Communicator::parseMessage() {
    char buf[MAX_MESSAGE_SIZE];
    for (int i = 0; i < MAX_MESSAGE_SIZE; i++) {
        int msg_idx_in_data_buf = (msg_start_idx + i) % DATA_BUFFER_SIZE;
        buf[i] = data_buffer[msg_idx_in_data_buf];
        if (msg_idx_in_data_buf == msg_end_idx) {
            break;
        }
        else if (msg_idx_in_data_buf == data_buffer_idx) {
            state = WAITING_FOR_START;
            return;
        }
    }

    InboundMessage incoming_msg(buf);
    if (!incoming_msg.isMsgVoid()) {

        if (((unsigned char)(incoming_msg.getSenderAndReceiver()) & 0x0F) != comm_id) {
            state = WAITING_FOR_START;
            return;
        }

        addInboundMessageToHistory(incoming_msg);
    } else {
        for (int i = 1; i < MAX_MESSAGE_SIZE; i++) {
            int msg_idx_in_data_buf = (msg_start_idx + i) % DATA_BUFFER_SIZE;
            if (data_buffer[msg_idx_in_data_buf] == START_FLAG ) {
                msg_start_idx = msg_idx_in_data_buf;
                parseMessage();
                return;
            } else if (msg_idx_in_data_buf == data_buffer_idx) {
                break;
            }
        }

    }
    state = WAITING_FOR_START;
}

bool Communicator::newMessageAvailable() {
    return is_new_msg[next_msg_to_view_idx] == true;
}

InboundMessage Communicator::getNewMessage() {
    InboundMessage msg_to_return = in_msg_history[next_msg_to_view_idx];
    is_new_msg[next_msg_to_view_idx] = false;
    next_msg_to_view_idx = (next_msg_to_view_idx + 1) % MSG_HISTORY_SIZE;
    return msg_to_return;
}


void Communicator::addInboundMessageToHistory(InboundMessage &recvd_msg) {
    in_msg_history[in_msg_history_idx] = recvd_msg;
    is_new_msg[in_msg_history_idx] = true;
    in_msg_history_idx = (in_msg_history_idx + 1) % MSG_HISTORY_SIZE;
}