FROM --platform=linux/x86_64 debian:12
SHELL ["/bin/bash", "-c"]
RUN apt update
RUN apt -y install build-essential python3 python-is-python3 uuid-dev nasm acpica-tools make git gcc-12 g++-12 g++-12-riscv64-linux-gnu gcc-riscv64-unknown-elf gcc-riscv64-linux-gnu g++-riscv64-linux-gnu
RUN git clone https://github.com/hnsdeveloper/edk2.git --branch edk2-stable202405 ~/edk2
RUN git -C ~/edk2 submodule update --init
ENV GCC5_RISCV64_PREFIX=riscv64-linux-gnu-
RUN source ~/edk2/edksetup.sh
RUN make -C ~/edk2/BaseTools
COPY build.sh /build.sh
ENTRYPOINT ["/build.sh"]
