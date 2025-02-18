/**
 * File:	lkmasg1.c
 * Adapted for Linux 5.15 by: John Aedo
 * Class:	COP4600-SP23
 */

#include <linux/module.h>	  // Core header for modules.
#include <linux/device.h>	  // Supports driver model.
#include <linux/kernel.h>	  // Kernel header for convenient functions.
#include <linux/fs.h>		  // File-system support.
#include <linux/uaccess.h>	  // User access copy function support.
#define DEVICE_NAME "lkmasg2" // Device name.
#define CLASS_NAME "char"	  ///< The device class -- this is a character device driver
#define MAX_LENGTH 256

MODULE_LICENSE("GPL");							///< The license type -- this affects available functionality
MODULE_AUTHOR("Dennis shelton/John Aedo");		///< The author -- visible when you use modinfo
MODULE_DESCRIPTION("lkmasg2 Kernel Module");	///< The description -- see modinfo
MODULE_VERSION("0.1");							///< A version number to inform users

/**
 * Important variables that store data and keep track of relevant information.
 */
static int major_number;
static int sizeOfMessage = 0;
static char message[MAX_LENGTH] = {0};
static int numberOpens = 0;              	///< Counts the number of times the device is opened

static struct class *lkmasg2Class = NULL;	///< The device-driver class struct pointer
static struct device *lkmasg2Device = NULL; ///< The device-driver device struct pointer

/**
 * Prototype functions for file operations.
 */
static int open(struct inode *, struct file *);
static int close(struct inode *, struct file *);
static ssize_t read(struct file *, char *, size_t, loff_t *);
static ssize_t write(struct file *, const char *, size_t, loff_t *);
static void shiftLeft(int);

/**
 * File operations structure and the functions it points to.
 */
static struct file_operations fops =
	{
		.owner = THIS_MODULE,
		.open = open,
		.release = close,
		.read = read,
		.write = write,
};

/**
 * Initializes module at installation
 */
int init_module(void)
{
	printk(KERN_INFO "lkmasg2: !!!!!!!!!!!!!!!!!!! HELLO !!!!!!!!!!!!!!!!!!!\n");

	// Allocate a major number for the device.
	major_number = register_chrdev(0, DEVICE_NAME, &fops);
	if (major_number < 0)
	{
		printk(KERN_ALERT "lkmasg2 could not register number.\n");
		return major_number;
	}
	printk(KERN_INFO "lkmasg2: registered correctly with major number %d\n", major_number);

	// Register the device class
	lkmasg2Class = class_create(THIS_MODULE, CLASS_NAME);
	if (IS_ERR(lkmasg2Class))
	{ // Check for error and clean up if there is
		unregister_chrdev(major_number, DEVICE_NAME);
		printk(KERN_ALERT "Failed to register device class\n");
		return PTR_ERR(lkmasg2Class); // Correct way to return an error on a pointer
	}
	printk(KERN_INFO "lkmasg2: device class registered correctly\n");

	// Register the device driver
	lkmasg2Device = device_create(lkmasg2Class, NULL, MKDEV(major_number, 0), NULL, DEVICE_NAME);
	if (IS_ERR(lkmasg2Device))
	{								 // Clean up if there is an error
		class_destroy(lkmasg2Class); // Repeated code but the alternative is goto statements
		unregister_chrdev(major_number, DEVICE_NAME);
		printk(KERN_ALERT "Failed to create the device\n");
		return PTR_ERR(lkmasg2Device);
	}
	printk(KERN_INFO "lkmasg2: device class created correctly\n"); // Made it! device was initialized

	return 0;
}

/*
 * Removes module, sends appropriate message to kernel
 */
void cleanup_module(void)
{
	printk(KERN_INFO "lkmasg2: removing module.\n");
	device_destroy(lkmasg2Class, MKDEV(major_number, 0)); // remove the device
	class_unregister(lkmasg2Class);						  // unregister the device class
	class_destroy(lkmasg2Class);						  // remove the device class
	unregister_chrdev(major_number, DEVICE_NAME);		  // unregister the major number
	printk(KERN_INFO "lkmasg2: Goodbye from the LKM!\n");
	unregister_chrdev(major_number, DEVICE_NAME);
	return;
}

/*
 * Opens device module, sends appropriate message to kernel
 */
static int open(struct inode *inodep, struct file *filep)
{
	numberOpens++;
	printk(KERN_INFO "lkmasg2: device opened.\n");

	return 0;
}

/*
 * Closes device module, sends appropriate message to kernel
 */
static int close(struct inode *inodep, struct file *filep)
{
	printk(KERN_INFO "lkmasg2: device closed.\n");

	strncpy(message, "", MAX_LENGTH);

	return 0;
}

static void shiftLeft(int start)
{
	int end = sizeOfMessage - start;
	if (end < 0)
		return;
	
	for (int i = 0; i < end; i++)
	{
		message[i] = message[i + start];
	}
	
	message[end] = '\0';

	sizeOfMessage = strlen(message);
}

/*
 * Reads from device, displays in userspace, and deletes the read data
 */
static ssize_t read(struct file *filep, char *buffer, size_t len, loff_t *offset)
{
	int errorCount = 0;
	char tempString[MAX_LENGTH];
	
	printk(KERN_INFO "BEFORE: sizeOfMessage=%d; len=%d\n\n", sizeOfMessage, (int)len);
	if (sizeOfMessage >= len) // 129 > 128
	{
		printk(KERN_INFO "???????%s\n\n", message);
		strcpy(tempString, message);

		errorCount = copy_to_user(buffer, message, len - 1);

		strcpy(message, tempString);
		shiftLeft(len - 1);
	}
	else
	{
		printk(KERN_INFO "!!!!!!!!%s\n\n", message);
		errorCount = copy_to_user(buffer, message, len - 1);
		strcpy(message, "");
		sizeOfMessage = 0;
	}
	printk(KERN_INFO "AFTER: sizeOfMessage=%d; len=%d\n\n", sizeOfMessage, (int)len);
	

	if (errorCount == 0)
	{
		printk(KERN_INFO "lkmasg2: Sent %d characters to the user\n", sizeOfMessage);
		// return (sizeOfMessage = 0);  // clear the position to the start and return 0
		return 0;
	}
	else
	{
		printk(KERN_INFO "lkmasg2: Failed to send %d characters to the user\n", errorCount);	
		return -EFAULT;              // Failed -- return a bad address message (i.e. -14)
	}

	return 0;
}

/*
 * Writes to the device
 */
static ssize_t write(struct file *filep, const char *buffer, size_t len, loff_t *offset)
{
	strncat(message, buffer, MAX_LENGTH - sizeOfMessage - 1);

	sizeOfMessage = strnlen(message, MAX_LENGTH);

	return sizeOfMessage;
}
