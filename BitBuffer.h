#pragma once

/*
	The advantage of bitbuffer class is that it can be simply transmitted between different objects.
*/

#include "ArrayUtils.h"
#include "Definitions.h"

class BitBuffer
{
public:
    BitBuffer(u_int i_byte_ptr, u_char i_bit_ptr) {
        outbuf = NULL;
        buflen = 0;
        byte_ptr = i_byte_ptr;
        bit_ptr = i_bit_ptr;
    };

    BitBuffer(u_int default_capacity) : BitBuffer(0,0){
        outbuf = new u_char[default_capacity];
        memset(outbuf, 0, sizeof(u_char)*default_capacity);
        buflen = default_capacity;
    };

    ~BitBuffer(){
        delete[] outbuf;
    }

    /*
        All properties public.
    */
    u_char* outbuf;
    u_long buflen;

    u_int byte_ptr;
    u_char bit_ptr;

    /*
        Add memory block.
    */
    void Add(u_char* block, u_int block_size)
    {
        if (block_size + byte_ptr + 1 > buflen)
            Resize(block_size + 1, true);

        if (bit_ptr == 0){
            memcpy(outbuf + byte_ptr, block, sizeof(u_char)*block_size);
            byte_ptr += block_size;
        }
        else {
            memcpy(outbuf + byte_ptr + 1, block, sizeof(u_char)*block_size);
            byte_ptr += block_size + 1;
            bit_ptr = 0;
        }
    };

    /*
        Resize - the function resize only if neccessary
    */
    void Resize(u_int additional_space, bool forced = false)
    {
        if (byte_ptr + 4 > buflen || forced)
            ArrayUtils::Resize(outbuf, buflen, additional_space);
    }

    /*
        Serialize section.
    */
};
