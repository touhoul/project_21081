#include "linux/module.h"
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/i2c.h>
#include <linux/device.h>

#define CNAME "si7006"

struct i2c_client *gclient;

struct cdev   *cdev;
struct class  *cls;
struct device *dev;

dev_t devno;
int   major = 0;
unsigned int minor = 0;

#if 1
//温湿度驱动接口
#define si7006_MAJIC 'w'
#define CMD_TEMP 1
#define CMD_HUMP 2
#define READ_TEMP _IOW(si7006_MAJIC, CMD_TEMP, int)
#define READ_HUMP _IOW(si7006_MAJIC, CMD_HUMP, int)
#endif

int i2c_read_temp_hump(unsigned char reg)
{
	int ret;
	unsigned short val;
	struct i2c_msg r_msg[] = {
		[0] = {
			.addr = gclient->addr,
			.flags = 0,
			.len = 1,
			.buf = &reg,
		},	
		[1] = {
			.addr = gclient->addr,
			.flags = 1,
			.len = 2,
			.buf = (char*)&val,
		},
	};
	ret = i2c_transfer(gclient->adapter, r_msg, ARRAY_SIZE(r_msg));
	if(ret != ARRAY_SIZE(r_msg)){
		printk("i2c_transfer error\n");
		return -EINVAL;
	}
	val = val >> 8 | val << 8;
	return val;

}

long si7006_ioctl(struct file *filp, unsigned int cmd, unsigned long args)
{
	int temp, hum, ret;
	switch(cmd){
	case READ_TEMP:
		temp = i2c_read_temp_hump(0xE3);
		temp  = temp & 0xffff;
		ret = copy_to_user((void*)args, (void*)&temp, sizeof(int));
		if(ret){
			printk("read temp error\n");
			return -EAGAIN;
		}
		break;
	case READ_HUMP:
		hum = i2c_read_temp_hump(0xE5);
		hum = hum & 0xffff;
		ret = copy_to_user((void*)args, (void*)&hum, sizeof(int));
		if(ret){
			printk("read hum error\n");
			return -EAGAIN;
		}
		break;
	default:
		break;
	}
	return 0;
}


struct file_operations fops = {
	.unlocked_ioctl = si7006_ioctl,
};


int create_my_device(int count)
{
	int ret = 0;
	int i;
	//1.申请对象
	cdev = cdev_alloc();
	if(cdev == NULL)
	{
		printk("cdev_alloc error\n");
		ret = -EINVAL;
		goto ERR_CDEV_ALLOC;
	}
	
	//2.对象初始化
	cdev_init(cdev, &fops);
	
	//3.设备号的申请
	ret = alloc_chrdev_region(&devno, minor, count, CNAME);
	if(ret)
	{
		ret = -EINVAL;
		goto ERR_ALLOC_CHRDEV;
	}
	minor = MINOR(devno);
	major = MAJOR(devno);
	
	//4.注册
	ret = cdev_add(cdev, devno, count);
	if(ret < 0)
	{
		goto ERR_CDEV_ADD;
	}
	//5.自动创建设备节点
	cls = class_create(THIS_MODULE, CNAME);
	if(IS_ERR(cls))
	{
		ret = -EINVAL;
		goto ERR_CLASS_CREATE;
	}
	for(i=0; i<count; i++)
	{
		dev = device_create(cls, NULL, devno, NULL, "si7006_%d", i);
		if(IS_ERR(dev))
		{
			ret = -ENOMEM;
			goto ERR_DEVICE_CREATE;
		}
	}
	return 0;
	
ERR_DEVICE_CREATE:
	for(--i; i>=0; i++)
	{
		device_destroy(cls, MKDEV(major, minor+i));
	}
	class_destroy(cls);
	
ERR_CLASS_CREATE:
	cdev_del(cdev);

ERR_CDEV_ADD:
	unregister_chrdev(major, CNAME);

ERR_ALLOC_CHRDEV:
	kfree(cdev);
	
ERR_CDEV_ALLOC:
	return ret;


}

int destroy_my_device(int count)
{
	int i;
	for(i=0; i<=count; i++)
	{
		device_destroy(cls, MKDEV(major, minor+i));
	}
	class_destroy(cls);

	cdev_del(cdev);

	unregister_chrdev(major, CNAME);

	kfree(cdev);
	
	return 0;

}


int si7006_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	printk("si7006_probe\n");
	gclient = client;
	if(create_my_device(1))
	{
		printk("ERROR\n");
	}
	return 0;

}
int si7006_remove(struct i2c_client *client)
{
	destroy_my_device(1);
	return 0;
}

struct of_device_id oftable[] = {
	{.compatible = "si7006", .data = 0},
	{},
};


struct i2c_driver si7006 = 
{
	.probe = si7006_probe,
	.remove = si7006_remove,
	.driver = {
		.name = "si7006",
		.of_match_table = oftable,
	},
};


module_i2c_driver(si7006);

MODULE_LICENSE("GPL");

