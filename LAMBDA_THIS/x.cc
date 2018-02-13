#include <iostream>

struct A
{
    int i = 5;

    auto f() { return [this]() -> A & { return *this; }; }

    auto g()
    {
        A a{*this};

        return [a]() -> int { return a.i; };
    }
};

int main()
{
    A *a = new A;

    auto lambda_f = a->f();
    auto lambda_g = a->g();

    delete a;

    if (&lambda_f() == a)
    {
        std::cout << "Dangling pointer!\n";
    }

    std::cout << lambda_g() << '\n';

    return 0;
}
