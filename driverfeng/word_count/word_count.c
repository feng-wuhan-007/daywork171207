#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <asm/uaccess.h>
#define DEVICE_NAME "wordcount"
static unsigned char mem[10000];
static char read_flag='y';
static int written_count=0;
//read 
static ssize_t word_count_read(struct file *file,char __user *buf,size_t count,loff_t *ppos)
{
	if(read_flag == 'n')
	{
		copy_to_user(buf,(void *)mem,written_count);
		printk("read count:%d",(int)written_count);
		read_flag='y';
		return written_count;
	}
	else
	{
		return 0;
	}
}
//write
static ssize_t word_count_write(struct file *file,const char __user *buf,size_t count,loff_t *ppos)
{
	
		copy_from_user(mem,buf,count);
		read_flag='n';
		written_count=count;
		printk("writtrn count:%d",(int)count);
		return count;
}
static int word_count_init(void)
{
	printk("word_count_init_success\n");
	return 0;
}
static void word_count_exit(void)
{
	printk("word_count_exit_success\n");
}
module_init(word_count_init);
module_exit(word_count_exit);

