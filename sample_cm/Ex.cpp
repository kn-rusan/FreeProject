/*****************************************************************************************************

	Ex.cpp

	Control Module SC02 & CM001 Program
	 
	
													2010.4.23 M.Utsumi@Arcdevice

******************************************************************************************************/

//SC基板1枚、CM基板1枚のサンプルプログラム
//CM基板を増やした場合にはModule01,Module02を適宜追加し
//Module01,Module02に関係するコメントアウトを外してください。

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
CM001 Module00;//CM基板を増やす場合は Module01, Module02というように増やしてください


//SC基板確認用
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


//CM基板確認用
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

//出力データをセットする
void SetData( CM001 *pModule, int iVal0,int iVal1 )
{

	// ポートAがPIOの場合のデータ設定
	if( pModule->bBoardID_PA == CM001_PIO_BOARD ){
		// バイト単位で設定する場合（PIOAは8bitまで有効）
		//PioBoardA.Word.wDirの各bitが1のとき入力、0のとき出力
		//出力のときにPioBoardA.Word.wDataの対応するbitを1で5Vを出力、0で0Vを出力
		//pModule->PioBoardA.Word.wData = ~pModule->PioBoardA.Word.wData;
		pModule->PioBoardA.Word.wDir = 0xFFFF;
		// ビット単位で設定する場合
		//PioBoardA.Bit.DIR1 = 0で1pinを出力に変更
		//PioBoardA.Bit.P1=1で1pinに5V出力を行う
		pModule->PioBoardA.Bit.DIR1 = 0;
		pModule->PioBoardA.Bit.P1 = ~Module00.PioBoardA.Bit.P1;
		
	}

	// ポートBのデータ設定（出力基板）
	switch(  pModule->bBoardID_PB ){
	case CM001_PWM_BOARD://PWM基板を使った場合
		pModule->PwmBoard.sPwm1 = iVal0;
		pModule->PwmBoard.sPwm2 = iVal1;
		printf("sPwm %d %d ", pModule->PwmBoard.sPwm1, pModule->PwmBoard.sPwm2 );
		break; 
	case CM001_DA_BOARD://DA基板を使った場合
		pModule->DaBoard.sDA = iVal0;
		printf("da %d ", pModule->DaBoard.sDA );
		break;
	case CM001_PIO_BOARD://PIO基板（タイプB）を使った場合
		// バイト単位で設定する場合（PIOBは10bitまで有効）
		Module00.PioBoardB.Word.wData = ~Module00.PioBoardB.Word.wData;
		pModule->PioBoardB.Word.wDir = 0x0000;
		// ビット単位で設定する場合
		//pModule->PioBoardB.Bit.P9 = ~pModule->PioBoardB.Bit.P9;
		//pModule->PioBoardB.Bit.DIR9 = 0;
		break;	
	}
}

//入力されたデータを表示
void IndecateData( CM001 *pModule )
{
	switch( pModule->bBoardID_PA){
	case CM001_ENC_BOARD://入力基板がENC基板の場合
		printf("enc %x ad %d ", pModule->EncBoard.lEncoder, pModule->EncBoard.wSensor[0] );
		break;
	case CM001_SENSOR_BOARD://入力基板がSENS基板の場合
		printf("sens %d  %d  %d  %d  %d  %d  %d  %d",
			pModule->SensorBoard.wSensor[0], pModule->SensorBoard.wSensor[1],
			pModule->SensorBoard.wSensor[2], pModule->SensorBoard.wSensor[3],
			pModule->SensorBoard.wSensor[4], pModule->SensorBoard.wSensor[5],
			pModule->SensorBoard.wSensor[6], pModule->SensorBoard.wSensor[7]
		);
		break;
	case CM001_PIO_BOARD://入力基板がPIO基板（タイプA）の場合
		printf("pioA Dir %04x Data %04x ", pModule->PioBoardA.Word.wDir , pModule->PioBoardA.Word.wData );
		break;
	}
	if( pModule->bBoardID_PB == CM001_PIO_BOARD )//出力基板がPIO基板（タイプB）の場合
	{
		printf("pioB Dir %04x Data %04x ", pModule->PioBoardB.Word.wDir , pModule->PioBoardB.Word.wData );
	}
	printf("\r\n");

}

// 基本的なデータの送受信．入出力データはクラスCM001のメンバ変数を使用
void Exchange( void )
{
	int iVal0 = 0;
	int iVal1 = 0;//出力用のデータの宣言と初期化
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

		//Unityからデータを取得
		server.Receive();

		if ((int)server.receive_num == 1) {
			iVal0 = 80 * sin(MES_Time_cur * 12 * pi);
		}
		else iVal0 = 0;

		//出力データを設定する
		SetData( &Module00, iVal0,iVal1 );
//		SetData( &Module01, iVal0,iVal1 );
//		SetData( &Module02, iVal0,iVal1 );


		// データ送受信
		MES_GetTimeCount();
		if(!Module00.Exchange(TRUE) ) loop =FALSE;
//		if(!Module01.Exchange(FALSE) ) loop =FALSE;
//		if(!Module02.Exchange(TRUE) ) loop =FALSE;
		MES_GetTimeCount();


		//入力データを表示
		IndecateData( &Module00 );
//		IndecateData( &Module01 );
//		IndecateData( &Module02 );

		//Unityへデータを送信
		server.Send(&Module00);
		
		iVal0 = 0;

		//  Sleep(100);
	}
	
	//Moduleを閉じるために出力データを0にする
	SetData( &Module00, 0 , 0);
//	SetData( &Module01, 0 , 0);
//	SetData( &Module02, 0 , 0);

	//0にしたデータを送信
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
	//Clientと接続
	server.Connect();

	// 初期化
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
	// 基本データの送受信（Basic）
	//Exchange()の中でループしているので出力データの計算、変更などはExchangeの中で行う
	Exchange();
	/**/

	//Exchangeから抜けたのでCM基板を閉じる
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

	//SC基板を閉じる
	printf("ScModule Close ");
	if(!ScModule.Close()){
		printf("Error:%s",ScModule.Error );
	}
	printf("OK\r\n");

	//Clientとの接続を切断
	server.Inf();

	return(1);
}

