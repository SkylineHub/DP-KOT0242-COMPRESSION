#include <iostream>
#include <fstream>
#include "Arithmetic.h"

using namespace std;

class Decompress {
public:
    void DecompressFile(string in_filename, string out_filename) {
        ifstream file(in_filename, std::ios::binary | std::ios::ate);
        streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);
        char* compressed_buffer = new char[size];
        if(file.read(compressed_buffer, size)) {
            auto* ac = new ArithmeticCoder<u_char>();
            u_char* decompressed_buf = ac->Decompress(reinterpret_cast<u_char *>(compressed_buffer), size, 5);
            streamsize output_size = ac->getDataLen();
            ofstream outfile(out_filename, std::ios::binary | std::ios::ate);
            outfile.write(reinterpret_cast<const char *>(decompressed_buf), output_size);
            outfile.close();
        }
        file.close();
    }

    void DecompressFileWithContext(string in_filename, string out_filename) {
        ifstream file(in_filename, std::ios::binary | std::ios::ate);
        streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);
        char* compressed_buffer = new char[size];
        if(file.read(compressed_buffer, size)) {
            u_int byte_ptr = 0;
            u_char bit_ptr = 0;

            u_long lengthHeader = Bitlib::ReadDWord(reinterpret_cast<u_char *>(compressed_buffer), byte_ptr, bit_ptr, 32);
            cout << lengthHeader << endl;

            u_long lengthFirst = Bitlib::ReadDWord(reinterpret_cast<u_char *>(compressed_buffer), byte_ptr, bit_ptr, 32);
            cout << lengthFirst << endl;

            u_long lengthSecond  = Bitlib::ReadDWord(reinterpret_cast<u_char *>(compressed_buffer), byte_ptr, bit_ptr, 32);
            cout << lengthSecond << endl;

            u_long lengthThird  = Bitlib::ReadDWord(reinterpret_cast<u_char *>(compressed_buffer), byte_ptr, bit_ptr, 32);
            cout << lengthThird << endl;

            u_long lengthForth  = Bitlib::ReadDWord(reinterpret_cast<u_char *>(compressed_buffer), byte_ptr, bit_ptr, 32);
            cout << lengthForth << endl;

            u_char* headerCompressed = new u_char[lengthHeader];
            u_int byte_ptr_dec = byte_ptr;

            for (u_long i = 0; i < lengthHeader; i++) {
                headerCompressed[i] = compressed_buffer[byte_ptr_dec++];
            }

            u_char* firstCompressed = new u_char[lengthFirst];
            for (u_long i = 0; i < lengthFirst; i++) {
                firstCompressed[i] = compressed_buffer[byte_ptr_dec++];
            }

            u_char* secondCompressed = new u_char[lengthSecond];
            for (u_long i = 0; i < lengthSecond; i++) {
                secondCompressed[i] = compressed_buffer[byte_ptr_dec++];
            }

            u_char* thirdCompressed = new u_char[lengthThird];
            for (u_long i = 0; i < lengthThird; i++) {
                thirdCompressed[i] = compressed_buffer[byte_ptr_dec++];
            }

            u_char* forthCompressed = new u_char[lengthForth];
            for (u_long i = 0; i < lengthForth; i++) {
                forthCompressed[i] = compressed_buffer[byte_ptr_dec++];
            }

            ofstream outfile(out_filename, std::ios::binary | std::ios::ate);

            auto* arithmeticCoderHeader = new ArithmeticCoder<u_char>();
            u_char* decompressedBufferHeader = arithmeticCoderHeader->Decompress(headerCompressed, lengthHeader, 5);

            auto* arithmeticCoderFirst = new ArithmeticCoder<u_char>();
            u_char* decompressedBufferFirst = arithmeticCoderFirst->Decompress(firstCompressed, lengthFirst, 5);

            auto* arithmeticCoderSecond = new ArithmeticCoder<u_char>();
            u_char* decompressedBufferSecond = arithmeticCoderSecond->Decompress(secondCompressed, lengthSecond, 5);

            auto* arithmeticCoderThird = new ArithmeticCoder<u_char>();
            u_char* decompressedBufferThird = arithmeticCoderThird->Decompress(thirdCompressed, lengthThird, 5);

            auto* arithmeticCoderForth = new ArithmeticCoder<u_char>();
            u_char* decompressedBufferForth = arithmeticCoderForth->Decompress(forthCompressed, lengthForth, 5);

            u_long length = arithmeticCoderHeader->getDataLen() + arithmeticCoderFirst->getDataLen() + arithmeticCoderSecond->getDataLen() + arithmeticCoderThird->getDataLen() + arithmeticCoderForth->getDataLen();

            auto* outputBuffer = new u_char[length];
            memset(outputBuffer, 0, sizeof(u_char)*(length));
            memcpy(outputBuffer, decompressedBufferHeader, sizeof(u_char)*arithmeticCoderHeader->getDataLen());

            u_long i = arithmeticCoderHeader->getDataLen();

            /*
            while (i != arithmeticCoderHeader->getDataLen()) {
                outputBuffer[i] = decompressedBufferHeader[i];
                i++;
            }
            */
            u_long byte_ptr_first = 0;
            u_long byte_ptr_second = 0;
            u_long byte_ptr_third = 0;
            u_long byte_ptr_forth = 0;


            while (i < length) {
                while (decompressedBufferFirst[byte_ptr_first] != '\n') {
                    outputBuffer[i++] = decompressedBufferFirst[byte_ptr_first++];
                }
                outputBuffer[i++] = decompressedBufferFirst[byte_ptr_first++];
                while (decompressedBufferSecond[byte_ptr_second] != '\n') {
                    outputBuffer[i++] = decompressedBufferSecond[byte_ptr_second++];
                }
                outputBuffer[i++] = decompressedBufferSecond[byte_ptr_second++];
                while (decompressedBufferThird[byte_ptr_third] != '\n') {
                    outputBuffer[i++] = decompressedBufferThird[byte_ptr_third++];
                }
                outputBuffer[i++] = decompressedBufferThird[byte_ptr_third++];
                while (decompressedBufferForth[byte_ptr_forth] != '\n' and i < length) {
                    outputBuffer[i++] = decompressedBufferForth[byte_ptr_forth++];
                }
                outputBuffer[i++] = decompressedBufferForth[byte_ptr_forth++];
            }

            //outfile.write(reinterpret_cast<const char *>(decompressedBufferHeader), arithmeticCoderHeader->getDataLen());
            //outfile.write(reinterpret_cast<const char *>(decompressedBufferFirst), arithmeticCoderFirst->getDataLen());
            //outfile.write(reinterpret_cast<const char *>(decompressedBufferSecond), arithmeticCoderSecond->getDataLen());
            //outfile.write(reinterpret_cast<const char *>(decompressedBufferThird), arithmeticCoderThird->getDataLen());
            //outfile.write(reinterpret_cast<const char *>(decompressedBufferForth), arithmeticCoderForth->getDataLen());
            outfile.write(reinterpret_cast<const char *>(outputBuffer), length);
            outfile.close();


            /*
            auto* ac = new ArithmeticCoder<u_char>();
            u_char* decompressed_buf = ac->Decompress(reinterpret_cast<u_char *>(compressed_buffer), size, 5);
            streamsize output_size = ac->getDataLen();
            ofstream outfile(out_filename, std::ios::binary | std::ios::ate);
            outfile.write(reinterpret_cast<const char *>(decompressed_buf), output_size);
             */
        }
        file.close();
    }
};