"""Main script."""
from __future__ import annotations

from pathlib import Path
import logging
import sys

from bascom import setup_logging
import click

from .utils import enable_ppp_controller, install_udev_rules

__all__ = ('main',)

log = logging.getLogger(__name__)


@click.group(context_settings={'help_option_names': ('-h', '--help')})
def main() -> None:
    """PS2 ParaParaParadise controller tool."""


@click.command(context_settings={'help_option_names': ('-h', '--help')})
@click.option('-d', '--debug', help='Enable debug level logging.', is_flag=True)
def enable(*, debug: bool = False) -> None:
    """Enable a PS2 ParaParaParadise controller."""  # noqa: DOC501
    setup_logging(debug=debug,
                  loggers={
                      'hidapi': {
                          'handlers': ('console',),
                          'propagate': False,
                      },
                      'pppps2pc': {
                          'handlers': ('console',),
                          'propagate': False,
                      },
                  })
    try:
        enable_ppp_controller()
    except OSError as e:
        click.echo('No device found or error enabling. Make sure the device is connected.',
                   err=True)
        raise click.Abort from e


@click.command(context_settings={'help_option_names': ('-h', '--help')})
@click.option('-d', '--debug', help='Enable debug level logging.', is_flag=True)
@click.option('-p', '--path', help='Path to ps2para. Not checked for validaity.')
@click.option('--rules-dir',
              help='Path to udev rules directory.',
              default='/etc/udev/rules.d',
              type=click.Path(exists=True, file_okay=False, resolve_path=True, path_type=Path))
def install_udev_rules_main(rules_dir: Path,
                            path: str | None = None,
                            *,
                            debug: bool = False) -> None:
    """Install udev rules for the device."""  # noqa: DOC501
    setup_logging(debug=debug,
                  loggers={
                      'pppps2pc': {
                          'handlers': ('console',),
                          'propagate': False,
                      },
                  })
    try:
        install_udev_rules(path or sys.argv[0], rules_dir)
    except PermissionError as e:
        click.echo('Run this command with higher permissions such as root or use sudo.', err=True)
        raise click.Abort from e


main.add_command(enable)
main.add_command(install_udev_rules_main, 'install-udev-rules')
