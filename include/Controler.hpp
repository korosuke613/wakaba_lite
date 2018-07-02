#ifndef __CONTROLER_HPP__
#define __CONTROLER_HPP__

#include <string>
#include <vector>

using std::string;
using std::vector;

class Controler {
 private:
  string recv_buf;
  vector<string> commands;

 public:
  Controler() = default;
  explicit Controler(string recv_buf) : recv_buf(recv_buf) {
    split(recv_buf, ' ');
  }

  vector<string> split(const string& s, char delim);
};

#endif