# About

This is an incomplete non-matching decompilation of the `DRIVER/USBKB.IRX` file found on the
[_ParaParaParadise_](http://redump.org/disc/29432/) disc for PlayStation 2. With minor modifications
this should be able to build with [PS2SDK](https://github.com/ps2dev/ps2sdk).

Because this is not expected to be a matching compilation, I will accept PRs that refactor, as long
as the behaviour (including useless behaviour) is maintained.

## printf

Some `printf` calls reference an offset from `$gp` for the format string. These should be replaced
with a literal format string. These are the candidates from the binary:

```plain
%02x
%c
%s%d
0x%02x
```
