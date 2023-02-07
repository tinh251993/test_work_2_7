#include "factory.h"
#include "mul.h"
#include "sum.h"
#include "sumExt.h"

int main(){
    Factory* factory = Factory::getInstance();
    A* a;
    B* b;
    C* c;

    factory->sum(a, 1, 3);
    factory->mul(b, 5, 6);
    factory->sumExt(c, 1, 3);

    delete(factory);

}