#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/usb.h>
#include <linux/usb/input.h>
#include <asm/unaligned.h>
#include <linux/limits.h>

static void report_handler(u8 *buf, struct input_dev *input)
{
  switch (buf[0]) {
  case 0x03:
    input_report_key(input, BTN_0, buf[1] & 0x01);
    input_report_key(input, BTN_1, buf[1] & 0x02);
    input_report_key(input, BTN_2, buf[1] & 0x04);
    input_report_key(input, BTN_3, buf[1] & 0x08);
    input_report_key(input, BTN_4, buf[1] & 0x10);
    input_report_key(input, BTN_5, buf[1] & 0x20);
    input_report_key(input, BTN_6, buf[1] & 0x40);
    input_report_key(input, BTN_7, buf[1] & 0x80);
    input_sync(input);
    break;
  case 0x07:
    input_report_key(input, BTN_TOOL_PEN, (~buf[1]) >> 6);
    input_report_key(input, BTN_TOUCH,    buf[1] & 0x01);
    input_report_key(input, BTN_STYLUS,   buf[1] & 0x02);
    input_report_abs(input, ABS_X, get_unaligned_le16(buf + 2));
    input_report_abs(input, ABS_Y, get_unaligned_le16(buf + 4));
    input_report_abs(input, ABS_PRESSURE, get_unaligned_le16(buf + 6));
    input_sync(input);
    break;
  }
}

static void urb_complete(struct urb *urb)
{
  switch (urb->status) {
  case 0:
    report_handler(urb->transfer_buffer, urb->context);
    usb_submit_urb(urb, GFP_ATOMIC);
    return;
  case -ECONNRESET:
  case -ENOENT:
  case -ESHUTDOWN:
    printk("[pentab] urb shutting down with %d\n", urb->status);
    return;
  default:
    printk("[pentab] urb status %d received\n", urb->status);
    usb_submit_urb(urb, GFP_ATOMIC);
    return;
  }
}

static int input_open(struct input_dev *input)
{
  struct urb *urb = input_get_drvdata(input);

  return usb_submit_urb(urb, GFP_KERNEL);
}

static void input_close(struct input_dev *input)
{
  struct urb *urb = input_get_drvdata(input);

  usb_kill_urb(urb);
}

static void setup_input_dev(struct input_dev *input, struct usb_interface *intf, char *phys)
{
  input->name = "Artist 12";
  input->phys = phys;
  input->open = input_open;
  input->close = input_close;
  input->dev.parent = &intf->dev;
  usb_to_input_id(interface_to_usbdev(intf), &input->id);

  input_set_capability(input, EV_KEY, BTN_TOOL_PEN);
  input_set_capability(input, EV_KEY, BTN_TOUCH);
  input_set_capability(input, EV_KEY, BTN_STYLUS);
  input_set_capability(input, EV_KEY, BTN_STYLUS2);
  input_set_capability(input, EV_KEY, BTN_0);
  input_set_capability(input, EV_KEY, BTN_1);
  input_set_capability(input, EV_KEY, BTN_2);
  input_set_capability(input, EV_KEY, BTN_3);
  input_set_capability(input, EV_KEY, BTN_4);
  input_set_capability(input, EV_KEY, BTN_5);
  input_set_capability(input, EV_KEY, BTN_6);
  input_set_capability(input, EV_KEY, BTN_7);
  input_set_abs_params(input, ABS_X, 0, 25400, 0, 0);
  input_set_abs_params(input, ABS_Y, 0, 15875, 0, 0);
  input_set_abs_params(input, ABS_PRESSURE, 0, 8192, 0, 0);
}

static int dev_probe(struct usb_interface *intf, const struct usb_device_id *id)
{
  struct usb_device *dev = interface_to_usbdev(intf);
  struct usb_endpoint_descriptor *epd = &intf->cur_altsetting->endpoint[0].desc;
  unsigned int pipe = usb_rcvintpipe(dev, epd->bEndpointAddress);
  struct urb *urb;
  void *buf;
  dma_addr_t dma;
  char *phys;
  struct input_dev *input;
  int res;

  printk("[pentab] probe\n");
  printk("[pentab]  idVendor=0x%04x\n", dev->descriptor.idVendor);
  printk("[pentab]  idProduct=0x%04x\n", dev->descriptor.idProduct);
  printk("[pentab]  bInterfaceNumber=%d\n", intf->cur_altsetting->desc.bInterfaceNumber);
  printk("[pentab]  bEndpointAddress=0x%02x\n", epd->bEndpointAddress);
  printk("[pentab]  wMaxPacketSize=%d\n", epd->wMaxPacketSize);
  printk("[pentab]  bInternal=%d\n", epd->bInterval);

  urb = usb_alloc_urb(0, GFP_KERNEL);
  if (urb == NULL) {
    printk("[pentab] usb_alloc_urb() failed\n");
    res = -ENOMEM;
    /* failed0 */
    kfree(phys);
    return res;
  }

  usb_set_intfdata(intf, urb);

  buf = usb_alloc_coherent(dev, epd->wMaxPacketSize, GFP_KERNEL, &dma);
  if (buf == NULL) {
    printk("[pentab] usb_alloc_coherent() failed\n");
    res = -ENOMEM;
    /* failed1 */
    usb_free_urb(urb);
    return res;
  }

  usb_fill_int_urb(urb, dev, pipe, buf, epd->wMaxPacketSize, urb_complete, NULL, epd->bInterval);
  urb->transfer_dma = dma;
  urb->transfer_flags |= URB_NO_TRANSFER_DMA_MAP;

  input = devm_input_allocate_device(&intf->dev);
  if (input == NULL) {
    printk("[pentab] devm_input_allocate_device() failed.\n");
    res = -ENOMEM;
    /* failed2 */
    usb_free_coherent(dev, epd->wMaxPacketSize, buf, dma);
    return res;
  }

  input_set_drvdata(input, urb);

  phys = kmalloc(PATH_MAX, GFP_KERNEL);
  if (phys == NULL) {
    printk("[pentab] kmalloc() failed\n");
    res = -ENOMEM;
    /* failed2 */
    usb_free_coherent(dev, epd->wMaxPacketSize, buf, dma);
    return res;
  }

  usb_make_path(dev, phys, PATH_MAX);
  strlcat(phys, "/input0", PATH_MAX);

  setup_input_dev(input, intf, phys);

  urb->context = input;

  res = input_register_device(input);
  if (res) {
    printk("[pentab] input_register_device() failed\n");
    /* failed3 */
    kfree(phys);
  }

  return res;
}

static void dev_disconnect(struct usb_interface *intf)
{
  struct urb *urb;
  struct input_dev *input;

  printk("[pentab] disconnect\n");

  urb = usb_get_intfdata(intf);

  if (urb) {
    usb_kill_urb(urb);
    usb_free_coherent(
      urb->dev,
      urb->transfer_buffer_length,
      urb->transfer_buffer,
      urb->transfer_dma);
    usb_free_urb(urb);
    input = urb->context;
    kfree(input->phys);
  }
}

static const struct usb_device_id devices[] =
{
  {
    .idVendor = 0x28bd,
    .idProduct = 0x000a,
    .bInterfaceNumber = 1,
    .match_flags =
      USB_DEVICE_ID_MATCH_VENDOR |
      USB_DEVICE_ID_MATCH_PRODUCT |
      USB_DEVICE_ID_MATCH_INT_NUMBER,
  },
  {}
};

static struct usb_driver driver =
{
  .name = "pentab",
  .id_table = devices,
  .probe = dev_probe,
  .disconnect = dev_disconnect,
};

static int __init mod_init(void)
{
  int ret;

  printk("[pentab] init\n");

  ret = usb_register(&driver);
  if (ret) {
    printk("[pentab] usb_resiger() failed\n");
    return ret;
  }

  return 0;
}

static void __exit mod_exit(void)
{
  printk("[pentab] exit\n");

  usb_deregister(&driver);
}

module_init(mod_init);
module_exit(mod_exit);

MODULE_LICENSE("GPL");
MODULE_DEVICE_TABLE(usb, devices);
