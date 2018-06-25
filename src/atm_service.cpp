// Copyright 2018 Futa HIRAKOBA

#include <unistd.h>
#include <iostream>
#include "atm.hpp"

/*
 * ATMメイン処理 (スレッド関数)
 *   [ARGUMENT]
 *      __arg  : スレッド引数構造体(ThreadParameter型)
 *  [RETURN]
 *     NONE
 */
void *atm_service(void *__arg) {
  ThreadParameter *threadParam =
      reinterpret_cast<ThreadParameter *>(__arg);   //スレッド引数構造体
  char recvBuf[BUFSIZE], sendBuf[BUFSIZE];
  int recvLen, sendLen;
  char comm[BUFSIZE], comm2[BUFSIZE];  //リクエストコマンド
  int perm1Int, perm2Int, perm3Int;
  pthread_t selfId;  // 自分自身のスレッドID
  int cnt;

  selfId = pthread_self();  // 自分自身のスレッドIDを取得
  std::cout << "[C_THREAD " << selfId << "] ATM SERVICE START ("
            << threadParam->soc << ")" << std::endl;
  while (1) {
    /* リクエストコマンド受信 */
    recvLen = receive_message(threadParam->soc, recvBuf, BUFSIZE);
    if (recvLen < 1) break;
    recvBuf[recvLen - 1] = '\0';  // <LF>を消去
    std::cout << "[C_THREAD " << selfId << "] RECV=> " << recvBuf << std::endl;
    /* リクエストコマンド解析 */
    cnt = sscanf(recvBuf, "%s", comm);
    /* コマンド判定 */
    if (strcmp(comm, BALANCE) == 0) {
      /* 残高照会 */
      if (sscanf(recvBuf, "%s %d", comm2, &perm1Int) == 2) {
        balance_func(threadParam->con, perm1Int, sendBuf);
      } else {
        snprintf(sendBuf, BUFSIZE, "%s %d%s", ER_STAT, E_CODE_5, ENTER);
      }

    } else if (strcmp(comm, DEPOSIT) == 0) {
      /* 入金 */
      if (sscanf(recvBuf, "%s %d %d", comm2, &perm1Int, &perm2Int) == 3) {
        deposit_func(threadParam->con, perm1Int, perm2Int, sendBuf);
      } else {
        snprintf(sendBuf, BUFSIZE, "%s %d%s", ER_STAT, E_CODE_5, ENTER);
      }

    } else if (strcmp(comm, WITHDRAW) == 0) {
      /* 出金 */
      if (sscanf(recvBuf, "%s %d %d", comm2, &perm1Int, &perm2Int) == 3) {
        withdraw_func(threadParam->con, perm1Int, perm2Int, sendBuf);
      } else {
        snprintf(sendBuf, BUFSIZE, "%s %d%s", ER_STAT, E_CODE_5, ENTER);
      }

    } else if (strcmp(comm, TRANSFER) == 0) {
      /* 振込 */
      if (sscanf(recvBuf, "%s %d %d %d", comm2, &perm1Int, &perm2Int,
                 &perm3Int) == 4) {
        transfer_func(threadParam->con, perm1Int, perm2Int, perm3Int, sendBuf);
      } else {
        snprintf(sendBuf, BUFSIZE, "%s %d%s", ER_STAT, E_CODE_5, ENTER);
      }

    } else {
      snprintf(sendBuf, BUFSIZE, "%s %d%s", ER_STAT, E_CODE_6, ENTER);
    }

    sendLen = strlen(sendBuf);
    send(threadParam->soc, sendBuf, sendLen, 0);
    std::cout << "[C_THREAD " << selfId << "] SEND=> " << sendBuf << std::endl;
  }  // END while()

  std::cout << "[C_THREAD " << selfId << "] ATM SERVICE END ("
            << threadParam->soc << ")" << std::endl;

  /* データベース接続を切断 */
  PQfinish(threadParam->con);
  /* ソケットを切断 */
  close(threadParam->soc);
  /* スレッド引数構造の領域解放 */
  free(threadParam);
}  // END
