#include <stdio.h>
#include <stdlib.h>
#include <sys/ptrace.h>
#include <sys/user.h>

//인젝션으로 넣을 코드
//dlopen 함수 호출 
void MyCode()
{
	__asm__ __volatile__(
	// ./mylib.so 문자열을 스택에 저장
	"push $0x00006F73 \n\t"
	"push $0x2E62696C \n\t"
	"push $0x796D2F2E \n\t"

	//문자열 주소 백업
	"mov %esp, %ebx \n\t"

	//옵션 push
	"push $0x1\n\t" 

	//문자열 주소 push 
	"push %ebx \n\t"

	//dlopen의 plt 설정
	"mov $0x8048410, %eax\n\t"
	//plt 호출
	"call %eax \n\t"

	//exit systemcall
	"mov $1, %eax \n\t"
	"mov $0, %ebx \n\t"
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




