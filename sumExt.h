#ifndef SUMEXT_H_
#define SUMEXT_H_

#include "sum.h"

class Factory;

class C{
public:
    
    C();
    ~C();    
    
private:
    friend class Factory;
    int16_t sumExt( int16_t, int16_t );
};


#endif  // SUMEXT_H_