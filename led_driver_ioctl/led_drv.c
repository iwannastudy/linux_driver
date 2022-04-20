#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/version.h>

#include <asm/mach/arch.h>
#include <mach/hardware.h>
#include <mach/gpio.h>
#include <asm/gpio.h>

#include "led_drv.h"

static int major;
static int minor;
struct cdev *led;
static dev_t devno;
static struct class *led_class;

#define DEVICE_NAME "led"

#define GPIO_LED_PIN_NUM    55  // gpio 1_23

static int led_open(struct inode *inode, struct file *file)
{
    try_module_get(THIS_MODULE);
    gpio_direction_output(GPIO_LED_PIN_NUM, 1);
    return 0;
}

static int led_release(struct inode *inode, struct file *file)
{
    module_put(THIS_MODULE);
    gpio_direction_output(GPIO_LED_PIN_NUM, 1);
    return 0;
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36)
static int led_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
#else
static int led_ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
#endif
{
    if(_IOC_TYPE(cmd) != LED_IOC_MAGIC)
    {
        return -ENOTTY;
    }

    if(_IOC_NR(cmd) > LED_IOCTL_MAXNR)
    {
        return -ENOTTY;
    }

    switch(cmd)
    {
        case LED_ON:
            gpio_set_value(GPIO_LED_PIN_NUM, 0);
            break;

        case LED_OFF:
            gpio_set_value(GPIO_LED_PIN_NUM, 1);
            break;

        default:
            gpio_set_value(27, 0);
    }

    return 0;
}

struct file_operations led_fops = 
{
    .owner = THIS_MODULE;
    .open = led_open;
    .release = led_release;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36)
    .unlocked_ioctl = led_ioctl;
#else
    .ioctl = led_ioctl;
#endif
};

static int __init led_init(void)
{
    int ret;

    gpio_free(GPIO_LED_PIN_NUM);
    if(gpio_request(GPIO_LED_PIN_NUM, "led_run"))
    {
        printk("request %s gpio failed \n", led_run);
        return -1;
    }

    ret = alloc_chrdev_region(&devno, minor, 1, "led");
    if(ret < 0)
    {
        printk(KERN_ERR "cannot get dev no\n");
        return -1;
    }
    major = MAJOR(devno);

    led = cdev_alloc();
    if(led != NULL)
    {
        cdev_init(led, &led_fops);
        led->owner = THIS_MODULE;
        if(cdev_add(led, devno, 1) != 0)
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

    led_class = class_create(THIS_MODULE, "led_class");
    if(IS_ERR(led_class))
    {
        printk(KERN_INFO "create class error!\n");
        return -1;
    }

    device_create(led_class, NULL, devno, NULL, "led");

    return 0;

error:
    unregister_chrdev_region(devno, 1);
    return ret;
}

static void __exit led_exit(void)
{
    cdev_del(led);
    unregister_chrdev_region(devno, 1);
    device_destroy(led_class, devno);
    class_destroy(led_class);
}

module_init(led_init);
module_exit(led_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("rivenWu, riven_wl1995@outlook.com");
