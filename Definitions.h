#pragma once

typedef unsigned char u_char;
typedef unsigned short u_short;
typedef unsigned int u_int;

#ifndef NULL
#define NULL 0
#endif

#define Xb1 1
#define Xb2 2
#define Xb3 4
#define Xb4 8
#define Xb5 16
#define Xb6 32
#define Xb7 64
#define Xb8 128

/*
	DBC parsing arguments
*/
#define DBC_SYM_ONLY 0
#define DBC_ALFANUM 1
#define DBC_PREDEFINED 2
#define DBC_DELIM_ONLY 3
#define DBC_XML 4
#define DBC_DYNAMIC 5

/*
	Re-Pair reduced message coding.
*/
#define FIXED_CODING 0
#define PHASED_CODING 1
#define HUFFMAN_CODING 2
#define ARITHMETIC_CODING 3
#define ADAPTIVE_ARITHMETIC_CODING 4
#define NO_CODING 5

/*
	Re-Pair dictionary message coding.
*/
#define GAMMA_CODING 0
#define ADVANCED_GAMMA_CODING 1
#define ADVANCED_FIBONACCI_CODING 2
#define ADVANCED_DELTA_CODING 3

const u_int DEFAULT_QUEUE_SIZE = 256;

struct HPRPotentialStat {
    u_int rule_count;
    u_int full_rule_count;
    u_int three_rule_count;
    u_int two_rule_count;
    u_int hpr2_rule_count;
};

struct NonUniformityStat {
    u_int alfa_len;
    u_int max_frequency;
    double ave_frequency;
    double H0;
    u_int fixed_bits;
    double redundancy;
};

namespace DBC
{
    class DBCTriplet
    {
    public:
        DBCTriplet(u_char s, u_char size, bool is_word);
        ~DBCTriplet();

        u_char m_s;
        u_char m_size;
        bool m_is_word;
    };
}

namespace RePair
{
    /*
    Triplet of two pointers to the previous and next occurence of the pair.
    Symbol
    */
    struct Triplet {
        //symbol
        u_int c;
        //adjacent symbols
        Triplet* prev;
        Triplet* next;

        //next or previous equal pair
        Triplet* prev_pair;
        Triplet* next_pair;

        //boolean to mark included pair
        bool included;
    };

    struct Digram {
        u_int d1;
        u_int d2;
        u_int z;
        u_int z_new;

        double dmH;
        bool opposite;
    };

    struct Expansion {
        double dmH_max;
        u_int dmH_max_f1;
        u_int dmH_max_f2;
        u_int count;

        u_int exp_rules;
        u_int tot_rules;

        u_int iteration;
    };
}
