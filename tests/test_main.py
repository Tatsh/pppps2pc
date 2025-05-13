from __future__ import annotations

from typing import TYPE_CHECKING
import sys

from pppps2pc.main import main

if TYPE_CHECKING:
    from pathlib import Path

    from click.testing import CliRunner
    from pytest_mock import MockerFixture


def test_enable_success(runner: CliRunner, mocker: MockerFixture) -> None:
    """Test enable command success."""
    mock_enable_ppp_controller = mocker.patch('pppps2pc.main.enable_ppp_controller')
    result = runner.invoke(main, ['enable'])
    assert result.exit_code == 0
    mock_enable_ppp_controller.assert_called_once()


def test_enable_no_device_found(runner: CliRunner, mocker: MockerFixture) -> None:
    """Test enable command when no device is found."""
    mocker.patch('pppps2pc.main.enable_ppp_controller', side_effect=OSError)
    result = runner.invoke(main, ['enable'])
    assert result.exit_code == 1


def test_install_udev_rules_success(runner: CliRunner, mocker: MockerFixture,
                                    tmp_path: Path) -> None:
    """Test install-udev-rules command success."""
    mock_install_udev_rules = mocker.patch('pppps2pc.main.install_udev_rules')
    rules_dir = tmp_path / 'rules.d'
    rules_dir.mkdir()
    result = runner.invoke(main, ['install-udev-rules', '--rules-dir', str(rules_dir)])
    assert result.exit_code == 0
    mock_install_udev_rules.assert_called_once_with(sys.argv[0], rules_dir)


def test_install_udev_rules_permission_error(runner: CliRunner, mocker: MockerFixture,
                                             tmp_path: Path) -> None:
    """Test install-udev-rules command when permission error occurs."""
    mocker.patch('pppps2pc.main.install_udev_rules', side_effect=PermissionError)
    rules_dir = tmp_path / 'rules.d'
    rules_dir.mkdir()
    result = runner.invoke(main, ['install-udev-rules', '--rules-dir', str(rules_dir)])
    assert result.exit_code == 1
    assert 'Run this command with higher permissions such as root or use sudo.' in result.output


def test_install_udev_rules_invalid_path(runner: CliRunner, mocker: MockerFixture) -> None:
    """Test install-udev-rules command with invalid path."""
    mock_install_udev_rules = mocker.patch('pppps2pc.main.install_udev_rules')
    invalid_path = '/invalid/path'
    result = runner.invoke(main, ['install-udev-rules', '--rules-dir', invalid_path])
    assert result.exit_code != 0
    mock_install_udev_rules.assert_not_called()
