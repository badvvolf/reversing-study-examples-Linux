//컴파일 : gcc -m32 -static

#include<stdio.h>
#include<sys/mman.h>
#include<fcntl.h>
#include<unistd.h>

int main(int argc, char * argv[])
{
	int fd;
	void * addr = NULL;

	fd = open(argv[1], O_RDWR);

	if((addr= mmap(0, 100, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE, fd, 0)) == (void*)-1)
	{
		printf("mmap error\n");
		return 0;
	}

	printf("%s\n", (char * )addr);

	munmap(addr, 100);
	close(fd);
}



