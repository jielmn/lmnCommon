#if defined(_DEBUG) && defined(WIN32)
#include "vld.h"
#endif

#include "LmnCommon.h"
#include <Windows.h>
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


int main(int argc, TCHAR* argv[])
{
	testing::InitGoogleTest(&argc, argv);
	int r = RUN_ALL_TESTS();

	getchar();
	return r;
}