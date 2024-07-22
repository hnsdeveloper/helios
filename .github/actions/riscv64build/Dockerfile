FROM --platform=linux/x86_64 debian:12

RUN apt update
RUN apt -y install make git gcc-12 g++-12 g++-12-riscv64-linux-gnu gcc-riscv64-unknown-elf gcc-riscv64-linux-gnu g++-riscv64-linux-gnu
RUN git clone https://github.com/hnsdeveloper/gnu-efi.git ~/gnu-efi
RUN make -C ~/gnu-efi CROSS_COMPILE=riscv64-linux-gnu- ARCH=riscv64
RUN make -C ~/gnu-efi install CROSS_COMPILE=riscv64-linux-gnu- ARCH=riscv64

COPY build.sh /build.sh
ENTRYPOINT ["/build.sh"]
