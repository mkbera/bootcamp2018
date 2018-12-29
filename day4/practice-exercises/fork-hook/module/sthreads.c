#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/mm.h>
#include<linux/mm_types.h>
#include<linux/file.h>
#include<linux/fs.h>
#include<linux/path.h>
#include<linux/slab.h>
#include<linux/sched.h>
#include<linux/kprobes.h>
#include<linux/uaccess.h>
#include<linux/device.h>


static void fork_callback (struct task_struct *newp)
{
   /* // comment 1
   struct pt_regs *newp_regs = task_pt_regs(newp);
   struct pt_regs *currp_regs = task_pt_regs(current);*/
   printk(KERN_INFO "Program [%s] with pid [%d]  creating. New process pid[%d] tgid[%d]\n", current->comm, current->pid, newp->pid, newp->tgid);
   /* //comment 1
   if(memcmp(current->comm, "fork", 4) == 0 || memcmp(current->comm, "pthread", 7) == 0){
        printk(KERN_INFO "Parent user RIP[%lx] Child user RIP[%lx]\n", currp_regs->ip, newp_regs->ip);
   }*/
   //comment 2
   /*if(memcmp(current->comm, "pthread", 7) == 0){
        printk(KERN_INFO "Thread stack pointer is @[%lx]\n", newp_regs->sp);
   }
  */ 
   jprobe_return();
}
static struct jprobe forkjp = {
        .entry          =  fork_callback,
        .kp.symbol_name = "wake_up_new_task"
};


int init_module(void)
{
        int err;
	printk(KERN_INFO "Hello kernel\n");
            
        if ((err = register_jprobe(&forkjp)) < 0){
              printk(KERN_INFO "Jprobe registration failed\n");
        }

	return 0;
}

void cleanup_module(void)
{
        disable_jprobe(&forkjp);
        unregister_jprobe(&forkjp);
	printk(KERN_INFO "Goodbye kernel\n");
}
MODULE_AUTHOR("deba@cse.iitk.ac.in");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Demo modules");
