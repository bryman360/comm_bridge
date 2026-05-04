# Communication Bridge

Meant to help bridge communications by providing a common API between different communication protocols (after the lower level protocols are implemented).

This uses pybind11 as a submodule to create a Python wrapper. Thus, this needs to be cloned with submodules in mind. (Ex: With the `--recurse-submodules` flag on initial clone or using `git submodule update --init --recursive` if this repo was initially cloned without submodules)

This build is done using Cmake. The Python module that gets built is located in the `{$COMM_BRIDGE_DIR}/python/lib` folder.

## Abstraction Implementation

For each communication protocol (i.e. Serial, I2C, etc.), a lower level definition needs to be implemented. The Serial protocol has already been implemented and is an example of how it is done.

Abstraction is implemented by:
    1. Creating a subclass of Communicator
    2. In the subclass, define `send(OutboundMessage msg_to_send)` and `recv(char (&buf)[MAX_MESSAGE_SIZE], int& n)` by implementing the low level method of communication (ex: for Arduino, use `Serial.print` and `Serial.read`)
    3. Add any other necessary handling of lower level communication protocol (ex: opening/closing ports, checking for data availability, etc.)

## Message Structure:

Start Flag (1 Byte) [Default is the character '~']
Sender and Receiver (1 Byte)
Command and Command Flags (1 Byte)
Payload Length (1 Byte)
Payload (0-250 Bytes by Default, 0-58 Bytes for Small Messaging [such as for Arduino])
Checksum (1 Byte)
End Flag (1 Byte) [Default is the character '\n']

## C++ Serial Communication Usage Example:

Init:

        SerialCommunicator scom(0, "/dev/ttyACM0", 9600);

Sending:

        const int recvr_id = 2;
        const int cmd_id = 1;
        const int cmd_flags = 0x00;
        std::string message = "MESSAGE TO SEND";
        scom.sendMessage(recvr_id, cmd_id, cmd_flags, message.c_str());

Receiving:

        while (scom.dataAvailable()) {
            scom.recvData();
            
            if (scom.newMessageAvailable()) {
                InboundMessage new_msg = scom.getNewMessage();
                char sender_and_receiver = new_msg.getSenderAndReceiver();
                char cmd_and_flags = new_msg.getCmdAndCmdFlags();
                char payload_length = new_msg.getPayloadLength();
                const char *payload = new_msg.getPayload();
                char checksum = new_msg.getChecksum();

                std::cout << "New Message Sender:" << int(sender_and_receiver >> 4) << std::endl;
                std::cout << "New Message Reciever:" << int(sender_and_receiver & 0xF) << std::endl;
                std::cout << "New Message Command:" << int(cmd_and_flags >> 4) << std::endl;
                std::cout << "New Message Command Flags:" << int(cmd_and_flags & 0xF) << std::endl;
                std::cout << "New Message Payload Length:" << int(payload_length) << std::endl;
                std::cout << "New Message Payload:" << payload << std::endl;
                std::cout << "New Message Checksum:" << int(checksum) << std::endl;
            }
        }

## Python Serial Communication Usage Example:

Init:

        scom = Comm.SerialCommunicator(0, '/dev/ttyACM0', 9600)
Sending:

        recvr_id = 2
        cmd_id = 1
        cmd_flags = 0x00
        message = "MESSAGE TO SEND"
        scom.sendMessage(recvr_id, cmd_id, cmd_flags, message)

Receiving:

        if scom.dataAvailable():
            scom.recvData()
            if scom.newMessageAvailable():
                new_msg = scom.getNewMessage()
                sender_and_receiver = new_msg.getSenderAndReceiver()
                cmd_and_flags = new_msg.getCmdAndCmdFlags()
                payload_length = new_msg.getPayloadLength()
                payload = new_msg.getPayload()
                checksum = new_msg.getChecksum()

                print("New Message Sender:", ord(sender_and_receiver) >> 4)
                print("New Message Receiver:", ord(sender_and_receiver) & 0xF)
                print("New Message Command:", ord(cmd_and_flags) >> 4)
                print("New Message Command Flags:", ord(cmd_and_flags) & 0xF)
                print("New Message Payload Length:", ord(payload_length))
                print("New Message Payload:", payload)
                print("New Message Checksum:", ord(checksum))
