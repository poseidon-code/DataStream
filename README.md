# DataStream

A modern binary data serializer in C++ 23 standard, almost like Qt's `QDataStream` class but with less functions and simple.

## Usage

```cpp
#include <cstdint>
#include <fstream>
#include <stdfloat>
#include <vector>

#include "DataStream/DataStream.hpp"
#include "DataStream/FixedPointQuantizer.hpp"


int main() {
    auto a = DataStream::byteswap(static_cast<uint16_t>(1));

    // using with containers
    std::vector<uint8_t> b(20,0);
    DataStream::Stream<DataStream::Mode::Output> bos(b);
    uint16_t bo = 1;
    bos << bo; // 0x01 0x00
    cis.set(bo, 0); // 0x01 0x00
    DataStream::Stream bis(b);
    uint16_t bi = 0;
    bis >> bi; // 0x01 0x00 = 1
    cos.get(bi, 0); // 0x01 0x00 = 1


    // using with raw arrays
    uint8_t c[20] = {0};
    DataStream::Stream<DataStream::Mode::Output, std::endian::big> cos(c);
    uint16_t co = 1;
    cos << co; // 0x00 0x01
    cos.set(co, 0); // 0x00 0x01
    DataStream::Stream<DataStream::Mode::Input, std::endian::little> cis(c);
    uint16_t ci = 0;
    cis >> ci; // 0x01 0x00 = 256
    cis.get(ci, 0); // 0x01 0x00 = 256


    // using with fstream
    std::fstream e("./test.bin", std::ios::in | std::ios::out | std::ios::trunc | std::ios::binary);
    DataStream::Stream<DataStream::Mode::Output, std::endian::big> eos(e);
    uint16_t eo = 1;
    eos << eo; // 0x00 0x01
    e.seekg(std::ios::beg);
    DataStream::Stream<DataStream::Mode::Input, std::endian::little> eis(e);
    uint16_t ei = 0;
    eis >> ei; // 0x01 0x00 = 256
    e.close();


    // using fixed point quantizer
    std::float64_t value = 67.9834672;
    DataStream::FixedPointQuantizer<std::float64_t> quantizer(-90.0, 90.0); // low precision on low number of bits
    auto quantized_value = quantizer.to_fpq(value); // quantized value to 64bit signed integer
    std::cout << quantized_value << std::endl;
    std::cout << quantizer.from_fpq(quantized_value) << std::endl; // switched back to floating point value


    return 0;
}
```

# [GPL v3 License](./LICENSE)

DataStream : A modern binary data serializer in C++ 23 standard.
Copyright (C) 2024  Pritam Halder

This program is free software: you can redistribute it and/or modify it under the terms of the
GNU General Public License as published by the Free Software Foundation, either version 3 of the
License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program.
If not, see <https://www.gnu.org/licenses/>.
