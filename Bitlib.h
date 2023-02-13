#pragma once

#include <iostream>
#include <math.h>

#include "Definitions.h"

static const u_char uchar_bits[8] = { Xb8, Xb7, Xb6, Xb5, Xb4, Xb3, Xb2, Xb1 };

using namespace std;

class Bitlib
{
public:

    /*
        Writes num_bits bits of num number into buffer buf, beginning at bit position 8*byte_ptr + bit_ptr (first free position)
        - if needed to store one bit then use Write(buf,byte_ptr,bit_ptr,0|1,1)
    */
    static inline void Write(u_char* buf, u_int& byte_ptr, u_char& bit_ptr, u_char num, u_char num_bits)
    {
        u_char free_bits = 8 - bit_ptr;

        //case a free_bits >= num_bits
        if (free_bits >= num_bits) {
            //all bits fits into buf[byte]
            //compute neccessary offset
            buf[byte_ptr] |= num << (free_bits - num_bits);
            //move to the bit_ptr to the empty position
            bit_ptr += num_bits;
        }
            //case b free_bits < num_bits
        else {
            u_char offset = num_bits - free_bits;
            //first part of the bits to the rest of the free bits
            buf[byte_ptr++] |= num >> offset;
            buf[byte_ptr] = num << (8 - offset);
            bit_ptr = offset;
        }

        if(bit_ptr == 8){
            byte_ptr++;
            bit_ptr = 0;
        }
    };

    static inline void Write(u_char* buf, u_int& byte_ptr, u_char& bit_ptr, u_int num, u_char num_bits)
    {
        u_char free_bits = 8 - bit_ptr;

        //case a free_bits >= num_bits
        if (free_bits >= num_bits) {
            //all bits fits into buf[byte]
            //compute neccessary offset
            buf[byte_ptr] |= num << (free_bits - num_bits);
            //move to the bit_ptr to the empty position
            bit_ptr += num_bits;
        }
            //case b free_bits < num_bits
        else {
            u_char offset = num_bits - free_bits;
            //first part of the bits to the rest of the free bits
            buf[byte_ptr++] |= num >> offset;
            buf[byte_ptr] = num << (8 - offset);
            bit_ptr = offset;
        }

        if(bit_ptr == 8){
            byte_ptr++;
            bit_ptr = 0;
        }
    };

    /*
        Intended for forward writing of bits. For instance in Fibonacci coding.
    */
    static inline void Write(u_char* buf, u_int& byte_ptr, u_char& bit_ptr, u_char num, u_char num_bits, u_int offset)
    {

    };

    static inline void Write(u_char* buf, u_int& byte_ptr, u_char& bit_ptr, u_long num, u_char num_bits)
    {
        u_char free_bits = 8 - bit_ptr;

        //case a free_bits >= num_bits
        if (free_bits >= num_bits) {
            //all bits fits into buf[byte]
            //compute neccessary offset
            buf[byte_ptr] |= num << (free_bits - num_bits);
            //move to the bit_ptr to the empty position
            bit_ptr += num_bits;
        }
            //case b free_bits < num_bits
        else {
            u_int rest_bits = num_bits;
            u_char offset = num_bits - free_bits;
            //first part of the bits to the rest of the free bits
            buf[byte_ptr++] |= num >> offset;
            rest_bits -= free_bits;
            //next bit to write
            //move to the left then to the write
            while(rest_bits > 8){
                buf[byte_ptr++] |= (num << (32 - rest_bits)) >> 24;
                rest_bits -= 8;
            }

            buf[byte_ptr] = num << (32 - rest_bits) >> 24;
            bit_ptr = rest_bits;
        }

        if (bit_ptr == 8) {
            byte_ptr++;
            bit_ptr = 0;
        }
    };

    static bool ReadBit(u_char* buffer, u_int& byte_ptr, u_char& bit_ptr)
    {
        bool bit = (buffer[byte_ptr] & uchar_bits[bit_ptr]) == uchar_bits[bit_ptr] ? true : false;

        if(++bit_ptr % 8 == 0){
            byte_ptr += 1;
            bit_ptr = 0;
        }

        return bit;
    };

    static u_char ReadByte(u_char* buffer, u_int& byte_ptr, u_char& bit_ptr, u_char num_bits)
    {
        u_char num = 0;

        u_char rest_bits = 8 - bit_ptr;
        if (rest_bits >= num_bits) {
            u_char byte = buffer[byte_ptr] << bit_ptr;
            num = byte >> ( 8 - num_bits);
            bit_ptr += num_bits;
        }
        else {
            rest_bits = num_bits - rest_bits;
            u_char byte = buffer[byte_ptr++] << bit_ptr;
            num = byte >> (8 - num_bits);
            num |= buffer[byte_ptr] >> (8 - rest_bits);
            bit_ptr = rest_bits;
        }

        if (bit_ptr % 8 == 0) {
            byte_ptr += 1;
            bit_ptr = 0;
        }

        return num;
    };

    static u_int ReadDWord(u_char* buffer, u_int& byte_ptr, u_char& bit_ptr, u_char num_bits)
    {
        u_int num = 0;

        u_char rest_bits = 8 - bit_ptr;
        if (rest_bits >= num_bits) {
            u_char byte = buffer[byte_ptr] << bit_ptr;
            num = byte >> (8 - num_bits);
            bit_ptr += num_bits;
        }
        else {
            u_int byte = buffer[byte_ptr++];
            num = (byte << (32 - (8 - bit_ptr))) >> (32 - num_bits);
            rest_bits = num_bits - rest_bits;

            while (rest_bits > 8) {
                //read and fill the rest of the bits in num
                byte = buffer[byte_ptr++];
                rest_bits -= 8;
                num |= byte << rest_bits;
            }
            num |= buffer[byte_ptr] >> (8 - rest_bits);
            bit_ptr = rest_bits;
        }

        if (bit_ptr % 8 == 0) {
            byte_ptr += 1;
            bit_ptr = 0;
        }

        return num;
    };

    static u_char* ReadBits(u_char* buffer, u_int& byte_ptr, u_char& bit_ptr, u_int num_bits)
    {
        bool rest_bits = false;
        u_int size = num_bits / 8;
        if (num_bits % 8 != 0) {
            size += 1;
            rest_bits = true;
        }

        u_char* buf = new u_char[size];
        memset(buf, 0, sizeof(u_char)*size);

        if (rest_bits) {
            for (u_int i = 0; i < size - 1; i++)
                buf[i] = ReadByte(buffer, byte_ptr, bit_ptr, 8);

            buf[size - 1] = ReadByte(buffer, byte_ptr, bit_ptr, num_bits % 8);
        }
        else {
            for (u_int i = 0; i < size; i++)
                buf[i] = ReadByte(buffer, byte_ptr, bit_ptr, 8);
        }

        return buf;
    };

    /* Shows binary version of number n*/
    static void ShowBinary(u_int n)
    {
        int k = 31;

        while (k >= 0) {
            u_int power = pow(2, k);
            if (n / power > 0) {
                cout << "1";
                n -= power;
            }
            else
                cout << "0";
            k--;
        }
        cout << endl;
    };
};
