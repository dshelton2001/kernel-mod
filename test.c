/**
 * @file   test.c
 * @author Derek Molloy
 * @date   7 April 2015
 * @version 0.1
 * @brief  A Linux user space program that communicates with the charkmod.c LKM. It passes a
 * string to the LKM and reads the response from the LKM. For this example to work the device
 * must be called /dev/charkmod.
 * @see http://www.derekmolloy.ie/ for a full description and follow-up descriptions.
 *
 * Adapted for COP 4600 by Dr. John Aedo
 */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_LENGTH 256           ///< The buffer length (crude but fine)
static char receive[BUFFER_LENGTH]; ///< The receive buffer from the LKM

int main(int argc, char *argv[])
{
    if (argc != 2 && argc != 3)
    {
        printf("Usage: test <path to device> <string to test with>\n");
        exit(0);
    }
    char *devicepath = argv[1];

    int ret, fd, fc;
    char stringToSend[BUFFER_LENGTH];
    strncpy(stringToSend, "", sizeof(stringToSend));

    printf("Starting device test code example...\n");
    fd = open(devicepath, O_RDWR); // Open the device with read/write access
    if (fd < 0)
    {
        perror("Failed to open the device...");
        return errno;
    }

    if (argc == 2)
    {
        printf("Type in a short string to send to the kernel module:\n");
        scanf("%[^\n]%*c", stringToSend); // Read in a string (with spaces)
    }
    else
    {
        strcpy(stringToSend, argv[2]);
    }
    
    printf("Writing message to the device [%s].\n", stringToSend);
    ret = write(fd, stringToSend, strlen(stringToSend)); // Send the string to the LKM
    if (ret < 0)
    {
        perror("Failed to write the message to the device.");
        return errno;
    }

    if(argc == 2)
    {
        printf("Press ENTER to read back from the device...\n");
        getchar();
    }

    printf("Reading from the device...\n");
    ret = read(fd, receive, BUFFER_LENGTH); // Read the response from the LKM
    if (ret < 0)
    {
        perror("Failed to read the message from the device.");
        return errno;
    }
    printf("The received message is: [%s]\n", receive);

    printf("End of the program\n");
    return 0;
}
