# Raspberry Pi 3B Traffic Light (Bare-metal AArch64)

Minimal bare-metal C project for Raspberry Pi 3 Model B using the system timer interrupt to drive a traffic light (red/yellow/green LEDs).

## Wiring
- Ground: GND (pin 9)
- Red LED: GPIO 17 (pin 11)
- Yellow LED: GPIO 27 (pin 3)
- Green LED: GPIO 22 (pin 15)

## Installation (Docker)

Build and run the project in a Docker container with all dependencies pre-installed.

### Build the Docker image

From the project root directory:

```bash
docker build -t lab5-baremetal .
```

### Build the project using Docker

Run the container and build `kernel8.img`:

```bash
docker run --rm -v "$(pwd):/workspace" -w /workspace lab5-baremetal make CROSS=aarch64-linux-gnu-
```

This command:
- `--rm`: Automatically removes the container after it exits
- `-v "$(pwd):/workspace"`: Mounts the current directory into `/workspace` in the container
- `-w /workspace`: Sets the working directory to `/workspace`
- `CROSS=aarch64-linux-gnu-`: Overrides the Makefile's default `aarch64-elf-` prefix to match the toolchain in the Docker image

The `kernel8.img` file will be created in your current directory and will be accessible outside the container.

### Clean build artifacts

To remove all compiled files and start fresh:

```bash
docker run --rm -v "$(pwd):/workspace" -w /workspace lab5-baremetal make clean
```

This removes all `.o` object files, `.elf` and `.img` files.

## Run

### Testing in QEMU (with UART Debug Output)

Run the kernel in QEMU to see GPIO state changes via UART serial console:

```bash
docker run --rm -it -v "$(pwd)/kernel8.img:/kernel8.img:ro" lab5-baremetal qemu-system-aarch64 -M raspi3b -m 1024 -kernel /kernel8.img -serial stdio -monitor none -nographic -append "console=serial0,115200"
```

This command:
- `-M raspi3b`: Emulates Raspberry Pi 3B
- `-serial stdio`: Connects UART0 output to your terminal
- `-nographic`: Disables graphical window, shows only serial console
- `-append "console=serial0,115200"`: Configures console parameters

**Expected output:**
```
[GPIO] Initializing pin 17
[GPIO] Set pin 17 as OUTPUT
[GPIO] Pin 17 -> LOW
[GPIO] Pin 17 -> HIGH
[GPIO] Pin 17 -> LOW
...
```

Each GPIO state change will be printed to the console. Since QEMU doesn't fully emulate GPIO hardware, UART provides the only way to observe program behavior.

Press `Ctrl+A` then `X` to exit QEMU, or `Ctrl+C` to stop the container.

### On Real Hardware

On SD card boot partition:
- Add to `config.txt`:
  ```
  arm_64bit=1
  kernel=kernel8.img
  ```
- Copy `kernel8.img` to the boot partition alongside the Raspberry Pi firmware files.
- Boot the Pi.

On real hardware, you'll see both the physical LED toggling and UART debug output (if connected to serial console).

## Notes
- SoC base addresses assume Raspberry Pi 3B: peripheral base 0x3F000000.
- UART debugging is integrated for development in QEMU. See `docs/uart_debugging.md` for details.
