FROM ubuntu:14.04

# Generate the working directory for installing Roxxy
WORKDIR /home/roxxy

COPY build.gyp build.gyp
COPY build.sh build.sh

COPY assets/ assets/
COPY src/ src/

# Install common dependencies
RUN sudo apt-get update
RUN sudo apt-get install -yq git \
 	curl \
 	cmake \
 	build-essential
 
RUN sudo apt-get install -yq \
    flex \
    bison \
    libkrb5-dev \
    libsasl2-dev \
    libnuma-dev \
    pkg-config \
    libssl-dev \
    libcap-dev \
    gperf \
    autoconf-archive \
    libevent-dev \
    libgoogle-glog-dev \
    wget \
    zip \
    unzip \
    ninja-build
    
# Get the pre-built cef binary
RUN wget https://s3.amazonaws.com/bam4d-experiments/roxxy/ceflib.tar.gz
RUN tar -xvf ceflib.tar.gz

# Clone and install google test
RUN git clone https://github.com/google/googletest.git
WORKDIR /home/roxxy/googletest
RUN git checkout tags/release-1.7.0
RUN cmake .
RUN sudo make
RUN sudo cp -a include/gtest /usr/include && \ 
	sudo cp -a libgtest_main.a libgtest.a /usr/lib/

# Clone and install proxygen
WORKDIR /home/roxxy/
RUN git clone https://github.com/facebook/proxygen.git
WORKDIR /home/roxxy/proxygen/proxygen
RUN git checkout tags/v0.32.0
RUN ./deps.sh && ./reinstall.sh

# Get the gradle dependencies
WORKDIR /home/roxxy/

RUN git clone https://chromium.googlesource.com/external/gyp.git
WORKDIR /home/roxxy/gyp
RUN sudo python setup.py install

RUN ./build.sh

EXEC /out/Release/Roxxy
