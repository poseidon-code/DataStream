/*
DataStream : A modern binary data serializer in C++ 23 standard.
Copyright (C) 2024  Pritam Halder

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.

Author : Pritam Halder
Email : pritamhalder.poseidon@proton.me
*/


#pragma once

#include <concepts>
#include <cstdint>
#include <stdfloat>
#include <type_traits>


template <typename T>
concept ValidFloatingType = std::is_floating_point_v<T> || std::is_same_v<T, std::float16_t> && !std::is_same_v<T, std::float128_t>;


template <typename T>
concept ValidIntermediateType =
    std::is_void_v<T> ||
    std::is_same_v<T, int8_t>  || std::is_same_v<T, uint8_t>  ||
    std::is_same_v<T, int16_t> || std::is_same_v<T, uint16_t> ||
    std::is_same_v<T, int32_t> || std::is_same_v<T, uint32_t> ||
    std::is_same_v<T, int64_t> || std::is_same_v<T, uint64_t>;


namespace DataStream {

template <ValidFloatingType T, ValidIntermediateType _IT = void>
class FixedPointQuantizer {
private:
    T minimum_value, maximum_value;

public:
    using IT = std::conditional_t<
        std::is_void_v<IT_>,
            std::conditional_t<std::is_same_v<T, std::float16_t>, int16_t,
            std::conditional_t<std::is_same_v<T, std::float32_t> || std::is_same_v<T, float>, int32_t,
            std::conditional_t<std::is_same_v<T, std::float64_t> || std::is_same_v<T, double>, int64_t,
        >>>,
        _IT
    >;

	static constexpr uint16_t Bits = sizeof(IT) * 8;

    FixedPointQuantizer() = default;

    FixedPointQuantizer(T minimum_value, T maximum_value)
        : minimum_value(minimum_value), maximum_value(maximum_value) {}

    FixedPointQuantizer(const FixedPointQuantizer& other)
        : minimum_value(other.minimum_value), maximum_value(other.maximum_value) {}

    FixedPointQuantizer(FixedPointQuantizer&& other) noexcept
        : minimum_value(std::move(other.minimum_value)),
          maximum_value(std::move(other.maximum_value)) {}

    FixedPointQuantizer& operator=(const FixedPointQuantizer& other) {
        if (this != &other) {
            this->minimum_value = other.minimum_value;
            this->maximum_value = other.maximum_value;
        }
        return *this;
    }

    FixedPointQuantizer& operator=(FixedPointQuantizer&& other) noexcept {
        if (this != &other) {
            this->minimum_value = std::move(other.minimum_value);
            this->maximum_value = std::move(other.maximum_value);
        }
        return *this;
    }

    ~FixedPointQuantizer() = default;

    static inline constexpr IT to_fpq(T value, T minimum, T maximum) {
        if (value > maximum) value = maximum;
        if (value < minimum) value = minimum;
        T normalised = value / (maximum - minimum);
        IT converted = normalised == 0.5 ? (1ULL << (Bits - 1)) : normalised * ((1ULL << Bits) - 1);
        return converted;
    }

    static inline constexpr T from_fpq(IT value, T minimum, T maximum) {
        T denormalised = static_cast<T>(value) / ((1ULL << Bits) - 1);
        return denormalised * (maximum - minimum);
    }

    inline constexpr IT to_fpq(T value) const {
        return to_fpq(value, this->minimum_value, this->maximum_value);
    }

    inline constexpr T from_fpq(IT value) const {
        return from_fpq(value, this->minimum_value, this->maximum_value);
    }
};

}
