#pragma once

#include <algorithm>
#include <bit>
#include <concepts>
#include <cstdint>
#include <fstream>
#include <span>
#include <stdexcept>
#include <type_traits>
#include <utility>




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


struct Mode {
Mode() = delete;
Mode(const Mode& o) = delete;
Mode(Mode&& o) noexcept = delete;
Mode& operator=(const Mode& o) = delete;
Mode& operator=(Mode&& o) noexcept = delete;
~Mode() = default;

using Type = uint8_t;
static const Type
    Input = 0b00000001, // take deserialized data from the stream into the program (input to program)
    Output = 0b00000010; // put serialized data to the stream from the prgram (output to stream)
};


template <
    DataStream::Mode::Type mode = (DataStream::Mode::Input | DataStream::Mode::Output),
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
    inline constexpr T byteswap(T value) const {
        return endiannes != std::endian::native ? DataStream::byteswap(value) : value;
    }

    inline void write(std::span<const uint8_t> data)
    requires ((mode & DataStream::Mode::Output) == DataStream::Mode::Output)
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
    requires ((mode & DataStream::Mode::Input) == DataStream::Mode::Input)
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
    requires (!std::is_array_v<Container> && std::is_convertible_v<typename Container::value_type, uint8_t>)
    Stream(Container& container)
        : data(std::span<uint8_t>(container.data(), container.size())),
        underlying_data(this->data.data())
    {}

    template <std::size_t N>
    Stream(uint8_t (&array)[N])
        : data(array, N),
        underlying_data(this->data.data())
    {}

    Stream(std::fstream& file)
        : file_stream(&file)
    {
        if (!this->file_stream->is_open())
            throw std::ios_base::failure("file stream not open");
    }

    ~Stream() = default;

    Stream(const Stream& o)
        : data(o.data),
        underlying_data(o.underlying_data),
        file_stream(o.file_stream),
        index(o.index)
    {}

    Stream& operator=(const Stream& o) {
        if (this == &o) return *this;
        data = o.data;
        underlying_data = o.underlying_data;
        file_stream = o.file_stream;
        index = o.index;
        return *this;
    }

    Stream(Stream&& o) noexcept
        : data(std::exchange(o.data, {})),
        underlying_data(std::exchange(o.underlying_data, nullptr)),
        file_stream(std::exchange(o.file_stream, nullptr)),
        index(std::exchange(o.index, 0))
    {}

    Stream& operator=(Stream&& o) noexcept {
        if (this == &o) return *this;
        data = std::exchange(o.data, {});
        underlying_data = std::exchange(o.underlying_data, nullptr);
        file_stream = std::exchange(o.file_stream, nullptr);
        index = std::exchange(o.index, 0);
        return *this;
    }

    template <typename T>
    requires std::is_arithmetic_v<T>
    Stream& operator<<(const T& value)
    requires (mode == DataStream::Mode::Output)
    {
        T output = this->byteswap(value);
        this->write(std::span<const uint8_t>(reinterpret_cast<const uint8_t*>(&output), sizeof(T)));
        return *this;
    }

    template <typename T>
    requires std::is_arithmetic_v<T>
    Stream& operator>>(T& value)
    requires (mode == DataStream::Mode::Input)
    {
        this->read(std::span<uint8_t>(reinterpret_cast<uint8_t*>(&value), sizeof(T)));
        value = this->byteswap(value);
        return *this;
    }

    template <typename T>
    requires std::is_arithmetic_v<T>
    inline void set(const T& value, size_t start_index)
    requires ((mode & DataStream::Mode::Output) == DataStream::Mode::Output)
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
    inline void get(T& value, size_t start_index) const
    requires ((mode & DataStream::Mode::Input) == DataStream::Mode::Input)
    {
        if (this->file_stream)
            throw std::logic_error("get() not supported with file stream");

        if (start_index + sizeof(T) > this->data.size())
            throw std::out_of_range("start index out of range");
        std::copy_n(this->underlying_data + start_index, sizeof(T), reinterpret_cast<uint8_t*>(&value));
        value = byteswap(value);
    }

    inline const uint8_t* get() const {
        return this->underlying_data;
    }
};

}
