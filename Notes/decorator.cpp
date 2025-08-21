#include <iostream>
#include <string>

/*
 * 装饰器设计模式实现
 *
 * 装饰器模式是一种结构型设计模式，它允许向一个对象动态地添加新的行为，
 * 而不需要修改该对象的基类或使用子类。装饰器模式通过创建一个装饰器类
 * 来包装原始类，从而在保持原始类接口不变的同时，扩展其功能。
 *
 * 在这个例子中，我们实现了一个咖啡店的订单系统，可以动态地为咖啡添加各种调料。
 */

// 组件接口：定义了对象的接口，所有具体组件和装饰器都必须实现这个接口
class Beverage {
public:
    virtual ~Beverage() {}
    virtual std::string getDescription() const = 0;
    virtual double cost() const = 0;
};

// 具体组件：实现了组件接口，是被装饰的对象
class Espresso : public Beverage {
public:
    std::string getDescription() const override {
        return "Espresso";
    }
    
    double cost() const override {
        return 1.99;
    }
};

class HouseBlend : public Beverage {
public:
    std::string getDescription() const override {
        return "House Blend Coffee";
    }
    
    double cost() const override {
        return 0.89;
    }
};

// 装饰器基类：继承自组件接口，包含一个组件对象的引用
// 它实现了组件接口，并持有一个指向组件对象的指针
class CondimentDecorator : public Beverage {
protected:
    Beverage* beverage;
    
public:
    CondimentDecorator(Beverage* bev) : beverage(bev) {}
    virtual ~CondimentDecorator() {
        delete beverage;
    }
};

// 具体装饰器：添加具体的功能
// 每个具体装饰器都扩展了组件的行为，通过在调用原始组件的方法前后添加自己的行为
class Mocha : public CondimentDecorator {
public:
    Mocha(Beverage* bev) : CondimentDecorator(bev) {}
    
    std::string getDescription() const override {
        return beverage->getDescription() + ", Mocha";
    }
    
    double cost() const override {
        return beverage->cost() + 0.20;
    }
};

class Whip : public CondimentDecorator {
public:
    Whip(Beverage* bev) : CondimentDecorator(bev) {}
    
    std::string getDescription() const override {
        return beverage->getDescription() + ", Whip";
    }
    
    double cost() const override {
        return beverage->cost() + 0.10;
    }
};

class Soy : public CondimentDecorator {
public:
    Soy(Beverage* bev) : CondimentDecorator(bev) {}
    
    std::string getDescription() const override {
        return beverage->getDescription() + ", Soy";
    }
    
    double cost() const override {
        return beverage->cost() + 0.15;
    }
};

int main() {
    std::cout << "=== 装饰器模式演示 ===" << std::endl;
    std::cout << std::endl;
    
    // 订一杯Espresso，不需要调料
    std::cout << "订单1: ";
    Beverage* beverage = new Espresso();
    std::cout << beverage->getDescription() << " $" << beverage->cost() << std::endl;
    
    // 订一杯HouseBlend，加上Mocha和Whip
    // 这里展示了装饰器的嵌套使用：HouseBlend -> Mocha -> Whip
    std::cout << "订单2: ";
    Beverage* beverage2 = new HouseBlend();
    beverage2 = new Mocha(beverage2);
    beverage2 = new Whip(beverage2);
    std::cout << beverage2->getDescription() << " $" << beverage2->cost() << std::endl;
    
    // 订一杯Espresso，加上双倍Mocha和Soy
    // 这里展示了装饰器的多次嵌套：Espresso -> Mocha -> Mocha -> Soy
    std::cout << "订单3: ";
    Beverage* beverage3 = new Espresso();
    beverage3 = new Mocha(beverage3);
    beverage3 = new Mocha(beverage3);
    beverage3 = new Soy(beverage3);
    std::cout << beverage3->getDescription() << " $" << beverage3->cost() << std::endl;
    
    delete beverage;
    delete beverage2;
    delete beverage3;
    
    std::cout << std::endl;
    std::cout << "=== 装饰器模式演示结束 ===" << std::endl;
    
    return 0;
}