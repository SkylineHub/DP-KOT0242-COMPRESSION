#pragma once

#include <string>

#include "Definitions.h"

using namespace std;

class ArrayUtils
{
public:

    /* resize bool array */
    static void Resize(bool*& a, u_long& a_len, u_int increment) {
        bool* new_array = new bool[a_len + increment];
        memset(new_array, 0, sizeof(bool)*(a_len + increment));
        memcpy(new_array, a, sizeof(bool)*a_len);

        delete[] a;
        a = new_array;
        a_len += increment;
    };

    /* resize u_char array */
    static void Resize(u_char*& a, u_long& a_len, u_int increment) {
        u_char* new_array = new u_char[a_len + increment];
        memset(new_array, 0, sizeof(u_char)*(a_len + increment));
        memcpy(new_array, a, sizeof(u_char)*a_len);

        delete[] a;
        a = new_array;
        a_len += increment;
        //cout << "Resize: " << a_len << endl;
    };

    static void Resize(char*& a, u_long& a_len, u_int increment) {
        char* new_array = new char[a_len + increment];
        memset(new_array, 0, sizeof(u_char)*(a_len + increment));
        memcpy(new_array, a, sizeof(u_char)*a_len);

        delete[] a;
        a = new_array;
        a_len += increment;
        //cout << "Resize: " << a_len << endl;
    };

    /* resize u_int array */
    static void Resize(u_int*& a, u_long& a_len, u_int increment) {
        u_int* new_array = new u_int[a_len + increment];
        memset(new_array, 0, sizeof(u_int)*(a_len + increment));
        memcpy(new_array, a, sizeof(u_int)*a_len);

        delete[] a;
        a = new_array;
        a_len += increment;
    };
};
