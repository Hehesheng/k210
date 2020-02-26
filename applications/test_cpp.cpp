#include <rtthread.h>

class FirstClass
{
   private:
    int data;

   public:
    int x;

    FirstClass(void)
    {
        this->data = 10;
        rt_kprintf("I am created.\n");
        rt_kprintf("pointer:0x%X.\n", this);
    }

    ~FirstClass(void) { rt_kprintf("Bye Bye.\n"); }

    void set_data(int value) { this->data = value; }
    int get_data(void) { return this->data; }

    FirstClass operator+(const FirstClass& b)
    {
        FirstClass ret;
        ret.data = this->data + b.data;
        return ret;
    }
};

static void new_class(void)
{
    FirstClass test1;
    FirstClass test2;

    FirstClass test3 = test1 + test2;

    rt_kprintf("value:%d\n", test3.get_data());
}
MSH_CMD_EXPORT(new_class, test class);
