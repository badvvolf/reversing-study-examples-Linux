#include <stdio.h>
#include <stdlib.h>
#include <sys/ptrace.h>
#include <sys/user.h>


int main(int argc, char * argv[])
{
	int pid, data_read;
	struct user_regs_struct regs_origin = {0,};
	struct user_regs_struct regs_modified = {0,};


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

	//대상 프로세스가 system call을 하여 멈출 때까지 진행
	ptrace(PTRACE_SYSCALL, pid, NULL, NULL);
	wait();

	//레지스터 값 얻기
	ptrace(PTRACE_GETREGS, pid, 0, &regs_origin);
	regs_modified = regs_origin;


	//---write---

	// 출력할 문자열을 bss영역에 저장
	//"use VDSO!"
	void * bss = 0x804a020;
	ptrace(PTRACE_POKEDATA, pid, bss, 0x20657375);
	ptrace(PTRACE_POKEDATA, pid, bss + 4, 0x4F534456);
	ptrace(PTRACE_POKEDATA, pid, bss + 8, 0x00000A21);

	regs_modified.ecx = bss; //출력할 메모리 주소
	regs_modified.ebx = 1; //fd
	regs_modified.edx = 10; //출력할 바이트 수
	regs_modified.orig_eax = 4; //시스템 콜 번호 설정

	//레지스터 값 변경
	ptrace(PTRACE_SETREGS, pid, 0, &regs_modified);

	//system call 완료 후 멈춤
	ptrace(PTRACE_SYSCALL, pid, NULL, NULL);
	wait();

	//___write___


	//원본 레지스터 값으로 변경
	ptrace(PTRACE_SETREGS, pid, 0, &regs_origin);
	
	//대상 프로세스 이어서 진행하도록 함
	ptrace(PTRACE_CONT, pid, 0, 0);

	//detach
	ptrace(PTRACE_DETACH, pid, 0, 0);
}




