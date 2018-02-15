//컴파일 : gcc -m32 -ldl

#include<stdio.h>
#include<unistd.h>
#include<dlfcn.h>

int main()
{
	dlopen(0, RTLD_NOW); //링킹, PLT/GOT 잡기
	while(1)
	{
		printf("실행중...\n");
		sleep(1);
	}

}



