#include <iostream>

class HelloWorld {
public:
  HelloWorld() {
    std::cout << "HelloWorld(): " << this << std::endl;
  }
  ~HelloWorld() {
    std::cout << "~HelloWorld(): " << this << std::endl;
  }

  int sum(int a, int b);
};

int HelloWorld::sum(int a, int b) {
  int c = a + b;
  std::cout << "HelloWorld::sum: " << a << " + " << b << " = " << c << std::endl;

  return c;
}

int summa(int a, int b) {
  HelloWorld h;
  return h.sum(a, b);
}

extern "C" {
int csumma(int a, int b) {
  return summa(a, b);
}
}

int main(int argc, char **argv) {
  int expected = 123;
  int a = 130;
  int b = -7;
  int actual = summa(a, b);
  if (actual != expected) {
    return 1;
  }
  return 0;
}
