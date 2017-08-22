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


int main(int argc, TCHAR* argv[])
{
	testing::InitGoogleTest(&argc, argv);
	int r = RUN_ALL_TESTS();

	getchar();
	return r;
}