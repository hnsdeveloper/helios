FROM --platform=linux/x86_64 debian:12

RUN apt update
RUN apt -y install make gcc-12 g++-12 g++-12-riscv64-linux-gnu gcc-riscv64-unknown-elf gcc-riscv64-linux-gnu g++-riscv64-linux-gnu

ENV CPLUS_INCLUDE_PATH=/usr/riscv64-linux-gnu/include/c++/12/riscv64-linux-gnu/:/usr/riscv64-linux-gnu/include:/usr/include/c++/12:/usr/include 
ENV C_INCLUDE_PATH=/usr/include
ENV ARCH=riscv64
ENV CXXPREFIX=riscv64-unknown-elf-

COPY entrypoint.sh /entrypoint.sh
ENTRYPOINT ["/entrypoint.sh"]
