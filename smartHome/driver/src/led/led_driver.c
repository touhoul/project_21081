#include <linux/module.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/gpio.h>
#include <linux/i2c.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/gpio.h>
#include <linux/of_gpio.h>
#include <linux/platform_device.h>
#include <linux/timer.h>

#define COUNT 2
#define CNAME "myAiLedCdev"
//led驱动接口
#define LED_MAGIC  'h'
#define CMD1_ON	  1
#define CMD1_OFF   0
#define CMD1_FALSH 2
#define CMD_ON	  3
#define CMD_OFF   4
#define CMD_FALSH 5

#define LED1_ON _IOW(LED_MAGIC, CMD1_ON, int)
#define LED1_OFF _IOW(LED_MAGIC, CMD1_OFF, int)
#define LED1_FLASH _IOW(LED_MAGIC, CMD1_FALSH, int)
#define LED3_ON _IOW(LED_MAGIC, CMD_ON, int)
#define LED3_OFF _IOW(LED_MAGIC, CMD_OFF, int)
#define LED3_FLASH _IOW(LED_MAGIC, CMD_FALSH, int)



int gpiono[3];
struct cdev *cdev;
struct class *cls;
struct device *dev;
unsigned int minor=0;
int major;
dev_t devno;
struct timer_list mytimer1;
struct timer_list mytimer2;

static int my_flag1=0;
static int my_flag2=0;


/*
ai_led{
	compatible = "ai,ai_led";
	led1 = <&gpioe 10 0>;
	led2 = <&gpiof 10 0>;
	led3 = <&gpioe 8 0>;
 } 
*/

void timer_function2(struct timer_list * timer_list)
{
	gpio_set_value(gpiono[1],!gpio_get_value(gpiono[1]));

	mod_timer(&mytimer2,jiffies + HZ);
}


void timer_function1(struct timer_list * timer_list)
{
	gpio_set_value(gpiono[0],!gpio_get_value(gpiono[0]));

	mod_timer(&mytimer1,jiffies + HZ);
}
long my_ai_led_ioctl(struct file *filp, unsigned int cmd, unsigned long args)
{
	//int data,ret;
	switch(cmd)
	{
		case LED1_ON:
			if(my_flag1 == 1)
			{
				del_timer(&mytimer1);
				my_flag1 = 0;
			}
			gpio_set_value(gpiono[0],1);
			
#if 0
			gpio_set_value(gpiono[2],1);
#endif
			
			break;
		case LED1_OFF:
			if(my_flag1 == 1)
			{
				del_timer(&mytimer1);
				my_flag1 = 0;
			}
			gpio_set_value(gpiono[0],0);

#if 0
			gpio_set_value(gpiono[2],0);
#endif
			break;
			
		case LED3_ON:
			if(my_flag2 == 1)
			{
				del_timer(&mytimer2);
				my_flag2 = 0;
			}
			gpio_set_value(gpiono[1],1);
			break;
		case LED3_OFF:
			if(my_flag2 == 1)
			{
				del_timer(&mytimer2);
				my_flag2 = 0;
			}
			gpio_set_value(gpiono[1],0);
			break;
			
		case LED1_FLASH:
			if(my_flag1 == 0)
			{
				my_flag1 = 1;
				add_timer(&mytimer1);
			}		
			break;
		case LED3_FLASH:
			if(my_flag2 == 0)
			{
				my_flag2 = 1;
				add_timer(&mytimer2);
			}		
			break;
		default:
			break;
			
	}
	return 0;
}

struct file_operations fops = {
	.unlocked_ioctl = my_ai_led_ioctl,
};

int pdrv_probe(struct platform_device *pdev)
{
	int ret, i;
	printk("line %d\n", __LINE__);
	//1.申请资源
	gpiono[0] = of_get_named_gpio(pdev->dev.of_node,"led1",0);
	if(gpiono[0] < 0)
	{
		printk("of_get_named_gpio failed: %d\n", __LINE__);
		return gpiono[0];
	}

	gpiono[1] = of_get_named_gpio(pdev->dev.of_node,"led3",0);
	if(gpiono[1] < 0)
	{
		printk("of_get_named_gpio failed: %d\n", __LINE__);
		return gpiono[1];
	}
	
#if 0
	gpiono[2] = of_get_named_gpio(pdev->dev.of_node,"led2",0);
	if(gpiono[2] < 0)
	{
		printk("of_get_named_gpio failed: %d\n", __LINE__);
		return gpiono[2];
	}
#endif
	
	//2.申请gpio号
	ret = gpio_request(gpiono[0], NULL);
	if(ret < 0)
	{
		printk("gpio_request failed: %d\n", __LINE__);
		return ret;
	}
	gpio_direction_output(gpiono[0], 0);

	ret = gpio_request(gpiono[1], NULL);
	if(ret < 0)
	{
		printk("gpio_request failed: %d\n", __LINE__);
		return ret;
	}
	gpio_direction_output(gpiono[1], 0);

#if 0
	ret = gpio_request(gpiono[2], NULL);
	if(ret < 0)
	{
		printk("gpio_request failed: %d\n", __LINE__);
		return ret;
	}
	gpio_direction_output(gpiono[2], 0);
#endif
	
	printk("gpiono[0]: %d gpiono[1]: %d\n", gpiono[0], gpiono[1]);

	//3.创建字符设备驱动
	//3.1分配对象
	cdev = cdev_alloc();
	if(cdev ==NULL)
	{
		printk("cdev alloc failed: %d\n", __LINE__);
		ret = -EINVAL;
		goto ERR1;
	}

	//3.2对象初始化
	cdev_init(cdev, &fops);

	//3.3申请设备号
	ret = alloc_chrdev_region(&devno,minor,COUNT,CNAME);
	if(ret < 0)
	{
		printk("alloc device number error: %d\n", __LINE__);
		goto ERR2;
	}
	major = MAJOR(devno);
	minor = MINOR(devno);

	printk("major: %d minor: %d\n", major, minor);

	//3.4注册
	ret = cdev_add(cdev,devno,COUNT);
	if(ret < 0)
	{
		printk("cdev_add failed: %d\n", __LINE__);
		goto ERR3;
	}

	//3.5自动创建设备节点
	cls = class_create(THIS_MODULE,"myAiLedCdev");
	if(IS_ERR(cls))
	{
		printk("class create failed: %d\n", __LINE__);
		ret = PTR_ERR(cls);
		goto ERR4;
	}
	for(i=0;i<COUNT;i++)
	{
		dev = device_create(cls,NULL,MKDEV(major,minor+i),NULL,
			"myAiLedCdev%d",i);
		if(IS_ERR(dev)){
		printk("device create failed: %d\n",__LINE__);
		ret = PTR_ERR(dev);
		goto ERR5;
		}
	}

	//5.初始化定时器
	mytimer1.expires = jiffies + HZ;
	timer_setup(&mytimer1,timer_function1,0);
	mytimer2.expires = jiffies + HZ;
	timer_setup(&mytimer2,timer_function2,0);

	return 0;
	
	ERR5:
		for(--i;i>=0;i--)
		{
			device_destroy(cls,MKDEV(major,minor+i));
		}
		class_destroy(cls);
	ERR4:
		cdev_del(cdev);
	ERR3:
		unregister_chrdev_region(devno,COUNT);
	ERR2:
		kfree(cdev);
	ERR1:
		return ret;
	
}
int pdrv_remove(struct platform_device *pdev)
{
	int i=0;
	gpio_set_value(gpiono[1],0);
	gpio_set_value(gpiono[0],0);
	
	del_timer(&mytimer1);
	del_timer(&mytimer2);
	gpio_free(gpiono[0]);
	gpio_free(gpiono[1]);	
	
#if 0
	gpio_set_value(gpiono[2],0);
	gpio_free(gpiono[2]);
#endif	

	for(i=0;i<COUNT;i++)
	{
		device_destroy(cls,MKDEV(major,minor+i));
	}
	class_destroy(cls);
	cdev_del(cdev);
	unregister_chrdev_region(devno,COUNT);
	kfree(cdev);
	return 0;
}

struct of_device_id oftable[] = {
	{.compatible = "ai,ai_led"},
	{/*end*/},
};

struct platform_driver pdrv={
	.probe = pdrv_probe,
	.remove = pdrv_remove,
	.driver = {
		.name = "ai_led",
		.of_match_table = oftable,
	}
};


module_platform_driver(pdrv);

MODULE_LICENSE("GPL");































