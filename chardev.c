#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>
#include <linux/uaccess.h>
#define MAJOR_NUMBER 61/* forward declaration */

static int Major;		/* Major number assigned to our device driver */
#define DEVICE_NAME "chardev"	/* Dev name as it appears in /proc/devices   */

int onebyte_open(struct inode *inode, struct file *filep);
int onebyte_release(struct inode *inode, struct file *filep);
ssize_t onebyte_read(struct file *filep, 
        char *buf, size_t count, 
        loff_t *f_pos);

ssize_t onebyte_write(struct file *filep, 
        const char *buf, size_t count, 
        loff_t *f_pos);

static void onebyte_exit(void);
/* definition of file_operation structure */
struct file_operations onebyte_fops = {
    read: onebyte_read,
    write: onebyte_write,
    open: onebyte_open,
    release: onebyte_release
};
char *onebyte_data = NULL;

/* 
 * Called when a process tries to open the device file, like
 * "cat /dev/mycharfile"
 */
int onebyte_open(struct inode *inode, struct file *filep)
{
    	return 0; // always successful
}
int onebyte_release(struct inode *inode, struct file *filep)
{
	return 0; // always successful
}
ssize_t onebyte_read(struct file *filep, char *buf, size_t count, loff_t *f_pos)
{
/*please complete the function on your own*/
	int bytes_read = 0;
			
	/* Check if the buffer has been written */
	if(*buf != 0){
	   return 0;	
	}
	copy_to_user(buf, onebyte_data, sizeof(char));	
	bytes_read ++;
	return bytes_read;
}
ssize_t onebyte_write(struct file *filep, const char *buf, size_t count, loff_t *f_pos)
{
/*please complete the function on your own*/

/**
 * copy_from_user: Returns number of bytes that could not be copied. On success, this will be zero.
 *
 * to
 * Destination address, in kernel space.
 *
 * from
 * Source address, in user space.
 *
 * n
 * Number of bytes to copy.
 */
	int bytes_write=0;
	copy_from_user(onebyte_data, buf, sizeof(char));

	/* Check the length of the bytes that cannot be written*/
	if(count> sizeof(char))
	{
		printk(KERN_ALERT "No space left on device\n");
		/*Return Linux System Error<28>: No space left on device */
		return -ENOSPC; 
	}
	bytes_write++;
	return bytes_write;

}
static int onebyte_init(void)
{
//    int result;
    // register the device
    Major = register_chrdev(0, DEVICE_NAME, &onebyte_fops);
    if (Major < 0) {
        printk(KERN_ALERT "Registering char device failed with %d\n", Major);
        return Major;
    }
    printk(KERN_INFO "I was assigned major number %d. To talk to\n", Major);
    printk(KERN_INFO "the driver, create a dev file with\n");
	printk(KERN_INFO "'mknod /dev/%s c %d 0'.\n", DEVICE_NAME, Major);
	printk(KERN_INFO "Try various minor numbers. Try to cat and echo to\n");
    printk(KERN_INFO "the device file.\n");
	printk(KERN_INFO "Remove the device file and module when done.\n");
    
    // allocate one byte of memory for storage
    // kmalloc is just like malloc, the second parameter is// the type of memory to be allocated.
    // To release the memory allocated by kmalloc, use kfree.
    onebyte_data = kmalloc(sizeof(char), GFP_KERNEL);
    if (!onebyte_data) {
        onebyte_exit();
        // cannot allocate memory
        // return no memory error, negative signify a failure
        return -ENOMEM;
    }
//    // initialize the value to be X
    *onebyte_data = 'X';
    printk(KERN_ALERT "This is a onebyte device module.\n");
    return 0;
}
static void onebyte_exit(void)
{
    // if the pointer is pointing to something
    if (onebyte_data) {
        // free the memory and assign the pointer to NULL
        kfree(onebyte_data);
        onebyte_data = NULL;
    }
    // unregister the device
    unregister_chrdev(MAJOR_NUMBER, DEVICE_NAME);
    printk(KERN_ALERT "Onebyte device module is unloaded\n");
}
MODULE_LICENSE("GPL");
module_init(onebyte_init);
module_exit(onebyte_exit);
