
//write(1, "hello\n", 7)

void DoSomeWeird()
{
	__asm__ __volatile__(

		//hello\n 문자열을 스택에 저장
		"push $0x00000A6F \n\t"
		"push $0x6C6C6568 \n\t"

		//문자열 주소 백업
		"mov %esp, %ecx \n\t"
	
		//문자열 길이 저장
		"mov $7, %edx \n\t"

		//fd 지정ffee07f8 ffee07fc

		"mov $1, %ebx \n\t"

		//시스템 콜 번호 설정
		"mov $0x4, %eax \n\t" 

		//시스템 콜
		"int $0x80 \n\t"

		//스택 정리
		"add $0x8, %esp \n\t"
	
		"pop %ebp \n\t"
		//작업이 끝난 뒤 tracer에게 제어를 넘김
		"int3 \n\t"

	
	);
}



