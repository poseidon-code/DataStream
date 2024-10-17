#pragma once

#include <algorithm>
#include <bit>
#include <concepts>
#include <cstdint>
#include <fstream>
#include <span>
#include <stdexcept>
#include <type_traits>




namespace DataStream {

template <typename T>
requires std::is_integral_v<T>
inline constexpr T byteswap(T value) {
    return std::byteswap(value);
}

template <typename T>
requires std::is_floating_point_v<T>
inline constexpr T byteswap(T value) {
    using equivalent_integer_type =
        std::conditional_t<
            sizeof(T) == 2, uint16_t, std::conditional_t<
                sizeof(T) == 4, uint32_t, std::conditional_t<
                    sizeof(T) == 8, uint64_t, __uint128_t
                >
            >
        >;

    equivalent_integer_type integer_type = std::bit_cast<equivalent_integer_type>(value);
    equivalent_integer_type swapped = std::byteswap(integer_type);
    return std::bit_cast<T>(swapped);
}


enum class Mode : bool {
    Input = false,
    Output
};


template <
    DataStream::Mode mode = DataStream::Mode::Output,
    std::endian endiannes = std::endian::native
>
class Stream {
private:
    std::span<uint8_t> data;
    uint8_t* underlying_data = nullptr;
    std::fstream* file_stream = nullptr;
    size_t index = 0;

    template <typename T>
    requires std::is_floating_point_v<T> || std::is_integral_v<T>
    inline constexpr T byteswap(T value) {
        return endiannes != std::endian::native ? DataStream::byteswap(value) : value;
    }

    inline void write(std::span<const uint8_t> data)
    requires (mode == DataStream::Mode::Input)
    {
        if (this->file_stream) {
            this->file_stream->write(reinterpret_cast<const char*>(data.data()), data.size());
            if (!*this->file_stream)
                throw std::ios_base::failure("file write failed");
        } else {
            if (this->index + data.size() > this->data.size())
                throw std::out_of_range("index out of range");
            std::copy_n(data.begin(), data.size(), this->underlying_data + this->index);
            this->index += data.size();
        }
    }

    inline void read(std::span<uint8_t> data)
    requires (mode == DataStream::Mode::Output)
    {
        if (this->file_stream) {
            this->file_stream->read(reinterpret_cast<char*>(data.data()), data.size());
            if (!*this->file_stream)
                throw std::ios_base::failure("file read failed");
        } else {
            if (this->index + data.size() > this->data.size())
                throw std::out_of_range("index out of range");
            std::copy_n(this->underlying_data + this->index, data.size(), data.data());
            this->index += data.size();
        }
    }


public:
    template <typename Container>
    requires (
        !std::is_array_v<Container>
        && std::is_convertible_v<typename Container::value_type, uint8_t>
    )
    Stream(Container& container)
        : data(std::span<uint8_t>(container.data(), container.size())),
        underlying_data(this->data.data()),
        index(0)
    {}

    template <std::size_t N>
    Stream(uint8_t (&array)[N])
        : data(array, N),
        underlying_data(this->data.data()),
        index(0)
    {}

    Stream(std::fstream& file)
        : file_stream(&file),
          index(0)
    {
        if (!this->file_stream->is_open())
            throw std::ios_base::failure("file stream not open");
    }

    template <typename T>
    requires std::is_arithmetic_v<T>
    Stream& operator<<(const T& value)
    requires (mode == DataStream::Mode::Input)
    {
        T output = this->byteswap(value);
        this->write(std::span<const uint8_t>(reinterpret_cast<const uint8_t*>(&output), sizeof(T)));
        return *this;
    }

    template <typename T>
    requires std::is_arithmetic_v<T>
    Stream& operator>>(T& value)
    requires (mode == DataStream::Mode::Output)
    {
        this->read(std::span<uint8_t>(reinterpret_cast<uint8_t*>(&value), sizeof(T)));
        value = this->byteswap(value);
        return *this;
    }

    template <typename T>
    requires std::is_arithmetic_v<T>
    inline void set(const T& value, size_t start_index)
    requires (mode == DataStream::Mode::Input)
    {
        if (this->file_stream)
            throw std::logic_error("set() not supported with file stream");

        T output = this->byteswap(value);
        if (start_index + sizeof(T) > this->data.size())
            throw std::out_of_range("start index out of range");
        std::copy_n(reinterpret_cast<const uint8_t*>(&output), sizeof(T), this->underlying_data + start_index);
    }

    template <typename T>
    requires std::is_arithmetic_v<T>
    inline void get(T& value, size_t start_index)
    requires (mode == DataStream::Mode::Output)
    {
        if (this->file_stream)
            throw std::logic_error("get() not supported with file stream");

        if (start_index + sizeof(T) > this->data.size())
            throw std::out_of_range("start index out of range");
        std::copy_n(this->underlying_data + start_index, sizeof(T), reinterpret_cast<uint8_t*>(&value));
        value = byteswap(value);
    }
};

}
