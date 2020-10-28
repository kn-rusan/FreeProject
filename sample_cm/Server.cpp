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

//���Ԍv���p
std::chrono::system_clock::time_point  start, end;

int Server::Connect() {

	// winsock2�̏�����
	WSAStartup(MAKEWORD(2, 0), &wsaData);
	// �\�P�b�g�̍쐬
	sock0 = socket(AF_INET, SOCK_STREAM, 0);
	// �\�P�b�g�̐ݒ�
	addr.sin_family = AF_INET;
	addr.sin_port = htons(12345);
	addr.sin_addr.S_un.S_addr = INADDR_ANY;
	bind(sock0, (struct sockaddr*) & addr, sizeof(addr));

	// TCP�N���C�A���g����̐ڑ��v����҂�
	listen(sock0, 5);
	printf("�N���C�A���g�̐ڑ���ҋ@���ł��B\n");
	// TCP�N���C�A���g����̐ڑ��v�����󂯕t����
	len = sizeof(client);
	sock = accept(sock0, (struct sockaddr*) & client, &len);
	printf("�N���C�A���g�ɐڑ����܂����B \n");

	return 0;
}

int Server::Receive() {

	end = std::chrono::system_clock::now();  // �v���I������
	double elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count(); //�����ɗv�������Ԃ��~���b�ɕϊ�
	start = std::chrono::system_clock::now(); // �v���J�n����

	//��M
	//�o�ߎ��Ԃ������Ă���B
	numrcv = recv(sock, buffer, BUFFER_SIZE, 0);
	//�T�[�o�iUnity���當���������ė��Ȃ���ΏI���j
	if (numrcv == 0 || numrcv == -1) {
		printf("rcv_err\n");
		status = closesocket(sock);
	}
	else printf("rcv_OK\n");
	//Unity����double�^�̐��l�������Ă����Ƃ��͂��̂܂܂ł悢
	std::string buffer_str = buffer;
	receive_num = std::stod(buffer_str.c_str());
	printf("moterflag : %lf �b \n", receive_num);

	return 0;
}

int Server::Send(CM001* pModule) {
	//���M�������f�[�^��message�ɂԂ�����
	//�v���O������������̎��Ԃ𑗂���ꍇ�i���x1ms�j
	//sprintf(message, "speed:%lf ms, frequency:%lf \n", elapsed, 1 / (elapsed / 1000));
	if (pModule->bBoardID_PA == CM001_SENSOR_BOARD) {
		sprintf(message, "%d %d %d %d\n",pModule->SensorBoard.wSensor[0], pModule->SensorBoard.wSensor[1], pModule->SensorBoard.wSensor[2], pModule->SensorBoard.wSensor[3]);
		send(sock, message, SEND_SIZE, 0);
		printf("���M %d, %d, %d, %d\n", pModule->SensorBoard.wSensor[0], pModule->SensorBoard.wSensor[1], pModule->SensorBoard.wSensor[2], pModule->SensorBoard.wSensor[3]);
	}

	return 0;
}



int Server::Inf()
{
	// TCP�Z�b�V�����̏I��
	closesocket(sock);
	// winsock2�̏I������
	WSACleanup();

	return 0;
}