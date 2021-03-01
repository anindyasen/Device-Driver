#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

#define MOD_DRV "/dev/mod_device"

#define WR_VALUE 0x24
#define RD_VALUE 0x25
#define DEL_CMD  0x27

int8_t write_buf[1024];
int8_t read_buf[1024];
int num=0;
int value=0;
int main()
{
	int fd;
	char option;
	printf("*********************************\n");
	printf("*******Device Driver APIs*******\n");

	fd = open(MOD_DRV, O_RDWR);
	if(fd < 0) {
		printf("Cannot open device file...\n");
		return 0;
	}

	while(1) {
		printf("****Please Enter the Option******\n");
		printf("        1. Write               \n");
		printf("        2. Read                 \n");
		printf("        3. IOCTL Read           \n");
		printf("        4. IOCTL Write          \n");
		printf("        5. IOCTL Delete         \n");
		printf("        6. Exit                 \n");
		printf("*********************************\n");
		scanf(" %c", &option);
		printf("Your Option = %c\n", option);

		switch(option) {
			case '1':
				printf("Enter the string to write into driver :");
				scanf("  %[^\t\n]s", write_buf);
				printf("Data Writing ...");
				write(fd, write_buf, strlen(write_buf)+1);
				printf("Done!\n");
				break;
			case '2':
				printf("Data Reading ...");
				read(fd, read_buf, 1024);
				printf("Done!\n\n");
				printf("Data = %s\n\n", read_buf);
				break;
            case '3':
                printf("Reading value from Driver\n");
                ioctl(fd, RD_VALUE, (int32_t*)&value);
                printf("value: 0x%x\n",value);
                break;
            case '4':
                printf("Enter value to send\n");
                scanf("%d",&num);
                ioctl(fd, WR_VALUE, (int32_t*)&num);
                break;
            case '5':
                ioctl(fd, DEL_CMD, (int32_t*)&value);
                printf("Value: 0x%x\n",value);
                break;
			case '6':
				close(fd);
				exit(1);
				break;
			default:
				printf("Enter Valid option = %c\n",option);
				break;
		}
	}
	close(fd);
}
