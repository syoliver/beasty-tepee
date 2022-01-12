FROM ubuntu:22.04

RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt-get install -y gcc gdb cmake python3 python3-pip

RUN pip3 install conan

COPY tools/build /home/src/tools/build
COPY tools/conan /home/src/tools/conan
COPY conanfile.txt /home/src/conanfile.txt

RUN cd /home/src/tools/build && ./boost-url-generate.sh

COPY . /home/src

RUN cd /home/src/tools/build && ./tepee-generate.sh

WORKDIR /home/src
