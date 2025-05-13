# Linux HID driver for home ParaParaParadise controller

Sends the magic bytes to enable the device on detection. To build, simply run `make`. An `install`
target is included to install to `/lib/modules/$(uname -r)/extra`.

You can manually load the module by running `modprobe konami_ppp.ko` after running `make`.

```shell
make install && modprobe konami-ppp
```
