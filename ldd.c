#include <linux/init.h>    // Provides Linux kernel module_init and module_exit macros
#include <linux/module.h>  // Core header for Linux kernel modules e.g ., MODULE_LICENSE, MODULE_AUTHOR, MODULE_DESCRIPTION, etc.
#include <linux/kernel.h>  // Provides kernel info e.g., printk
#include <linux/proc_fs.h> // Provides /proc filesystem functionality e.g., proc_create, proc_remove
#include <linux/uaccess.h> // Provides copy_to_user and copy_from_user functions

MODULE_LICENSE("GPL");                                      // this is a must
MODULE_AUTHOR("Sanketh J H");                               // optional
MODULE_DESCRIPTION("My first Dynamically loadable module"); // optional

// Maximum size for input from user space
#define MAX_INPUT_SIZE 128

/**
 * @brief Pointer to custom proc file entry structure
 *
 * This static variable stores a pointer to a proc_dir_entry structure that
 * represents a custom node in the /proc filesystem. The proc_dir_entry
 * structure contains information about the proc file including permissions,
 * owner, and operations that can be performed on it.
 */
static struct proc_dir_entry *custom_proc_node;

// Buffer to hold messages exchanged between user space and kernel space
static char message_buffer[MAX_INPUT_SIZE + 1] = "Hello from ldd demo driver entry!\n";

/**
 * ldd_proc_read - read handler for the proc fs entry
 * @file_pointer:   Pointer to the open file (struct file *) provided by the kernel.
 * @user_space_buffer:    User-space buffer (char __user *) to copy the output into.
 * @count:  Maximum number of bytes to read (size_t).
 * @offset:   Pointer to the file position (loff_t *); should be advanced by the
 *          number of bytes successfully returned to the user.
 *
 * @note: This function is called when a read operation is performed on the
 *        /proc/ldd_demo_driver entry.
 * @return: non-negative byte count on success (0 for EOF), negative errno on error.
 */
static ssize_t ldd_read_proc(struct file *file_pointer, char __user *user_space_buffer, size_t count, loff_t *offset)
{
    printk("ldd_read_proc: entry\n");
    size_t len = strlen(message_buffer);

    if (*offset >= len || count < len)
    {
        printk("ldd_read_proc: exit - nothing to read\n");
        return 0; // EOF or nothing to read
    }

    if (copy_to_user(user_space_buffer, message_buffer, len)) {
        printk(KERN_ERR "ldd_read_proc: copy_to_user failed\n");
        return -EFAULT;
    }

    *offset += len;
    printk("ldd_read_proc: exit\n");
    return len;
}

/**
 * @brief Write handler for the proc file entry
 *
 * This function handles write operations to the proc file. It copies data from user space
 * to kernel space, ensuring the input doesn't exceed the maximum allowed size.
 * The received message is null-terminated and logged to the kernel log.
 *
 * @param file_pointer Pointer to the file structure
 * @param user_space_buffer Buffer containing data from user space
 * @param count Number of bytes to write
 * @param offset Position in file (unused)
 *
 * @note This function is called when a write operation is performed on the
 *       /proc/ldd_demo_driver entry.
 * @return Number of bytes written on success, negative errno on failure
 *         -EFAULT if copy_from_user fails
 */
static ssize_t ldd_write_proc(struct file *file_pointer, const char __user *user_space_buffer, size_t count, loff_t *offset)
{
    printk("ldd_write_proc: entry\n");
    
    if (count > MAX_INPUT_SIZE) {
        printk(KERN_WARNING "ldd_write_proc: input truncated from %zu to %d bytes\n", count, MAX_INPUT_SIZE);
        count = MAX_INPUT_SIZE;
    }

    if (copy_from_user(message_buffer, user_space_buffer, count)) {
        printk(KERN_ERR "ldd_write_proc: copy_from_user failed\n");
        return -EFAULT;
    }

    message_buffer[count] = '\0'; // Null-terminate the string
    printk("ldd_write_proc: received: %s\n", message_buffer);
    printk("ldd_write_proc: exit\n");
    return count;
}

/**
 * @brief Structure defining the proc file operations
 *
 * This structure is used to define the operations that can be performed
 * on the /proc/ldd_demo_driver entry, including read and write handlers.
 */
struct proc_ops driver_proc_ops = {
    .proc_read = ldd_read_proc,
    .proc_write = ldd_write_proc
};

/**
 * @brief Initialization function for the LDD demo driver module
 *
 * This function is called when the module is loaded into the kernel.
 * It creates a proc filesystem entry named "ldd_demo_driver" and performs
 * initial setup of the driver.
 *
 * @return 0 on success, -1 on failure to create proc entry
 *
 * @note Uses proc_create() to create /proc/ldd_demo_driver entry
 * @note Prints kernel messages on entry and exit for debugging
 */
static int __init mydrv_init(void)
{
    printk(KERN_INFO "Hello you are loading the ldd module!: entry\n");
    
    custom_proc_node = proc_create("ldd_demo_driver", 0644, NULL, &driver_proc_ops);
    if (!custom_proc_node) {
        printk(KERN_ERR "Failed to create /proc entry\n");
        return -ENOMEM;
    }
    
    printk(KERN_INFO "ldd module loading: exit\n");
    return 0;
}

/**
 * @brief Cleanup function called when the module is unloaded
 *
 * This function performs cleanup operations when the module is being unloaded.
 * It removes the previously created proc entry and prints debug messages to
 * kernel log to track the unloading process.
 *
 * @note Uses proc_remove() to delete /proc/ldd_demo_driver entry
 * @note This function must reverse all operations done in the module init function
 * @note Prints kernel messages on entry and exit for debugging
 */
static void __exit mydrv_exit(void)
{
    printk(KERN_INFO "ldd module unloading: entry\n");
    
    if (custom_proc_node) {
        proc_remove(custom_proc_node);
        custom_proc_node = NULL;
    }
    
    printk(KERN_INFO "ldd module unloading: exit\n");
}

module_init(mydrv_init);
module_exit(mydrv_exit);
