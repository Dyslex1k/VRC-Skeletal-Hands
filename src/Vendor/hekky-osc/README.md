# Hekky OSC

An OSC Library for C++, initially based on [CoreOSC](https://github.com/PaciStardust/CoreOSC-UTF8).

### Motivation

<p align="center">
<img alt="XKCD 927 - Standards" src="https://user-images.githubusercontent.com/7695629/204110847-b113dea6-32cc-48e5-8b40-8e1f36ab085b.png">
</p>

> [XKCD 927](https://xkcd.com/927/)

OSC libraries in C++ suck. The APIs are either extremely verbose, the library has too many dependencies and is a nightmare to get working, or difficult to use without leaking memory.

I'm also using this library as an excuse to learn C++ better and improve my ability to write good C++ code.

### License

This project is licensed under the MIT license. CoreOSC is also licensed under the MIT license.

---

# Example

```cpp
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
```

## Supported platforms

| Platform | Supported |
| -------- | --------- |
| Windows  | ✅         |
| MacOS    | ❌         |
| Linux    | ❌         |

## Goal

This library aims to provide a simple and easy to use API for using OSC. It aims to conform to the entire OSC 1.0 specification. (Not yet achieved)

| Feature                                         | Supported |
| ----------------------------------------------- | --------- |
| Sending OSC messages                            | ✅         |
| Receiving OSC messages                          | ❌         |
| Sending primitive data types (int, float, etc.) | ✅         |
| 32-bit RGBA color                               | ❌         |
| OSC Timetag                                     | ❌         |
| MIDI                                            | ❌         |
| Null                                            | ❌         |
| Arrays                                          | ❌         |
| Bundles                                         | ❌         |
| ASCII Character                                 | ❌         |
