/**
 * @file
 * Raw memory related functionality
 * 
 * @copyright
 * @verbatim
   Copyright @ 2017 Audi Electronics Venture GmbH. All rights reserved.

       This Source Code Form is subject to the terms of the Mozilla
       Public License, v. 2.0. If a copy of the MPL was not distributed
       with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

   If it is not possible or desirable to put the notice in a particular file, then
   You may include the notice in a location (such as a LICENSE file in a
   relevant directory) where a recipient would be likely to look for such a notice.

   You may add additional accurate notices of copyright ownership.
   @endverbatim
 */
#ifndef A_UTILS_UTIL_MEMORY_BITSERIALIZER_INCLUDED
#define A_UTILS_UTIL_MEMORY_BITSERIALIZER_INCLUDED

#include <algorithm>

#include "a_util/memory.h"
#include "a_util/result.h"

namespace a_util
{
namespace memory
{

//define all needed error types and values locally
_MAKE_RESULT(-4, ERR_POINTER);
_MAKE_RESULT(-5, ERR_INVALID_ARG);

/// Enum describing the endianess
typedef enum
{
    bit_little_endian = 1,
    bit_big_endian = 2,
} Endianess;

/**
* Returns the endianess of the platform
* @return See \ref Endianess
*/
Endianess get_platform_endianess();

namespace detail
{
/**
* Format the bit pattern of a uint64_t value to a string
* Used for debug purposes.
*
* @param [in] value The value to print.
* @retval The bitstring
*/
std::string formatBits(uint64_t value);

/**
* Convert the endianess of a signal by correctly swapping the byte order if required.
* The variable signal is a uint64_t, but the value that needs conversion might be smaller.
* The parameter bit_length determines if a 16, 32 or 64 value should be swapped.
* For a LE system, reading BE signals of 3, 5, 6 or 7 bytes length the value will be aligned
* within a 4 or 8 byte value before swapping bytes.
*
* @param [in,out] signal      Pointer to the variable to store the read value in.
* @param [in]     endianess   Parameter describing the endianess of the bitfield to convert.
* @param [in]     bit_length   Number of bits to read.
*
* @return Returns a standard result code.
*/
a_util::result::Result convertSignalEndianess(uint64_t *signal, Endianess endianess, size_t bit_length);

/**
* Converter Base
* Contains the base methods used by all inheriting Converter classes.
*/
template<typename T>
class ConverterBase {
protected:
    /**
    * Read value from bitfield.
    * Operating on a uint64_t copy to allow bit shifting and masking operations.
    *
    * @param [in]  buffer      Pointer to the memory buffer to read from.
    * @param [in]  start_bit    Bit position to start reading from. The least significant bit
    *                           has the index 0.
    * @param [in]  bit_length   Number of bits to read.
    * @param [out] value       Pointer to the variable to store the read value in.
    * @param [in]  endianess   Parameter describing the endianess of the bitfield to read from.
    *
    * @return Returns a standard result code.
    */
    static a_util::result::Result readSignal(uint8_t *buffer, size_t start_bit, size_t bit_length, T* value,
        Endianess endianess = get_platform_endianess())
    {
        /*
        *   offset_end         offset_start
        *        _                   ____
        *       | |                 |    |
        *  ....|...abcde|fghijklm|no......|.... Buffer (index 0 on the right end side)
        *          |_______________|
        *              bit_length  ^
        *                          |
        *                          start_bit
        */

        // 1) COPY relevant bytes of Buffer content to result variable.
        uint64_t result = 0; // Result value
        uint64_t ninth_byte = 0; // variable to eventually store a ninth byte from buffer
        size_t bytes_to_read = 0;
        copyBytesFromBuffer(buffer, &result, start_bit, bit_length, &ninth_byte, &bytes_to_read);

        // 2) TRIM unrelevant bits and SHIFT to align value.

        // Number of bits the start position is offset from 0 (0 for aligned signal)
        size_t offset_start = start_bit % 8;
        // Number of bits the end position is offset from the end of the last byte (0 for complete bytes)
        size_t offset_end = (8 - ((start_bit + bit_length) % 8)) % 8;

        /**********************************************************************************************
        *      Distinguish between LE and BE operating systems to get the shift operations right     *
        **********************************************************************************************/
        // On LE System
        if (get_platform_endianess() == bit_little_endian)
        {
            /* Use bit mask to remove bits on the higher end, which do not belong to the value to read.
            *
            *   ...|...abcde|fghijklm|no......|  =>  000|000abcde|fghijklm|no......|
            *
            */
            cutLeadingBits(&result, bit_length + offset_start);

            /* Shift right to align start at position 0 (also trims the right end).
            *
            *   000|000abcde|fghijklm|no......|  =>  000|00000000|0abcdefg|hijklmno|
            *
            */
            result >>= offset_start;

            // Eventually get bits from the copied 9th byte.
            if (ninth_byte > 0) // nothing to take care of if nothing was copied or all copied bits are 0.
            {
                // deleteAll unwanted bits from ninth byte.
                cutLeadingBits(&ninth_byte, (8 - offset_end));
                size_t bit_size = sizeof(result) * 8;
                // Shift requested bits from the 9th byte into the right position to be combined with result.
                ninth_byte <<= (bit_size - offset_start);
                // merge value together from all nine bytes.
                result = result | ninth_byte;
            }

            // BE Signal needs byte order swapping.
            if (endianess == bit_big_endian)
            {
                // Only for reading partial bytes. Filling the missing bits differs from LE Signal.
                if (bit_length % 8 != 0)
                {
                    /* Shift left to align end position.
                    *
                    *   000|0abcdefg|hijklmno|  =>  000|abcdefgh|ijklmno0|
                    *
                    */
                    result <<= (offset_end + offset_start) % 8;

                    /* Shift bits within MSByte, filling the gap with 0s.
                    *
                    *   000|abcdefgh|ijklmno0|  =>  000|abcdefgh|0ijklmno|
                    *                ^                        ^
                    *              MSByte                   MSByte
                    */
                    uint8_t *ms_byte = (uint8_t*)&result;
                    ms_byte[0] >>= (offset_end + offset_start) % 8;
                }

                /* swap bytes to LE.
                *
                *   000|abcdefgh|0ijklmno|  =>  000|0ijklmno|abcdefgh|
                *
                */
                detail::convertSignalEndianess(&result, endianess, bit_length);
            }
        }
        // On BE System
        else
        {
            /* swap bytes to simulate LE shifting operations.
            *
            *   |...abcde|fghijklm|no......|...  =>  ...|no......|fghijklm|...abcde|
            *
            */
            detail::convertSignalEndianess(&result, bit_little_endian, sizeof(result) * 8);

            // LE Signal
            if (endianess == bit_little_endian)
            {
                /* Use bit mask to remove bits on the higher end, which do not belong to the value to read.
                *
                *   ...|no......|fghijklm|...abcde|  =>  ...|no......|fghijklm|000abcde|
                *
                */
                cutLeadingBits(&result, (sizeof(result) * 8) - offset_end); // Cut away only offset_end bits.

                /* Shift right to align bits within LSByte (BE Shifting!).
                *
                *   ...|no......|fghijklm|000abcde|  =>  ...|hijklmno|0abcdefg|00000000|
                *
                */
                result >>= offset_start;

                /* Shift right to align LSByte on the left side (BE Shifting!).
                *
                *   ...|hijklmno|0abcdefg|00000000|  =>  |hijklmno|0abcdefg|000
                *
                * Shift over all
                * empty bytes = (all bits - occupied bits (bit_length) - already shifted bits) / number of bytes
                */
                result >>= (((sizeof(result) * 8) - bit_length - offset_start) / sizeof(result)) * 8;

                // No further byte swap, because there has been one swap before the shift operations already.
            }
            // BE Signal
            else
            {
                /* Shift left to align bits within LSByte (now rightmost byte because of byte swap).
                * Also deletes bits from end offset.
                *
                *   ...|no......|fghijklm|...abcde|  =>  ...|........|ijklmno.|abcdefgh|
                *
                */
                result <<= offset_end;

                // Only for reading partial bytes. Filling the missing bits differs from LE Signal.
                if (bit_length % 8 != 0)
                {
                    /* Shift bits within MSByte to move 0s to the highest bits (also deleting all unwanted bits from start offset).
                    *
                    *   ...|........|ijklmno.|abcdefgh|  =>  ...|........|0ijklmno|abcdefgh|
                    *
                    */
                    uint8_t *ms_byte = (uint8_t*)&result + (bit_length / 8); // position of the value's MSByte
                    ms_byte[0] >>= (offset_end + offset_start) % 8; // amount of unused bits within MSByte
                }

                /* swap bytes back to BE
                *
                *   ...|........|0ijklmno|abcdefgh|  =>  |abcdefgh|0ijklmno|...
                *
                */
                detail::convertSignalEndianess(&result, bit_little_endian, sizeof(result) * 8); // Change the simulated LE value back

                /* Use bit mask to remove bits on the higher end, which do not belong to the value to read.
                *
                *   |abcdefgh|0ijklmno|...  =>  |abcdefgh|0ijklmno|000
                *
                * Remove everything behind the value length plus the gap within MSByte.
                */
                cutLeadingBits(&result, bit_length + (offset_end + offset_start) % 8);
            }
        }

        // Copy the resulting value to the target variable. No Casting! Data might be lost otherwise.
        size_t sz = std::min(sizeof(*value), sizeof(result));
        a_util::memory::copy(value, sz, &result, sz);

        return a_util::result::SUCCESS;
    }

    /**
    * Write value to bitfield.
    * Operating on a uint64_t copy to allow bit shifting and masking operations.
    *
    * @param [in]  buffer      Pointer to the memory buffer to write to.
    * @param [in]  start_bit    Bit position to start writing to. The least significant bit
    *                           has the index 0.
    * @param [in]  bit_length   Number of bits to write.
    * @param [out] value       Value to write to the bitfield.
    * @param [in]  endianess   Parameter describing the endianess of the bitfield to write to.
    *
    * @return Returns a standard result code.
    */
    static a_util::result::Result writeSignal(uint8_t *buffer, size_t start_bit, size_t bit_length, T value,
        Endianess endianess = get_platform_endianess())
    {
        // 1) Copy relevant bytes of Buffer content to be overwritten.
        uint64_t buffer_copy = 0;
        uint64_t ninth_byte = 0; // storage variable for the ninth bit from buffer
        size_t bytes_to_read = 0;
        copyBytesFromBuffer(buffer, &buffer_copy, start_bit, bit_length, &ninth_byte, &bytes_to_read);

        // 2) Erase Bits from Buffer copy that will be overwritten TODO: BE LE system difference important here?
        // Number of bits the start position is offset from 0
        size_t offset_start = start_bit % 8;
        // Number of bits the end position is offset from the end of the last byte
        size_t offset_end = (8 - ((start_bit + bit_length) % 8)) % 8;
        uint64_t mask_left = ~0ULL;
        if ((bit_length + offset_start) >= (sizeof(mask_left) * 8))
        {
            mask_left = 0;
        }
        else
        {
            mask_left = ~0ULL;
            mask_left <<= (bit_length + offset_start);
        }
        uint64_t mask = ~0ULL;
        mask <<= offset_start;
        mask = ~mask;
        mask |= mask_left;

        buffer_copy &= mask;

        // 3) Copy value to UInt64 variable to work with.
        uint64_t signal;
        a_util::memory::copy(&signal, sizeof(signal), &value, sizeof(signal));

        // 4) Keep only nLength bits: Remove bits that should not be written to the Buffer.
        cutLeadingBits(&signal, bit_length);

        // 5) Shift to align at start bit position. 
        int shift_amount = (int)offset_start; // Initialized to fit LE Signal shift

        // BE Signal
        if (endianess == bit_big_endian)
        {
            // swap bytes
            detail::convertSignalEndianess(&signal, endianess, bit_length);
            // Remove gap for partial bytes within MSByte
            uint8_t *ms_byte = (uint8_t*)&signal;
            int ms_shift = (8 - (bit_length % 8)) % 8;
            ms_byte[0] <<= ms_shift;
            shift_amount -= ms_shift;
        }

        // Copy most significant byte to ninth buffer byte before losing bits with the shift.
        if ((offset_start + bit_length) > (sizeof(signal) * 8))
        {
            uint64_t signal_for_ninth_byte = signal;
            signal_for_ninth_byte >>= (sizeof(signal) - 1) * 8;
            signal_for_ninth_byte >>= (8 - offset_start); // Only LE Signal
            uint64_t mask = ~0ULL;
            mask <<= (8 - offset_end);
            ninth_byte &= mask;
            ninth_byte |= signal_for_ninth_byte;
        }

        if (shift_amount < 0)
        {
            signal >>= std::abs(shift_amount);
        }
        else
        {
            signal <<= shift_amount;
        }

        // 7) Write bytes with integrated signal back to the buffer. 
        buffer_copy |= signal;

        size_t sz = std::min(bytes_to_read, sizeof(signal));
        a_util::memory::copy(buffer + (start_bit / 8), sz, &buffer_copy, sz);

        // Eventually copy ninth byte back to buffer
        if (bytes_to_read > sizeof(signal))
        {
            a_util::memory::copy(buffer + (start_bit / 8) + sizeof(signal), 1, &ninth_byte, 1);
        }

        return a_util::result::SUCCESS;
    }

    /**
    * Set the highest bits of a uint64_t value to zero. The number of bit_length lowest bits
    * remain unchanged.
    *
    * @param [out] value       Pointer to the variable to trim.
    * @param [in]  bit_length   Number of trailing bits to remain unchanged.
    *
    * @return Returns a standard result code.
    */
    static a_util::result::Result cutLeadingBits(uint64_t *value, size_t bit_length)
    {
        size_t bit_size = (sizeof(*value) * 8);
        if (bit_length < bit_size)
        {
            uint64_t mask = ~0ULL;
            mask >>= (bit_size - bit_length);
            *value &= mask;
        }

        return a_util::result::SUCCESS;
    }

    /**
    * Copy bytes_to_read number of bytes from the buffer to value and ninth_byte.
    * Determines how many bytes need to be copied to receive a copy of all bits in the range described
    * by start_bit and bit_length. The maximum for bit_length is 64, but for unaligned values the range
    * may exceed 8 bytes. In this case, the required ninth byte will be copied to ninth_byte.
    *
    * @param [in]  buffer      Pointer to the memory buffer to copy from.
    * @param [out] value       Pointer to the variable to store the copied value in.
    * @param [in]  start_bit    Bit position to start reading from. The least significant bit
    *                           has the index 0.
    * @param [in]  bit_length   Number of bits to read.
    * @param [out] ninth_byte   Pointer to the variable to eventually store a copied ninth byte in.
    * @param [out] bytes_to_read Number of bytes that need to be copied to attain all requested bits.
    *
    * @return Returns a standard result code.
    */
    static a_util::result::Result copyBytesFromBuffer(uint8_t *buffer, uint64_t *value, size_t start_bit, size_t bit_length, uint64_t *ninth_byte, size_t *bytes_to_read)
    {
        // Byte within the buffer to start reading at
        size_t start_byte = start_bit / 8;

        // Number of bits to read: signal length + bits to fill in the offset on both sides for unaligned signals
        size_t bits_to_read = bit_length + (start_bit % 8);
        if ((bits_to_read % 8) > 0)
        {
            bits_to_read += (8 - (bits_to_read % 8));
        }
        // Number of bytes to read from the buffer
        *bytes_to_read = bits_to_read / 8;

        // Copy up to 8 bytes to result
        if (*bytes_to_read > (size_t)sizeof(*value))
        {
            a_util::memory::copy(value, sizeof(*value), buffer + start_byte, sizeof(*value));
        }
        else
        {
            a_util::memory::copy(value, *bytes_to_read, buffer + start_byte, *bytes_to_read);
        }

        // The max signal size is 8 byte, but if the signal is not aligned, it might spread over 9 bytes.

        if (*bytes_to_read > sizeof(*value))
        {
            // Get a copy of the most significant byte, which could not yet be saved to result
            a_util::memory::copy(ninth_byte, 1, buffer + start_byte + sizeof(*value), 1);
        }

        return a_util::result::SUCCESS;
    }
};

/// Template converter class to differentiate between float, signed and unsigned integer values.
template<typename T, int is_signed, int is_floating_point> class Converter;

/// Partially specialized class for Unsigned Integers
template<typename T>
class Converter<T, 0, 0> : public ConverterBase<T>
{
public:
    /**
    * Read unsigned integer from bitfield.
    *
    * @param [in]  buffer      Pointer to the memory buffer to read from.
    * @param [in]  start_bit    Bit position to start reading from. The least significant bit
    *                           has the index 0.
    * @param [in]  bit_length   Number of bits to read.
    * @param [out] value       Pointer to the variable to store the read value in.
    * @param [in]  endianess   Parameter describing the endianess of the bitfield to read from.
    *
    * @return Returns a standard result code.
    */
    static a_util::result::Result read(uint8_t *buffer, size_t start_bit, size_t bit_length, T* value,
        Endianess endianess)
    {
        return ConverterBase<T>::readSignal(buffer, start_bit, bit_length, value, endianess);
    }

    /**
    * Write unsigned integer to bitfield.
    *
    * @param [in]  buffer      Pointer to the memory buffer to write to.
    * @param [in]  start_bit    Bit position to start writing to. The least significant bit
    *                           has the index 0.
    * @param [in]  bit_length   Number of bits to write.
    * @param [out] value       Value to write to the bitfield.
    * @param [in]  endianess   Parameter describing the endianess of the bitfield to write to.
    *
    * @return Returns a standard result code.
    */
    static a_util::result::Result write(uint8_t *buffer, size_t start_bit, size_t bit_length, T value,
        Endianess endianess)
    {
        return ConverterBase<T>::writeSignal(buffer, start_bit, bit_length, value, endianess);
    }
};

/// Partially specialized class for Signed Integers
template<typename T>
class Converter<T, 1, 0> : public ConverterBase<T>
{
public:
    /**
    * Read signed integer from bitfield.
    *
    * @param [in]  buffer      Pointer to the memory buffer to read from.
    * @param [in]  start_bit    Bit position to start reading from. The least significant bit
    *                           has the index 0.
    * @param [in]  bit_length   Number of bits to read.
    * @param [out] value       Pointer to the variable to store the read value in.
    * @param [in]  endianess   Parameter describing the endianess of the bitfield to read from.
    *
    * @return Returns a standard result code.
    */
    static a_util::result::Result read(uint8_t *buffer, size_t start_bit, size_t bit_length, T* value,
        Endianess endianess)
    {
        a_util::result::Result res = ConverterBase<T>::readSignal(buffer, start_bit, bit_length, value, endianess);
        if (res != a_util::result::SUCCESS)
        {
            return res;
        }

        // replicate sign bit
        *value <<= (sizeof(T) * 8) - bit_length;
        *value >>= (sizeof(T) * 8) - bit_length;
        return a_util::result::SUCCESS;
    }

    /**
    * Write signed integer to bitfield.
    *
    * @param [in]  buffer      Pointer to the memory buffer to write to.
    * @param [in]  start_bit    Bit position to start writing to. The least significant bit
    *                           has the index 0.
    * @param [in]  bit_length   Number of bits to write.
    * @param [out] value       Value to write to the bitfield.
    * @param [in]  endianess   Parameter describing the endianess of the bitfield to write to.
    *
    * @return Returns a standard result code.
    */
    static a_util::result::Result write(uint8_t *buffer, size_t start_bit, size_t bit_length, T value,
        Endianess endianess)
    {
        // Nothing special to take care of for writing signed integers, compared to writing unsigned integers.
        return ConverterBase<T>::writeSignal(buffer, start_bit, bit_length, value, endianess);
    }
};

/// Specialized class for Floats (Floats are always signed!)
template<typename T>
class Converter<T, 1, 1> : public ConverterBase<T>
{
public:
    /**
    * Read tFloat from bitfield.
    *
    * @param [in]  buffer      Pointer to the memory buffer to read from.
    * @param [in]  start_bit    Bit position to start reading from. The least significant bit
    *                           has the index 0.
    * @param [in]  bit_length   Number of bits to read.
    * @param [out] value       Pointer to the variable to store the read value in.
    * @param [in]  endianess   Parameter describing the endianess of the bitfield to read from.
    *
    * @return Returns a standard result code.
    */
    static a_util::result::Result read(uint8_t *buffer, size_t start_bit, size_t bit_length, T* value,
        Endianess endianess)
    {
        // Read only values of size tFloat
        if (sizeof(T) * 8 == bit_length)
        {
            return ConverterBase<T>::readSignal(buffer, start_bit, bit_length, value, endianess);
        }
        else
        {
            return ERR_INVALID_ARG;
        }
    }

    /**
    * Write tFloat to bitfield.
    *
    * @param [in]  buffer      Pointer to the memory buffer to write to.
    * @param [in]  start_bit    Bit position to start writing to. The least significant bit
    *                           has the index 0.
    * @param [in]  bit_length   Number of bits to write.
    * @param [out] value       Value to write to the bitfield.
    * @param [in]  endianess   Parameter describing the endianess of the bitfield to write to.
    *
    * @return Returns a standard result code.
    */
    static a_util::result::Result write(uint8_t *buffer, size_t start_bit, size_t bit_length, T value,
        Endianess endianess)
    {
        // Write only values of size tFloat
        if (sizeof(T) * 8 == bit_length)
        {
            return ConverterBase<T>::writeSignal(buffer, start_bit, bit_length, value, endianess);
        }
        else
        {
            return ERR_INVALID_ARG;
        }
    }
};

} // namespace detail

/// Bit Serializer Class
class BitSerializer
{
public:
    /**
    * Constructor
    */
    BitSerializer(void* data, size_t data_size) :
        _buffer(static_cast<uint8_t*>(data)), _buffer_bytes(data_size),
        _buffer_bits(_buffer_bytes * 8)
    {
    }

    /**
    * Default Constructor
    */
    BitSerializer() : _buffer(NULL), _buffer_bytes(0), _buffer_bits(0)
    {
    }

    /**
    * Read value from bitfield. Value can be of type tFloat or an unsigned or signed integer.
    *
    *  ....|...*****|********|**......|.... Buffer (index 0 on the right end side)
    *          |_______________|
    *              bit_length  ^
    *                          |
    *                          start_bit
    *
    * @param [in]  start_bit    Bit position to start reading from. The least significant bit
    *                           has the index 0.
    * @param [in]  bit_length   Number of bits to read.
    * @param [out] value       Pointer to the variable to store the read value in.
    * @param [in]  endianess   Parameter describing the endianess of the bitfield to read from.
    *
    * @return Returns a standard result code.
    */
    template<typename T>
    a_util::result::Result read(size_t start_bit, size_t bit_length, T* value,
        Endianess endianess = get_platform_endianess())
    {
        // Check if in range
        a_util::result::Result result_code = checkForInvalidArguments(start_bit, bit_length, sizeof(T));
        if (result_code != a_util::result::SUCCESS)
        {
            return result_code;
        }

        // Call template function
        detail::Converter<T, std::is_signed<T>::value,
            std::is_floating_point<T>::value>
            ::read(_buffer, start_bit, bit_length, value, endianess);

        return a_util::result::SUCCESS;
    }

    /**
    * Write value to bitfield. Value can be of type tFloat or an unsigned or signed integer.
    *
    *  ....|...*****|********|**......|.... Buffer (index 0 on the right end side)
    *          |_______________|
    *              bit_length  ^
    *                          |
    *                          start_bit
    *
    * @param [in]  start_bit    Bit position to start writing to. The least significant bit
    *                           has the index 0.
    * @param [in]  bit_length   Number of bits to write.
    * @param [out] value       Value to write to the bitfield.
    * @param [in]  endianess   Parameter describing the endianess of the bitfield to write to.
    *
    * @return Returns a standard result code.
    */
    template<typename T>
    a_util::result::Result write(size_t start_bit, size_t bit_length, T value,
        Endianess endianess = get_platform_endianess())
    {
        // Check if in range
        a_util::result::Result result_code = checkForInvalidArguments(start_bit, bit_length, sizeof(T));
        if (result_code != a_util::result::SUCCESS)
        {
            return result_code;
        }

        // Call template function
        detail::Converter<T, std::is_signed<T>::value,
            std::is_floating_point<T>::value>
            ::write(_buffer, start_bit, bit_length, value, endianess);

        return a_util::result::SUCCESS;
    }

private:
    /// internal buffer
    uint8_t *_buffer;
    /// size of internal buffer in bytes
    size_t _buffer_bytes;
    /// size of internal buffer in bits
    size_t _buffer_bits;

    /**
    * Check if the parameters for the reading and writing access are valid.
    * The variable to read from or into might be too small and the accessed region of the memory buffer
    * might be out of range.
    *
    * @param [in]  start_bit     Bit position to start reading from. The least significant bit
    *                            has the index 0.
    * @param [in]  bit_length    Number of bits to read.
    * @param [in]  size_variable Size of the variable to read into or write from.
    *
    * @return Returns a standard result code.
    */
    a_util::result::Result checkForInvalidArguments(size_t start_bit, size_t bit_length, size_t size_variable)
    {
        if (!_buffer)
        {
            return ERR_POINTER;
        }

        // Check invalid starting point
        if (start_bit >= _buffer_bits)
        {
            return ERR_INVALID_ARG;
        }

        // Check out of buffer bounds or length < 1
        if ((bit_length < 1)
            || (_buffer_bits < start_bit + bit_length))
        {
            return ERR_INVALID_ARG;
        }

        // Check variable size
        if (size_variable * 8 < bit_length)
        {
            return ERR_INVALID_ARG;
        }

        return a_util::result::SUCCESS;
    }

};

} // namespace memory
} // namespace a_util

#endif // A_UTILS_UTIL_MEMORY_BITSERIALIZER_INCLUDED
