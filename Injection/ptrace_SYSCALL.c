#include <stdio.h>
#include <stdlib.h>
#include <sys/ptrace.h>
#include <sys/user.h>

int main(int argc, char * argv[])
{
	int pid, i, data_read = 0;
	struct user_regs_struct regs = {0,};
	struct user_regs_struct regs_to_use = {0,};

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
	ptrace(PTRACE_GETREGS, pid, 0, &regs);

	printf("eip : %p\n", regs.eip);
	printf("eax : %d\n", regs.eax);
	printf("orig_eax : %d\n", regs.orig_eax);
	printf("esp : %p\n", regs.esp);
	printf("\n");



	regs_to_use = regs;

	regs_to_use.orig_eax = 4;
	regs_to_use.ebx = 1;
	regs_to_use.ecx = regs.eip;
	regs_to_use.edx = 5;
	

		
	//레지스터 값 변경
	ptrace(PTRACE_SETREGS, pid, 0, &regs_to_use);
	
	//대상 프로세스가 system call을 하여 멈출 때까지 진행
	ptrace(PTRACE_SYSCALL, pid, NULL, NULL);
	wait();

	//레지스터 값 얻기
	ptrace(PTRACE_GETREGS, pid, 0, &regs_to_use);

	printf("eip : %p\n", regs_to_use.eip);
	printf("eax : %d\n", regs_to_use.eax);
	printf("orig_eax : %d\n", regs_to_use.orig_eax);
	printf("esp : %p\n", regs_to_use.esp);
	printf("\n");




	//regs.eax = regs.orig_eax;
	//regs.eip = regs.eip - 4;
	
	printf("eip : %p\n", regs.eip);
	printf("eax : %d\n", regs.eax);
	printf("orig_eax : %d\n", regs.orig_eax);
	printf("esp : %p\n", regs.esp);
	printf("\n");

	//레지스터 값 변경
	ptrace(PTRACE_SETREGS, pid, 0, &regs);
	

	
/*
	//스택에 값 쓰기
	ptrace(PTRACE_POKEDATA, pid, regs.esp, 0xcafebade);
*/
/*	//스택 값 읽기
	for(i = 0; i<10; i++)
	{
		data_read = ptrace(PTRACE_PEEKDATA, pid, regs.eip + i*4, NULL);
		printf("%08x\n", data_read);
	}	
	
*/
	//detach
	ptrace(PTRACE_CONT, pid, 0, 0);
	ptrace(PTRACE_DETACH, pid, 0, 0);
}




