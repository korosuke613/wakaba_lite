#include "DataBase.hpp"
#include <string>
#include <iostream>

using std::string;

string DataBase::get_conn_info() {
  string conn_info;
  conn_info = "host=" + host + " port=" + port + " dbname=" + name +" user=" + user + " password=" + password;
  return conn_info;
}

PGconn* DataBase::connect_db() {
  con = PQconnectdb(get_conn_info().c_str());
  status_check();
  return con;
}

void DataBase::status_check() {
  if (PQstatus(con) == CONNECTION_BAD) {
    std::cout << "Connection to database '" + host + ":" + port + " " + name +
                     "' failed."
              << std::endl;
    printf("%s", PQerrorMessage(con));
    con = NULL;  // GPconnをNULL
  } else {
    printf("Connected to database %s:%s %s\n", host.c_str(), port.c_str(),
           name.c_str());
  }
}