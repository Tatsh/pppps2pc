from typing import Collection

class Device:
    def open(self, ven: int, prod: int) -> None:
        ...

    def write(self, b: bytes | Collection[int]) -> int:
        ...


def device() -> Device:
    ...
