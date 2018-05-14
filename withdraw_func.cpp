#include "atm.hpp"
/**                                   
 * 出金サービス
 *  [ARGUMENT]
 *   __con            : PGconnオブジェクト
 *    __id            : 口座ID
 *    __withdrawValue : 出金額
 *    __sendBuf       : 送信用配列のポインタ
 *  [RETURN]
 *    Success : 0
 *    Error   : -1
 **/
int withdraw_func(PGconn *__con, int __id, int __withdrawValue, char *__sendBuf){
	char          sql[BUFSIZE];
	PGresult      *res;   //PGresultオブジェクト
	int           resultRows;
	AccountBank   account;
	int           resultBalance;

	/* 出金額のチェック */
	if( __withdrawValue < 0 ){
		sprintf(__sendBuf, "%s %d%s", ER_STAT, E_CODE_2, ENTER);
		return -1;
	}

	/* SELECTのSQL文作成 */
	sprintf(sql, "SELECT id, balance, min_balance FROM netbank WHERE id = %d", __id);
	/* SQLコマンド実行 */
	res = PQexec(__con, sql);
	/* SQLコマンド実行結果状態を確認 */
	if(PQresultStatus(res) != PGRES_TUPLES_OK){
		printf("%s", PQresultErrorMessage(res));
		sprintf(__sendBuf, "%s %d%s", ER_STAT, E_CODE_7, ENTER);
		return -1;
	}
	/* 結果の行数（レコード数）を取得 */
	resultRows = PQntuples(res);
	/* 口座IDのチェック */
	if(resultRows != 1){
		sprintf(__sendBuf, "%s %d%s", ER_STAT, E_CODE_1, ENTER);
		return -1;
	}
	account.id         = atoi(PQgetvalue(res, 0, 0));
	account.balance    = atoi(PQgetvalue(res, 0, 1));
	account.minBalance = atoi(PQgetvalue(res, 0, 2));
	/* 引出し限度額のチェック */
	resultBalance = account.balance - __withdrawValue;
	if(resultBalance < account.minBalance){
		sprintf(__sendBuf, "%s %d%s", ER_STAT, E_CODE_4, ENTER);
		return -1;
	}
	/* 残高更新SQLを作成 */
	sprintf(sql, "UPDATE netbank SET balance = %d WHERE id = %d",
			resultBalance, account.id);
	/* SQLコマンド実行 */
	res = PQexec(__con, sql);
	/* SQLコマンド実行結果状態を確認 */
	if( PQresultStatus(res) != PGRES_COMMAND_OK){
		printf("%s", PQresultErrorMessage(res));
		sprintf(__sendBuf, "%s %d%s", ER_STAT, E_CODE_7, ENTER);
		return -1;
	}
	/* UPDATEされた行数を取得 */
	if(atoi(PQcmdTuples(res)) == 1){
		sprintf(__sendBuf, "%s %d %d%s", OK_STAT
				, account.id
				, resultBalance
				, ENTER);
	}
	return 0;
}
