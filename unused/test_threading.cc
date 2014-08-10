#include <cstdlib>
#include <iostream>

#include "thread.h"

ReadWriteLock rwlock;
int wlock = 0;
int rlock = 0;

void do_some_writing (void)
{
  rwlock.write_lock();
  wlock++;
  std::cout << "W" << wlock << " " << std::flush;

  if (rlock > 0)
    std::cout << "__ ERROR __ " << std::flush;

  struct timespec sleep_time;
  sleep_time.tv_sec = 0;
  sleep_time.tv_nsec = 100000;
  ::nanosleep(&sleep_time, 0);

  wlock--;
  std::cout << "W" << wlock << " " << std::flush;
  rwlock.unlock();
}

void do_some_reading (void)
{
  rwlock.read_lock();
  rlock++;
  std::cout << "R" << rlock << " " << std::flush;

  if (wlock > 0)
    std::cout << "__ ERROR __ " << std::flush;

  struct timespec sleep_time;
  sleep_time.tv_sec = 0;
  sleep_time.tv_nsec = 100000;
  ::nanosleep(&sleep_time, 0);

  rlock--;
  std::cout << "R" << rlock << " " << std::flush;
  rwlock.unlock();
}

class MyThread : public Thread
{
  private:
    int id;

  public:
    MyThread (int id) : id(id)
    {
    }

    void* run (void)
    {
      std::cout << "__ST" << id << " " << std::flush;
      for (std::size_t i = 0; i < 100; ++i)
      {
        int rnd = ::rand() % 8;
        if (rnd == 0)
          do_some_writing();
        else
          do_some_reading();
      }

      std::cout << "__DT" << id << " " << std::flush;
    }
};

int main (void)
{
  ::srand(::time(0));
  MyThread t0(0);
  MyThread t1(1);
  MyThread t2(2);
  t0.pt_create();
  t1.pt_create();
  t2.pt_create();
  t0.pt_join();
  std::cout << "T0: Joined!" << std::endl;
  t1.pt_join();
  std::cout << "T1: Joined!" << std::endl;
  t2.pt_join();
  std::cout << "T2: Joined!" << std::endl;
  return 0;
}
