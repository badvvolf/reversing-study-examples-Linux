#include<stdio.h>

void DoSomeWeird()
{
	printf("Hello!\n");
}

__attribute__((constructor)) void like_DllMain()
{
	DoSomeWeird();
}


