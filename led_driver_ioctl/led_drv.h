#ifndef __LED_DRV_H__
#define __LED_DRV_H__

#define LED_IOC_MAGIC   'L'
#define LED_ON  _IO(LED_IOC_MAGIC, 0)
#define LED_OFF _IO(LED_IOC_MAGIC, 1)

#define LED_IOCTL_MAXNR 2

#endif
