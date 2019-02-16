
// MIT License
//
// Copyright (c) 2019 degski
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <type_traits>
#include <utility>

#ifndef nl
#define nl '\n'
#endif


// From: https://www.redblobgames.com/grids/hexagons/#map-storage


template<std::size_t R>
using rows_size = std::integral_constant<std::size_t, 2 * R + 3>;
template<std::size_t R>
using cols_size = std::integral_constant<std::size_t, 4 * R + 3>;
template<std::size_t R>
using data_size = std::integral_constant<std::size_t, rows_size<R>::value * cols_size<R>::value>;

template<std::size_t R>
using uidx = std::conditional_t < data_size<( 2 * R + 1 )>::value < std::numeric_limits<std::uint8_t>::max ( ), std::uint8_t, std::uint16_t>;
template<std::size_t R>
using sidx = std::conditional_t < data_size<( 2 * R + 1 )>::value < std::numeric_limits<std::int8_t >::max ( ), std::int8_t, std::int16_t >;

template<std::intptr_t R, bool zero_base = true>
struct Hex {

    static_assert ( R > 0, "the radius should be larger than 0" );

    using value_type = sidx<R>;

    [[ nodiscard ]] static constexpr value_type radius ( ) noexcept {
        return R;
    }

    value_type q = value_type { -Hex::radius ( ) - 1 }, r = value_type { -Hex::radius ( ) - 1 };

    void nil ( ) noexcept {
        q = value_type { -Hex::radius ( ) - 1 }; r = value_type { -Hex::radius ( ) - 1 };
    }

    [[ nodiscard ]] bool operator == ( const Hex & rhs_ ) const noexcept {
        return q == rhs_.q and r == rhs_.r;
    }
    [[ nodiscard ]] bool operator != ( const Hex & rhs_ ) const noexcept {
        return q != rhs_.q or r != rhs_.r;
    }

    [[ nodiscard ]] inline bool in_valid ( ) const noexcept {
        if constexpr ( zero_base ) {
            return std::abs ( q ) > radius ( ) or std::abs ( r ) > radius ( ) or std::abs ( -q - r ) > radius ( );
        }
        else {
            return std::abs ( q - radius ( ) ) > radius ( ) or std::abs ( r - radius ( ) ) > radius ( ) or std::abs ( -q - r + ( 2 * radius ( ) ) ) > radius ( );
        }
    }
    [[ nodiscard ]] static constexpr bool in_valid ( value_type q_, value_type r_ ) noexcept {
        if constexpr ( zero_base ) {
            return std::abs ( q_ ) > radius ( ) or std::abs ( r_ ) > radius ( ) or std::abs ( -q_ - r_ ) > radius ( );
        }
        else {
            return std::abs ( q_ - radius ( ) ) > radius ( ) or std::abs ( r_ - radius ( ) ) > radius ( ) or std::abs ( -q_ - r_ + ( 2 * radius ( ) ) ) > radius ( );
        }
    }
    [[ nodiscard ]] inline bool valid ( ) const noexcept {
        return not ( in_valid ( ) );
    }
    [[ nodiscard ]] static constexpr bool valid ( value_type q_, value_type r_ ) noexcept {
        return not ( in_valid ( q_, r_ ) );
    }

    template<typename Stream>
    [ [ maybe_unused ] ] friend Stream & operator << ( Stream & out_, const Hex & h_ ) noexcept {
        out_ << '<' << h_.q << ' ' << h_.r << '>';
        return out_;
    }
};


template<typename T, std::size_t R, bool zero_base = true, typename SizeType = int, typename = std::enable_if_t<std::is_default_constructible_v<T>, T>>
struct HexContainer {

    using size_type = SizeType;
    using value_type = T;
    using pointer = T * ;
    using const_pointer = const T *;
    using reference = T & ;
    using const_reference = const T &;

    [[ nodiscard ]] static constexpr size_type radius ( ) noexcept {
        return static_cast< std::size_t > ( R );
    }
    [[ nodiscard ]] static constexpr size_type width ( ) noexcept {
        return 2 * radius ( ) + 1;
    }
    [[ nodiscard ]] static constexpr size_type height ( ) noexcept {
        return 2 * radius ( ) + 1;
    }
    [[ nodiscard ]] static constexpr size_type size ( ) noexcept {
        return width ( ) * height ( );
    }

    T m_data [ 2 * R + 1 ] [ 2 * R + 1 ];

    HexContainer ( ) noexcept : m_data { { T ( ) } } { }

    [[ nodiscard ]] reference at ( const size_type q_, const size_type r_ ) noexcept {
        if constexpr ( zero_base ) {
            return m_data [ r_ ] [ q_ ];
        }
        else {
            return m_data [ r_ + radius ( ) ] [ q_ + std::max ( radius ( ), r_ ) ];
        }
    }
    [[ nodiscard ]] const_reference at ( const size_type q_, const size_type r_ ) const noexcept {
        return at ( q_, r_ );
    }
    [[ nodiscard ]] reference at ( const Hex<R> & h_ ) noexcept {
        return at ( h_.q, h_.r );
    }
    [[ nodiscard ]] const_reference at ( const Hex<R> & h_ ) const noexcept {
        return at ( h_.q, h_.r );
    }

    [[ nodiscard ]] reference operator [ ] ( const Hex<R> & h_ ) noexcept {
        return at ( h_ );
    }
    [[ nodiscard ]] const_reference operator [ ] ( const Hex<R> & h_ ) const noexcept {
        return at ( h_ );
    }

    [[ nodiscard ]] pointer data ( ) noexcept {
        return & m_data [ 0 ] [ 0 ];
    }
    [[ nodiscard ]] const_pointer data ( ) const noexcept {
        return & m_data [ 0 ] [ 0 ];
    }

    template<typename Stream>
    [ [ maybe_unused ] ] friend Stream & operator << ( Stream & out_, const HexContainer & h_ ) noexcept {
        for ( int r = 0; r < height ( ); ++r ) {
            for ( int q = 0; q < width ( ); ++q ) {
                out_ << std::setw ( 3 ) << h_.m_data [ r ] [ q ];
            }
            out_ << nl;
        }
        return out_;
    }
};


int main ( ) {

    HexContainer<int, 3, false> center_based;

    center_based.at ( 0, -3 ) = 1;
    center_based.at ( 1, -3 ) = 2;
    center_based.at ( -3, 0 ) = 3;
    center_based.at ( 0, 0 ) = 4;
    center_based.at ( 3, 0 ) = 5;
    center_based.at ( 2, 1 ) = 8;
    center_based.at ( 0, 3 ) = 16;

    std::cout << "cb\n\n" << center_based << nl;

    HexContainer<int, 3, true> zero_based;

    zero_based.at ( 3, 0 ) = 1;
    zero_based.at ( 4, 0 ) = 2;
    zero_based.at ( 0, 3 ) = 3;
    zero_based.at ( 3, 3 ) = 4;
    zero_based.at ( 6, 3 ) = 5;
    zero_based.at ( 5, 4 ) = 8;
    zero_based.at ( 3, 6 ) = 16;

    std::cout << "zb\n\n" << zero_based << nl;

    return EXIT_SUCCESS;
}


/*

cb

  0  0  0  1  2  0  0
  0  0  0  0  0  0  0
  0  0  0  0  0  0  0
  3  0  0  4  0  0  5
  0  0  0  0  0  8  0
  0  0  0  0  0  0  0
  0  0  0 16  0  0  0

zb

  0  0  0  1  2  0  0
  0  0  0  0  0  0  0
  0  0  0  0  0  0  0
  3  0  0  4  0  0  5
  0  0  0  0  0  8  0
  0  0  0  0  0  0  0
  0  0  0 16  0  0  0


Y:\REPOS\Amit\x64\Debug\Amit.exe (process 10636) exited with code 0.
Press any key to close this window . . .

*/