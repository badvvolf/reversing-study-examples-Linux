#include <stdio.h>
#include <stdlib.h>
#include <sys/ptrace.h>
#include <sys/user.h>

//인젝션으로 넣을 코드
//open + mmap 시스템 콜 호출
void * MyCode()
{
	__asm__ __volatile__(

	"nop \n\t"
	"nop \n\t"
	"nop \n\t"
	"nop \n\t"
	"nop \n\t"

	//---open---
	// ./mylib2.so 문자열을 스택에 저장
	"push $0x006F732E \n\t"
	"push $0x3262696C \n\t"
	"push $0x796D2F2E \n\t"

	//문자열 주소 백업
	"mov %esp, %ebx \n\t"

	//flag 설정
	"mov $0x2, %ecx\n\t"

	//시스템 콜 번호 설정
	"mov $0x5, %eax \n\t" 
	//시스템 콜
	"int $0x80 \n\t"

	//___open___
	
	//---mmap---
	
	"mov %eax, %edi\n\t" //fd 설정

	"mov $0, %ebx\n\t" //addr 설정
	
	"mov $0x1AF4, %ecx\n\t" //length 설정

	"mov $0x7, %edx\n\t" //prot 설정

	"mov $0x2, %esi\n\t" //flags 설정
	
	//offset 설정
	"push %ebp\n\t"
	"mov $0, %ebp\n\t"

	//시스템 콜 번호 설정
	"mov $0xC0, %eax \n\t" 
	//시스템 콜
	"int $0x80 \n\t"

	//___mmap___

	//ebp 복원
	"pop %ebp\n\t"
	//스택 정리
	"add $0xC, %esp\n\t"

	//mmap 호출을 마치면 bp를 통해 인젝터로 돌아옴. 
	//인젝션된 코드로 eip 전환
	"int3\n\t"

	);
}

int main(int argc, char * argv[])
{
	int pid, data_read;
	struct user_regs_struct regs_origin = {0,};
	struct user_regs_struct regs_modified = {0,};
	void * startRoutine = 0x08048346;

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
	ptrace(PTRACE_GETREGS, pid, 0, &regs_origin);

	//기존 코드영역에 내 코드 인젝션
	//프로그램 시작 루틴인 _start함수에 덮어썼음 (프로그램 시작 후 필요없는 루틴)
	for(int i =0; i<size; i+=4)
		ptrace(PTRACE_POKEDATA, pid, startRoutine+i, *(int*)(MyCode+i));

	//eip변경
	regs_modified = regs_origin;
	regs_modified.eip = startRoutine;
	ptrace(PTRACE_SETREGS, pid, 0, &regs_modified);

	//대상 프로세스 이어서 진행하도록 함
	ptrace(PTRACE_CONT, pid, 0, 0);

	//mmap호출된 후 bp에 걸리기를 기다림
	wait();
	

	//레지스터 값 얻기
	ptrace(PTRACE_GETREGS, pid, 0, &regs_modified);

	//실행하고자 하는 함수 주소로 이동
	regs_modified.eip = regs_modified.eax + 0x4d0;
	ptrace(PTRACE_SETREGS, pid, 0, &regs_modified);

	//대상 프로세스 이어서 진행하도록 함
	ptrace(PTRACE_CONT, pid, 0, 0);

	//원하는 작업 완료 후 bp에 걸리기를 기다림
	wait();

	//원본 레지스터 복구
	ptrace(PTRACE_SETREGS, pid, 0, &regs_origin);
	
	//대상 프로세스 이어서 진행하도록 함
	ptrace(PTRACE_CONT, pid, 0, 0);

	//detach
	ptrace(PTRACE_DETACH, pid, 0, 0);
}




