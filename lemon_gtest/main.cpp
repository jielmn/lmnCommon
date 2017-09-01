#if defined(_DEBUG) && defined(WIN32)
#include "vld.h"
#endif

#include <Winsock2.h>
#include <Windows.h>
#include "LmnCommon.h"

#include <gtest/gtest.h>

TEST( String, StrTrim )
{
	char buf[32] = {0};
	int  len = 0;
	int  ret = 0;
	strcpy( buf, " abc\t\r\n  " );

	StrTrim( buf );
	len = strlen( buf );
	ret = strcmp( buf, "abc" );

	ASSERT_EQ( 3, len );
	ASSERT_EQ( 0, ret );
}

TEST( CONTAINER, ARRAY )
{
	PArray p = InitArray(0);

	DWORD ret = Append2Array(p,(void*)1);
	ASSERT_EQ( 0, ret );

	ret = Insert2Array(p,0,(void*)2);
	ASSERT_EQ( 0, ret );

	ret = Insert2Array(p,0,(void*)3);
	ASSERT_EQ( 0, ret );

	DWORD n = GetArraySize( p );
	ASSERT_EQ( 3, n );

	int num = (int)GetFromArray( p, 0 );
	ASSERT_EQ( 3, num );

	num = (int)GetFromArray( p, 1 );
	ASSERT_EQ( 2, num );

	num = (int)GetFromArray( p, 2 );
	ASSERT_EQ( 1, num );



	BOOL bRet = EraseArray( p, 0 );
	ASSERT_TRUE( bRet?true:false );

	n = GetArraySize( p );
	ASSERT_EQ( 2, n );

	bRet = EraseArray( p, 0 );
	ASSERT_TRUE( bRet?true:false );

	n = GetArraySize( p );
	ASSERT_EQ( 1, n );

	DeinitArray( p );
}



static int s_received_cnt = 0;
void MyClientEvent( SelixNode pSeli, FdType fd, int liEvent, int error, void * context ){
	const char * pStr = (const char *)"aaa";
	int ret = 0;

	switch( liEvent ) {
		case liEvConnect:
			{
				int len = strlen(pStr);
				int tmp = len;

				ret = lixTcpSend( pSeli, fd, pStr, &len );
				assert( ret == 0 );
				assert( len == tmp );
			}
			break;
		case liEvClose:
			{
				lixClose( pSeli, fd );
			}
		default:
			break;
	}
}

void * MyClient(void * pArg) {
	SelixNode pNode = SelixNode(pArg);

	DWORD dwTimeTick = LmnGetTickCount();
	//printf("time tick = %u\n", dwTimeTick );

	FdType fdClient = lixOpen ( pNode, LI_PROTOTYPE_TCP, 0, 0, MyClientEvent, pArg, 0 );
	if ( fdClient == -1 ) {
		fprintf( stderr, "create client socket error!\n");
		return (void *)1;
	}
	//printf("fdClient = %u\n", fdClient );

	int ret = lixConnect ( pNode, fdClient, inet_addr("127.0.0.1"), 51111, 0 );
	if ( 0 != ret ) {
		fprintf( stderr, "connect error!\n");
		return (void *)2;
	}

	return pArg;
}


void ClientPorc( SelixNode pSeli, FdType fd, int liEvent, int error, void * context ){
	switch ( liEvent ) {
		case liEvRead:
			{
				int len = 256;
				char buf[256] = {0};
				int n = lixTcpRecv( pSeli, fd, buf, &len );
				//printf("received:%s\n",buf);
				s_received_cnt++;
				ASSERT_TRUE( 0 == strcmp( buf, "aaa" ) );
			}
			break;
		default:
			break;
	}
}


void MyServerEvent( SelixNode pSeli, FdType fd, int liEvent, int error, void * context ){
	switch ( liEvent ) {
		case liEvConnect:
			break;
		case liEvAccept:
			{
				lixAccept( pSeli, fd, ClientPorc, 0);
			}
			break;
		case liEvClose:
			lixClose( pSeli, fd );
		default:
			break;
	}
}

TEST( INTERNET, SERVER )
{
	int ret = 0;

	ret = selixInit( 7 );
	ASSERT_EQ( 0, ret );

	SelixNode pSeli = selixCreateNode( );
	ASSERT_FALSE( 0 == pSeli );

	FdType fdServer = lixOpen( pSeli, LI_PROTOTYPE_TCP,  htonl(INADDR_ANY), 51111, MyServerEvent,0,0 );
	ASSERT_FALSE( fdServer == -1 );

	ret = lixListen( pSeli, fdServer, 10 );
	ASSERT_EQ( 0, ret );

	// 创建客户端线程
	LmnThrdType mythrd1 = LmnCreateThread( MyClient, pSeli, 0, TRUE );
	LmnThrdType mythrd2 = LmnCreateThread( MyClient, pSeli, 0, TRUE );
	LmnThrdType mythrd3 = LmnCreateThread( MyClient, pSeli, 0, TRUE );

	ASSERT_FALSE( 0 == mythrd1 );
	ASSERT_FALSE( 0 == mythrd2 );
	ASSERT_FALSE( 0 == mythrd3 );

	for ( int i = 0; i < 100 && s_received_cnt < 3; i++ ) {
		DWORD dwMs = 100;
		selixSelect( pSeli, &dwMs );
	}

	//LmnWait4Thrd( mythrd1 );
	//LmnWait4Thrd( mythrd2 );
	//LmnWait4Thrd( mythrd3 );

	ASSERT_EQ( 3, s_received_cnt );
	
	ret = selixEnd( );
	ASSERT_EQ( 0, ret );
}


int main(int argc, TCHAR* argv[])
{
	testing::InitGoogleTest(&argc, argv);
	int r = RUN_ALL_TESTS();

	getchar();
	return r;
}