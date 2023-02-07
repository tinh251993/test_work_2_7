#include "factory.h"

Factory* Factory::instance_ = nullptr;

Factory::Factory()
{
}

Factory::~Factory(){
    if (nullptr != instance_){
        delete(instance_);
    }
}

Factory *Factory::getInstance(){
    std::lock_guard<std::mutex> lock(mutex_);
    if (nullptr == instance_){        
        instance_ = new Factory();
    }
    return instance_;
}

int16_t Factory::sum(A* a, int16_t first_param, int16_t second_param){
    std::lock_guard<std::mutex> lock(mutex_);
    if (nullptr == a){
        a = new A();
    }

    return a->sum(first_param, second_param);

}

int16_t Factory::mul(B* b,int16_t first_param, int16_t second_param){
    std::lock_guard<std::mutex> lock(mutex_);
    if (nullptr == b){
        b = new B();
    }

    return b->mul(first_param, second_param);
}

int16_t Factory::sumExt(C* c, int16_t first_param, int16_t second_param){
    std::lock_guard<std::mutex> lock(mutex_);
    if (nullptr == c){
        c = new C();
    }

    return c->sumExt(first_param, second_param);
}

void Factory::remove(A* a){
    std::lock_guard<std::mutex> lock(mutex_);
    
    delete(a);
}

void Factory::remove(B* b){
    std::lock_guard<std::mutex> lock(mutex_);
    
    delete(b);
}

void Factory::remove(C* c){
    std::lock_guard<std::mutex> lock(mutex_);
    
    delete(c);
}
