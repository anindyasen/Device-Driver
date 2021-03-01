#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include<linux/slab.h>                 //kmalloc()
#include<linux/uaccess.h>              //copy_to/from_user()
#include <linux/ioctl.h>

#define WR_VALUE 0x24
#define RD_VALUE 0x25
#define DEL_CMD  0x27

//This is the size of the memory mapped from hardware where all read 
//and write will happen
#define drv_mem_size 1024

uint8_t *drv_mem = NULL;
int32_t value = 0;
 
dev_t dev = 0;
static struct class *dev_class;
static struct cdev mod_cdev;
 
static int __init mod_driver_init(void);
static void __exit mod_driver_exit(void);
static ssize_t mod_read(struct file *filp, char __user *buf, size_t len,loff_t * off);
static ssize_t mod_write(struct file *filp, const char *buf, size_t len, loff_t * off);
static long mod_ioctl(struct file *file, unsigned int cmd, unsigned long arg);

static struct file_operations drv_fops =
{
    .owner          = THIS_MODULE,
    .read           = mod_read,
    .write          = mod_write,
    .unlocked_ioctl = mod_ioctl,
};

 
static ssize_t mod_read(struct file *filp, char __user *buf, size_t len, loff_t *off)
{
    if(0 < copy_to_user(buf, drv_mem, drv_mem_size)) {
        printk(KERN_ERR "copy_to_user failed\n");
        return -1;
    }
    printk(KERN_INFO "Data Write : Done!\n");
    return drv_mem_size;
}
static ssize_t mod_write(struct file *filp, const char __user *buf, size_t len, loff_t *off)
{        
    if(0 < copy_from_user(drv_mem, buf, len)) {
        printk(KERN_ERR "copy_from_user failed\n");
        return -1;
    }
    printk(KERN_INFO "Data Read : Done!\n");
    return len;
}
 
static long mod_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    switch(cmd) {
        case WR_VALUE:
            if(0 < copy_from_user(&value ,(int32_t*) arg, sizeof(value))){
                printk(KERN_ERR "copy_from_user failed\n");
                return -1;
            }
            printk(KERN_INFO "Value = %d\n", value);
            break;
        case RD_VALUE:
            if(0 < copy_to_user((int32_t*) arg, &value, sizeof(value))){
                printk(KERN_ERR "copy_to_user failed\n");
                return -1;
            }
            break;
        case DEL_CMD:
            value = 0;
            if(0 < copy_to_user((int32_t*) arg, &value, sizeof(value))){
                printk(KERN_ERR "copy_to_user failed\n");
                return -1;
            }
            break;
    }
   return 0;
}

static int __init 
mod_driver_init(void)
{
    /*Creating Physical memory. This memmory will be treated as the mapped mem
     * of the real harware.*/
    if((drv_mem = kmalloc(drv_mem_size , GFP_KERNEL)) == 0){
        printk(KERN_INFO "Cannot allocate memory in kernel\n");
        return -1;
    }

    //Initializing device mem to 0
    memset(drv_mem, 0, drv_mem_size);
    /*Allocating Major number*/
    if((alloc_chrdev_region(&dev, 0, 1, "Mod Driver")) <0){
        printk(KERN_INFO "Cannot allocate major number\n");
        return -1;
    }
    printk(KERN_INFO "Major = %d Minor = %d \n",MAJOR(dev), MINOR(dev));

    /*Creating cdev structure*/
    cdev_init(&mod_cdev,&drv_fops);

    /*Adding character device to the system*/
    if((cdev_add(&mod_cdev,dev,1)) < 0){
        printk(KERN_INFO "Cannot add the device to the system\n");
        goto r_class;
    }

    /*Creating struct class*/
    if((dev_class = class_create(THIS_MODULE,"mod_class")) == NULL){
        printk(KERN_INFO "Cannot create the struct class\n");
        goto r_class;
    }

    /*Creating device*/
    if((device_create(dev_class,NULL,dev,NULL,"mod_device")) == NULL){
        printk(KERN_INFO "Cannot create the Device 1\n");
        goto r_device;
    }
    printk(KERN_INFO "Device Driver Insert...Done!!!\n");
    return 0;

r_device:
    class_destroy(dev_class);
r_class:
    unregister_chrdev_region(dev,1);
    return -1;
}


void __exit 
mod_driver_exit(void)
{
    device_destroy(dev_class,dev);
    class_destroy(dev_class);
    cdev_del(&mod_cdev);
    unregister_chrdev_region(dev, 1);
	printk(KERN_INFO "Device Driver Remove...Done!!!\n");
    kfree(drv_mem);
	printk(KERN_INFO "Kernel Module Removed Successfully...\n");
}

module_init(mod_driver_init);
module_exit(mod_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Anindya Sen <anindyasen007@gmail.com>");
MODULE_DESCRIPTION("A sample kernel module device driver");
MODULE_VERSION("1:0.1");
