#ifndef SUM_H_
#define SUM_H_
//include lib
#include <cstdio>
#include <cinttypes>

//include file
class C;
class Factory;

class A{
public:

    A();
    ~A();

private:
    friend class C;
    friend class Factory;
    
    int16_t sum( int16_t, int16_t );
};

#endif  // SUM_H_