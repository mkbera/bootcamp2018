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



static int major;
atomic_t  device_opened;
static struct class *sthreads_class;
struct device *sthreads_device;

static struct process_info *pinfo = NULL;

static int do_open(struct inode *inode, struct file *file)
{
        if(atomic_read(&device_opened) != 0)
            return -EBUSY;
       
        atomic_inc(&device_opened);
        try_module_get(THIS_MODULE);

        printk(KERN_INFO "%s: Process %s pid = %d opened the device successfully\n", __func__, current->comm, current->pid);
        pinfo = kmalloc(sizeof(struct process_info), GFP_KERNEL);
        if(!pinfo)
             return -ENOMEM;
        
        pinfo->pid = current->pid;
        pinfo->is_valid_key = 0;
        pinfo->is_valid_callback = 0;
        pinfo->key = 0;
        pinfo->error_callback = 0;
        return 0;
}

static int do_release(struct inode *inode, struct file *file)
{
        atomic_dec(&device_opened);
        module_put(THIS_MODULE);
        printk(KERN_INFO "%s: Process %s pid = %d closed the device successfully\n", __func__, current->comm, current->pid);
        kfree(pinfo);
        return 0;
}

/*
//TODO
static int handle_command(struct command_t *cmd)
{
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
*/
static ssize_t
do_write(struct file *filp, const char *buff, size_t len, loff_t * off)
{
            /*
            //TODO:  Implement the write call to pass commands etc. by calling
            //    handle_command. Comment/Delete the last two lines
            struct command_t cmd;
            // Read the cmd from buff and invoke handle command
            //Your code
            */
            printk(KERN_INFO "%s: pid = %d is writing %s\n", __func__, current->pid, buff);
            return len;
}

static ssize_t do_read(struct file *filp,
                           char *buffer,
                           size_t length,
                           loff_t * offset)
{
        /*
        //TODO: Comment the last three lines
        if(length < sizeof(struct process_info)){
            printk(KERN_INFO "%s: Size is not sufficient\n", __func__);
            return -EINVAL;
        }
        memcpy(buffer, pinfo, sizeof(struct process_info));
        return sizeof(struct process_info);
        */ 
        printk(KERN_INFO "%s: pid = %d is reading %ld bytes\n", __func__, current->pid, length);
        memcpy(buffer, "Hello!", 7);
        return 7;
}

static struct file_operations fops = {
        .read = do_read,
        .write = do_write,
        .open = do_open,
        .release = do_release,
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
 
        printk(KERN_INFO "%s: Device /dev/%s is now up and running\n", __func__, DEVNAME);
        atomic_set(&device_opened, 0);
       
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
	printk(KERN_INFO "%s: Device /dev/%s is now destroyed\n", __func__, DEVNAME);
}
MODULE_AUTHOR("deba@cse.iitk.ac.in");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Demo modules");
