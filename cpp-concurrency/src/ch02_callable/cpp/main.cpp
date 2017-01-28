#include <thread>
#include <iostream>

class CallableTest {
public:
  CallableTest() = default;
  CallableTest(const CallableTest& callable_test) {
    std::cout << "copy constructor is called" << std::endl;
  }
  void operator()() const {
    std::cout << "function call operator is called" << std::endl;
  }
};

int main() {
  CallableTest test;
  std::cout << "before thread creation" << std::endl;
  std::thread mythread(test);
  std::cout << "after thread creation" << std::endl;
  mythread.join();
  return 0;
}
