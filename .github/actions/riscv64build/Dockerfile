FROM --platform=linux/x86_64 debian:12

RUN apt update
RUN apt -y install make git gcc-12 g++-12 g++-12-riscv64-linux-gnu gcc-riscv64-unknown-elf gcc-riscv64-linux-gnu g++-riscv64-linux-gnu
RUN git clone https://github.com/hnsdeveloper/gnu-efi.git ~/gnu-efi
RUN make -C ~/gnu-efi CROSS_COMPILE=riscv64-linux-gnu- ARCH=riscv64
RUN make -C ~/gnu-efi install CROSS_COMPILE=riscv64-linux-gnu- ARCH=riscv64
ENV CPLUS_INCLUDE_PATH=/usr/riscv64-linux-gnu/include/c++/12/riscv64-linux-gnu/:/usr/riscv64-linux-gnu/include:/usr/include/c++/12:/usr/include:/usr/local/include:/usr/local/include/efi
ENV C_INCLUDE_PATH=/usr/riscv64-linux-gnu/include/c++/12/riscv64-linux-gnu/:/usr/riscv64-linux-gnu/include:/usr/include/c++/12:/usr/include:/usr/local/include:/usr/local/include/efi
ENV ARCH=riscv64
ENV CXXPREFIX=riscv64-unknown-elf-

COPY entrypoint.sh /entrypoint.sh
ENTRYPOINT ["/entrypoint.sh"]
