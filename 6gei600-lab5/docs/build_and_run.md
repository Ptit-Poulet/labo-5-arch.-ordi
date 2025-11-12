# Build and Run

## Prerequisites
- AArch64 bare‑metal toolchain in PATH (e.g., `aarch64-elf-gcc`)
- Raspberry Pi 3 Model B and SD card
- Pi firmware files on the SD card boot partition

## Build
```bash
make
```
Outputs:
- `kernel8.elf`: ELF image (for debugging)
- `kernel8.img`: raw binary used by Pi firmware

`Makefile` highlights:
- Cross tools configurable via `CROSS` (default `aarch64-elf-`)
- Uses `-ffreestanding -nostdlib -nostartfiles` for bare‑metal
- Links with `linker.ld`

## Wiring (LED traffic light example)
- Ground: GND (pin 9)
- Red LED: GPIO 17 (pin 11)
- Yellow LED: GPIO 27 (pin 13)
- Green LED: GPIO 22 (pin 15)

Only the red LED (GPIO 17) is used in this starter.

## SD card setup
Create or edit `config.txt` on the boot partition:

```1:4:./Lab5/config.txt
arm_64bit=1
kernel=kernel8.img
```

Copy `kernel8.img` to the boot partition alongside the Pi firmware files (`bootcode.bin`, `start.elf`, etc.).

## Boot
- Insert SD card and power on the Pi
- The LED on GPIO 17 should toggle at ~0.5 Hz (every 100 ticks of 10 ms)

## Troubleshooting
If nothing happens, see `docs/troubleshooting.md`.

