#include <iostream>

#include "hekky-osc.hpp"

int main()
{
    // Open a UDP socket, pointing to localhost on port 9000
    auto udpSender = hekky::osc::UdpSender("127.0.0.1", 9000, 9001);

    auto message = hekky::osc::OscMessage("/osc/test/int32");
    message.Push(12);
    udpSender.Send(message);

    auto message2 = hekky::osc::OscMessage("/osc/test/double");
    message2.Push(3.14159265358979323846264338327950288419716939937510);
    udpSender.Send(message2);

    auto message3 = hekky::osc::OscMessage("/osc/test/int64");
    message3.Push(2345678890123456789LL);
    message3.Push(80LL);
    udpSender.Send(message3);

    auto message4 = hekky::osc::OscMessage("/osc/test/string");
    message4.Push("Hello World!");
    udpSender.Send(message4);

    auto serialPacking = hekky::osc::OscMessage("/osc/vector/float32");
    serialPacking.Push(1.4142135624f); // sqrt(2)
    serialPacking.Push(3.1415926536f); // pi
    serialPacking.Push(2.7182818285f); // e
    udpSender.Send(serialPacking);

    // Alternatively, you can encode the same message like this:
    auto chainPacking = hekky::osc::OscMessage("/osc/vector/float32");
    chainPacking.Push(1.4142135624f).Push(3.1415926536f).Push(2.7182818285f);
    udpSender.Send(chainPacking);

    // Closing it manually isn't needed, it gets closed via the destructor automatically!
    // udpSender.Close();

    std::cout << "Done!\n";
}