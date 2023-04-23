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

static int ppp_send_output_report(struct hid_device *hdev)
{
	u8 buf[] = { 0, 3 };
	const size_t buf_size = sizeof(buf);
	if (hid_hw_raw_request(hdev, 0, buf, buf_size, HID_OUTPUT_REPORT,
			       HID_REQ_SET_REPORT) != buf_size)
		return 1;
	return 0;
}

#ifdef CONFIG_PM
static int ppp_resume(struct hid_device *hdev)
{
	return ppp_send_output_report(hdev);
}
#endif

static int ppp_input_configured(struct hid_device *hdev,
				struct hid_input *hidinput)
{
	return ppp_send_output_report(hdev);
}

static const struct hid_device_id ppp_devices[] = { { HID_USB_DEVICE(0x0507,
								     0x0011) },
						    {} };

MODULE_DEVICE_TABLE(hid, ppp_devices);

static struct hid_driver ppp_driver = {
	.name = "konami-ppp",
	.id_table = ppp_devices,
	.input_configured = ppp_input_configured,
#ifdef CONFIG_PM
	.resume = ppp_resume,
	.reset_resume = ppp_resume,
#endif
};

module_hid_driver(ppp_driver);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Andrew Udvare");
MODULE_DESCRIPTION("HID driver for the home ParaParaParadise controller");
