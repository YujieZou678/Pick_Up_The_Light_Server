#include "mysubthread.h"

#include "mydatabase.h"
#include "threadpool.h"

MySubThread::MySubThread()
{
    this->myDatabase = new MyDataBase("root", "root");
}

MySubThread::~MySubThread()
{
    delete this->myDatabase;
    this->myDatabase = nullptr;
}

void MySubThread::add_fd(int)
{

}

void MySubThread::receive_file(int)
{

}

void MySubThread::update_all_info_live_list()
{

}

void MySubThread::update_all_info_vod_list()
{

}
