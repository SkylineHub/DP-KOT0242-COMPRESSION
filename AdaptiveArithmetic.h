#pragma once

#include "Arithmetic.h"

template<class T>
class AdaptiveArithmeticCoder: public ArithmeticCoder<T>
{
private:
    u_int m_src_len;

public:
    AdaptiveArithmeticCoder(T* data, u_int datalen, u_int alfa_len) {
        this->m_data = data;
        m_src_len = datalen;
        this->m_datalen = alfa_len;

        this->m_alfa_len = alfa_len;

        this->m_model = new ArithmeticModel<T>(this->m_alfa_len);
    };

    AdaptiveArithmeticCoder() {
        this->m_model = NULL;
    };

    ~AdaptiveArithmeticCoder() {
        delete this->m_model;
        this->m_model = NULL;
    }

    u_char* Compress(u_int& length)
    {
        u_int buf_len = 8 * this->m_alfa_len;
        u_char* outbuf = new u_char[buf_len];
        memset(outbuf, 0, sizeof(u_char)*buf_len);

        u_int byte_ptr = 0;
        u_char bit_ptr = 0;

        //write alfa size
        u_char alfa_bits = ceil(log2(this->m_alfa_len));
        Bitlib::Write(outbuf, byte_ptr, bit_ptr, alfa_bits, 5);
        Bitlib::Write(outbuf, byte_ptr, bit_ptr, this->m_alfa_len - 1, alfa_bits);

        Bitlib::Write(outbuf, byte_ptr, bit_ptr, m_src_len, 32);

        u_int L = 0;
        u_int H = UINT32_MAX;
        int pending_bits = 0;

        for (u_int i = 0; i < m_src_len; i++) {
            this->EncodeNext(this->m_data[i], L, H, outbuf, buf_len, byte_ptr, bit_ptr, pending_bits);
            this->m_model->Increment(this->m_data[i]);
            if (buf_len < (byte_ptr + 8))
                ArrayUtils::Resize(outbuf, buf_len, buf_len);

            if (i % 10000 == 0)
                cout << i << "/" << m_src_len << endl;
            this->m_datalen++;
        }
        this->EncodeFinish(L, pending_bits, outbuf, buf_len, byte_ptr, bit_ptr);

        cout << endl << byte_ptr << "/" << (u_int)bit_ptr << endl;

        if (bit_ptr > 0)
            length = byte_ptr + 1;
        else
            length = byte_ptr;
        return outbuf;
    };

    T* Decompress(u_char* inbuf, u_int buflen)
    {
        u_int byte_ptr = 0;
        u_char bit_ptr = 0;

        //first decompose header
        u_char alfa_bits = Bitlib::ReadByte(inbuf, byte_ptr, bit_ptr, 5);
        this->m_alfa_len = Bitlib::ReadDWord(inbuf, byte_ptr, bit_ptr, alfa_bits) + 1;
        this->m_datalen = this->m_alfa_len;

        this->m_model = new ArithmeticModel<T>(this->m_alfa_len);

        m_src_len = Bitlib::ReadDWord(inbuf, byte_ptr, bit_ptr, 32);

        T* m_data = new u_int[m_src_len];
        u_int len = 0;
        u_int L = 0;
        u_int H = 0xFFFFFFFFU;
        u_int code = 0;
        this->ReadBits(code, 32, inbuf, buflen, byte_ptr, bit_ptr);

        while (m_src_len != len) {
            m_data[len] = DecodeNext(L, H, code, this->m_datalen, inbuf, buflen, byte_ptr, bit_ptr);
            this->m_model->Increment(m_data[len]);
            this->m_datalen++;
            len++;
        }

        return m_data;
    };
};
