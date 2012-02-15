#include <cstdio>
#include <string>

// force single threaded sigslot
#define SIGSLOT_PURE_ISO 1
#include "sigslot.h"

using sigslot::signal;

#define __signals
#define __slots

#define DBG(FMT,...) printf("%s: " FMT "\n", __PRETTY_FUNCTION__, ##__VA_ARGS__)

class Foo: public sigslot::has_slots<>, public sigslot::has_signals<Foo>
{
public:

    Foo()
    {
        connect(signalIntEmitted, &Foo::handleIntEmission);
        connect(signalSomethingHappened, &Foo::handleSomething);

        // above is just some template-sugar for these calls
        //connect(signalIntEmitted, this, &Foo::handleIntEmission);
        //connect(signalSomethingHappened, this, &Foo::handleSomething);
    }

    void doStuff()
    {
        DBG("");
        signalIntEmitted(42);
    }

public __slots:

    void handleIntEmission(int i)
    {
        DBG("i = %d", i);
        signalSomethingHappened();
    }

    void handleSomething()
    {
        DBG("");
    }

public __signals:

    signal<int> signalIntEmitted;
    signal<> signalSomethingHappened;
    signal<std::string &> signalStr;
};

class Bar: public sigslot::has_slots<>
{
public:

    Bar(Foo *foo) :
        signalSig(foo->signalIntEmitted)
    {
        foo->signalIntEmitted.connect(this, &Bar::handleIntEmission);
        foo->signalStr.connect(this, &Bar::handleStr);
    }

public __slots:

    void handleIntEmission(int i)
    {
        DBG("i = %d", i);
    }

    void handleStr(std::string &str)
    {
        DBG("str = %s", str.c_str());
        str += ", World!";
    }

public __signals:

    signal<int> signalSig;

};

int main()
{
    Foo foo;
    Bar bar(&foo);

    foo.doStuff();

    std::string str = "Hello";
    foo.signalStr(str);

    DBG("str = %s", str.c_str());

    printf("\n");

    bar.signalSig.SIGSLOT_EMIT(1);

    return 0;
}

