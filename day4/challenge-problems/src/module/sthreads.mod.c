#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

__visible struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0x525b86d, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0xe16b0d30, __VMLINUX_SYMBOL_STR(unregister_jprobe) },
	{ 0x9760bdd0, __VMLINUX_SYMBOL_STR(device_destroy) },
	{ 0x6bc3fbc0, __VMLINUX_SYMBOL_STR(__unregister_chrdev) },
	{ 0xeb9c2039, __VMLINUX_SYMBOL_STR(class_destroy) },
	{ 0xb1bedeb7, __VMLINUX_SYMBOL_STR(register_jprobe) },
	{ 0x320a4d5f, __VMLINUX_SYMBOL_STR(device_create) },
	{ 0xb587ca13, __VMLINUX_SYMBOL_STR(__class_create) },
	{ 0xc6587ef2, __VMLINUX_SYMBOL_STR(__register_chrdev) },
	{ 0x37a0cba, __VMLINUX_SYMBOL_STR(kfree) },
	{ 0x46434934, __VMLINUX_SYMBOL_STR(disable_kprobe) },
	{ 0xfd37ed40, __VMLINUX_SYMBOL_STR(module_put) },
	{ 0xc993bd44, __VMLINUX_SYMBOL_STR(try_module_get) },
	{ 0xc447dd93, __VMLINUX_SYMBOL_STR(enable_kprobe) },
	{ 0x26932b93, __VMLINUX_SYMBOL_STR(kmem_cache_alloc_trace) },
	{ 0x6dc7dc1e, __VMLINUX_SYMBOL_STR(kmalloc_caches) },
	{ 0x1b9aca3f, __VMLINUX_SYMBOL_STR(jprobe_return) },
	{ 0xff4155a, __VMLINUX_SYMBOL_STR(current_task) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{ 0xbdfb6dbb, __VMLINUX_SYMBOL_STR(__fentry__) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "3EEEA7D22F3E91538ACDD63");
