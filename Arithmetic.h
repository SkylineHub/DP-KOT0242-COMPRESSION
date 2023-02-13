#pragma once

#include <iostream>
#include <math.h>

#include "ArrayUtils.h"
#include "Bitlib.h"
#include "BitBuffer.h"
#include "Definitions.h"
#include "Node.h"

using namespace std;


struct ArithmeticProb
{
    u_int lower;
    u_int upper;
};


template<class T>
class ArithmeticModel
{
private:
    u_int* m_f;
    u_int m_alfa_size;
    u_long max_freq;
    ArithmeticProb** m_probs;
    Node* root = newNode('|');

public:
    /*
        Static version of model.
    */
    ArithmeticModel(T* src, u_int src_len, u_int alfa_size) {
        m_alfa_size = alfa_size + 1;
        m_f = new u_int[m_alfa_size];
        m_probs = new ArithmeticProb*[m_alfa_size];
        memset(m_f, 0, sizeof(u_int)*m_alfa_size);

        for (u_int i = 0; i < src_len; i++)
            m_f[src[i]]++; //m_f['A']++;

        //count cumuls
        m_probs[0] = new ArithmeticProb();
        m_probs[0]->lower = 0;
        m_probs[0]->upper = m_f[0];

        for (u_int i = 1; i < m_alfa_size; i++) {
            m_probs[i] = new ArithmeticProb();
            m_probs[i]->lower = m_probs[i - 1]->upper;
            m_probs[i]->upper = m_probs[i]->lower + m_f[i];
        }

    };

    void searchChild(Node* parent) {
        for (int i = 0; i < 128; i++) {
            if (parent->child[i] != nullptr) {
                //Node* child = parent->child[i];
                //cout << int(child->key) << " - " << char(child->key) << " - " << child->freq << " - " << child->lower  << " - " << child->upper << endl;
                searchChild(parent->child[i]);
            }
        }
        //cout << "k" << endl;
    }

    void searchChild(Node* parent, int &level, u_int &currentLow) {
        Node* child;
        for (int i = 0; i < 128; i++) {
            if (parent->child[i] != nullptr) {
                if (level == 4) {
                    child = parent->child[i];
                    child->lower=currentLow;
                    currentLow = child->lower + child->freq;
                    child->upper=currentLow;
                }
                //cout << int(child->key) << " - " << char(child->key) << " - " << child->freq << endl;
                //cout << level << endl;
                if (level < 4) {
                    level += 1;
                    searchChild(parent->child[i], level, currentLow);
                }
            }
        }

        level -= 1;
        //cout << "k" << endl;
    }

    ArithmeticModel(T* src, u_int src_len, u_int alfa_size, u_int k, u_int s) {
        for (long i = 0; i < src_len-5; i++) {
            addNode(root, src[i]);
        }

        u_int currentLow = 0;

        for (int i = 0; i < 128; i++) {
            if (root->child[i] != nullptr) {
                Node *child = root->child[i];
                child->lower = currentLow;
                currentLow = child->lower + child->freq;
                child->upper = currentLow;
            }
        }
    };

    void setChildProb(Node* parent, int &level, u_int k) {
        Node* child;
        for (int i = 0; i < 128; i++) {
            if (parent->child[i] != nullptr){
                //cout << i << endl;
                child = parent->child[i];
                child->lower = parent->currentLow;
                parent->currentLow = child->lower + child->freq;
                child->upper = parent->currentLow;
                //cout << level << " - (" << char(parent->child[i]->key) << ") - " << parent->child[i]->lower << " - " << parent->child[i]->upper << " - " << char(parent->child[i]->parent->key) << endl;
                if (max_freq < child->freq) {
                    max_freq = child->freq;
                }
                if (level < k) {
                    level += 1;
                    setChildProb(parent->child[i], level, k);
                }
            }
        }
        level -= 1;
        //cout << "k" << endl;
    }

    ArithmeticModel(T* src, u_int src_len) {
        for (long i = 0; i < src_len-2; i++) {
            addNode(root, src[i], src[i+1]);
        }

        addNode(root, src[src_len-1]);

        int level = 0;

        //cout << level << " - " << char(root->child[src[0]]->key) << " - " << root->child[src[0]]->lower << " - " << root->child[src[0]]->upper << " - " << char(child->parent->key) << endl;

        setChildProb(root, level, 1);
    };

    ArithmeticModel() {

    };

    ArithmeticModel(T* src, u_int src_len, u_int alfa_size, u_int k) {
        max_freq = 0;
        createSixLevelTree(src, src_len, alfa_size);
    };

    void createSixLevelTree(T* src, u_int src_len, u_int alfa_size) {
        cout << "Creating six level model" << endl;
        for (long i = 0; i < src_len-6; i++) {
            addNode(root, src[i], src[i+1], src[i+2], src[i+3], src[i+4], src[i+5]);
        }

        addLastNode(root, src[src_len-6], src[src_len-5], src[src_len-4], src[src_len-3], src[src_len-2], src[src_len-1]);
        addNode(root, src[src_len-5], src[src_len-4], src[src_len-3], src[src_len-2], src[src_len-1]);
        addNode(root, src[src_len-4], src[src_len-3], src[src_len-2], src[src_len-1]);
        addNode(root, src[src_len-3], src[src_len-2], src[src_len-1]);
        addNode(root, src[src_len-2], src[src_len-1]);
        addNode(root, src[src_len-1]);

        int level = 0;
        root->freq = src_len;

        cout << "Setting prob" << endl;

        setChildProb(root, level, 6);
    };

    void createFiveLevelTree(T* src, u_int src_len, u_int alfa_size) {
        for (long i = 0; i < src_len-5; i++) {
            addNode(root, src[i], src[i+1], src[i+2], src[i+3], src[i+4]);
        }

        addNode(root, src[src_len-5], src[src_len-4], src[src_len-3], src[src_len-2], src[src_len-1]);
        addNode(root, src[src_len-4], src[src_len-3], src[src_len-2], src[src_len-1]);
        addNode(root, src[src_len-3], src[src_len-2], src[src_len-1]);
        addNode(root, src[src_len-2], src[src_len-1]);
        addNode(root, src[src_len-1]);

        int level = 0;
        root->freq = src_len;

        setChildProb(root, level, 5);
    };

    void createFourLevelTree(T* src, u_int src_len, u_int alfa_size) {
        for (long i = 0; i < src_len-4; i++) {
            addNode(root, src[i], src[i+1], src[i+2], src[i+3]);
        }

        addNode(root, src[src_len-4], src[src_len-3], src[src_len-2], src[src_len-1]);
        addNode(root, src[src_len-3], src[src_len-2], src[src_len-1]);
        addNode(root, src[src_len-2], src[src_len-1]);
        addNode(root, src[src_len-1]);

        int level = 0;
        root->freq = src_len;

        setChildProb(root, level, 4);
    };

    void createThreeLevelTree(T* src, u_int src_len, u_int alfa_size) {
        for (long i = 0; i < src_len-3; i++) {
            addNode(root, src[i], src[i+1], src[i+2]);
        }

        addNode(root, src[src_len-3], src[src_len-2], src[src_len-1]);
        addNode(root, src[src_len-2], src[src_len-1]);
        addNode(root, src[src_len-1]);

        int level = 0;
        root->freq = src_len;

        setChildProb(root, level, 3);
    };

    void createTwoLevelTree(T* src, u_int src_len, u_int alfa_size) {
        for (long i = 0; i < src_len-2; i++) {
            addNode(root, src[i], src[i+1]);
        }

        addNode(root, src[src_len-2], src[src_len-1]);
        addNode(root, src[src_len-1]);

        int level = 0;
        root->freq = src_len;

        setChildProb(root, level, 2);
    };

    void createOneLevelTree(T* src, u_int src_len, u_int alfa_size) {
        for (long i = 0; i < src_len-1; i++) {
            addNode(root, src[i]);
        }

        addNode(root, src[src_len-1]);

        int level = 0;
        root->freq = src_len;

        setChildProb(root, level, 1);
    };

    /*
        Static initialization for adaptive models and decompression.
    */
    ArithmeticModel(u_int alfa_size) {
        m_alfa_size = alfa_size + 1;
        m_f = new u_int[m_alfa_size];
        m_probs = new ArithmeticProb*[m_alfa_size];

        for (u_int i = 0; i < m_alfa_size; i++)
            m_f[i] = 1;

        m_probs[0] = new ArithmeticProb();
        m_probs[0]->lower = 0;
        m_probs[0]->upper = m_f[0];
        for (u_int i = 1; i < m_alfa_size; i++) {
            m_probs[i] = new ArithmeticProb();
            m_probs[i]->lower = m_probs[i - 1]->upper;
            m_probs[i]->upper = m_probs[i]->lower + m_f[i];
        }
    };

    ~ArithmeticModel() {
        delete[] m_f;

        for (u_int i = 0; i < m_alfa_size; i++)
            delete m_probs[i];
        delete[] m_probs;

    };

    ArithmeticProb* GetProbability(T c) {
        return m_probs[c];
    };

    Node* GetRoot() {
        return root;
    };

    u_long MaxFrequency() {
        u_long max_f = 0;
        for (u_long i = 0; i < m_alfa_size-1; i++) {
            if (max_f < m_f[i])
                max_f = m_f[i];
        }
        return max_f;
    };

    u_long GetMaxFrequency() {
        return max_freq;
    }

    void SetFrequency(T c, u_long f) {
        m_f[c] = f;
    };

    u_long Frequency(u_long c) {
        return m_f[c];
    };

    void Cumulate() {
        //count cumuls
        m_probs[0] = new ArithmeticProb();
        m_probs[0]->lower = 0;
        m_probs[0]->upper = m_f[0];

        for (u_int i = 1; i < m_alfa_size; i++) {
            m_probs[i] = new ArithmeticProb();
            m_probs[i]->lower = m_probs[i - 1]->upper;
            m_probs[i]->upper = m_probs[i]->lower + m_f[i];
        }
    };

    ArithmeticProb* GetChar(u_int count, u_int& sym)
    {
        for (u_int i = 0; i < m_alfa_size; i++) {
            if (m_probs[i]->lower <= count && m_probs[i]->upper > count) {
                sym = i;
                return m_probs[i];
            }
        }

        cerr << "Error: Arithmetic Model: count do not fit into cumul." << endl;
        return NULL;
    };

    inline void Increment(u_int symbol)
    {
        //increment symbol's frequency
        m_f[symbol]++;
        //update cumuls
        if (symbol != 0) {
            for (u_int i = symbol; i < m_alfa_size; i++) {
                m_probs[i]->lower = m_probs[i - 1]->upper;
                m_probs[i]->upper++;
            }
        }
        else {
            m_probs[0]->lower = 0;
            m_probs[0]->upper = m_f[0];
            for (u_int i = 1; i < m_alfa_size; i++) {
                m_probs[i]->lower = m_probs[i - 1]->upper;
                m_probs[i]->upper++;
            }
        }
    };
};

template <class T>
class ArithmeticCoder
{
protected:
    u_long m_alfa_len;

    double* m_probs;
    u_int* m_cumuls;

    T* m_data;
    u_long m_datalen;

    ArithmeticModel<T>* m_model;
    u_long m_buf_len;

    void EncodeNext(T sym, u_int& low, u_int& high, u_char* outbuf, u_long& buflen, u_int& byte_ptr, u_char& bit_ptr, int& pending_bits) {
        long long range = high - low;
        range += 1;

        ArithmeticProb* p = m_model->GetProbability(sym);

        high = low + (p->upper * range) / m_datalen - 1;
        low = low + (p->lower * range) / m_datalen;

        for (;;) {
            if (low >= 0x80000000U && high >= 0x80000000U) {
                //the most significant bit 1 is equal
                OutputBitPlusPending(1, pending_bits, outbuf, buflen, byte_ptr, bit_ptr);
                low <<= 1;
                high <<= 1;
                high |= 1;
            }
            else if (low < 0x80000000 && high < 0x80000000U) {
                //the most significant bit 1 is equal
                OutputBitPlusPending(0, pending_bits, outbuf, buflen, byte_ptr, bit_ptr);
                low <<= 1;
                high <<= 1;
                high |= 1;
            }
            else if (low >= 0x40000000 && high < 0xC0000000U){
                low <<= 1;
                high <<= 1;
                high |= 1;

                low += 0x80000000;
                high += 0x80000000;

                pending_bits++;
            }
            else
                break;
        }
    };

    void EncodeNextWithK(Node *sym, u_int& low, u_int& high, u_char* outbuf, u_long& buflen, u_int& byte_ptr, u_char& bit_ptr, int& pending_bits) {
        long long range = high - low;
        range += 1;

        high = low + (sym->upper * range) / sym->parent->freq - 1;
        low = low + (sym->lower * range) / sym->parent->freq;

        for (;;) {
            if (low >= 0x80000000U && high >= 0x80000000U) {
                //the most significant bit 1 is equal
                OutputBitPlusPending(1, pending_bits, outbuf, buflen, byte_ptr, bit_ptr);
                low <<= 1;
                high <<= 1;
                high |= 1;
            }
            else if (low < 0x80000000 && high < 0x80000000U) {
                //the most significant bit 1 is equal
                OutputBitPlusPending(0, pending_bits, outbuf, buflen, byte_ptr, bit_ptr);
                low <<= 1;
                high <<= 1;
                high |= 1;
            }
            else if (low >= 0x40000000 && high < 0xC0000000U){
                low <<= 1;
                high <<= 1;
                high |= 1;

                low += 0x80000000;
                high += 0x80000000;

                pending_bits++;
            }
            else
                break;
        }
    };

    void OutputBitPlusPending(u_char bit, int &pending_bits, u_char* outbuf, u_long& buflen, u_int& byte_ptr, u_char& bit_ptr)
    {
        Bitlib::Write(outbuf, byte_ptr, bit_ptr, (u_char)bit, 1);
        while (pending_bits > 0) {
            if (bit == 0) {
                //cout << "1";
                Bitlib::Write(outbuf, byte_ptr, bit_ptr, (u_char)1, 1);
            }
            else {
                //cout << "0";
                Bitlib::Write(outbuf, byte_ptr, bit_ptr, (u_char)0, 1);
            }
            //cout << endl;
            pending_bits--;
        }
    };

    void EncodeFinish(u_int L, int& pending_bits, u_char* outbuf, u_long buflen, u_int& byte_ptr, u_char& bit_ptr)
    {
        pending_bits += 1;

        if (L < 0x40000000)
            OutputBitPlusPending(0, pending_bits, outbuf, buflen, byte_ptr, bit_ptr);
        else
            OutputBitPlusPending(1, pending_bits, outbuf, buflen, byte_ptr, bit_ptr);
    };

    u_int DecodeNext(u_int& L, u_int& H, u_int& code, u_int cumul_tot, u_char* outbuf, u_int buflen, u_int& byte_ptr, u_char& bit_ptr)
    {
        long long range = H - L;
        range += 1;
        long long p_sum = code - L;
        //long long p_sum = code;
        u_int count = ((p_sum + 1) * cumul_tot - 1) / range;

        u_int sym;
        ArithmeticProb* p = m_model->GetChar(count, sym);

        H = L + (range*p->upper) / cumul_tot - 1;
        L = L + (range*p->lower) / cumul_tot;

        while (true) {
            if (H < 0x80000000) {

            }
            else if (L >= 0x80000000) {
                code -= 0x80000000;
                L -= 0x80000000;
                H -= 0x80000000;
            }
            else if (L >= 0x40000000 && H < 0xC0000000U) {
                code -= 0x40000000;
                L -= 0x40000000;
                H -= 0x40000000;
            }
            else {
                break;
            }

            L <<= 1;
            H <<= 1;
            H += 1;
            ReadBits(code, 1, outbuf, buflen, byte_ptr, bit_ptr);
        }


        return sym;
    };

    Node* findNode(Node* parent, u_int count) {
        //cout << "Finding:" << count << endl;
        for(int i = 0; i < 128; i++) {
            if (parent->child[i] != nullptr) {
                //cout << parent->child[i]->lower << " <= " << count << " & " << parent->child[i]->upper << " > " << count << endl;
                if (parent->child[i]->lower <= count && parent->child[i]->upper > count)
                    return parent->child[i];
            }
        }

        cerr << "With K: Error: Arithmetic Model: count do not fit into cumul." << endl;
        return nullptr;
    }

    u_int DecodeNextWithK(u_int& L, u_int& H, u_int& code, u_int cumul_tot, u_char* outbuf, u_int buflen, u_int& byte_ptr, u_char& bit_ptr, Node* root)
    {
        long long range = H - L;
        range += 1;
        long long p_sum = code - L;
        //long long p_sum = code;
        u_int count = ((p_sum + 1) * root->freq - 1) / range;

        Node* node = findNode(root, count);

        H = L + (range*node->upper) / root->freq - 1;
        L = L + (range*node->lower) / root->freq;

        while (true) {
            if (H < 0x80000000) {

            }
            else if (L >= 0x80000000) {
                code -= 0x80000000;
                L -= 0x80000000;
                H -= 0x80000000;
            }
            else if (L >= 0x40000000 && H < 0xC0000000U) {
                code -= 0x40000000;
                L -= 0x40000000;
                H -= 0x40000000;
            }
            else {
                break;
            }

            L <<= 1;
            H <<= 1;
            H += 1;
            ReadBits(code, 1, outbuf, buflen, byte_ptr, bit_ptr);
        }


        return node->key;
    };

    /*
        reads bit_no of bits from outbuf into LSB of code
    */
    void ReadBits(u_int& code, u_int bit_no, u_char* outbuf, u_int buflen, u_int& byte_ptr, u_char& bit_ptr)
    {
        while (bit_no != 0) {
            code <<= 1;
            if (byte_ptr < buflen) {
                if (Bitlib::ReadBit(outbuf, byte_ptr, bit_ptr))
                    code = code | 1;
            }

            bit_no--;
        }
    };

public:
    //decompression
    ArithmeticCoder() {
    };

    ArithmeticCoder(T* data, u_int datalen, u_int alfa_len) {
        m_data = data;
        m_datalen = datalen;

        m_alfa_len = alfa_len;

        m_model = new ArithmeticModel<T>(m_data, m_datalen, m_alfa_len);
    };

    ArithmeticCoder(T* data, u_int datalen, u_int alfa_len, u_int k) {
        m_data = data;
        m_datalen = datalen;

        m_alfa_len = alfa_len;
        //m_model = new ArithmeticModel<T>(m_data, m_datalen);
        m_model = new ArithmeticModel<T>(m_data, m_datalen, m_alfa_len, k+1);
        //m_model = new ArithmeticModel<T>(m_data, m_datalen, m_alfa_len, k, 0);
    };

    ~ArithmeticCoder() {
        delete m_model;
    };

    u_char* Compress(u_int& length)
    {
        u_long buf_len = 8 * m_alfa_len;
        u_char* outbuf = new u_char[buf_len];
        memset(outbuf, 0, sizeof(u_char)*buf_len);

        u_int byte_ptr = 0;
        u_char bit_ptr = 0;

        /*
            Store frequencies.
            - in 5 bits store n = up(log2(alfa_size))
            - store alfa size using n bits
            - find max_f
            - compute m=up(log2(maxf))
            - in 5 bits store m
            - in m bits store all frequencies
        */

        u_char alfa_bits = ceil(log2(m_alfa_len));
        Bitlib::Write(outbuf, byte_ptr, bit_ptr, alfa_bits, 5);
        Bitlib::Write(outbuf, byte_ptr, bit_ptr, m_alfa_len - 1, alfa_bits);
        //prvních 5 bitů --> na kolika bitech je velikost abecedy
        //podle velikosti prvních 5 bitů --> známe velikost abecedy

        u_long max_f = m_model->MaxFrequency();

        u_char max_f_bits = ceil(log2(max_f+1));
        Bitlib::Write(outbuf, byte_ptr, bit_ptr, max_f_bits, 5);
        //na dalších 5 bitech je počet bitů, na kterých je zapsána maximální frekvence

        for (u_int i = 0; i < m_alfa_len; i++) {
            if (byte_ptr + 4 > buf_len)
                ArrayUtils::Resize(outbuf, buf_len, m_alfa_len);

            Bitlib::Write(outbuf, byte_ptr, bit_ptr, m_model->Frequency(i), max_f_bits);
            //zapisování jednotlivých frekvencí na maximální možný počet bitů
        }

        Bitlib::Write(outbuf, byte_ptr, bit_ptr, m_datalen, 32);
        //na posledních 32 bitech hlavičky je zapsána délka dat

        u_int L = 0;
        u_int H = UINT32_MAX;
        int pending_bits = 0;

/*
        for (u_long i = 0; i < m_datalen; i++) {
            EncodeNext(m_data[i], L, H, outbuf, buf_len, byte_ptr, bit_ptr, pending_bits);
            //cout << byte_ptr << endl;
            if (buf_len < (byte_ptr + 8))
                ArrayUtils::Resize(outbuf, buf_len, buf_len);
        }
        EncodeFinish(L, pending_bits, outbuf, buf_len, byte_ptr, bit_ptr);
*/
        //cout << "buflen:" << buf_len << endl;
        //cout << endl << byte_ptr << "/" << (u_int)bit_ptr << endl;

        if (bit_ptr > 0)
            length = byte_ptr + 1;
        else
            length = byte_ptr;

        m_buf_len = byte_ptr;
        return outbuf;
    };

    void nodeWalkthroughT(Node* root) {
        for (int i = 0; i < 128; i++) {
            if (root->child[i] != nullptr) {
                //cout << root->child[i]->key << " - " << root->child[i]->freq << endl;
                nodeWalkthroughT(root->child[i]);
            }
            if (i == 127) {
                //cout << "k" << endl;
            }
        }
    }

    void treeWalkthrough() {
        Node* root = m_model->GetRoot();
        nodeWalkthroughT(root);
    }

    void nodeWalkthrough(Node* parent, u_char*& outbuf, u_char max_f_bits, u_long& buf_len, u_int& byte_ptr, u_char& bit_ptr) {
        for (int i = 0; i < 128; i++) {
            if (parent->child[i] != nullptr) {
                //cout << "-" << u_int(parent->child[i]->key) << " - " << parent->child[i]->freq << endl;
                if (byte_ptr + max_f_bits + 7 > buf_len)
                    ArrayUtils::Resize(outbuf, buf_len, buf_len);

                Bitlib::Write(outbuf, byte_ptr, bit_ptr, u_int(parent->child[i]->key), 7);
                Bitlib::Write(outbuf, byte_ptr, bit_ptr, u_long(parent->child[i]->freq), max_f_bits);
                nodeWalkthrough(parent->child[i], outbuf, max_f_bits, buf_len, byte_ptr, bit_ptr);
            }
        }
        if (byte_ptr + max_f_bits + 7 > buf_len)
            ArrayUtils::Resize(outbuf, buf_len, buf_len);

        Bitlib::Write(outbuf, byte_ptr, bit_ptr, u_int(6), 7);
        Bitlib::Write(outbuf, byte_ptr, bit_ptr, u_long(0), max_f_bits);
    }

    void CompressHeader(u_char*& outbuf, u_long& buf_len, u_int& byte_ptr, u_char& bit_ptr){
        u_long max_f = m_model->GetMaxFrequency();
        u_char max_f_bits = ceil(log2(max_f+1));
        //cout << int(max_f_bits) << endl;
        Bitlib::Write(outbuf, byte_ptr, bit_ptr, max_f_bits, 5);
        //na dalších 5 bitech je počet bitů, na kterých je zapsána maximální frekvence

        Node* root = m_model->GetRoot();

        nodeWalkthrough(root, outbuf, max_f_bits, buf_len, byte_ptr, bit_ptr);
        //cout << max_f << endl;

        if (byte_ptr + 32 > buf_len)
            ArrayUtils::Resize(outbuf, buf_len, buf_len);

        Bitlib::Write(outbuf, byte_ptr, bit_ptr, m_datalen, 32);
        //na posledních 32 bitech hlavičky je zapsána délka dat
    }

    void DecompressHeader(u_char* inbuf, u_int& byte_ptr, u_char& bit_ptr) {
        //first decompose header
        u_char max_f_bits = Bitlib::ReadByte(inbuf, byte_ptr, bit_ptr, 5);
        //cout << ":" << int(max_f_bits) << endl;

        m_model = new ArithmeticModel<T>();
        Node* root = m_model->GetRoot();
        Node* parent = m_model->GetRoot();
        u_char key;
        u_long freq;

        while(true) {
            key = Bitlib::ReadDWord(inbuf, byte_ptr, bit_ptr, 7);
            freq = Bitlib::ReadDWord(inbuf, byte_ptr, bit_ptr, max_f_bits);
            //cout << int(key) << " - " << freq << endl;
            if (key == 6 & freq == 0 & parent == root) {
                break;
            }
            if (key == 6 & freq == 0) {
                parent = parent->parent;
            } else {
                //cout << "-" << int(key) << " - " << freq << endl;
                Node* children = newNode(key, parent, freq);
                parent->child[key] = children;
                parent = children;
            }
        }

        m_datalen = Bitlib::ReadDWord(inbuf, byte_ptr, bit_ptr, 32);
        m_model->GetRoot()->freq = m_datalen;
    }

    void CompressBody(u_char*& outbuf, u_long& buf_len, u_int& byte_ptr, u_char& bit_ptr, u_int k) {
        u_int L = 0;
        u_int H = UINT32_MAX;
        int pending_bits = 0;
        Node *root = m_model->GetRoot();

        if (buf_len < (byte_ptr + 8))
            ArrayUtils::Resize(outbuf, buf_len, buf_len);

        EncodeNextWithK(root->child[m_data[0]], L, H, outbuf, buf_len, byte_ptr, bit_ptr, pending_bits);
        if (buf_len < (byte_ptr + 8))
            ArrayUtils::Resize(outbuf, buf_len, buf_len);

        EncodeNextWithK(root->child[m_data[0]]->child[m_data[1]], L, H, outbuf, buf_len, byte_ptr, bit_ptr, pending_bits);
        if (buf_len < (byte_ptr + 8))
            ArrayUtils::Resize(outbuf, buf_len, buf_len);

        EncodeNextWithK(root->child[m_data[0]]->child[m_data[1]]->child[m_data[2]], L, H, outbuf, buf_len, byte_ptr, bit_ptr, pending_bits);
        if (buf_len < (byte_ptr + 8))
            ArrayUtils::Resize(outbuf, buf_len, buf_len);

        EncodeNextWithK(root->child[m_data[0]]->child[m_data[1]]->child[m_data[2]]->child[m_data[3]], L, H, outbuf, buf_len, byte_ptr, bit_ptr, pending_bits);
        if (buf_len < (byte_ptr + 8))
            ArrayUtils::Resize(outbuf, buf_len, buf_len);

        EncodeNextWithK(root->child[m_data[0]]->child[m_data[1]]->child[m_data[2]]->child[m_data[3]]->child[m_data[4]], L, H, outbuf, buf_len, byte_ptr, bit_ptr, pending_bits);
        if (buf_len < (byte_ptr + 8))
            ArrayUtils::Resize(outbuf, buf_len, buf_len);

        cout << m_datalen << endl;
        for (u_long i = k; i < m_datalen; i++) {
            //Node *child = root->child[m_data[i-4]]->child[m_data[i-3]]->child[m_data[i-2]]->child[m_data[i-1]]->child[m_data[i]];
            //0
            //EncodeNextWithK(root->child[m_data[i-1]], L, H, outbuf, buf_len, byte_ptr, bit_ptr, pending_bits);
            //1
            //EncodeNextWithK(root->child[m_data[i-1]]->child[m_data[i]], L, H, outbuf, buf_len, byte_ptr, bit_ptr, pending_bits);
            //2
            //EncodeNextWithK(root->child[m_data[i-2]]->child[m_data[i-1]]->child[m_data[i]], L, H, outbuf, buf_len, byte_ptr, bit_ptr, pending_bits);
            //3
            //EncodeNextWithK(root->child[m_data[i-3]]->child[m_data[i-2]]->child[m_data[i-1]]->child[m_data[i]], L, H, outbuf, buf_len, byte_ptr, bit_ptr, pending_bits);
            //4
            //EncodeNextWithK(root->child[m_data[i-4]]->child[m_data[i-3]]->child[m_data[i-2]]->child[m_data[i-1]]->child[m_data[i]], L, H, outbuf, buf_len, byte_ptr, bit_ptr, pending_bits);
            //5
            Node* node = root->child[m_data[i-5]]->child[m_data[i-4]]->child[m_data[i-3]]->child[m_data[i-2]]->child[m_data[i-1]]->child[m_data[i]];
            //cout << i << ": " << node->key << endl;
            EncodeNextWithK(node, L, H, outbuf, buf_len, byte_ptr, bit_ptr, pending_bits);

            if (buf_len < (byte_ptr + 8))
                ArrayUtils::Resize(outbuf, buf_len, buf_len);
        }
        EncodeFinish(L, pending_bits, outbuf, buf_len, byte_ptr, bit_ptr);
    }

    u_char* Compress(u_long& length, u_int k)
    {
        u_long buf_len = 8 * m_alfa_len;
        u_char* outbuf = new u_char[buf_len];
        memset(outbuf, 0, sizeof(u_char)*buf_len);

        u_int byte_ptr = 0;
        u_char bit_ptr = 0;

        CompressHeader(outbuf, buf_len, byte_ptr, bit_ptr);
        //byte_ptr = 0;
        //bit_ptr = 0;
        //DecompressHeader(outbuf, byte_ptr, bit_ptr);
        CompressBody(outbuf, buf_len, byte_ptr, bit_ptr, k);

        if (bit_ptr > 0)
            length = byte_ptr + 1;
        else
            length = byte_ptr;

        return outbuf;
    };

    T* Decompress(u_char* inbuf, u_int buflen, int k)
    {
        u_int byte_ptr = 0;
        u_char bit_ptr = 0;

        DecompressHeader(inbuf, byte_ptr, bit_ptr);

        int level = 0;
        m_model->setChildProb(m_model->GetRoot(), level, 6);

        T* m_data = new T[m_datalen];
        u_int len = 0;
        u_int L = 0;
        u_int H = 0xFFFFFFFFU;
        u_int code = 0;
        ReadBits(code, 32, inbuf, buflen, byte_ptr, bit_ptr);

        Node* node = m_model->GetRoot();
        m_data[len++] = DecodeNextWithK(L,H, code, m_datalen, inbuf, buflen, byte_ptr, bit_ptr, node);
        node = node->child[m_data[len-1]];
        m_data[len++] = DecodeNextWithK(L,H, code, m_datalen, inbuf, buflen, byte_ptr, bit_ptr, node);
        node = node->child[m_data[len-1]];
        m_data[len++] = DecodeNextWithK(L,H, code, m_datalen, inbuf, buflen, byte_ptr, bit_ptr, node);
        node = node->child[m_data[len-1]];
        m_data[len++] = DecodeNextWithK(L,H, code, m_datalen, inbuf, buflen, byte_ptr, bit_ptr, node);
        node = node->child[m_data[len-1]];
        m_data[len++] = DecodeNextWithK(L,H, code, m_datalen, inbuf, buflen, byte_ptr, bit_ptr, node);
        node = node->child[m_data[len-1]];
        m_data[len++] = DecodeNextWithK(L,H, code, m_datalen, inbuf, buflen, byte_ptr, bit_ptr, node);
        //cout << m_data << endl;

        while (m_datalen != len) {
            node = m_model->GetRoot()->child[m_data[len-5]]->child[m_data[len-4]]->child[m_data[len-3]]->child[m_data[len-2]]->child[m_data[len-1]];
            //cout << ":" << node->key << endl;
            m_data[len++] = DecodeNextWithK(L,H, code, m_datalen, inbuf, buflen, byte_ptr, bit_ptr, node);
        }

        return m_data;
    };

    T* Decompress(u_char* inbuf, u_int buflen)
    {
        u_int byte_ptr = 0;
        u_char bit_ptr = 0;

        //first decompose header
        u_char alfa_bits = Bitlib::ReadByte(inbuf, byte_ptr, bit_ptr, 5);
        m_alfa_len = Bitlib::ReadDWord(inbuf, byte_ptr, bit_ptr, alfa_bits) + 1;

        u_char f_bits = Bitlib::ReadByte(inbuf, byte_ptr, bit_ptr, 5);

        m_model = new ArithmeticModel<T>(m_alfa_len);
        for (u_int i = 0; i < m_alfa_len; i++)
            m_model->SetFrequency(i, Bitlib::ReadDWord(inbuf, byte_ptr, bit_ptr, f_bits));
        m_model->Cumulate();

        m_datalen = Bitlib::ReadDWord(inbuf, byte_ptr, bit_ptr, 32);
        T* m_data = new T[m_datalen];
        u_int len = 0;
        u_int L = 0;
        u_int H = 0xFFFFFFFFU;
        u_int code = 0;
        ReadBits(code, 32, inbuf, buflen, byte_ptr, bit_ptr);

        while (m_datalen != len) {
            m_data[len++] = DecodeNext(L,H, code, m_datalen, inbuf, buflen, byte_ptr, bit_ptr);
        }

        return m_data;
    };

    u_long getDataLen() {
        return m_datalen;
    }

    T* Decompress(BitBuffer* bB, u_int buflen)
    {
        //first decompose header
        u_char alfa_bits = Bitlib::ReadByte(bB->outbuf, bB->byte_ptr, bB->bit_ptr, 5);
        m_alfa_len = Bitlib::ReadDWord(bB->outbuf, bB->byte_ptr, bB->bit_ptr, alfa_bits) + 1;

        u_char f_bits = Bitlib::ReadByte(bB->outbuf, bB->byte_ptr, bB->bit_ptr, 5);

        m_model = new ArithmeticModel<T>(m_alfa_len);
        for (u_int i = 0; i < m_alfa_len; i++)
            m_model->SetFrequency(i, Bitlib::ReadDWord(bB->outbuf, bB->byte_ptr, bB->bit_ptr, f_bits));
        m_model->Cumulate();

        m_datalen = Bitlib::ReadDWord(bB->outbuf, bB->byte_ptr, bB->bit_ptr, 32);

        T* m_data = new T[m_datalen];
        u_int len = 0;
        u_int L = 0;
        u_int H = 0xFFFFFFFFU;
        u_int code = 0;
        ReadBits(code, 32, bB->outbuf, bB->buflen, bB->byte_ptr, bB->bit_ptr);

        while (m_datalen != len) {
            m_data[len++] = DecodeNext(L, H, code, m_datalen, bB->outbuf, bB->buflen, bB->byte_ptr, bB->bit_ptr);
        }

        return m_data;
    };
};