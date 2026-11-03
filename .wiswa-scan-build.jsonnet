{
  scope: 'Linux HID driver under kernel/. The PS2 driver decompilation is not built.',
  build_command: 'make -C "$KERNEL_HEADERS" M="$GITHUB_WORKSPACE/kernel" modules',
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
      name: 'Install kernel headers',
      run: |||
        sudo apt-get update
        sudo apt-get install -y linux-headers-generic
        headers=$(find /usr/src -maxdepth 1 -type d -name 'linux-headers-*-generic' | sort -V | tail -1)
        test -n "$headers"
        echo "KERNEL_HEADERS=$headers" >> "$GITHUB_ENV"
      |||,
    },
  ],
}
