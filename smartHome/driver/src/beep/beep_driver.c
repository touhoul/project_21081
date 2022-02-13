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
#include "./../../inc/beep.h"


#define COUNT		1 //设备节点个数
#define CNAME   "mybeep"//设备节点名
#define GET_CMD_SIZE(cmd) ((cmd>>16)&0x3fff)


struct cdev *pcdev = NULL;
dev_t devno = 0;//设备号
unsigned int major = 0;//主设备号
unsigned int minor = 0;//次设备号
struct class *pclass = NULL;//目录句柄
struct device *pdevice = NULL;//设备节点

mygpio_t *gpiob = NULL;
myrcc_t *rcc = NULL;
tim2_3_4_5_t *tim4 = NULL;

/************************************* struct file_operations ***********************************************/

int beepdrv_open(struct inode *inode, struct file *pfile)
{
	printk("[beepdrv_open okay]  %s:%s:__%d__\n",__FILE__,__func__,__LINE__);
	//映射地址
	gpiob = ioremap(GPIOB_PHY_BASE, sizeof(mygpio_t));
	if(gpiob == NULL){
		printk("ioremap gpiob error\n");
		return -ENOMEM;
	}
	
	rcc = ioremap(RCC_ENB,sizeof(myrcc_t));
	if(rcc == NULL){
		printk("ioremap rcc error\n");
		return -ENOMEM;
	}
	tim4 = ioremap(TIM4_ENB,sizeof(tim2_3_4_5_t));
	if(tim4 == NULL){
		printk("ioremap tim4 error\n");
		return -ENOMEM;
	}
	//蜂鸣器初始化	
	beep_tim4_init();
	
	return 0;
}

int beepdrv_close(struct inode *inode, struct file *pfile)
{
	printk("[beepdrv_close okay]  %s:%s:__%d__\n",__FILE__,__func__,__LINE__);
	//取消映射
	tim4->CCER &= ~(0x1 << 0);
	tim4->CCER &= ~(0x1 << 2);
	tim4->CR1 &= ~(0x1 << 0);//停止计数
	rcc->MP_AHB4ENSETR &= ~(0x1 << 1);
	rcc->MP_APB1ENSETR &= ~(0x1 << 2);
	iounmap(tim4);
	iounmap(gpiob);
	iounmap(rcc);
	return 0;
}

long beepdrv_ioctl(struct file *pfile, unsigned int cmd, unsigned long args)
{
	int ret;
	//printk("[beepdrv_ioctl okay]  %s:%s:__%d__\n",__FILE__,__func__,__LINE__);
	switch(cmd)
	{
		case BEEP_ON:
			tim4->CCR1 = 300; //占空比 (1000-CCR1)/1000 = 70%
			//使能捕获比较器
			tim4->CCER |= (0x1 << 0);
			tim4->CCER |= (0x1 << 2);
			tim4->CR1  |= (0x1 << 0);
			break;		
		case BEEP_OFF:
			//tim4->CCR1 = 1000; //占空比 (1000-CCR1)/1000 = 0%
			//不使能捕获比较器
			tim4->CCER &= ~(0x1 << 0);
			tim4->CCER &= ~(0x1 << 2);
			tim4->CR1 &= ~(0x1 << 0);//停止计数
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
	.open = beepdrv_open,
	.release = beepdrv_close,
	.unlocked_ioctl = beepdrv_ioctl,
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

static int __init beep_driver_init(void)
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
}

static void __exit beep_driver_exit(void)
{
	//销毁字符设备驱动
	cdev_remove();
}

module_init(beep_driver_init);
module_exit(beep_driver_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("this is FS_MP1A_beep_driver");
MODULE_AUTHOR("TouhouL <hz35fengmo@163.com>");


void beep_tim4_init()  //PB6--TIM4_CH1--pwm--AF2
{
	// 1. 设置gpiob6，tim4的时钟使能,MP_AHB4ENSETR[1],MP_APB1ENSETR[2]
	rcc->MP_AHB4ENSETR |= (0x1 << 1);
	rcc->MP_APB1ENSETR |= (0x1 << 2);
	// 2. 设置PB6引脚为复用功能       gpiob_MODER[13:12]
	gpiob->MODER &= (~(0x3 << 12));
	gpiob->MODER |= (0x2 << 12);
	// 3. 设置PB6引脚为tim4_CH1功能   gpiob_AFRL[27:24] = AF2
	gpiob->AFRL &= (~(0xF << 24));
	gpiob->AFRL |= (0x2 << 24);
	// 4. 设置预分频寄存器，tim4_PSC[15:0]  CK_PSC + 1 = 209MHz  
	// 		提供给TIM1的时钟源的频率是209MHz
	tim4->PSC = 209-1;
	// 5. 设置PWM方波的最终的周期  tim4_ARR[16:0]
	// 		得到一个1000-2000Hz的方波
	tim4->ARR = 1000;
	// 5. 设置PWM方波的占空比   tim4_CCR1[16:0]
	tim4->CCR1 = 500;//默认占空比 (1000-CCR1)/1000 = 50%
	// 6. 设置tim4_CH1通道为PWM1模式  
	// 		tim4_CCMR1[16]  tim4_CCMR1[6:4]  pwm模式1  = 0b0110
	tim4->CCMR1 &= (~(0x1 << 16));
	tim4->CCMR1 &= (~(0x7 << 4));
	tim4->CCMR1 |= (0x6 << 4);
	// 7. 设置tim4_CH1通道使能TIMx_CCR1预加载使能寄存器
	// 		tim4_CCMR1[3] = 0x1
	tim4->CCMR1 |= (0x1 << 3);
	// 8. 设置tim4_CH1通道输出PWM方波的极性，
	// 	  tim4_CCER[3] = 0b0	tim4_CCER[1] = 0x1 or 0x0（L/H）
	tim4->CCER &= (~(0x1 << 3));
	tim4->CCER |= (0x1 << 1);;//起始低电平
	// 9. 设置tim4_CH1通道的输出使能位disable，不输出PWM方波 
	// 		tim4_CCER[0]
	tim4->CCER &= ~(0x1 << 0);
	tim4->CCER &= ~(0x1 << 2);
	// 10. 设置tim4_CH1通道的预装载寄存器的缓冲区的使能
	// 		tim4_CR1[7] = 0x1
	tim4->CR1 |= (0x1 << 7);
	// 11. 设置定时器的计数方式，边沿对齐
	// 		tim4_CR1[6:5] = 0x0
	tim4->CR1 &= (~(0x3 << 5));
	// 12 设置定时器计数的方向，采用递减计数/递增计数 
	tim4->CR1 |= (0x1 << 4);//递减计数
	// 13. 不使能tim4_CH1计数器 
	tim4->CR1 &= ~(0x1 << 0);

}

			
























