xp-pen_driver
===

This is XP-Pen Pentablet driver on Ubuntu.

# 1. Environment

Ubuntu 18.04


# 2. Device data

## 2.1. USB device data

| model     | VendorID | ProductID | InterfaceNumber | EndpointAddress | wMaxPacketSize |
| :-------- | -------: | --------: | --------------: |---------------: | -------------: |
| Artist 12 |   0x28bd |    0x080a |               0 |            0x81 |         0x000a |
|           |          |           |               1 |            0x82 |         0x0008 |
|           |          |           |               2 |            0x83 |         0x000a |

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

| Byte  |    0 | 1   | 2   | 3   |
| :---- | ---: | :-- | :-- | :-- |
| Value | 0x03 | Key | Key | Key |

**Satus**  

| Bit    |    7 |                      6 |  5 |                          4 |  3 |  2 |                     1 |                             0 |
| :----- | ---: | ---------------------: | -: | -------------------------: | -: | -: | --------------------: | ----------------------------: |
| Assign |    1 | In Enable Area(True=1) |  0 | Out of Enable Area(True=1) |  0 |  0 | Button Stylus(True=1) | Button Touch(Contact)(True=1) |

Pen data interrupts and overwrites Express key data.

&nbsp;

# 3. Dependency

libusb-1.0-0


# 4. Analyze serial input data

## 4.1. Dependency

``` sh
$ sudo apt-get install libusb-1.0-0-dev

```

## 4.2. Compile

``` sh
$ make test
```

## 4.3. How to test

``` sh
$ sudo ./test
loop start (Ctrl+C to exit)
07 90 30 1d d0 2b 00 00 
07 90 30 1d d0 2b 00 00 
07 90 30 1d d0 2b 00 00 
...
```

If data-values are not displayed, Fix ``INTF_NUM``, ``EP_ADDR``, ``MAX_PACKET_SIZE`` at test.c.  
See lsusb command.  

&nbsp;

That's all.  
