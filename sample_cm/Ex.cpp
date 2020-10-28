/*****************************************************************************************************

	Ex.cpp

	Control Module SC02 & CM001 Program
	 
	
													2010.4.23 M.Utsumi@Arcdevice

******************************************************************************************************/

//SC���1���ACM���1���̃T���v���v���O����
//CM��𑝂₵���ꍇ�ɂ�Module01,Module02��K�X�ǉ���
//Module01,Module02�Ɋ֌W����R�����g�A�E�g���O���Ă��������B

#include"Server.h"
#include<windows.h>
#include<stdio.h>
#include<conio.h>
#include<string.h>
#include<math.h>
#include"SC02.h"
#include"CM001.h"
#include"Measurement.h"
#include<math.h>

Server server;

CM_PARAM Param ={
	//   dt[s], fCtrlTimeout fComTimeout 
		0.001f, 1.00f, 1.000f,
};


SC02 ScModule;
CM001 Module00;//CM��𑝂₷�ꍇ�� Module01, Module02�Ƃ����悤�ɑ��₵�Ă�������


//SC��m�F�p
BOOL TestSC( void )
{

//	if( !ScModule.SetID( 1 ) )	return( FALSE );
	if( !ScModule.GetID() ) return( FALSE );
	printf("ID = %d\r\n", ScModule.bID );
/*
	if( !ScModule.GetVersion() ) return( FALSE );
//	printf("%x\r\n", ScModule.bVersion );
//	printf("Status %d \r\n", ScModule.bStatus );
/**/
/*
	printf("Hit any key\r\n");
	getch();
	if( !ScModule.Blink(3) ) return( FALSE );

	printf("Status %d \r\n", ScModule.bStatus );
/**/
	return( TRUE );
}


//CM��m�F�p
BOOL TestCM( void )
{
	Module00.GetVersion();
	printf( "CM001 Ver.%d.%d\r\n", 0x0F & Module00.bVersion >>4, 0x0F & Module00.bVersion );

	if( !Module00.Ping() ) return( FALSE );
	printf("Ping OK\r\n", ScModule.bStatus, Module00.bStatus );

	_getch();
	Module00.Blink( 2 );
	printf("status SC02 %d CM001 %d\r\n", ScModule.bStatus, Module00.bStatus );

	return( TRUE );
}

//�o�̓f�[�^���Z�b�g����
void SetData( CM001 *pModule, int iVal0,int iVal1 )
{

	// �|�[�gA��PIO�̏ꍇ�̃f�[�^�ݒ�
	if( pModule->bBoardID_PA == CM001_PIO_BOARD ){
		// �o�C�g�P�ʂŐݒ肷��ꍇ�iPIOA��8bit�܂ŗL���j
		//PioBoardA.Word.wDir�̊ebit��1�̂Ƃ����́A0�̂Ƃ��o��
		//�o�͂̂Ƃ���PioBoardA.Word.wData�̑Ή�����bit��1��5V���o�́A0��0V���o��
		//pModule->PioBoardA.Word.wData = ~pModule->PioBoardA.Word.wData;
		pModule->PioBoardA.Word.wDir = 0xFFFF;
		// �r�b�g�P�ʂŐݒ肷��ꍇ
		//PioBoardA.Bit.DIR1 = 0��1pin���o�͂ɕύX
		//PioBoardA.Bit.P1=1��1pin��5V�o�͂��s��
		pModule->PioBoardA.Bit.DIR1 = 0;
		pModule->PioBoardA.Bit.P1 = ~Module00.PioBoardA.Bit.P1;
		
	}

	// �|�[�gB�̃f�[�^�ݒ�i�o�͊�j
	switch(  pModule->bBoardID_PB ){
	case CM001_PWM_BOARD://PWM����g�����ꍇ
		pModule->PwmBoard.sPwm1 = iVal0;
		pModule->PwmBoard.sPwm2 = iVal1;
		printf("sPwm %d %d ", pModule->PwmBoard.sPwm1, pModule->PwmBoard.sPwm2 );
		break; 
	case CM001_DA_BOARD://DA����g�����ꍇ
		pModule->DaBoard.sDA = iVal0;
		printf("da %d ", pModule->DaBoard.sDA );
		break;
	case CM001_PIO_BOARD://PIO��i�^�C�vB�j���g�����ꍇ
		// �o�C�g�P�ʂŐݒ肷��ꍇ�iPIOB��10bit�܂ŗL���j
		Module00.PioBoardB.Word.wData = ~Module00.PioBoardB.Word.wData;
		pModule->PioBoardB.Word.wDir = 0x0000;
		// �r�b�g�P�ʂŐݒ肷��ꍇ
		//pModule->PioBoardB.Bit.P9 = ~pModule->PioBoardB.Bit.P9;
		//pModule->PioBoardB.Bit.DIR9 = 0;
		break;	
	}
}

//���͂��ꂽ�f�[�^��\��
void IndecateData( CM001 *pModule )
{
	switch( pModule->bBoardID_PA){
	case CM001_ENC_BOARD://���͊��ENC��̏ꍇ
		printf("enc %x ad %d ", pModule->EncBoard.lEncoder, pModule->EncBoard.wSensor[0] );
		break;
	case CM001_SENSOR_BOARD://���͊��SENS��̏ꍇ
		printf("sens %d  %d  %d  %d  %d  %d  %d  %d",
			pModule->SensorBoard.wSensor[0], pModule->SensorBoard.wSensor[1],
			pModule->SensorBoard.wSensor[2], pModule->SensorBoard.wSensor[3],
			pModule->SensorBoard.wSensor[4], pModule->SensorBoard.wSensor[5],
			pModule->SensorBoard.wSensor[6], pModule->SensorBoard.wSensor[7]
		);
		break;
	case CM001_PIO_BOARD://���͊��PIO��i�^�C�vA�j�̏ꍇ
		printf("pioA Dir %04x Data %04x ", pModule->PioBoardA.Word.wDir , pModule->PioBoardA.Word.wData );
		break;
	}
	if( pModule->bBoardID_PB == CM001_PIO_BOARD )//�o�͊��PIO��i�^�C�vB�j�̏ꍇ
	{
		printf("pioB Dir %04x Data %04x ", pModule->PioBoardB.Word.wDir , pModule->PioBoardB.Word.wData );
	}
	printf("\r\n");

}

// ��{�I�ȃf�[�^�̑���M�D���o�̓f�[�^�̓N���XCM001�̃����o�ϐ����g�p
void Exchange( void )
{
	int iVal0 = 0;
	int iVal1 = 0;//�o�͗p�̃f�[�^�̐錾�Ə�����
	BOOL loop=TRUE;
	
	MES_StartTimeCount();

	

	while(loop){
	    printf("Time %2d:%02d:%02d dt %5.3fms ",(int)(MES_Time_cur/60.0f),(int)MES_Time_cur%60,(int)(100.0f*MES_Time_cur)%100, MES_Time_int * 1000.0f);

		double pi = 3.1415926535;
		
		if(_kbhit()){
			switch(_getch()){
			//case '2': iVal0 = 100 * sin(MES_Time_cur * 10 * pi); break;
			//case '5': iVal0 = 0; break;
			case 27: iVal0 = 0; loop = FALSE; break;
			}
		}
		
		
		//if (iVal0 < 1023) {
		//iVal0 = 100 * sin(MES_Time_cur * 10 * pi);
		//else iVal0 = 1023;

		//printf("%d\n", iVal0);
		//printf("%d\n", (int)MES_Time_cur%60);

		//Unity����f�[�^���擾
		server.Receive();

		if ((int)server.receive_num == 1) {
			iVal0 = 80 * sin(MES_Time_cur * 12 * pi);
		}
		else iVal0 = 0;

		//�o�̓f�[�^��ݒ肷��
		SetData( &Module00, iVal0,iVal1 );
//		SetData( &Module01, iVal0,iVal1 );
//		SetData( &Module02, iVal0,iVal1 );


		// �f�[�^����M
		MES_GetTimeCount();
		if(!Module00.Exchange(TRUE) ) loop =FALSE;
//		if(!Module01.Exchange(FALSE) ) loop =FALSE;
//		if(!Module02.Exchange(TRUE) ) loop =FALSE;
		MES_GetTimeCount();


		//���̓f�[�^��\��
		IndecateData( &Module00 );
//		IndecateData( &Module01 );
//		IndecateData( &Module02 );

		//Unity�փf�[�^�𑗐M
		server.Send(&Module00);
		
		iVal0 = 0;

		//  Sleep(100);
	}
	
	//Module����邽�߂ɏo�̓f�[�^��0�ɂ���
	SetData( &Module00, 0 , 0);
//	SetData( &Module01, 0 , 0);
//	SetData( &Module02, 0 , 0);

	//0�ɂ����f�[�^�𑗐M
	if(!Module00.Exchange(TRUE) ) loop =FALSE;
//	if(!Module01.Exchange(FALSE) ) loop =FALSE;
//	if(!Module02.Exchange(TRUE) ) loop =FALSE;
}

void PrintCM001Status( CM001* cm )
{
	printf("status ScModule %d CM001 %d\r\n", cm->pParentModule->bStatus, cm->bStatus );
}

int main( int argc, char* argv[] )
{
	//Client�Ɛڑ�
	server.Connect();

	// ������
	printf("Open ScModule ");
	if( ( ScModule.Open( CM_MASTER_ID ))==NULL ){
		printf( "Error %s\r\n", ScModule.Error );
		return( 0 );
	}
	printf("OK\r\n");
	//	TestSC();
	/**/
	printf("Open Module00 ");
	if( !Module00.Open( &ScModule, 0, 0 )){
		printf( "%s", Module00.Error );
		return( 0 );
	}
	printf("OK\r\n");
	PrintCM001Status( &Module00  );

	//	TestCM();
	/*
	printf("Open Module01 ");
	if( !Module01.Open( &ScModule, 0, 1 )){
		printf( "%s", Module01.Error );
		return( 0 );
	}
	printf("OK\r\n");
	PrintCM001Status( &Module01  );
	/*
	printf("Open Module02 ");
	if( !Module00.Open( &ScModule, 0, 0 )){
		printf( "%s", Module02.Error );
		return( 0 );
	}
	printf("OK\r\n");
	PrintCM001Status( &Module02  );
	/**/


	/**/
	// ��{�f�[�^�̑���M�iBasic�j
	//Exchange()�̒��Ń��[�v���Ă���̂ŏo�̓f�[�^�̌v�Z�A�ύX�Ȃǂ�Exchange�̒��ōs��
	Exchange();
	/**/

	//Exchange���甲�����̂�CM������
	printf("Close Module00 ");
	if( !Module00.Close() ){
		printf("Error:%s",Module00.Error );
	}
	printf("OK\r\n");
	/*
	printf("Close Module01 ");
	if( !Module01.Close() ){
		printf("Error:%s",Module01.Error );
	}
	printf("OK\r\n");
	/*
	printf("Close Module02 ");
	if( !Module02.Close() ){
		printf("Error:%s",Module02.Error );
	}
	printf("OK\r\n");
	/**/

	//SC������
	printf("ScModule Close ");
	if(!ScModule.Close()){
		printf("Error:%s",ScModule.Error );
	}
	printf("OK\r\n");

	//Client�Ƃ̐ڑ���ؒf
	server.Inf();

	return(1);
}

