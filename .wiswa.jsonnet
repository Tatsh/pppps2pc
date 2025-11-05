local utils = import 'utils.libjsonnet';

{
  description: 'Extremely simple tool to enable the ParaParaParadise PS2 controller on a PC.',
  keywords: ['controller', 'para para paradise', 'peripherals', 'ps2', 'udev', 'usb', 'utilities'],
  project_name: 'pppps2pc',
  version: '0.1.1',
  want_main: true,
  copilot: {
    intro: 'pppps2pc is a simple tool to enable the ParaParaParadise PS2 controller on a PC.',
  },
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
}
