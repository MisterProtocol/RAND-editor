#include <stdio.h>
#include <stdint.h>

#define M1L -1L
int64_t


main()
{
/*
	Test1(1);
	Test1(-1);
	Test1(-1L);
	Test(-1);
	Test(-1L);
  */


	Test1(1);
	Test1(-1L);
	Test1(M1L);
	Test(-1);
	Test(M1L);
}

Test(n)
int64_t n;
{
       printf("Test %x %d %ld size %d\n", n,n,n,sizeof(n));
}


Test1(n)
int32_t n;
{

       printf("Test1 %x %d %ld size %d\n", n,n,n,sizeof(n));
}
