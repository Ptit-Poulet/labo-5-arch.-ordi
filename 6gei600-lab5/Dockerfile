FROM debian:bookworm-slim

RUN apt-get update && apt-get install -y \
    gcc-aarch64-linux-gnu \
    qemu-system-aarch64 \
    gdb-multiarch \
    make

EXPOSE 1234

WORKDIR /workspace

CMD ["/bin/bash"]

