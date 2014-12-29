#include <sys/types.h>
#include <sys/module.h>
#include <sys/sysproto.h>
#include <sys/sysent.h>
#include <sys/kernel.h>
#include <sys/systm.h>
#include <machine/specialreg.h>

extern u_int64_t hook_fast_syscall, hook_fast_syscall32;
extern u_int64_t Xfast_syscall, Xfast_syscall32;

struct rk_args{
	int cmd;
};

static void attach_rk(void);
static void detach_rk(void);

int status = 0;

static int main(struct thread *td, struct rk_args *arg){

	if(arg->cmd == 1 && status == 0)
		attach_rk();
	else if(arg->cmd == 0 && status == 1)
		detach_rk();

	return 0;
}

static void attach_rk(void){
	wrmsr(MSR_LSTAR,(u_int64_t)&hook_fast_syscall);
	wrmsr(MSR_CSTAR,(u_int64_t)&hook_fast_syscall32);	
	status = 1;
}

static void detach_rk(void){
	wrmsr(MSR_LSTAR,(u_int64_t)&Xfast_syscall);
	wrmsr(MSR_CSTAR,(u_int64_t)&Xfast_syscall32);
	status = 0;
}

static struct sysent func_sysent = {
	1,
	(sy_call_t *)main
};

static int offset = NO_SYSCALL;

static int load (struct module *module, int cmd, void *arg)
{
	int error = 0;

	switch (cmd)
	{
		case MOD_LOAD:

		break;
		case MOD_UNLOAD:
			detach_rk();
		break;
		default:
			error = EOPNOTSUPP;
		break;
	}

return error;
}

SYSCALL_MODULE(rk, &offset, &func_sysent, load, NULL);
