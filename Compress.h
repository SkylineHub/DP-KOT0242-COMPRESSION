#include <iostream>
#include <fstream>
#include "Arithmetic.h"

using namespace std;

class Compress {
private:
    struct Buffer {
        u_long length;
        u_long size;
        char* data;
    };

    Buffer* newBuffer(u_long size) {
        auto* buffer = new Buffer;
        buffer->size = size;
        buffer->data = new char[size];
        memset(buffer->data, 0, sizeof(char)*buffer->size);
        return buffer;
    }
public:
    void CompressFile(const string& in_filename, string out_filename, int k) {
        ifstream file(in_filename, std::ios::binary | std::ios::ate);
        streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);
        char* buffer = new char[size];

        if(file.read(buffer, size)) {
            auto *arithmeticCoder = new ArithmeticCoder(buffer, size, 128, k);
            u_long length = 0;

            u_char *outbuf = arithmeticCoder->Compress(length, k);
            streamsize out_size = length;
            ofstream outfile(out_filename, std::ios::binary | std::ios::ate);
            outfile.write(reinterpret_cast<const char *>(outbuf), out_size);
            outfile.close();

        }

        file.close();
    }

    void CompressFileWithContext(const string& in_filename, string out_filename, int k) {
        ifstream file(in_filename, std::ios::binary | std::ios::ate);

        string line;
        auto* header = newBuffer(8192);

        auto* first = newBuffer(8192);

        auto* second = newBuffer(8192);

        auto* third = newBuffer(8192);

        auto* forth = newBuffer(8192);

        file.seekg(-1, std::ios::end);
        char ch = file.get();
        file.seekg(0, std::ios::beg);

        while(getline(file, line)) {
            if (line[0] == '0') {
                if (first->size < (first->length + line.length() + 1))
                    ArrayUtils::Resize(first->data, first->size, first->size);
                for (char ch : line) {
                    first->data[first->length] = ch;
                    first->length += 1;
                }
                first->data[first->length] = '\n';
                first->length += 1;
                continue;
            }
            if (line[0] == '1') {
                if (second->size < (second->length + line.length() + 1))
                    ArrayUtils::Resize(second->data, second->size, second->size);
                for (char ch : line) {
                    second->data[second->length] = ch;
                    second->length += 1;
                }
                second->data[second->length] = '\n';
                second->length += 1;
                continue;
            }
            if (line[0] == 'Q' && line[3] == '1') {
                if (third->size < (third->length + line.length() + 1))
                    ArrayUtils::Resize(third->data, third->size, third->size);
                for (char ch : line) {
                    third->data[third->length] = ch;
                    third->length += 1;
                }
                third->data[third->length] = '\n';
                third->length += 1;
                continue;
            }
            if (line[0] == 'Q' && line[3] == '2') {
                if (forth->size < (forth->length + line.length() + 1))
                    ArrayUtils::Resize(forth->data, forth->size, forth->size);
                for (char ch : line) {
                    forth->data[forth->length] = ch;
                    forth->length += 1;
                }
                forth->data[forth->length] = '\n';
                forth->length += 1;
                continue;
            }
            if (line[0] == '#') {
                if (header->size < (header->length + line.length() + 1))
                    ArrayUtils::Resize(header->data, header->size, header->size);
                for (char ch : line) {
                    header->data[header->length] = ch;
                    header->length += 1;
                }
                header->data[header->length] = '\n';
                header->length += 1;
                continue;
            }
            cout << "Error" << endl;
        }


        if (ch != '\n') {
            forth->length -= 1;
        }

        file.close();

        auto *arithmeticCoderHeader = new ArithmeticCoder(header->data, header->length, 128, k);
        u_long lengthHeader = 0;

        auto *arithmeticCoderFirst = new ArithmeticCoder(first->data, first->length, 128, k);
        u_long lengthFirst = 0;

        auto *arithmeticCoderSecond = new ArithmeticCoder(second->data, second->length, 128, k);
        u_long lengthSecond = 0;

        auto *arithmeticCoderThird = new ArithmeticCoder(third->data, third->length, 128, k);
        u_long lengthThird = 0;

        auto *arithmeticCoderForth = new ArithmeticCoder(forth->data, forth->length, 128, k);
        u_long lengthForth = 0;

        ofstream outfile(out_filename, std::ios::binary | std::ios::ate);

        u_char *outbufHeader = arithmeticCoderHeader->Compress(lengthHeader, k);
        u_char *outbufFirst = arithmeticCoderFirst->Compress(lengthFirst, k);
        u_char *outbufSecond = arithmeticCoderSecond->Compress(lengthSecond, k);
        u_char *outbufThird = arithmeticCoderThird->Compress(lengthThird, k);
        u_char *outbufForth = arithmeticCoderForth->Compress(lengthForth, k);

        u_char* sizeBuffer = new u_char[20];
        memset(sizeBuffer, 0, sizeof(u_char)*20);
        u_int byte_ptr = 0;
        u_char bit_ptr = 0;

        Bitlib::Write(sizeBuffer, byte_ptr, bit_ptr, lengthHeader, 32);
        Bitlib::Write(sizeBuffer, byte_ptr, bit_ptr, lengthFirst, 32);
        Bitlib::Write(sizeBuffer, byte_ptr, bit_ptr, lengthSecond, 32);
        Bitlib::Write(sizeBuffer, byte_ptr, bit_ptr, lengthThird, 32);
        Bitlib::Write(sizeBuffer, byte_ptr, bit_ptr, lengthForth, 32);
        outfile.write(reinterpret_cast<const char *>(sizeBuffer), 20);

        outfile.write(reinterpret_cast<const char *>(outbufHeader), lengthHeader);
        outfile.write(reinterpret_cast<const char *>(outbufFirst), lengthFirst);
        outfile.write(reinterpret_cast<const char *>(outbufSecond), lengthSecond);
        outfile.write(reinterpret_cast<const char *>(outbufThird), lengthThird);
        outfile.write(reinterpret_cast<const char *>(outbufForth), lengthForth);

        outfile.close();
    }
};