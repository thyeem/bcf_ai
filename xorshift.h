#ifndef XORSHIFT_H
#define XORSHIFT_H
#include "common.h"
using namespace std;

class XORShift128p {

public:
    uint64_t s[2];

public:
    XORShift128p();
    ~XORShift128p();
    uint64_t    next    ();
    void        jump    ();
    uint64_t    rotl    (const uint64_t x, int k);


};

class XORShift1024m {

public:
    uint64_t    s[16];
    int         p;

public:
    XORShift1024m();
    ~XORShift1024m();
    uint64_t    next    ();
    void        jump    ();
};
    
#endif

