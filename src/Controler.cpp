#include "Controler.hpp"
#include <sstream>
#include <iostream>

void Controler::split(const string& s, char delim) {
  std::stringstream ss(s);
  string item;

  while (std::getline(ss, item, delim)) {
    if (!item.empty()) {
      commands.push_back(item);
    }
  }
}

/*
int main(void) {
  Controler c("aaa bbbb ccccc");
  auto elems = c.split("aaa bbbb ccccc", ' ');
  std::cout << elems.at(0) << std::endl;
}
*/