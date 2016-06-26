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
    python-pip \
    zip \
    unzip \
    ninja-build \
    libpng-dev
    
# Get the pre-built cef binary
RUN wget https://s3.amazonaws.com/bam4d-experiments/roxxy/ceflib.tar.gz
RUN tar -xvf ceflib.tar.gz

# Clone and install google test
RUN git clone https://github.com/google/googletest.git
WORKDIR /home/roxxy/googletest
RUN git checkout 0a439623f75c029912728d80cb7f1b8b48739ca4
RUN cmake .
RUN make
RUN sudo cp -a googletest/include/gtest /usr/include && \ 
	sudo cp -a googlemock/gtest/libgtest_main.a googlemock/gtest/libgtest.a /usr/lib/
RUN sudo cp -a googlemock/include/gmock /usr/include && \ 
	sudo cp -a googlemock/libgmock_main.a googlemock/libgmock.a /usr/lib/

# Clone and install proxygen
WORKDIR /home/roxxy/
RUN git clone https://github.com/facebook/proxygen.git
WORKDIR /home/roxxy/proxygen/proxygen
RUN git checkout 00612209fa827f1a6ad0dea498435a9cfd449624
RUN ./deps.sh && ./reinstall.sh

# Get the gradle dependencies
WORKDIR /home/roxxy/
RUN git clone https://chromium.googlesource.com/external/gyp.git
WORKDIR /home/roxxy/gyp
RUN sudo pip install setuptools
RUN sudo python setup.py install

WORKDIR /home/roxxy/
RUN ./build.sh

EXEC /out/Release/Roxxy
