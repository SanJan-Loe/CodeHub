#include <iostream>
#include <bit>

using namespace std;

template<class T>
class MySharedPtr {
public:
    T* ptr;
    int *refCount;
    MySharedPtr(T* p) : ptr(p),refCount(new int(1)) {}
    MySharedPtr(const MySharedPtr& other) : ptr(other.ptr), refCount(other.refCount){
        (*refCount)++;
    }


    MySharedPtr(MySharedPtr&& other) noexcept : ptr(other.ptr), refCount(other.refCount) {
        other.ptr = nullptr;
        other.refCount = nullptr;
    }

    MySharedPtr& operator=(const MySharedPtr& other) {
        if (this != &other) {
            if (--(*refCount) == 0) {
                delete ptr;
                delete refCount;
            }

            this ->ptr = other.ptr;
            refCount = other.refCount;
            (*refCount)++;
        }
        return *this;
    }

    MySharedPtr& operator=(MySharedPtr&& other) noexcept {
        if(this != &other) {
            if( --(*refCount) == 0) {
                delete ptr;
                delete refCount;
            }

            ptr = other.ptr;
            refCount = other.refCount;
            other.ptr = nullptr;
            other.refCount = nullptr;
        }

        return *this;
    }

    ~MySharedPtr() {
        if (--(*refCount) == 0) {
            delete ptr;
            delete refCount;
        }
    }

    T& operator*() const { return *ptr; }
    T* operator->() const { return ptr; }
    int use_count() const { return refCount ? *refCount : 0; }
};

int main() {

    if constexpr (std::endian::native == std::endian::big) {
        // 大端序系统
        std::cout << "Big Endian\n";
    } else if constexpr (std::endian::native == std::endian::little) {
        // 小端序系统
        std::cout << "Little Endian\n";
    } else {
        std::cout << "Mixed Endian\n";
    }

    return 0;
}