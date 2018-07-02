#include "Controler.hpp"
#include <sstream>
#include <iostream>
#include <string>
#include <vector>

void Controler::split(const string& s, char delim) {
  std::stringstream ss(s);
  string item;
  commands.clear();
  while (std::getline(ss, item, delim)) {
    if (!item.empty()) {
      commands.push_back(item);
    }
  }
  command = commands.at(0);
  params = commands;
  params.erase(params.begin());
}

std::string Controler::get_command() { return command; }
std::vector<std::string> Controler::get_params() { return params; }
/*
int main(void) {
  Controler c("aaa bbbb ccccc");
  auto elems = c.split("aaa bbbb ccccc", ' ');
  std::cout << elems.at(0) << std::endl;
}
*/