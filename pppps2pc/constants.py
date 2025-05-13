"""Constants."""
from __future__ import annotations

KONAMI_VENDOR_ID = 0x0507
"""
Konami USB vendor ID.

:meta hide-value:
"""
PPP_PS2_PRODUCT_ID = 0x0011
"""
USB ParaParaParadise controller product ID.

:meta hide-value:
"""
UDEV_RULE_LINE = ('ENV{DEVTYPE}=="usb_device", ACTION=="add", SUBSYSTEM=="usb", '
                  f'ATTR{{idVendor}}=="{KONAMI_VENDOR_ID:04x}", '
                  f'ATTR{{idProduct}}=="{PPP_PS2_PRODUCT_ID:04x}", RUN+="%(ps2para)s enable"')
"""udev rule line for the device.

Has placeholder ``%(ps2para)s`` for the full path to the script.

:meta hide-value:
"""
ENABLE_MAGIC_BYTES = [0x0, 0x3]
"""
Magic bytes to enable the device.

:meta hide-value:
"""
UDEV_RULES_FILENAME = '70-ppp.rules'
"""
udev rules filename.

:meta hide-value:
"""
