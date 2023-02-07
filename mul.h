#ifndef MUL_H_
#define MUL_H_
#include <cstdio>
#include <cinttypes>

class Factory;

class B{
public:
    B();
    ~B();

private:
    friend class Factory;
    int16_t mul( int16_t, int16_t );
};
#endif  // MUL_H_