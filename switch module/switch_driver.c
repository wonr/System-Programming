#include<linux/gpio.h>
#include<linux/init.h>
#include<linux/module.h>
#include<linux/delay.h>
#include<linux/fs.h>
#include<linux/slab.h>
#include<linux/platform_device.h>
#include<linux/kernel.h>
#include<linux/ioport.h>
#include<linux/version.h>
#include<linux/uaccess.h>
#include<asm/io.h>
#include<asm/uaccess.h>

#define SWITCH1_IO 20
#define SWITCH2_IO 21
#define GYRO_SC_IO 17
#define GYRO_SD_IO 27

#define DEV_NAME "switch_driver"
#define DEV_NUM 240

MODULE_LICENSE("GPL");

int switch_open(struct inode *pinode, struct file *pfile) {
	printk(KERN_ALERT "Open switch driver\n");

	return 0;
}

int switch_close(struct inode *pinode, struct file *pfile) {
	printk(KERN_ALERT "Close switch driver\n");

	return 0;
}

ssize_t switch_read(struct file *pfile, char __user *buffer, size_t length, loff_t *offset) {
	int switch1, switch2;
	int ret;
	//if none of button switch are selected result is '0' by init
	char result[256] = "0";

	printk(KERN_ALERT "Read switch driver\n");
	
	gpio_request(SWITCH1_IO, "SWITCH1_IO");
	gpio_request(SWITCH2_IO, "SWITCH2_IO");

	gpio_direction_input(SWITCH1_IO);
	gpio_direction_input(SWITCH2_IO);

	switch1 = gpio_get_value(SWITCH1_IO);
	switch2 = gpio_get_value(SWITCH2_IO);

	printk(KERN_ALERT "Button1: %d\n", gpio_get_value(SWITCH1_IO));
	printk(KERN_ALERT "Button1: %d\n", gpio_get_value(SWITCH2_IO));
	printk(KERN_ALERT "Button1: %s\n", result);

	//if button is selected return button number
	if(switch2) sprintf(result, "%s", "2");;
	if(switch1) sprintf(result, "%s", "1");;

	//send a return message to user application
	ret = copy_to_user(buffer, result, strlen(result));

	return 0;
}

struct file_operations fop = {
	.owner = THIS_MODULE,
	.open = switch_open,
	.release = switch_close,
	.read = switch_read,
};

int __init switch_init(void) {
	printk(KERN_ALERT "Init switch driver\n");

	register_chrdev(DEV_NUM, DEV_NAME, &fop);

	return 0;
}

void __exit switch_exit(void) {
	printk(KERN_ALERT "Exit switch driver\n");

	gpio_free(SWITCH1_IO);
	gpio_free(SWITCH2_IO);
	unregister_chrdev(DEV_NUM, DEV_NAME);
}

module_init(switch_init);
module_exit(switch_exit);
