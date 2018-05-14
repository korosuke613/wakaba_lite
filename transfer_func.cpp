#include "atm.hpp"
#include <iostream>
/**                                                                            
 * 振込サービス
 *  [ARGUMENT]                                                           
 *    __con           : PGconnオブジェクト                       
 *    __idSrc         : 振込元口座ID
 *    __idDest        : 振込先口座ID               
 *    __transferValue : 振込額
 *    __sendBuf       : 送信用配列のポインタ                    
 *  [RETURN]                                                    
 *    Success : 0                                                               
 *    Error   : -1                                                    
 **/                                   
int transfer_func(PGconn *__con, int __idSrc, int __idDest, int __transferValue, char *__sendBuf){    
	char        sql[BUFSIZE], sqlSrc[BUFSIZE], sqlDest[BUFSIZE];
	PGresult    *res, *resSrc, *resDest;   //PGresultオブジェクト
	int         resultRows;      
	AccountBank accountSrc, accountDest;                
	int         resultBalanceSrc, resultBalanceDest;
	std::string	sqlBegin="BEGIN", sqlCommit="COMMIT", sqlRollback="ROLLBACK";

	/* 振込額のチェック */
	if( __transferValue < 0 ){
		sprintf(__sendBuf, "%s %d%s", ER_STAT, E_CODE_2, ENTER);
		return -1;
	}

	/* 振込元口座IDチェック */
	sprintf(sql, "SELECT id, balance, min_balance FROM netbank WHERE id = %d", __idSrc);
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
	accountSrc.id         = atoi(PQgetvalue(res, 0, 0));
	accountSrc.balance    = atoi(PQgetvalue(res, 0, 1));
	accountSrc.minBalance = atoi(PQgetvalue(res, 0, 2));

	/* 振込先口座IDチェック */
	sprintf(sql, "SELECT id, balance, max_balance  FROM netbank WHERE id = %d", __idDest);
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
	accountDest.id          = atoi(PQgetvalue(res, 0, 0));
	accountDest.balance     = atoi(PQgetvalue(res, 0, 1));
	accountDest.maxBalance = atoi(PQgetvalue(res, 0, 2));

	/* これ以降は自分で考えましょう */

	/* 引出し限度額のチェック */
	resultBalanceSrc = accountSrc.balance - __transferValue;
	if(resultBalanceSrc < accountSrc.minBalance){
		sprintf(__sendBuf, "%s %d%s", ER_STAT, E_CODE_4, ENTER);
		return -1;
	}
	/* 預金限度額のチェック */
	resultBalanceDest = accountDest.balance + __transferValue;
	if(resultBalanceDest > accountDest.maxBalance){
		sprintf(__sendBuf, "%s %d%s", ER_STAT, E_CODE_3, ENTER);
		return -1;
	}

	/* トランザクション処理を開始 */
	res = PQexec(__con, sqlBegin.c_str());
	if( PQresultStatus(res) != PGRES_COMMAND_OK){
		printf("%s", PQresultErrorMessage(res));
		sprintf(__sendBuf, "%s %d%s", ER_STAT, E_CODE_7, ENTER);
		return -1;
	}

	/* 残高更新SQLを作成 */
	sprintf(sql, "UPDATE netbank SET balance = %d WHERE id = %d",
			resultBalanceSrc, accountSrc.id);
	/* SQLコマンド実行 */
	res = PQexec(__con, sql);
	/* SQLコマンド実行結果状態を確認 */
	if( PQresultStatus(res) != PGRES_COMMAND_OK){
		printf("%s", PQresultErrorMessage(res));
		sprintf(__sendBuf, "%s %d%s", ER_STAT, E_CODE_7, ENTER);
		PQexec(__con, sqlRollback.c_str()); //トランザクション処理をrollback
		return -1;
	}
	/* UPDATEされた行数を取得 */
	if(atoi(PQcmdTuples(res)) == 1){
		sprintf(__sendBuf, "%s %d %d%s", OK_STAT
				, accountSrc.id
				, resultBalanceSrc
				, ENTER);
	}

	/* 残高更新SQLを作成 */
	sprintf(sql, "UPDATE netbank SET balance = %d WHERE id = %d",
			resultBalanceDest, accountDest.id);
	/* SQLコマンド実行 */
	res = PQexec(__con, sql);
	/* SQLコマンド実行結果状態を確認 */
	if(PQresultStatus(res) != PGRES_COMMAND_OK){
		printf("%s", PQresultErrorMessage(res));
		sprintf(__sendBuf, "%s %d%s", ER_STAT, E_CODE_7, ENTER);
		PQexec(__con, sqlRollback.c_str()); //トランザクション処理をrollback
		return -1;
	}
	/* UPDATEされた行数を取得 */
	if(PQresultStatus(res) == PGRES_COMMAND_OK){
		sprintf(__sendBuf, "%s %d %d%s", OK_STAT
				, accountDest.id
				, resultBalanceDest
				, ENTER);
	}

	/* トランザクション処理を正常に終了 */
	res = PQexec(__con, sqlCommit.c_str());
	if(PQresultStatus(res) != PGRES_COMMAND_OK){
		printf("%s", PQresultErrorMessage(res));
		sprintf(__sendBuf, "%s %d%s", ER_STAT, E_CODE_7, ENTER);
		return -1;
	}

	return 0;
}
