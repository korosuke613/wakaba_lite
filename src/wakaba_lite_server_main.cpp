// Copyright 2018 Futa HIRAKOBA

/**
 * wakaba liteサーバ (マルチスレッド + libpq版)
 * wakaba_lite_server_main.c
 **/

#include <iostream>
#include <string>
#include "DataBase.hpp"
#include "atm.hpp"

void service_start(int __lsoc);

int main(int argc, char *argv[]) {
  int listenSoc;
  u_short port;

  if (argc == 2)
    port = atoi(argv[1]);
  else
    port = PORT;
  /* listenソケット作成 */
  if ((listenSoc = setup_listen(port)) < 0) {
    exit(1);
  }
  //スレッド起動ベース関数
  service_start(listenSoc);
}  // END main()

/**
 * スレッド起動ベース
 *  [ARGUMENT]
 *    __lsoc : LISTENソケットディスクリプタ
 *  [RETURN]
 *    NONE
 **/
void service_start(int __lsoc) {
  DataBase db;
  char connInfo[BUFSIZE];
  pthread_t worker;              // スレッドID用
  ThreadParameter *threadParam;  // スレッド引数用構造体
  struct sockaddr_in client;
  unsigned int client_len;
  int s_new;

  while (1) {
    client_len = sizeof(client);
    /* 通信コネクション確立 */
    printf("[MAIN THREAD] Accept Call...\n");
    s_new = accept(__lsoc, (struct sockaddr *)&client, &client_len);
    /* スレッド引数構造体に値をセット */
    /* スレッド引数構造体の領域は動的メモリとして確保 */
    threadParam =
        reinterpret_cast<ThreadParameter *>(malloc(sizeof(ThreadParameter)));
    threadParam->c_ipaddr = client.sin_addr;  // クライアントのIPアドレス
    threadParam->c_port = client.sin_port;  // クライアントのポート
    threadParam->soc = s_new;  // コネクション用ソケットディスクリプタ
    /* データベース接続 */
    threadParam->con = db.connect_db();
    /* スレッドの生成 */
    pthread_create(&worker, NULL, wakaba_lite_service,
                   reinterpret_cast<void *>(threadParam));
    printf("[MAIN THREAD] Created thread ID: %u¥n", worker);
    /* スレッドのデタッチ(終了を待たない) */
    pthread_detach(worker);
  }  // END while()
}  // END service_start()
