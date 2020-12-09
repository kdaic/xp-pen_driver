#include <stdio.h>
#include <stdlib.h>
#include <libusb.h>
#include <signal.h>

#define VID 0x28bd
#define PID 0x080a

/* corresponds to bInterfaceNumber ($ lsusb -d PID:VID -v) */
/* 0 or 1 or 2*/
#define INTF_NUM 1
/* corresponds to bEndpointAddress ($ lsusb -d PID:VID -v) */
/* 0x82(INTF_NUM=0) or 0x81(INTF_NUM=1) or 0x83(INTF_NUM=2) */
#define EP_ADDR 0x81
/* corresponds to wMaxPacketSize ($ lsusb -d PID:VID -v) */
/* 0x000a(EP_ADDR=0x81,0x83) or 0x0008(EP_ADDR=0x82) */
#define MAX_PACKET_SIZE 0x000a

#define TRUE 1

int Flag = 1;

void signal_handler(int signum)
{
	Flag = 0;
}

int main()
{
	uint8_t buffer[MAX_PACKET_SIZE];
	int transferred;
	int r;

	r = libusb_init(NULL);
	if (r < 0) {
		printf("libusb_init() failed: %s\n", libusb_error_name(r));
		return 0;
	}

	libusb_device_handle *handle = libusb_open_device_with_vid_pid(NULL, VID, PID);
	if (handle == NULL) {
		printf("libusb_open_device_with_vid_pid() failed: %s\n", libusb_error_name(r));
    /* exit */
    libusb_exit(NULL);
	}

	r = libusb_set_auto_detach_kernel_driver(handle, TRUE);
	if (r != LIBUSB_SUCCESS) {
		printf("libusb_set_auto_detach_kernel_driver() failed: %s\n", libusb_error_name(r));
    /* exit */
    libusb_exit(NULL);
	}

	libusb_device *device = libusb_get_device(handle);

	r = libusb_claim_interface(handle, INTF_NUM);
	if (r != 0) {
		printf("libusb_claim_interface() failed: %s\n", libusb_error_name(r));
    /* close */
    libusb_close(handle);
	}

	if(signal(SIGINT, signal_handler) == SIG_ERR) {
		printf("singal() Failed\n");
    /* close */
    libusb_close(handle);
	}

	printf("loop start (Ctrl+C to exit)\n");

	while (Flag) {
		r = libusb_interrupt_transfer(handle, EP_ADDR, buffer, sizeof(buffer), &transferred, 100);
		switch (r) {
			case 0:
				for (int i = 0; i < transferred; i++)
					printf("%02x ", buffer[i]);
				printf("\n");
				break;
			case LIBUSB_ERROR_TIMEOUT:
				break;
			default:
				printf("libusb_interrupt_transfer() failed: %s\n", libusb_error_name(r));
				/* release; */
        libusb_release_interface(handle, INTF_NUM);
				break;
		}
	}

	return 0;
}
