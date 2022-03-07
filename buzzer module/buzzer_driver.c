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

#define BUZZER_IO 26

#define DEV_NAME "buzzer_driver"
#define DEV_NUM 241

MODULE_LICENSE("GPL");

int buzzer_open(struct inode *pinode, struct file *pfile) {
	printk(KERN_ALERT "Open buzzer driver\n");

	return 0;
}

int buzzer_close(struct inode *pinode, struct file *pfile) {
	printk(KERN_ALERT "Close buzzer driver\n");

	return 0;
}

ssize_t buzzer_write(struct file *pfile, const char __user *buffer, size_t length, loff_t *offset) {
  char msg[256];
  int loop = 0;

  printk(KERN_ALERT "Write buzzer driver\n");

  //receive message from user application
  if(copy_from_user(msg, buffer, length)) {
    printk(KERN_ERR "write error\n");
  }

  gpio_request(BUZZER_IO, "BUZZER_IO");

  //if message's each character is '1' beep 125ms or '0' beep 125ms
  while(msg[loop] != '\0') {
    if(msg[loop] == '1') {
      gpio_direction_output(BUZZER_IO, 1);
      msleep(125);
    } else if(msg[loop] == '0') {
      gpio_direction_output(BUZZER_IO, 0);	
      msleep(125);
    }
    loop++;
  }
  
  //after the beep by message turn off beep and sleep 1s
  gpio_direction_output(BUZZER_IO, 0);	
  msleep(1000);

  return length;
}

struct file_operations fop = {
	.owner = THIS_MODULE,
	.open = buzzer_open,
	.release = buzzer_close,
	.write = buzzer_write,
};

int __init buzzer_init(void) {
	printk(KERN_ALERT "Init buzzer driver\n");

	register_chrdev(DEV_NUM, DEV_NAME, &fop);

	return 0;
}

void __exit buzzer_exit(void) {
	printk(KERN_ALERT "Exit buzzer driver\n");

	gpio_free(BUZZER_IO);
	unregister_chrdev(DEV_NUM, DEV_NAME);
}

module_init(buzzer_init);
module_exit(buzzer_exit);
