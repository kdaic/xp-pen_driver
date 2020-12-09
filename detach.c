#include <stdio.h>
#include <stdlib.h>
#include <libusb.h>

#define VID 0x28bd
#define PID 0x080a
#define INTF_NUM 1

int main()
{
	int r;

	r = libusb_init(NULL);
	if (r < 0) {
		printf("libusb_init() failed: %s\n", libusb_error_name(r));
    return 0;
	}

	libusb_device_handle *handle = libusb_open_device_with_vid_pid(NULL, VID, PID);
	if (handle == NULL) {
		printf("libusb_open_device_with_vid_pid() failed: %s\n", libusb_error_name(r));
		/* exit; */
    libusb_exit(NULL);
    return 0;
	}

	r = libusb_detach_kernel_driver(handle, INTF_NUM);
	if (r != 0) {
		printf("libusb_detach_kernel_driver() failed: %s\n", libusb_error_name(r));
    /* close */
    libusb_close(handle);
	}

	printf("detached %04x:%04x %d\n", VID, PID, INTF_NUM);
	return 0;
}
