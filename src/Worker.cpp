#include "Worker.hpp"

int Worker::balance(int id) {
  char sql[BUFSIZE];
  PGresult *res;  // PGresultオブジェクト
  int resultRows;
  AccountBank account;  //預金口座構造体

  /* 残高取得SELECTのSQL文作成 */
  sprintf(sql, "SELECT id, name, balance FROM netbank WHERE id = %d", id);
  /* SQLコマンド実行 */
  res = PQexec(__con, sql);
  /* SQLコマンド実行結果状態を確認 */
  if (PQresultStatus(res) != PGRES_TUPLES_OK) {
    printf("%s", PQresultErrorMessage(res));
    sprintf(__sendBuf, "%s %d%s", ER_STAT, E_CODE_7, ENTER);
    return -1;
  }
  /* 結果の行数（レコード数）を取得 */
  resultRows = PQntuples(res);
  /* 口座IDのチェック */
  if (resultRows != 1) {
    sprintf(__sendBuf, "%s %d%s", ER_STAT, E_CODE_1, ENTER);
    return -1;
  }
  /* 残高照会取得 */
  account.id = atoi(PQgetvalue(res, 0, 0));
  account.name = PQgetvalue(res, 0, 1);
  account.balance = atoi(PQgetvalue(res, 0, 2));
  /* レスポンスメッセージ作成 */
  sprintf(__sendBuf, "%s %d %s %d%s", OK_STAT, account.id, account.name,
          account.balance, ENTER);
  /* PGresultに割当られた記憶領域を開放 */
  PQclear(res);
  return 0;
}  // END
