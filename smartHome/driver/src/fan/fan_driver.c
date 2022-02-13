#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/io.h>
#include <linux/mod_devicetable.h>
#include <linux/platform_device.h>
#include "./../../inc/fan.h"


#define COUNT		1 //设备节点个数
#define CNAME   "myfan"//设备节点名
#define GET_CMD_SIZE(cmd) ((cmd>>16)&0x3fff)


struct cdev *pcdev = NULL;
dev_t devno = 0;//设备号
unsigned int major = 0;//主设备号
unsigned int minor = 0;//次设备号
struct class *pclass = NULL;//目录句柄
struct device *pdevice = NULL;//设备节点

mygpio_t *gpioe = NULL;
myrcc_t *rcc = NULL;
mytim1_t *tim1 = NULL;

/************************************* struct file_operations ***********************************************/

int fandrv_open(struct inode *inode, struct file *pfile)
{
	printk("[fandrv_open okay]  %s:%s:__%d__\n",__FILE__,__func__,__LINE__);
	//映射地址
	gpioe = ioremap(GPIOE_PHY_BASE, sizeof(mygpio_t));
	if(gpioe == NULL){
		printk("ioremap gpioe error\n");
		return -ENOMEM;
	}
	
	rcc = ioremap(RCC_ENB,sizeof(myrcc_t));
	if(rcc == NULL){
		printk("ioremap rcc error\n");
		return -ENOMEM;
	}
	tim1 = ioremap(TIM1_ENB,sizeof(mytim1_t));
	if(tim1 == NULL){
		printk("ioremap tim1 error\n");
		return -ENOMEM;
	}
	//风扇初始化	
	fan_tim1_init();
	
	return 0;
}

int fandrv_close(struct inode *inode, struct file *pfile)
{
	printk("[fandrv_close okay]  %s:%s:__%d__\n",__FILE__,__func__,__LINE__);
	//取消映射
	tim1->CCER &= ~(0x1 << 0);
	tim1->CCER &= ~(0x1 << 2);
	tim1->CR1 &= ~(0x1 << 0);//停止计数
	rcc->MP_AHB4ENSETR &= ~(0x1 << 4);
	rcc->MP_APB2ENSETR &= ~(0x1 << 0);
	iounmap(tim1);
	iounmap(gpioe);
	iounmap(rcc);
	return 0;
}

long fandrv_ioctl(struct file *pfile, unsigned int cmd, unsigned long args)
{
	int ret;
	//printk("[fandrv_ioctl okay]  %s:%s:__%d__\n",__FILE__,__func__,__LINE__);
	switch(cmd)
	{
		case FAN_LEVEL_1:
			tim1->CCR1 = 500; //占空比 (1000-CCR1)/1000 = 50%
			//使能捕获比较器
			tim1->CCER |= (0x1 << 0);
			tim1->CCER |= (0x1 << 2);
			tim1->CR1  |= (0x1 << 0);
			break;	
		case FAN_LEVEL_2:
			tim1->CCR1 = 300; //占空比 (1000-CCR1)/1000 = 70%
			//使能捕获比较器
			tim1->CCER |= (0x1 << 0);
			tim1->CCER |= (0x1 << 2);
			tim1->CR1  |= (0x1 << 0);
			break;
		case FAN_LEVEL_3:
			tim1->CCR1 = 0; //占空比 (1000-CCR1)/1000 = 100%
			//使能捕获比较器
			tim1->CCER |= (0x1 << 0);
			tim1->CCER |= (0x1 << 2);
			tim1->CR1  |= (0x1 << 0);
			break;		
		case FAN_OFF:
			//tim1->CCR1 = 1000; //占空比 (1000-CCR1)/1000 = 0%
			//不使能捕获比较器
			tim1->CCER &= ~(0x1 << 0);
			tim1->CCER &= ~(0x1 << 2);
			tim1->CR1 &= ~(0x1 << 0);//停止计数
			break;
		default:;
	}
	ret = copy_to_user((void*)args,(void*)&cmd,GET_CMD_SIZE(cmd));                         
    /*if(ret)                                                                                                                                                                               
    {                                                                                                     
        printk("[copy_to_user error]  %s:%s:__%d__\n",__FILE__,__func__,__LINE__);                                                          
        return -EINVAL;                                                                                       
    }*/
    
	return 0;
}


struct file_operations fops={
	.open = fandrv_open,
	.release = fandrv_close,
	//.read = fandrv_read,
	//.write = fandrv_write,
	.unlocked_ioctl = fandrv_ioctl,
};


/************************************* cdev_create / cdev_remove ********************************************/
int cdev_create(void)
{
	int ret = 0;
	
	//1.分配对象
	pcdev = cdev_alloc();
	if(NULL == pcdev)
	{
		printk("[cdev_alloc error]  %s:%s:__%d__\n",__FILE__,__func__,__LINE__);
		ret = -ENOMEM;
		goto ERR1;
	}
	//2.初始化对象
	cdev_init(pcdev, &fops);
	ret = alloc_chrdev_region(&devno, minor, COUNT, CNAME);
	if(ret < 0)
	{
		printk("[alloc_chrdev_region error]  %s:%s:__%d__\n",__FILE__,__func__,__LINE__);
		goto ERR2;
	}
	major = MAJOR(devno);//获取主设备号
	//3.注册对象
	ret = cdev_add(pcdev, devno, COUNT);
	if(ret < 0)
	{
		printk("[cdev_add error]  %s:%s:__%d__\n",__FILE__,__func__,__LINE__);
		goto ERR3;
	}
	//4.自动创建设备节点
	pclass = class_create(THIS_MODULE, CNAME);
	if(IS_ERR(pclass))
	{
		printk("[class_create error]  %s:%s:__%d__\n",__FILE__,__func__,__LINE__);
		ret = PTR_ERR(pclass);
		goto ERR4;
	}
	pdevice = device_create(pclass, NULL, devno, NULL, CNAME);
	if(IS_ERR(pdevice))
	{
		printk("[device_create error]  %s:%s:__%d__\n",__FILE__,__func__,__LINE__);
		ret = PTR_ERR(pdevice);
		goto ERR5;
	}
	
	printk("[cdev_create okay]  %s:%s:__%d__\n",__FILE__,__func__,__LINE__);
	return 0;

ERR5:
	class_destroy(pclass);
ERR4:
	cdev_del(pcdev);
ERR3:
	unregister_chrdev_region(devno, COUNT);
ERR2:
	kfree(pcdev);
ERR1:
	return ret;
}

void cdev_remove(void)
{
	//1.销毁设备节点
	device_destroy(pclass, devno);
	class_destroy(pclass);
	//2.注销对象
	cdev_del(pcdev);
	//3.释放设备号
	unregister_chrdev_region(devno, COUNT);
	//4.释放对象
	kfree(pcdev);
	printk("[cdev_remove okay]  %s:%s:__%d__\n",__FILE__,__func__,__LINE__);
}

/************************************* __init / __exit *****************************************************/

static int __init fan_driver_init(void)
{
	int ret = 0;
	
	//1.创建驱动
	ret = cdev_create();
	if(ret < 0)
	{
		printk("[cdev_create error]  %s:%s:__%d__\n",__FILE__,__func__,__LINE__);
		return ret;
	}
	
	return 0;
	//return platform_driver_register(&plat_drv);
}

static void __exit fan_driver_exit(void)
{
	//销毁字符设备驱动
	cdev_remove();
	//platform_driver_unregister(&plat_drv);
}

module_init(fan_driver_init);
module_exit(fan_driver_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("this is FS_MP1A_fan_driver");
MODULE_AUTHOR("TouhouL <hz35fengmo@163.com>");


void fan_tim1_init()//PE9--TIM1_CH1--pwm--AF1
{
	// 1. 设置GPIOE9，TIM1的时钟使能   
	rcc->MP_AHB4ENSETR |= (0x1 << 4);
	rcc->MP_APB2ENSETR |= (0x1 << 0);
	// 2. 设置PE9引脚为复用功能       GPIOE_MODER
	gpioe->MODER &= (~(0x3 << 18));
	gpioe->MODER |= (0x2 << 18);
	// 3. 设置PE9引脚为TIM1_CH1功能   GPIOE_AFRH = AF1
	gpioe->AFRH &= (~(0xF << 4));
	gpioe->AFRH |= (0x1 << 4);
	// 4. 设置预分频寄存器，TIM1_PSC[15:0]  CK_PSC + 1 = 209MHz  
	// 		提供给TIM1的时钟源的频率是209MHz
	tim1->PSC = 209-1;
	// 5. 设置PWM方波的最终的周期  TIM1_ARR[16:0]
	// 		得到一个1000-2000Hz的方波
	tim1->ARR = 1000;
	// 5. 设置PWM方波的占空比   TIM1_CCR1[16:0]
	tim1->CCR1 = 300;//默认占空比 (1000-CCR1)/1000 = 70%
	// 6. 设置TIM1_CH1通道为PWM1模式  
	// 		TIM1_CCMR1[16]  TIM1_CCMR1[6:4]  pwm模式1  = 0b0110
	tim1->CCMR1 &= (~(0x1 << 16));
	tim1->CCMR1 &= (~(0x7 << 4));
	tim1->CCMR1 |= (0x6 << 4);
	// 7. 设置TIM1_CH1通道使能TIMx_CCR1预加载使能寄存器
	// 		TIM1_CCMR1[3] = 0x1
	tim1->CCMR1 |= (0x1 << 3);
	// 8. 设置TIM1_CH1通道输出PWM方波的极性，
	// 	  TIM1_CCER[3] = 0b0	TIM1_CCER[1] = 0x1 or 0x0（L/H）
	tim1->CCER &= (~(0x1 << 3));
	tim1->CCER |= (0x1 << 1);;//起始低电平
	// 9. 设置TIM1 _CH1通道的输出使能位disable，不输出PWM方波 
	// 		TIM1 _CCER[0]
	tim1->CCER &= ~(0x1 << 0);
	tim1->CCER &= ~(0x1 << 2);
	// 10. 设置TIM1 _CH1通道的预装载寄存器的缓冲区的使能
	// 		TIM1 _CR1[7] = 0x1
	tim1->CR1 |= (0x1 << 7);
	// 11. 设置定时器的计数方式，边沿对齐
	// 		TIM1 _CR1[6:5] = 0x0
	tim1->CR1 &= (~(0x3 << 5));
	// 12 设置定时器计数的方向，采用递减计数/递增计数 
	tim1->CR1 |= (0x1 << 4);//递减计数
	tim1->BDTR |= (0x1 << 15);
	// 13. 不使能TIM1 _CH1计数器 
	tim1->CR1 &= ~(0x1 << 0);

}

			
























