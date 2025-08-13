#include <iostream>
#include <cstdlib>

using namespace std;


class A{
    public:
        char a;
        double b;
        virtual void func()  {}
};

class B : public A {};

int main () {

    std::cout <<"here get the empty class A size "<< sizeof(A) <<endl;
    std::cout <<"here get the empty class B size "<< sizeof(B) <<endl;
    std::cout <<"here get the empty class B size "<< sizeof(B) <<endl;

    return 0;
}