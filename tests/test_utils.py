from __future__ import annotations

from typing import TYPE_CHECKING

from pppps2pc.constants import ENABLE_MAGIC_BYTES
from pppps2pc.utils import enable_ppp_controller, install_udev_rules

if TYPE_CHECKING:
    from pathlib import Path

    from pytest_mock import MockerFixture


def test_install_udev_rules_creates_file_with_correct_content(mocker: MockerFixture,
                                                              tmp_path: Path) -> None:
    mock_write_text = mocker.patch('pathlib.Path.write_text')
    ps2para_path = '/mock/path/to/ps2para'
    rules_dir = tmp_path / 'rules.d'
    rules_dir.mkdir()
    install_udev_rules(ps2para_path, rules_dir)
    mock_write_text.assert_called_once_with(
        'ENV{DEVTYPE}=="usb_device", ACTION=="add", SUBSYSTEM=="usb", ATTR{idVendor}=="0507", '
        'ATTR{idProduct}=="0011", RUN+="/mock/path/to/ps2para enable"\n')


def test_install_udev_rules_uses_default_directory(mocker: MockerFixture) -> None:
    mock_generate_udev_rule = mocker.patch('pppps2pc.utils.generate_udev_rule',
                                           return_value='mock_rule_line')
    mock_write_text = mocker.patch('pathlib.Path.write_text')
    ps2para_path = '/mock/path/to/ps2para'
    install_udev_rules(ps2para_path)
    mock_generate_udev_rule.assert_called_once_with(ps2para_path)
    mock_write_text.assert_called_once_with('mock_rule_line\n')


def test_enable_ppp_controller_writes_enable_magic_bytes(mocker: MockerFixture) -> None:
    mock_hid_device = mocker.patch('pppps2pc.utils.hid.device')
    mock_write = mock_hid_device.return_value.write
    enable_ppp_controller()
    mock_hid_device.assert_called_once()
    mock_write.assert_called_once_with(ENABLE_MAGIC_BYTES)
