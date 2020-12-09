xp-pen_driver
===

This is XP-PEN Pentablet driver for Ubuntu Linux.

# 1. Environment

## 1.1. Target model of Pentablet

Artist 12

## 1.2. Target OS

Ubuntu 18.04

&nbsp;

# 2. Device data

## 2.1. USB device data

| model     | VendorID | ProductID | InterfaceNumber | EndpointAddress | wMaxPacketSize |
| :-------- | -------: | --------: | --------------: |---------------: | -------------: |
| Artist 12 |   0x28bd |    0x080a |               0 |            0x82 |         0x0008 |
|           |          |           |               1 |            0x81 |         0x000a |
|           |          |           |               2 |            0x83 |         0x000a |

From [analyzing usb data (Pen data & Express key data)](#4-analyze-pen-data),  
The signal of EndpointAddress 0x83 corresponds to Pen data & Express key data,  

Pen data has priority over Express key data.  

## 2.2. Pen data

| Byte  |    0 |      1 |    2 |    3 |    4 |    5 |              6 |              7 |
| :---- | ---: | -----: | ---: | ---: | ---: | ---: | -------------: | -------------: |
| Value | 0x07 | Status | X    | X    | Y    | Y    | pressure level | pressure level |

**Satus**  

| Bit    |    7 |                      6 |  5 |                          4 |  3 |  2 |                     1 |                             0 |
| :----- | ---: | ---------------------: | -: | -------------------------: | -: | -: | --------------------: | ----------------------------: |
| Assign |    1 | In Enable Area(True=1) |  0 | Out of Enable Area(True=1) |  0 |  0 | Button Stylus(True=1) | Button Touch(Contact)(True=1) |


## 2.3. Express key (Tablet button on the left side) data

| Byte  |    0 | 1          | 2          | 3          |
| :---- | ---: | :--------- | :--------- | :--------- |
| Value | 0x03 | Key-Status | Key-Status | Key-Status |

**Key-Satus**  

| Bit    | 7      | 6      | 5      | 4      | 3      | 2      | 1      | 0      |
| :---   | ---:   | ---:   | ---:   | ---:   | ---:   | ---:   | ---:   | ---:   |
| Assign | T.B.D. | T.B.D. | T.B.D. | T.B.D. | T.B.D. | T.B.D. | T.B.D. | T.B.D. |

Pen data interrupts and overwrites Express key data.

&nbsp;

# 3. Dependency of driver

libusb-1.0-0

&nbsp;

# 4. Analyze USB data of Pentablet

## 4.1. Dependency for test

libusb-1.0-0-dev package is needed.

```
$ sudo apt-get install libusb-1.0-0-dev

```

## 4.2. Make

```
$ make test
```

## 4.3. How to test

```
$ sudo ./test
loop start (Ctrl+C to exit)
07 90 30 1d d0 2b 00 00 
07 90 30 1d d0 2b 00 00 
07 90 30 1d d0 2b 00 00 
...
```

If data-values are not displayed,  
Fix ``INTF_NUM``, ``EP_ADDR``, ``MAX_PACKET_SIZE`` in test.c.  
Also See command  ``lsusb`` & ``lsusb -d 28bd:080a -v`` result.  

&nbsp;

# 5. Make & Install driver

## 5.1. Make

```
$ make
```

make ``pentab.ko``, ``detach.ko``.

## 5.2. Install

register pentab.ko into kernel.

```
$ sudo make install
```

Try ``dmesg`` and check whether the pentablet is recognized as usb device.

## 5.3. Uninstall

```
$ sudo make uninstall
```

pentab.ko is removed from kernel.

&nbsp;

# 6. Make Claen

```
$ make clean
```
pentab.ko, detach, test are removed.

&nbsp;

That's all.  
