local utils = import 'utils.libjsonnet';

{
  uses_user_defaults: true,
  description: 'Extremely simple tool to enable the ParaParaParadise PS2 controller on a PC.',
  keywords: ['controller', 'para para paradise', 'peripherals', 'ps2', 'udev', 'usb', 'utilities'],
  security_policy_supported_versions: { '0.1.x': ':white_check_mark:' },
  project_name: 'pppps2pc',
  version: '0.1.3',
  want_main: true,
  want_flatpak: true,
  publishing+: { flathub: 'sh.tat.pppps2pc' },
  pyproject+: {
    project+: {
      scripts: {
        ps2para: 'pppps2pc.main:main',
      },
    },
    tool+: {
      poetry+: {
        dependencies+: {
          hidapi: utils.latestPypiPackageVersionCaret('hidapi'),
        },
        group+: {
          dev+: {
            dependencies+: {
              // 'types-hidapi': '',
            },
          },
        },
      },
    },
  },
  local common_apt_packages = ['libudev-dev', 'libusb-1.0-0-dev'],
  github+: {
    workflows+: {
      qa+: {
        apt_packages: common_apt_packages,
      },
      tests+: {
        apt_packages: common_apt_packages,
      },
    },
  },
  gitignore+: [
    'kernel/*.cmd',
    'kernel/*.ko',
    'kernel/*.mod*',
    'kernel/*.o',
    'kernel/Module.symvers',
    'kernel/modules.order',
  ],
  prettierignore+: [
    'kernel/',
    'ps2-decompile/',
  ],
  snapcraft+: {
    apps+: {
      pppps2pc+: {
        command: 'bin/ps2para',
      },
    },
  },
  flatpak+: { command: 'ps2para' },
}
