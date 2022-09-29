FROM ubuntu:focal
 
# set noninteractive installation
RUN export DEBIAN_FRONTEND=noninteractive
WORKDIR /build
# install tzdata package
RUN apt-get update
RUN apt-get install -y tzdata
# set your timezone
RUN ln -fs /usr/share/zoneinfo/America/New_York /etc/localtime
RUN dpkg-reconfigure --frontend noninteractive tzdata
RUN apt-get install -y clang gcc curl git wget cmake ninja-build 
RUN curl -LO https://github.com/casey/just/releases/download/1.5.0/just-1.5.0-x86_64-unknown-linux-musl.tar.gz && tar xvf just-1.5.0-x86_64-unknown-linux-musl.tar.gz  && mv just /usr/bin/
RUN curl -LO https://github.com/rui314/mold/releases/download/v1.5.0/mold-1.5.0-x86_64-linux.tar.gz && tar xvf mold-1.5.0-x86_64-linux.tar.gz && mv mold-1.5.0-x86_64-linux/libexec/* /usr/libexec/ && mv mold-1.5.0-x86_64-linux/bin/* /usr/bin/
RUN apt-get install libasound2-dev mesa-common-dev libx11-dev libxrandr-dev libxi-dev xorg-dev libgl1-mesa-dev libglu1-mesa-dev lld -y
RUN ln -s $(which wasm-ld-13 || which wasm-ld-12 || which wasm-ld-11 || which wasm-ld-10) /usr/bin/wasm-ld
ADD . /build
RUN rm -rf build/
RUN just configure_release
RUN just build_release
RUN mkdir -p /out
CMD ["cp", "build/release/main", "/out/shooter2_c"]
