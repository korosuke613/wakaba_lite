#ifndef __WORKER_HPP__
#define __WORKER_HPP__

#ifdef __APPLE__
#include <libpq-fe.h>
#define PSQL_IP "localhost"
#elif __linux__
#include <postgresql/libpq-fe.h>
#define PSQL_IP "kite.cs.miyazaki-u.ac.jp"
#endif

#include "atm.hpp"

class Worker {
 private:
  PGconn *__con;
  char *__sendBuf;

 public:
  int barance(int id);
};

#endif __WORKER_HPP__