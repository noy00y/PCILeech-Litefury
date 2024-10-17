#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/elfnote-lto.h>
#include <linux/export-internal.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

#ifdef CONFIG_UNWINDER_ORC
#include <asm/orc_header.h>
ORC_HEADER;
#endif

BUILD_SALT;
BUILD_LTO_INFO;

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
	{ 0x8ddd8aad, "schedule_timeout" },
	{ 0x1000e51, "schedule" },
	{ 0x9cb986f2, "vmalloc_base" },
	{ 0xf0fdf6cb, "__stack_chk_fail" },
	{ 0xb2fd5ceb, "__put_user_4" },
	{ 0x618911fc, "numa_node" },
	{ 0x7a4eba5b, "kmem_cache_alloc" },
	{ 0xb3f985a8, "sg_alloc_table" },
	{ 0xc49f028b, "cdev_add" },
	{ 0xbcb36fe4, "hugetlb_optimize_vmemmap_key" },
	{ 0xd03d09d0, "pci_find_capability" },
	{ 0xfe487975, "init_wait_entry" },
	{ 0x98378a1d, "cc_mkdec" },
	{ 0x4682a504, "pci_enable_msi" },
	{ 0x6091797f, "synchronize_rcu" },
	{ 0x92d5838e, "request_threaded_irq" },
	{ 0xb662e586, "device_create" },
	{ 0x91f6b78b, "class_create" },
	{ 0x28b4a9b9, "finish_swait" },
	{ 0x4dfa8d4b, "mutex_lock" },
	{ 0xcb715bb4, "kmem_cache_free" },
	{ 0xe2b899d4, "dma_alloc_attrs" },
	{ 0x3d39f5c9, "pci_read_config_word" },
	{ 0x950eb34e, "__list_del_entry_valid_or_report" },
	{ 0x5a97d2f4, "pci_aer_clear_nonfatal_status" },
	{ 0x53a1e8d9, "_find_next_bit" },
	{ 0x5a5a2271, "__cpu_online_mask" },
	{ 0xdc4fde3d, "kthread_stop" },
	{ 0xfef216eb, "_raw_spin_trylock" },
	{ 0xcefb0c9f, "__mutex_init" },
	{ 0xd35cce70, "_raw_spin_unlock_irqrestore" },
	{ 0x53d94996, "pci_iounmap" },
	{ 0xd790ade1, "pci_restore_state" },
	{ 0x706c5a65, "preempt_count_sub" },
	{ 0xfb578fc5, "memset" },
	{ 0xc183e92f, "pci_set_master" },
	{ 0x5b8239ca, "__x86_return_thunk" },
	{ 0x17de3d5, "nr_cpu_ids" },
	{ 0x6b10bee1, "_copy_to_user" },
	{ 0xd9a5ea54, "__init_waitqueue_head" },
	{ 0x8ae61d4, "kthread_bind" },
	{ 0xa88e89b3, "pcie_capability_clear_and_set_word_unlocked" },
	{ 0x15ba50a6, "jiffies" },
	{ 0xda4ddab, "kthread_create_on_node" },
	{ 0x4f9a2015, "dma_set_coherent_mask" },
	{ 0x3c3ff9fd, "sprintf" },
	{ 0xc38259af, "__this_cpu_preempt_check" },
	{ 0xc4f04a94, "dma_free_attrs" },
	{ 0x999e8297, "vfree" },
	{ 0x6091b333, "unregister_chrdev_region" },
	{ 0x3213f038, "mutex_unlock" },
	{ 0xaf17e1a1, "pci_release_regions" },
	{ 0xfbe215e4, "sg_next" },
	{ 0x3beef470, "__folio_put" },
	{ 0x6729d3df, "__get_user_4" },
	{ 0xb0a72726, "kobject_set_name" },
	{ 0x18ae6279, "device_destroy" },
	{ 0x2cf56265, "__dynamic_pr_debug" },
	{ 0x45b8f0d3, "set_page_dirty_lock" },
	{ 0xf210b24f, "pci_disable_msix" },
	{ 0xfeaeac2, "pci_disable_device" },
	{ 0x78d27962, "boot_cpu_data" },
	{ 0xbc66ea2, "pcie_set_readrq" },
	{ 0x17c91bf6, "dma_set_mask" },
	{ 0xeec969de, "dma_unmap_sg_attrs" },
	{ 0x8874d531, "kmalloc_trace" },
	{ 0x46cf10eb, "cachemode2protval" },
	{ 0x50503492, "pci_read_config_byte" },
	{ 0xd6ee688f, "vmalloc" },
	{ 0x8466051a, "pci_write_config_word" },
	{ 0xb5b54b34, "_raw_spin_unlock" },
	{ 0xabc0d406, "cdev_init" },
	{ 0xeb233a45, "__kmalloc" },
	{ 0xe2c17b5d, "__SCT__might_resched" },
	{ 0xbf54de63, "kmalloc_caches" },
	{ 0x753ce748, "cdev_del" },
	{ 0xdfec2275, "kmem_cache_destroy" },
	{ 0x50a2f52c, "dma_map_sg_attrs" },
	{ 0x2d3385d3, "system_wq" },
	{ 0x587f22d7, "devmap_managed_key" },
	{ 0x34eaca82, "pci_save_state" },
	{ 0xc1514a3b, "free_irq" },
	{ 0xc80ab559, "swake_up_one" },
	{ 0x3afc8c60, "get_user_pages_fast" },
	{ 0xa78af5f3, "ioread32" },
	{ 0xe3ec2f2b, "alloc_chrdev_region" },
	{ 0x88db9f48, "__check_object_size" },
	{ 0x7696f8c7, "__list_add_valid_or_report" },
	{ 0xb5081175, "param_ops_uint" },
	{ 0x13c49cc2, "_copy_from_user" },
	{ 0x90b3081b, "pci_enable_device" },
	{ 0xf229424a, "preempt_count_add" },
	{ 0x4a453f53, "iowrite32" },
	{ 0x7f02188f, "__msecs_to_jiffies" },
	{ 0xc277333a, "pci_enable_device_mem" },
	{ 0xd1259982, "pci_iomap" },
	{ 0xa5cd6647, "pci_alloc_irq_vectors" },
	{ 0x656e4a6e, "snprintf" },
	{ 0xc5b6f236, "queue_work_on" },
	{ 0x494e3393, "vm_get_page_prot" },
	{ 0xc8c85086, "sg_free_table" },
	{ 0x48d88a2c, "__SCT__preempt_schedule" },
	{ 0x608741b5, "__init_swait_queue_head" },
	{ 0x92540fbf, "finish_wait" },
	{ 0x62d4f655, "class_destroy" },
	{ 0x8949782d, "__pci_register_driver" },
	{ 0xf16ebdf0, "pci_disable_msi" },
	{ 0x6df1aaf1, "kernel_sigaction" },
	{ 0x28e7de69, "pci_request_regions" },
	{ 0xe14524f3, "remap_pfn_range" },
	{ 0x37a0cba, "kfree" },
	{ 0x77985bdd, "pcpu_hot" },
	{ 0x6d802564, "__put_devmap_managed_page_refs" },
	{ 0x8c26d495, "prepare_to_wait_event" },
	{ 0xb3f7646e, "kthread_should_stop" },
	{ 0xe2964344, "__wake_up" },
	{ 0xded12510, "pci_irq_vector" },
	{ 0x1d370c9a, "kmem_cache_create" },
	{ 0x34db050b, "_raw_spin_lock_irqsave" },
	{ 0xb19a5453, "__per_cpu_offset" },
	{ 0xba8fbd64, "_raw_spin_lock" },
	{ 0x8c5fa68b, "pci_unregister_driver" },
	{ 0xbdfb6dbb, "__fentry__" },
	{ 0x553fbb5d, "wake_up_process" },
	{ 0x65487097, "__x86_indirect_thunk_rax" },
	{ 0x122c3a7e, "_printk" },
	{ 0x3f466422, "prepare_to_swait_event" },
	{ 0x1d24c881, "___ratelimit" },
	{ 0x6b910e0, "module_layout" },
};

MODULE_INFO(depends, "");

MODULE_ALIAS("pci:v000010EEd00009048sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000010EEd00009044sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000010EEd00009042sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000010EEd00009041sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000010EEd0000903Fsv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000010EEd00009038sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000010EEd00009028sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000010EEd00009018sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000010EEd00009034sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000010EEd00009024sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000010EEd00009014sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000010EEd00009032sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000010EEd00009022sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000010EEd00009012sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000010EEd00009031sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000010EEd00009021sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000010EEd00009011sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000010EEd00008011sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000010EEd00008012sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000010EEd00008014sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000010EEd00008018sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000010EEd00008021sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000010EEd00008022sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000010EEd00008024sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000010EEd00008028sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000010EEd00008031sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000010EEd00008032sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000010EEd00008034sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000010EEd00008038sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000010EEd00007011sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000010EEd00007012sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000010EEd00007014sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000010EEd00007018sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000010EEd00007021sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000010EEd00007022sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000010EEd00007024sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000010EEd00007028sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000010EEd00007031sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000010EEd00007032sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000010EEd00007034sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000010EEd00007038sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000010EEd00006828sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000010EEd00006830sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000010EEd00006928sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000010EEd00006930sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000010EEd00006A28sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000010EEd00006A30sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000010EEd00006D30sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000010EEd00004808sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000010EEd00004828sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000010EEd00004908sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000010EEd00004A28sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000010EEd00004B28sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000010EEd00002808sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001D0Fd0000F000sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001D0Fd0000F001sv*sd*bc*sc*i*");

MODULE_INFO(srcversion, "E50329112A40D685F5E000D");
