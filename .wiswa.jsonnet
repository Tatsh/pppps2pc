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
+ {
  want_scan_build: true,
  github+: { workflows+: {
    scan_build+: {
      scope: 'All Linux HID driver objects under kernel/, without module packaging. '
             + 'The PS2 driver decompilation is not built.',
      build_command: |||-
        make -C "$KERNEL_SOURCE" M="$GITHUB_WORKSPACE/kernel" \
          LLVM=1 CC="$CC" HOSTCC="$CC" HOSTCXX="$CXX" konami_ppp.o
      |||,
      configure_command: 'true',
      documentation_steps: [
        {
          name: 'Build project homepage',
          uses: 'actions/jekyll-build-pages@44a6e6beabd48582f863aeeb6cb2151cc1716697',
          with: { destination: 'build/html' },
        },
      ],
      setup_steps: [
        {
          name: 'Prepare kernel source with Clang',
          run: |||
            sudo apt-get update
            sudo apt-get install -y bc bison clang curl flex libelf-dev libssl-dev llvm lld xz-utils
            kernel_source="$GITHUB_WORKSPACE/.wiswa-ci/scan-build/linux-6.12.115"
            archive="$kernel_source.tar.xz"
            mkdir -p "$kernel_source"
            curl --fail --location --retry 3 \
              https://cdn.kernel.org/pub/linux/kernel/v6.x/linux-6.12.115.tar.xz -o "$archive"
            printf '%s  %s\n' \
              0ec90fe4b20422b1b664b77e6d2dde22b40bd41e7eb2cad8ac8739a4749cb1bb "$archive" \
              | sha256sum --check
            tar -xf "$archive" -C "$kernel_source" --strip-components=1
            make -C "$kernel_source" LLVM=1 defconfig
            make -C "$kernel_source" LLVM=1 --jobs=2 modules_prepare
            echo "KERNEL_SOURCE=$kernel_source" >> "$GITHUB_ENV"
          |||,
        },
      ],
    },
  } },
}
