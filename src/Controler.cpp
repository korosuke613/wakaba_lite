#include "Controler.hpp"
#include <sstream>
#include <iostream>
#include <string>
#include <vector>

std::vector<std::string> Controler::split(const string& s, char delim) {
  std::stringstream ss(s);
  string item;
  commands.clear();
  while (std::getline(ss, item, delim)) {
    if (!item.empty()) {
      commands.push_back(item);
    }
  }
  return commands;
}

/*
int main(void) {
  Controler c("aaa bbbb ccccc");
  auto elems = c.split("aaa bbbb ccccc", ' ');
  std::cout << elems.at(0) << std::endl;
}
*/