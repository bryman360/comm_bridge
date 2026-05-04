#include "include/Messages.hpp"

InboundMessage::InboundMessage(const char* message) {
    void_msg = false;
    start_flag = message[0];
    sender_and_recvr = message[1];
    cmd_and_cmd_flags = message[2];
    payload_length = message[3];
    if (payload_length > MAX_PAYLOAD_SIZE) {
        void_msg = true;
    } else {
        strncpy(payload, message + MSG_HEADER_BYTES, payload_length);
        payload[payload_length] = '\0';
        checksum = message[MSG_HEADER_BYTES + payload_length];
        end_flag = message[MSG_HEADER_BYTES + payload_length + 1];
    }

    if (!void_msg && start_flag != START_FLAG) {
        void_msg = true;
    }
    else if (!void_msg && end_flag != END_FLAG) {
        void_msg = true;
    }
    else if (!void_msg && !validateChecksum()) {
        void_msg = true;
    }

};

InboundMessage::~InboundMessage() {};

bool InboundMessage::validateChecksum() {
    unsigned char calculatedChecksum = 0;
    calculatedChecksum = calculatedChecksum ^ int(start_flag);
    calculatedChecksum = calculatedChecksum ^ int(sender_and_recvr);
    calculatedChecksum = calculatedChecksum ^ int(cmd_and_cmd_flags);
    calculatedChecksum = calculatedChecksum ^ int(payload_length);
    for (int i = 0; i < payload_length; i++) {
        calculatedChecksum = calculatedChecksum ^ int(payload[i]);
    }
    calculatedChecksum = calculatedChecksum ^ int(end_flag);
    calculatedChecksum = calculatedChecksum | 1;

    if (calculatedChecksum != int(checksum)) return false;
    else return true;
}

#ifndef ARDUINO
std::string InboundMessage::print(bool pretty) {
    std::stringstream output;
    if (pretty) {
        output << "SFlag: " <<  start_flag;
        output << ", Sender/Recvr: " << sender_and_recvr;
        output << ", Cmd/CmdFlags: " << cmd_and_cmd_flags;
        output << ", Payload Length: " << payload_length;
        output << ", Payload: " <<  payload;
        output << ", Checksum: " << checksum;
        output << ", EFlag: " << end_flag;
    }
    else {
        output << start_flag;
        output << sender_and_recvr;
        output << cmd_and_cmd_flags;
        output << payload_length;
        output << payload;
        output << checksum;
        output << end_flag;
    }
    return output.str();
};

std::string InboundMessage::hex(bool pretty, bool include_opener) {
    std::stringstream output;
    std::string opener = include_opener ? "0x" : "";
    if (pretty) {
        output << "Start Flag: " << opener << std::setfill('0') << std::setw(2) << std::hex << int(start_flag) << std::endl;
        output << "Sender: " << opener << std::setfill('0') << std::setw(2) << std::hex << int(sender_and_recvr) << std::endl;
        output << "Cmd/RespFlag: " << opener << std::setfill('0') << std::setw(2) << std::hex << int(cmd_and_cmd_flags) << std::endl;
        output << "Payload Length: " << opener << std::setfill('0') << std::setw(2) << std::hex << int(payload_length) << std::endl;
        output << "Payload: " << opener;
        for (int i = 0; i < payload_length; i++) {
            output << std::setfill('0') << std::setw(2) << std::hex << int(payload[i]);
        }
        output << std::endl;
        output << "Checksum: " << opener << std::setfill('0') << std::setw(2) << std::hex << int(checksum) << std::endl;
        output << "End Flag: " << opener << std::setfill('0') << std::setw(2) << std::hex << int(end_flag) << std::endl;

    }
    else {
        output << opener;
        output << std::setfill('0') << std::setw(2) << std::hex << int(start_flag);
        output << std::setfill('0') << std::setw(2) << std::hex << int(sender_and_recvr);
        output << std::setfill('0') << std::setw(2) << std::hex << int(cmd_and_cmd_flags);
        output << std::setfill('0') << std::setw(2) << std::hex << int(payload_length);
        for (int i = 0; i < payload_length; i++) {
            output << std::setfill('0') << std::setw(2) << std::hex << int(payload[i]);
        }
        output << std::setfill('0') << std::setw(2) << std::hex << int(checksum);
        output << std::setfill('0') << std::setw(2) << std::hex << int(end_flag);
    }

    return output.str();
};

std::string OutboundMessage::hex(bool pretty, bool include_opener) {
    std::stringstream output;
    std::string opener = include_opener ? "0x" : "";
    if (pretty) {
        output << "Start Flag: " << opener << std::setfill('0') << std::setw(2) << std::hex << int(START_FLAG) << std::endl;
        output << "Sender/Recvr: " << opener << std::setfill('0') << std::setw(2) << std::hex << ((sender_id << 4) | recvr_id) << std::endl;
        output << "Cmd/CmdFlags: " << opener << std::setfill('0') << std::setw(2) << std::hex << ((cmd_id << 4) | cmd_flags) << std::endl;
        output << "Payload Length: " << opener << std::setfill('0') << std::setw(2) << std::hex << int(payload_length) << std::endl;
        output << "Payload: " << opener;
        for (int i = 0; i < payload_length; i++) {
            output << std::setfill('0') << std::setw(2) << std::hex << int(buf[i + MSG_HEADER_BYTES]);
        }
        output << std::endl;
        output << "Checksum: " << opener << std::setfill('0') << std::setw(2) << std::hex << calculateChecksum() << std::endl;
        output << "End Flag: " << opener << std::setfill('0') << std::setw(2) << std::hex << int(END_FLAG) << std::endl;
    }
    else {
        output << opener;
        output << std::setfill('0') << std::setw(2) << std::hex << int(START_FLAG);
        output << std::setfill('0') << std::setw(2) << std::hex << ((sender_id << 4) | recvr_id);
        output << std::setfill('0') << std::setw(2) << std::hex << ((cmd_id << 4) | cmd_flags);
        output << std::setfill('0') << std::setw(2) << std::hex << int(payload_length);
        for (int i = 0; i < payload_length; i++) {
            output << std::setfill('0') << std::setw(2) << std::hex << int(buf[i + MSG_HEADER_BYTES]);
        }
        output << std::setfill('0') << std::setw(2) << std::hex << calculateChecksum();
        output << std::setfill('0') << std::setw(2) << std::hex <<  int(END_FLAG);
    }
    return output.str();
};
#endif //ARDUINO

const char* OutboundMessage::chars() {
    buf[0] = START_FLAG;
    buf[1] = char((sender_id << 4) | recvr_id);
    buf[2] = char((cmd_id << 4) | cmd_flags);
    buf[3] = char(payload_length);
    buf[MSG_HEADER_BYTES + payload_length] = char(calculateChecksum());
    buf[MSG_HEADER_BYTES + payload_length + 1] = END_FLAG;
    buf[MSG_HEADER_BYTES + payload_length + 2] = '\0';
    return buf;
}

int OutboundMessage::calculateChecksum() {
    unsigned char calculated_checksum = 0;
    calculated_checksum = calculated_checksum ^ int(START_FLAG);
    calculated_checksum = calculated_checksum ^ ((sender_id << 4) | recvr_id);
    calculated_checksum = calculated_checksum ^ ((cmd_id << 4) | cmd_flags);
    calculated_checksum = calculated_checksum ^ payload_length;
    for (int i = 0; i < payload_length; i++) {
        calculated_checksum = calculated_checksum ^ int(buf[i + MSG_HEADER_BYTES]);
    }
    calculated_checksum = calculated_checksum ^ int(END_FLAG);
    calculated_checksum = calculated_checksum | 1;
    return calculated_checksum;
};

void OutboundMessage::setPayload(const char* message) {
    int message_length = strlen(message);
    if (message_length >= MAX_PAYLOAD_SIZE) {
        strncpy(buf + MSG_HEADER_BYTES, message, MAX_PAYLOAD_SIZE);
        payload_length = MAX_PAYLOAD_SIZE;
    }
    else {
        strncpy(buf + MSG_HEADER_BYTES, message, message_length);
        payload_length = (unsigned char)message_length;
    }
};

void OutboundMessage::setSenderID(unsigned char id) {
    sender_id = id % 16;
};

void OutboundMessage::setRecvrID(unsigned char id) {
    recvr_id = id % 16;
};

void OutboundMessage::setCmdID(unsigned char id) {
    cmd_id = id % 16;
};

void OutboundMessage::setCmdFlags(unsigned char flag_bits) {
    cmd_flags = flag_bits % 16;
};

int OutboundMessage::getLength() {
    return MSG_HEADER_BYTES + 2 + payload_length;
}