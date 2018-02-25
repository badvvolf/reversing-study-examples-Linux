#define _GNU_SOURCE
#include<stdio.h>
#include<string.h>
#include<dlfcn.h>

typedef ssize_t (*orgnWrite)(int fd, const void * buf, size_t n);


//대신 호출될 함수
ssize_t write (int fd, const void * buf, size_t n)
{
	char * myString = "API Hooking\n";
	n = strlen(myString);
	orgnWrite ow = (orgnWrite)dlsym(RTLD_NEXT, "write");
	return ow(fd, myString, n);
}


