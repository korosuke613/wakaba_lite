/**
 * atm.h
 **/

#ifndef _INCLUDE_ATM_
#define _INCLUDE_ATM_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <time.h>
#include <sys/time.h>
#include <pthread.h>

#ifdef __APPLE__ 
  #include <libpq-fe.h>
  #define PSQL_IP "localhost"
#elif __linux__
  #include <postgresql/libpq-fe.h>
  #define PSQL_IP "kite.cs.miyazaki-u.ac.jp"
#endif

#define BUFSIZE 1024
#define PORT 10000     //ATMサーバのポート番号
#define ENTER "\n" //<LF>

//*** プロトコルコマンド ***//
#define BALANCE    "BALAN"  //残高照会コマンド
#define DEPOSIT    "DEPOT"  //入金コマンド
#define WITHDRAW   "WDRAW"  //出金コマンド
#define TRANSFER   "TRANS"  //振込コマンド

//*** レスポンスステータス ***//
#define OK_STAT    "+OK"     //成功
#define ER_STAT    "-ERR"    //失敗

//*** エラーコード ***//
#define E_CODE_1    100  //指定された口座IDの預金口座が存在しない
#define E_CODE_2    200  //指定された金額が適切でない
#define E_CODE_3    300  //貯金限度額を超えている
#define E_CODE_4    400  //借り入れ金額を下回っている 
#define E_CODE_5    500  //リクエストコマンドの引数エラー
#define E_CODE_6    600  //リクエストコマンドが存在しない
#define E_CODE_7    700  //データベースエラー

//*** 預金口座構造体 ***//
typedef struct _AccountBank {
  int     id;            //口座ID
  char    *name;         //口座名
  int     balance;       //預金残高
  int     maxBalance;    //貯金限度額
  int     minBalance;    //借り入れ限度額
}AccountBank;

//*** スレッド関数に渡す引数情報構造体 ***//
typedef struct _ThreadParamter {
  struct in_addr c_ipaddr;  //IPアドレス
  in_port_t      c_port;    //ポート番号
  int            soc;       //コネクション用ソケットディスクリプタ
  PGconn         *con;      //PGconnオブジェクト（データベース）
}ThreadParameter;

extern int receive_message(int __s, char *__buf, int __maxlen);
extern int setup_listen(u_short __port);
extern void *wakaba_lite_service(void *arg);

extern int balance_func(PGconn *__con, int __id, char *__sendBuf);
extern int deposit_func(PGconn *__con, int __id, int __depositValue, char *__sendBuf);
extern int withdraw_func(PGconn *__con, int __id, int __withdrawValue, char *__sendBuf);
extern int transfer_func(PGconn *__con, int __idSrc, int __idDest, int __transferValue, char *__sendBuf);

#endif
