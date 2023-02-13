#include <iostream>

#include "Node.h"
#include "Compress.h"
#include "Decompress.h"
#include "DebugHelper.h"

using namespace std;

void searchChild(Node* parent) {
    for (int i = 0; i < 127; i++) {
        if (parent->child[i] != nullptr) {
            Node* child = parent->child[i];
            cout << int(child->key) << " - " << char(child->key) << " - " << child->freq << " - " << child->lower  << " - " << child->upper << endl;
            searchChild(parent->child[i]);
        }
    }
    cout << "k" << endl;
}

void Compress() {
    auto* compress = new class Compress();
    compress->CompressFileWithContext("GM24149.bnx", "GM24149.bin", 5);
}

void Decompress() {
    auto* decompress = new class Decompress();
    decompress->DecompressFileWithContext("first_test.bin", "GM09888_dec.txt");
}

void Debug() {
    DebugHelper* debugController = new DebugHelper();
    debugController->writeMoleculeHeadersToFile("GM11428.bnx", "GM11428_mh.txt");
}

int main() {
    //Compress();
    //Decompress();
    Debug();
    return 0;
}
