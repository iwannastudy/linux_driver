#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>

static int major = 232;     // default major dev no
static int minor = 0;       // default minor dev no

// module param
module_param(major, int, S_IRUGO);
module_param(minor, int, S_IRUGO);

struct cdev *char_cdev;
static dev_t devno;
static struct class *char_cdev_class;

#define DEVICE_NAME "char_dev"

static int char_cdev_open(struct inode *inode, struct file *file)
{
    try_module_get(THIS_MODULE);
    printk(KERN_INFO DEVICE_NAME "opend!\n");
    return 0;
}

static int char_cdev_release(struct inode *inode, struct file *file)
{
    printk(KERN_INFO DEVICE_NAME "closed!\n");
    module_put(THIS_MODULE);
    return 0;
}

static ssize_t char_cdev_read(struct file *file, char *buf, size_t count, loff_t *f_pos)
{
    printk(KERN_INFO DEVICE_NAME "read method!\n");
    return count;
}

static ssize_t char_cdev_write(struct file *file, const char *buf, size_t count, loff_t *f_pos)
{
    printk(KERN_INFO DEVICE_NAME "write method!\n");
    return count;
}

static int char_cdev_ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
{
    printk(KERN_INFO DEVICE_NAME "ioctl method!\n");
    return 0;
}

struct file_operations char_cdev_fops = 
{
    .owner      = THIS_MODULE,
    .read       = char_cdev_read,
    .write      = char_cdev_write,
    .open       = char_cdev_open,
    .release    = char_cdev_release,
    .ioctl      = char_cdev_ioctl
};

static int __init char_cdev_init(void)
{
    int ret;

    if(major > 0)
    {
        devno = MKDEV(major, minor);
        ret = register_chrdev_region(devno, 1, "char_dev");
    }
    else
    {
        ret = alloc_chrdev_region(&devno, minor, 1, "char_dev");
        major = MAJOR(devno);
    }

    if(ret < 0)
    {
        printk(KERN_ERR "cannot get major %d\n", major);
        return -1;
    }

    char_cdev = cdev_alloc();
    if(char_cdev != NULL)
    {
        // init cdev
        cdev_init(char_cdev, &char_cdev_fops);
        char_cdev->owner = THIS_MODULE;
        // add the cdev to system
        if(cdev_add(char_cdev, devno, 1) != 0)
        {
            printk(KERN_ERR "add cdev error!\n");
            goto error;
        }
    }
    else
    {
        printk(KERN_ERR "cdev_alloc error!\n");
        return -1;
    }

    // create a node in /sys/class/
    char_cdev_class = class_create(THIS_MODULE, "char_cdev_class");
    if(IS_ERR(char_cdev_class))
    {
        printk(KERN_ERR "create class error!\n");
        return -1;
    }

    // create a node in /sys/
    //device_create(char_cdev_class, NULL, devno, NULL, "char_cdev", "%d", MINOR(devno));
    device_create(char_cdev_class, NULL, devno, NULL, "char_cdev");
    printk(KERN_INFO "Init module, major dev id = %d.\n", major);
    return 0;

error:
    unregister_chrdev_region(devno, 1);
    return ret;
}

static void __exit char_cdev_exit(void)
{
    cdev_del(char_cdev);
    unregister_chrdev_region(devno, 1);
    device_destroy(char_cdev_class, devno);
    class_destroy(char_cdev_class);
    printk(KERN_INFO "Exit module.\n");
}

module_init(char_cdev_init);
module_exit(char_cdev_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("RivenWu, riven_wl1995@outlook.com");
