#include <bit>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <numeric>
#include <stdfloat>
#include <vector>

#include "DataStream/DataStream.hpp"



template<typename T, typename DS>
void benchmark(
    const uint64_t size,
    DS& ds,
    const std::string& buffer_type,
    const std::string& value_type
) {
    std::vector<uint64_t> elpased;

    for (size_t c = 0; c < 100; ++c) {
        auto s1 = std::chrono::steady_clock::now();
        for (uint64_t i = 0; i < size; ++i) {
            ds.set(static_cast<T>(i), sizeof(T) * i);
        }
        auto e1 = std::chrono::steady_clock::now();
        auto t1 = std::chrono::duration_cast<std::chrono::milliseconds>(e1 - s1).count();

        elpased.push_back(t1);
    }

    std::cout << "- Benchmark -"
        << "\n" << "DataStream Buffer Type : " << buffer_type
        << "\n" << "Value Type : " << value_type
        << "\n" << "No. of Data Inserted : " << size
        << "\n" << "Average (of 100 runs) Time Taken : " << std::reduce(elpased.cbegin(), elpased.cend()) / 100 << "ms"
    << "\n\n";
}




int main() {
    const uint64_t size = 1'000'000;

    using dt1 = uint8_t;
    std::vector<uint8_t> b1(size * sizeof(dt1), 0x00);
    DataStream::Stream<DataStream::Mode::Input, std::endian::little> ds1(b1);
    benchmark<dt1, decltype(ds1)>(size, ds1, "std::vector<uint8_t>", "uint8_t");

    using dt2 = uint16_t;
    std::vector<uint8_t> b2(size * sizeof(dt2), 0x00);
    DataStream::Stream<DataStream::Mode::Input, std::endian::little> ds2(b2);
    benchmark<dt2, decltype(ds2)>(size, ds2, "std::vector<uint8_t>", "uint16_t");

    using dt3 = uint32_t;
    std::vector<uint8_t> b3(size * sizeof(dt3), 0x00);
    DataStream::Stream<DataStream::Mode::Input, std::endian::little> ds3(b3);
    benchmark<dt3, decltype(ds3)>(size, ds3, "std::vector<uint8_t>", "uint32_t");

    using dt4 = uint64_t;
    std::vector<uint8_t> b4(size * sizeof(dt4), 0x00);
    DataStream::Stream<DataStream::Mode::Input, std::endian::little> ds4(b4);
    benchmark<dt4, decltype(ds4)>(size, ds4, "std::vector<uint8_t>", "uint64_t");

    using dt5 = int8_t;
    std::vector<uint8_t> b5(size * sizeof(dt5), 0x00);
    DataStream::Stream<DataStream::Mode::Input, std::endian::little> ds5(b5);
    benchmark<dt5, decltype(ds5)>(size, ds5, "std::vector<uint8_t>", "int8_t");

    using dt6 = int16_t;
    std::vector<uint8_t> b6(size * sizeof(dt6), 0x00);
    DataStream::Stream<DataStream::Mode::Input, std::endian::little> ds6(b6);
    benchmark<dt6, decltype(ds6)>(size, ds6, "std::vector<uint8_t>", "int16_t");

    using dt7 = int32_t;
    std::vector<uint8_t> b7(size * sizeof(dt7), 0x00);
    DataStream::Stream<DataStream::Mode::Input, std::endian::little> ds7(b7);
    benchmark<dt7, decltype(ds7)>(size, ds7, "std::vector<uint8_t>", "int32_t");

    using dt8 = int64_t;
    std::vector<uint8_t> b8(size * sizeof(dt8), 0x00);
    DataStream::Stream<DataStream::Mode::Input, std::endian::little> ds8(b8);
    benchmark<dt8, decltype(ds8)>(size, ds8, "std::vector<uint8_t>", "int64_t");

    using dt9 = std::float16_t;
    std::vector<uint8_t> b9(size * sizeof(dt9), 0x00);
    DataStream::Stream<DataStream::Mode::Input, std::endian::little> ds9(b9);
    benchmark<dt9, decltype(ds9)>(size, ds9, "std::vector<uint8_t>", "float16_t");

    using dt10 = std::float32_t;
    std::vector<uint8_t> b10(size * sizeof(dt10), 0x00);
    DataStream::Stream<DataStream::Mode::Input, std::endian::little> ds10(b10);
    benchmark<dt10, decltype(ds10)>(size, ds10, "std::vector<uint8_t>", "float32_t");

    using dt11 = std::float64_t;
    std::vector<uint8_t> b11(size * sizeof(dt11), 0x00);
    DataStream::Stream<DataStream::Mode::Input, std::endian::little> ds11(b11);
    benchmark<dt11, decltype(ds11)>(size, ds11, "std::vector<uint8_t>", "float64_t");

    using dt12 = std::float128_t;
    std::vector<uint8_t> b12(size * sizeof(dt12), 0x00);
    DataStream::Stream<DataStream::Mode::Input, std::endian::little> ds12(b12);
    benchmark<dt12, decltype(ds12)>(size, ds12, "std::vector<uint8_t>", "float128_t");

    return 0;
}
