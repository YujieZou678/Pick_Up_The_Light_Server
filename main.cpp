#include <iostream>
using std::cout;
using std::endl;
#include "threadpool.h"

void task(void*,void*,void*) {
    std::this_thread::sleep_for(std::chrono::seconds(5));
    cout << 123 << endl;
}

int main()
{
    ThreadPool pool(1, 5);
    Task t(task);
    pool.add_task(t);
    pool.add_task(t);
    pool.add_task(t);
    pool.add_task(t);

    std::this_thread::sleep_for(std::chrono::seconds(30));
    return 0;
}
