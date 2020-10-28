#include "Server.h"
#include "CM001.h"

WSADATA wsaData;
SOCKET sock0;
struct sockaddr_in addr;
struct sockaddr_in client;
int len;
SOCKET sock;
int status;
int numrcv;
char message[SEND_SIZE];
char buffer[BUFFER_SIZE];
double receive_num;

//時間計測用
std::chrono::system_clock::time_point  start, end;

int Server::Connect() {

	// winsock2の初期化
	WSAStartup(MAKEWORD(2, 0), &wsaData);
	// ソケットの作成
	sock0 = socket(AF_INET, SOCK_STREAM, 0);
	// ソケットの設定
	addr.sin_family = AF_INET;
	addr.sin_port = htons(12345);
	addr.sin_addr.S_un.S_addr = INADDR_ANY;
	bind(sock0, (struct sockaddr*) & addr, sizeof(addr));

	// TCPクライアントからの接続要求を待つ
	listen(sock0, 5);
	printf("クライアントの接続を待機中です。\n");
	// TCPクライアントからの接続要求を受け付ける
	len = sizeof(client);
	sock = accept(sock0, (struct sockaddr*) & client, &len);
	printf("クライアントに接続しました。 \n");

	return 0;
}

int Server::Receive() {

	end = std::chrono::system_clock::now();  // 計測終了時間
	double elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count(); //処理に要した時間をミリ秒に変換
	start = std::chrono::system_clock::now(); // 計測開始時間

	//受信
	//経過時間が送られてくる。
	numrcv = recv(sock, buffer, BUFFER_SIZE, 0);
	//サーバ（Unityから文字が送られて来なければ終了）
	if (numrcv == 0 || numrcv == -1) {
		printf("rcv_err\n");
		status = closesocket(sock);
	}
	else printf("rcv_OK\n");
	//Unityからdouble型の数値が送られてきたときはこのままでよい
	std::string buffer_str = buffer;
	receive_num = std::stod(buffer_str.c_str());
	printf("moterflag : %lf 秒 \n", receive_num);

	return 0;
}

int Server::Send(CM001* pModule) {
	//送信したいデータをmessageにぶち込む
	//プログラム一周期分の時間を送くる場合（精度1ms）
	//sprintf(message, "speed:%lf ms, frequency:%lf \n", elapsed, 1 / (elapsed / 1000));
	if (pModule->bBoardID_PA == CM001_SENSOR_BOARD) {
		sprintf(message, "%d %d %d %d\n",pModule->SensorBoard.wSensor[0], pModule->SensorBoard.wSensor[1], pModule->SensorBoard.wSensor[2], pModule->SensorBoard.wSensor[3]);
		send(sock, message, SEND_SIZE, 0);
		printf("送信 %d, %d, %d, %d\n", pModule->SensorBoard.wSensor[0], pModule->SensorBoard.wSensor[1], pModule->SensorBoard.wSensor[2], pModule->SensorBoard.wSensor[3]);
	}

	return 0;
}



int Server::Inf()
{
	// TCPセッションの終了
	closesocket(sock);
	// winsock2の終了処理
	WSACleanup();

	return 0;
}