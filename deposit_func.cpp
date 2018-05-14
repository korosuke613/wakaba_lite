#include "atm.hpp"
/**                                                                                      
 * 入金サービス                                                                          
 *  [ARGUMENT]                                                           
 *    __con       : PGconnオブジェクト                       
 *    __id        : 口座ID               
 *    __deositValue  : 入金額                                                            
 *    __sendBuf   : 送信用配列のポインタ                    
 *  [RETURN]                                                    
 *    Success : 0                                                               
 *    Error   : -1                                                    
 **/                                   
int deposit_func(PGconn *__con, int __id, int __depositValue, char *__sendBuf){    
	char        sql[BUFSIZE];
	PGresult    *res;   //PGresultオブジェクト
	int         resultRows;      
	AccountBank account;                
	int         resultBalance;

	/* 入金額のチェック */
	if( __depositValue < 0){
		sprintf(__sendBuf, "%s %d%s", ER_STAT, E_CODE_2, ENTER);
		return -1;
	}

	/* SELECTのSQL文作成 */
	sprintf(sql, "SELECT id, balance, max_balance FROM netbank WHERE id = %d", __id);
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

	/* SELECT実行結果の値を取得 (口座ID, 預金残高, 預金限度額 */  
	account.id         = atoi(PQgetvalue(res, 0, 0));
	account.balance    = atoi(PQgetvalue(res, 0, 1));
	account.maxBalance = atoi(PQgetvalue(res, 0, 2));

	/* 預金限度額のチェック */
	resultBalance = account.balance + __depositValue;
	if(resultBalance > account.maxBalance){
		sprintf(__sendBuf, "%s %d%s", ER_STAT, E_CODE_3, ENTER);
		return -1;
	}
	/* 残高更新SQLを作成 */
	sprintf(sql, "UPDATE netbank SET balance = %d WHERE id = %d",
			resultBalance, account.id);
	/* SQLコマンド実行 */
	res = PQexec(__con, sql);
	/* SQLコマンド実行結果状態を確認 */
	if(PQresultStatus(res) != PGRES_COMMAND_OK){
		printf("%s", PQresultErrorMessage(res));
		sprintf(__sendBuf, "%s %d%s", ER_STAT, E_CODE_7, ENTER);
		return -1;
	}
	/* UPDATEされた行数を取得 */
	if(PQresultStatus(res) == PGRES_COMMAND_OK){
		sprintf(__sendBuf, "%s %d %d%s", OK_STAT
				, account.id
				, resultBalance
				, ENTER);
	}
	return 0;
}
