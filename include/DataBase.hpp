#ifndef __DATA_BASE_HPP__
#define __DATA_BASE_HPP__

#ifdef __APPLE__
#include <libpq-fe.h>
#define PSQL_IP "localhost"
#elif __linux__
#include <postgresql/libpq-fe.h>
#define PSQL_IP "kite.cs.miyazaki-u.ac.jp"
#endif

#include <string>
#include "atm.hpp"

using std::string;

class DataBase {
 private:
  string host = PSQL_IP;
  string port = "5432";
  string name = "db72";
  string user = "dbuser72";
  string password = "dbpass72";
  PGconn* con;

 public:
  string get_conn_info();
  PGconn *connect_db();
  void status_check();
};

#endif