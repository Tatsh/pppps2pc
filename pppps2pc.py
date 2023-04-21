import sys

import hid


def main() -> int:
    dev = hid.device()
    try:
        dev.open(0x0507, 0x0011)
    except OSError:
        return 1
    dev.write([0, 3])
    return 0


if __name__ == '__main__':
    sys.exit(main())
