#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0xc58bbbf, "module_layout" },
	{ 0x31263050, "i2c_del_driver" },
	{ 0x330f46c4, "i2c_register_driver" },
	{ 0x37a0cba, "kfree" },
	{ 0x6bc3fbc0, "__unregister_chrdev" },
	{ 0x795c8806, "cdev_del" },
	{ 0x3d5c2631, "class_destroy" },
	{ 0x586876b1, "device_destroy" },
	{ 0x6f96cf6e, "device_create" },
	{ 0xbc8b9e1b, "__class_create" },
	{ 0x181a86f5, "cdev_add" },
	{ 0xe3ec2f2b, "alloc_chrdev_region" },
	{ 0x823ce766, "cdev_init" },
	{ 0x64677ce3, "cdev_alloc" },
	{ 0x189c5980, "arm_copy_to_user" },
	{ 0xefd6cf06, "__aeabi_unwind_cpp_pr0" },
	{ 0x86332725, "__stack_chk_fail" },
	{ 0xc5850110, "printk" },
	{ 0x84442284, "i2c_transfer" },
	{ 0x8f678b07, "__stack_chk_guard" },
};

MODULE_INFO(depends, "");

