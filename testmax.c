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
    if (argc != 2)
    {
        printf("Usage: test <path to device>\n");
        exit(0);
    }
    char *devicepath = argv[1];

    int ret, fd;
    char stringOne[BUFFER_LENGTH];
    strncpy(stringOne, "", sizeof(stringOne));

    char str2[BUFFER_LENGTH] = "uG4TcZCVevqP7qAu2XH5Dd24lxM97YsUfoSV2Uy2UsfmVvqTPQsaIXnvlai8GdKH7pmZC35haH12dDcgINzA6XEQo3OUIwQConJXxfGA4dc2Wh7tIWABDT2CudQGmPgs";

    strncat(stringOne, str2, sizeof(stringOne));

    printf("Starting device test max code example...\n");
    fd = open(devicepath, O_RDWR); // Open the device with read/write access
    if (fd < 0)
    {
        perror("Failed to open the device...");
        return errno;
    }
    
    printf("Writing message to the device [%s] twice.\n", stringOne);
    ret = write(fd, stringOne, strlen(stringOne)); // Send the string to the LKM
    if (ret < 0)
    {
        perror("Failed to write the message to the device.");
        return errno;
    }
    ret = write(fd, stringOne, strlen(stringOne)); // Send the string to the LKM
    if (ret < 0)
    {
        perror("Failed to write the message to the device.");
        return errno;
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
