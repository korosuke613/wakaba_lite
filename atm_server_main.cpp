/**
 * ATMサーバ (マルチスレッド + libpq版)
 * atm_server_main.c
 **/

#include "atm.hpp"

void service_start(int __lsoc);

int main(int argc, char *argv[]){
	int listenSoc;
	u_short port;

	if( argc == 2 ) port = atoi(argv[1]);
	else            port = PORT;
	/* listenソケット作成 */
	if( (listenSoc = setup_listen(port)) < 0 ){
		exit(1);
	}
	//スレッド起動ベース関数
	service_start(listenSoc);
}//END main()

/**
 * スレッド起動ベース
 *  [ARGUMENT]
 *    __lsoc : LISTENソケットディスクリプタ
 *  [RETURN]
 *    NONE
 **/
void service_start(int __lsoc){
	char *dbHost  = PSQL_IP;
	char *dbPort  = "5432";
	char *dbName  = "db72"; //データベース名
	char *dbLogin = "dbuser72";
	char *dbPwd   = "dbpass72";
	char connInfo[BUFSIZE];
	pthread_t worker;        //スレッドID用
	ThreadParameter *threadParam;  //スレッド引数用構造体
	struct sockaddr_in client;
	unsigned int client_len;
	int s_new;

	while(1){
		client_len = sizeof(client);
		/* 通信コネクション確立 */
		printf("[MAIN THREAD] Accept Call...\n");
		s_new = accept(__lsoc, (struct sockaddr *)&client, &client_len);
		/* スレッド引数構造体に値をセット */
		/* スレッド引数構造体の領域は動的メモリとして確保 */
		threadParam = (ThreadParameter *)malloc(sizeof(ThreadParameter));
		threadParam->c_ipaddr = client.sin_addr;  //クライアントのIPアドレス
		threadParam->c_port = client.sin_port;    //クライアントのポート
		threadParam->soc = s_new;                 //コネクション用ソケットディスクリプタ
		/* データベース接続パラメータ設定 */
		sprintf(connInfo, "host=%s port=%s dbname=%s user=%s password=%s",
				dbHost, dbPort, dbName, dbLogin, dbPwd);
		/* データベース接続 */
		threadParam->con = PQconnectdb(connInfo);
		/* 接続状態を確認 */
		if(PQstatus(threadParam->con) == CONNECTION_BAD){
			printf("Connection to database '%s:%s %s' failed.\n", dbHost, dbPort, dbName);
			printf("%s", PQerrorMessage(threadParam->con));
			threadParam->con = NULL; //GPconnをNULL
		}else{
			printf("Connected to database %s:%s %s\n", dbHost, dbPort, dbName);
		}
		/* スレッドの生成 */
		pthread_create(&worker, NULL, atm_service, (void *)threadParam);
		printf("[MAIN THREAD] Created thread ID: %u\n", worker);
		/* スレッドのデタッチ(終了を待たない) */
		pthread_detach(worker);
	}//END while()
}//END service_start()
