"""Utilities."""
from __future__ import annotations

from pathlib import Path
import logging

import hid  # type: ignore[import-not-found]

from .constants import ENABLE_MAGIC_BYTES, UDEV_RULES_FILENAME, UDEV_RULE_LINE

log = logging.getLogger(__name__)


def enable_ppp_controller() -> None:
    """Enable a PS2 ParaParaParadise controller."""
    log.debug('Enabling device.')
    hid.device().write(ENABLE_MAGIC_BYTES)


def generate_udev_rule(ps2para: Path | str) -> str:
    """
    Generate udev rule for the device.

    Parameters
    ----------
    ps2para : Path | str
        Path to the script to run when the device is added.

    Returns
    -------
    str
        udev rule line for the device.
    """
    log.debug('Using ps2para at path: %s', ps2para)
    return UDEV_RULE_LINE % {'ps2para': ps2para}


def install_udev_rules(ps2para: Path | str, rules_dir: str | Path | None = None) -> None:
    """
    Install udev rules for the device.

    Parameters
    ----------
    ps2para : Path | str
        Path to the script to run when the device is added.
    rules_dir : Path | None
        Path to udev rules directory.
    """
    target = (Path(rules_dir) if rules_dir else Path('/etc/udev/rules.d')) / UDEV_RULES_FILENAME
    udev_rule_line = generate_udev_rule(ps2para)
    log.debug('Writing udev rule line: `%s` in file `%s`.', udev_rule_line, target)
    target.write_text(f'{udev_rule_line}\n')
    log.info('Installed udev rules to %s.', target)
