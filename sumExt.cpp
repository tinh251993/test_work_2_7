#include "sumExt.h"

C::C()
{

}

C::~C(){

}

int16_t C::sumExt(int16_t first_param, int16_t second_param){
    A* a_sum = new A();

    int16_t ret =  a_sum->sum(first_param, second_param) + 5;

    delete(a_sum);
    
    return ret;
}