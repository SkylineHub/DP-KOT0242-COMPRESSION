#include <iostream>
#include <fstream>
#include "Arithmetic.h"

using namespace std;

class DebugHelper {
public:
    void writeMoleculeHeadersToFile(string inFilename, string outFilename) {
        ifstream file(inFilename, std::ios::binary | std::ios::ate);
        file.seekg(0, std::ios::beg);

        long molHeaderCount = 0;
        u_long buf_len = 8 * 128;

        u_char* outbuf = new u_char[buf_len];
        memset(outbuf, 0, sizeof(u_char)*buf_len);

        string line;

        int headerLineIndex = 0;
        u_char* headerLine;

        while(getline(file, line)) {
            if (line[0] == '0') {
                if (buf_len < (molHeaderCount + line.length() + 1))
                    ArrayUtils::Resize(outbuf, buf_len, buf_len);
                for (char ch : line) {
                    outbuf[molHeaderCount] = ch;
                    molHeaderCount += 1;
                }
                outbuf[molHeaderCount] = '\n';
                molHeaderCount += 1;
                continue;
            }
            if (line[0] == '#' and line[1] == '0' and line[2] == 'h') {
                headerLine = new u_char[line.length()];
                memset(headerLine, 0, sizeof(u_char)*line.length());
                for (char ch : line) {
                    headerLine[headerLineIndex] = ch;
                    headerLineIndex += 1;
                }
                headerLine[headerLineIndex] = '\n';
                headerLineIndex += 1;
                continue;
            }
        }

        file.close();

        ofstream outfile(outFilename, std::ios::binary | std::ios::ate);
        outfile.write(reinterpret_cast<const char *>(headerLine), headerLineIndex);
        outfile.write(reinterpret_cast<const char *>(outbuf), molHeaderCount);
        outfile.close();

    }

    void treeWalkthrough(string in_filename) {
        ifstream file(in_filename, std::ios::binary | std::ios::ate);
        streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);
        char* buffer = new char[size];

        if(file.read(buffer, size)) {
            auto *arithmeticCoder = new ArithmeticCoder(buffer, size, 127, 1);
            arithmeticCoder->treeWalkthrough();
        }

        file.close();
    }

};

