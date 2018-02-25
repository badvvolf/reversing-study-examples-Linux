#include<string.h>
#include <unistd.h>


int main(int argc, char * argv[])
{
	char * buf = "this is my string\n";
	write(1, buf, strlen(buf));
	
}



