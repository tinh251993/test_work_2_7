#ifndef FACTORY_CPP_
#define FACTORY_CPP_
#include "sum.h"
#include "mul.h"
#include "sumExt.h"

#include <mutex>

class Factory{
public:
    Factory();
    ~Factory();
    static Factory* getInstance();
    int16_t sum(A*, int16_t, int16_t);
    int16_t mul(B*, int16_t, int16_t);
    int16_t sumExt(C*, int16_t, int16_t);
    void remove(A*);
    void remove(B*);
    void remove(C*);
private:
    Factory* instance_;

    static std::mutex mutex_;
};

#endif  // FACTORY_CPP_