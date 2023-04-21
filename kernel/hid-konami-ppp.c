/*
 * HID driver for home ParaParaParadise controller
 *
 * Supported devices:
 *  - ParaParaParadise controller for PlayStation 2
 *
 * Copyright (c) 2023 Andrew Udvare
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#include <linux/hid.h>
#include <linux/module.h>

#define VENDOR_HOSIDEN 0x0507
#define DEV_KONAMI_PPP 0x0011

static int ppp_input_configured(struct hid_device *hdev,
				struct hid_input *hidinput)
{
	int ret = 0;
	const int buf_size = 2;
	u8 *buf = kmalloc(buf_size, GFP_KERNEL);
	buf[0] = 0;
	buf[1] = 3;
	ret = hid_hw_raw_request(hdev, 0, buf, buf_size, HID_OUTPUT_REPORT,
				 HID_REQ_SET_REPORT);
	kfree(buf);
	return ret != 2 ? 1 : 0;
}

static const struct hid_device_id ppp_devices[] = {
	{ HID_USB_DEVICE(VENDOR_HOSIDEN, DEV_KONAMI_PPP) },
	{}
};

MODULE_DEVICE_TABLE(hid, ppp_devices);

static struct hid_driver ppp_driver = { .name = "konami-ppp",
					.id_table = ppp_devices,
					.input_configured =
						ppp_input_configured };

module_hid_driver(ppp_driver);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Andrew Udvare");
MODULE_DESCRIPTION("HID driver for the home ParaParaParadise controller");
