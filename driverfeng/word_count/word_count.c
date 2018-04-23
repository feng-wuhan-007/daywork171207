#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <asm/uaccess.h>

#define DEVICE_NAME "word_count"

static unsigned char mem[10000];
//static char read_flag = 'y';
//static int written_count = 0;
static int word_count = 0;

#define TRUE -1
#define FALSE 0

static char is_spacewhite(char c)
{
	if(c == ' ' || c == 9 || c == 13 || c == 10)
	return TRUE;
	else
	return FALSE;
}

static int get_word_count(const char *buf)
{
	int n = 1;
	int i = 0;
	char c = ' ';

	char flag = 0;

	if(*buf == '\0')
	return 0;

	if( is_spacewhite(*buf) == TRUE)
	n--;

	for(; (c = *(buf+i)) != '\0';i++)
	{
		if(flag ==1 && is_spacewhite(c) == FALSE)
		{
			flag = 0;
		}
		else if(flag ==1 && is_spacewhite(c) == TRUE)
		{
			continue;
		}

		if(is_spacewhite(c) == TRUE)
		{
			n++;
			flag = 1;
		}
	}

	if(is_spacewhite(*(buf+i-1)) == TRUE)
		n--;

	return n;
			
		
	
}
//read
static ssize_t word_count_read(struct file *file,char __user *buf,size_t count ,loff_t *ppos)
{
	/*if(read_flag == 'n')
	{
	copy_to_user(buf,(void *)mem,written_count);
	printk("read count:%d",(int)written_count);
	read_flag = 'y';
	return written_count;
	}
	else
	{
	return 0;
	}*/
	unsigned char temp[4];
	temp[0] = word_count >> 24;
	temp[1] = word_count >> 16;
	temp[2] = word_count >> 8;
	temp[3] = word_count ;
	copy_to_user(buf,(void *)temp,4);
	printk("read count:%d",(int)count);
	return count;
}

//write
static ssize_t word_count_write(struct file *file,const char __user *buf,size_t count ,loff_t *ppos)
{
	/*copy_from_user(mem,buf,count);
	read_flag = 'n';
	written_count = count;
	printk("written count:%d",(int)count);
	return count;*/
	ssize_t written = count;
	copy_from_user(mem,buf,count);
	mem[count]= '\0';
	word_count = get_word_count(mem);
	printk("written count:%d",(int)word_count);
	return written;

	
}

static struct file_operations dev_fops =
{
.owner = THIS_MODULE,
.read  = word_count_read,
.write = word_count_write
};

static struct miscdevice misc =
{
.minor = MISC_DYNAMIC_MINOR,
.name = DEVICE_NAME,
.fops = &dev_fops
};

//init driver
static int word_count_init(void)
{
	//register 
	int ret;
	ret = misc_register(&misc);
	printk("word_count_init_success\n");
	return ret;
}
static void word_count_exit(void)
{
	//delete 
	misc_deregister(&misc);
	printk("word_count_exit_success\n");
	
}

//register driver
module_init(word_count_init);
//deregister driver
module_exit(word_count_exit);


MODULE_AUTHOR("fengmc");
MODULE_DESCRIPTION("word count");
MODULE_ALIAS("word count module");
MODULE_LICENSE("GPL");

