#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "include/Communicator.hpp"
#include "include/SerialCommunicator.hpp"

namespace py = pybind11;


PYBIND11_MODULE(Communicator, m, py::mod_gil_not_used()) {
    m.doc() = "C++ Communicator plugin for Python";
    py::class_<Communicator>(m, "Communicator")
        .def(py::init<unsigned char>())
        .def("sendMessage", &Communicator::sendMessage, py::arg("recvr_id"), py::arg("cmd_id"), py::arg("cmd_flags"), py::arg("msg") = "")
        .def("recvData", &Communicator::recvData)
        .def("newMessageAvailable", &Communicator::newMessageAvailable)
        .def("getNewMessage", &Communicator::getNewMessage);

    py::class_<SerialCommunicator, Communicator>(m, "SerialCommunicator")
        .def(py::init<unsigned char, std::string, int>(), py::arg("id"), py::arg("port"), py::arg("baudrate") = B9600)
        .def("openSerialPort", &SerialCommunicator::openSerialPort)
        .def("closeSerialPort", &SerialCommunicator::closeSerialPort)
        .def("isConnected", &SerialCommunicator::isConnected)
        .def("dataAvailable", &SerialCommunicator::dataAvailable);
    
    py::class_<InboundMessage>(m, "InboundMessage")
        .def(py::init<const char*>())
        .def("print", &InboundMessage::print, py::arg("pretty") = false)
        .def("hex", &InboundMessage::hex, py::arg("pretty") = false, py::arg("include_opener") = false)
        .def("getSenderAndReceiver", &InboundMessage::getSenderAndReceiver)
        .def("getCmdAndCmdFlags", &InboundMessage::getCmdAndCmdFlags)
        .def("getPayloadLength", &InboundMessage::getPayloadLength)
        .def("getPayload", &InboundMessage::getPayload)
        .def("getChecksum", &InboundMessage::getChecksum);

};