# DataStream

A modern binary data serializer in C++ 23 standard, almost like Qt's `QDataStream` class but with less functions and simple.

## Usage

```cpp
#include <cstdint>
#include <fstream>
#include <vector>

#include "DataStream/DataStream.hpp"


int main() {
    auto a = DataStream::byteswap(static_cast<uint16_t>(1));

    // using with containers
    std::vector<uint8_t> b(20,0);
    DataStream::Stream<DataStream::Mode::Input> bis(b);
    uint16_t bi = 1;
    bis << bi; // 0x01 0x00
    cis.set(bi, 0); // 0x01 0x00
    DataStream::Stream bos(b);
    uint16_t bo = 0;
    bos >> bo; // 0x01 0x00 = 1
    cos.get(bo, 0); // 0x01 0x00 = 1


    // using with raw arrays
    uint8_t c[20] = {0};
    DataStream::Stream<DataStream::Mode::Input, std::endian::big> cis(c);
    uint16_t ci = 1;
    cis << ci; // 0x00 0x01
    cis.set(ci, 0); // 0x00 0x01
    DataStream::Stream<DataStream::Mode::Output, std::endian::little> cos(c);
    uint16_t co = 0;
    cos >> co; // 0x01 0x00 = 256
    cos.get(co, 0); // 0x01 0x00 = 256


    // using with fstream
    std::fstream d("./test.bin", std::ios::in | std::ios::out | std::ios::trunc | std::ios::binary);
    DataStream::Stream<DataStream::Mode::Input, std::endian::big> fis(d);
    uint16_t fi = 1;
    fis << fi; // 0x00 0x01
    d.seekg(std::ios::beg);
    DataStream::Stream<DataStream::Mode::Output, std::endian::little> fos(d);
    uint16_t fo = 0;
    fos >> fo; // 0x01 0x00 = 256
    d.close();


    return 0;
}
```

# [GPL v3 License](./LICENSE)

Copyright (C) 2024 Pritam Halder

This program is free software: you can redistribute it and/or modify it under the terms of the
GNU General Public License as published by the Free Software Foundation, either version 3 of the
License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program.
If not, see <https://www.gnu.org/licenses/>.
