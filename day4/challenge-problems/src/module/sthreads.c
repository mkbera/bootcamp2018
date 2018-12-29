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
#include "command.h"

struct process_info{
                        pid_t pid;
                        int is_valid_key;
                        int is_valid_callback;
                        unsigned long key;
                        unsigned long error_callback;
};

static int major;
atomic_t  device_opened;
static struct class *sthreads_class;
struct device *sthreads_device;

static struct process_info *pinfo = NULL;


static void fork_callback (struct task_struct *newp)
{
   jprobe_return();
}
static struct jprobe forkjp = {
        .entry          =  fork_callback,
        .kp.symbol_name = "wake_up_new_task"
};


static int demo_open(struct inode *inode, struct file *file)
{
        if(atomic_read(&device_opened) != 0)
            return -EBUSY;
       
        pinfo = kmalloc(sizeof(struct process_info), GFP_KERNEL);
        if(!pinfo)
             return -ENOMEM;
        
        pinfo->pid = current->pid;
        pinfo->is_valid_key = 0;
        pinfo->is_valid_callback = 0;
 
        enable_jprobe(&forkjp);
        atomic_inc(&device_opened);
        try_module_get(THIS_MODULE);

        printk(KERN_INFO "Device opened successfully\n");
        return 0;
}

static int demo_release(struct inode *inode, struct file *file)
{
        atomic_dec(&device_opened);
        module_put(THIS_MODULE);
        printk(KERN_INFO "Device closed successfully\n");
        disable_jprobe(&forkjp);
        kfree(pinfo);
        return 0;
}
static ssize_t demo_read(struct file *filp,
                           char *buffer,
                           size_t length,
                           loff_t * offset)
{
        printk(KERN_INFO "Read not implemented\n");
        return -EINVAL;
}

static int handle_command(struct command_t *cmd)
{
   BUG_ON(!pinfo);
   if(current->pid != pinfo->pid)
       return -EPERM;
   switch(cmd->command)
   {
        case REG_KEY:
                      pinfo->key = cmd->param;
                      pinfo->is_valid_key = 1;
                      break;
        case UNREG_KEY:
                      pinfo->is_valid_key = 0;
                      break;
        case REG_ERR_FN:
                      pinfo->is_valid_callback = 1;
                      pinfo->error_callback = cmd->param;
                      break;
        case UNREG_ERR_FN:
                      pinfo->is_valid_callback = 0;
                      break;
        default:
                       printk(KERN_INFO "Invalid command [%ld]\n", cmd->command);
                       return -EINVAL;
   }
  return (sizeof(struct command_t));
}

static ssize_t
demo_write(struct file *filp, const char *buff, size_t len, loff_t * off)
{
            struct command_t cmd;
            stac();
            memcpy(&cmd, buff, sizeof(struct command_t));
            clac();
            return handle_command(&cmd);
}

static struct file_operations fops = {
        .read = demo_read,
        .write = demo_write,
        .open = demo_open,
        .release = demo_release,
};

static char *sthreads_devnode(struct device *dev, umode_t *mode)
{
        if (mode && dev->devt == MKDEV(major, 0))
                *mode = 0666;
        return NULL;
}

int init_module(void)
{
        int err;
	printk(KERN_INFO "Hello kernel\n");
            
        major = register_chrdev(0, DEVNAME, &fops);
        err = major;
        if (err < 0) {      
             printk(KERN_ALERT "Registering char device failed with %d\n", major);   
             goto error_regdev;
        }                 
        
        sthreads_class = class_create(THIS_MODULE, DEVNAME);
        err = PTR_ERR(sthreads_class);
        if (IS_ERR(sthreads_class))
                goto error_class;

        sthreads_class->devnode = sthreads_devnode;

        sthreads_device = device_create(sthreads_class, NULL,
                                        MKDEV(major, 0),
                                        NULL, DEVNAME);
        err = PTR_ERR(sthreads_device);
        if (IS_ERR(sthreads_device))
                goto error_device;
 
        printk(KERN_INFO "I was assigned major number %d. To talk to\n", major);                                                              
        atomic_set(&device_opened, 0);
       
         if (register_jprobe(&forkjp) < 0){
              printk(KERN_INFO "Jprobe registration failed\n");
        }
        disable_jprobe(&forkjp);

	return 0;

error_device:
         class_destroy(sthreads_class);
error_class:
        unregister_chrdev(major, DEVNAME);
error_regdev:
        return  err;
}

void cleanup_module(void)
{
        device_destroy(sthreads_class, MKDEV(major, 0));
        class_destroy(sthreads_class);
        unregister_chrdev(major, DEVNAME);
        unregister_jprobe(&forkjp);
	printk(KERN_INFO "Goodbye kernel\n");
}
MODULE_AUTHOR("deba@cse.iitk.ac.in");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Demo modules");
