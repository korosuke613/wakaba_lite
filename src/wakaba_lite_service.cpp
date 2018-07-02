// Copyright 2018 Futa HIRAKOBA

#include <unistd.h>
#include <iostream>
#include <string>
#include "Controler.hpp"
#include "atm.hpp"
/*
 * ATMメイン処理 (スレッド関数)
 *   [ARGUMENT]
 *      __arg  : スレッド引数構造体(ThreadParameter型)
 *  [RETURN]
 *     NONE
 */
void *wakaba_lite_service(void *__arg) {
  ThreadParameter *threadParam =
      reinterpret_cast<ThreadParameter *>(__arg);  //スレッド引数構造体
  char recvBuf[BUFSIZE], sendBuf[BUFSIZE];
  int recvLen, sendLen;
  pthread_t selfId;  // 自分自身のスレッドID
  int cnt;
  Controler controler;

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
    controler.split(recvBuf, ' ');
    auto command = controler.get_command();
    auto params = controler.get_params();
    /* コマンド判定 */
    if (command == BALANCE) {
      /* 残高照会 */
      if (params.size() == 1) {
        balance_func(threadParam->con, std::stoi(params.at(0)), sendBuf);
      } else {
        snprintf(sendBuf, BUFSIZE, "%s %d%s", ER_STAT, E_CODE_5, ENTER);
      }

    } else if (command == DEPOSIT) {
      /* 入金 */
      if (params.size() == 2) {
        deposit_func(threadParam->con, std::stoi(params.at(0)),
                     std::stoi(params.at(1)), sendBuf);
      } else {
        snprintf(sendBuf, BUFSIZE, "%s %d%s", ER_STAT, E_CODE_5, ENTER);
      }

    } else if (command == WITHDRAW) {
      /* 出金 */
      if (params.size() == 2) {
        withdraw_func(threadParam->con, std::stoi(params.at(0)),
                      std::stoi(params.at(1)), sendBuf);
      } else {
        snprintf(sendBuf, BUFSIZE, "%s %d%s", ER_STAT, E_CODE_5, ENTER);
      }

    } else if (command == TRANSFER) {
      /* 振込 */
      if (params.size() == 3) {
        transfer_func(threadParam->con, std::stoi(params.at(0)),
                      std::stoi(params.at(1)), std::stoi(params.at(2)),
                      sendBuf);
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
