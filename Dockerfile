FROM ubuntu:14.04

# Generate the working directory for installing Roxxy
WORKDIR /home/roxxy

# Copy the source and the gradle files
COPY gradlew gradlew
COPY gradlew.bat gradlew.bat
COPY build.gradle build.gradle

COPY src/ src/

# Install common dependencies
RUN sudo apt-get update
RUN sudo apt-get install -yq git \
 	curl \
 	cmake 
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
    wget
    
# Get the pre-built cef binary
RUN wget https://s3.amazonaws.com/bam4d-experiments/roxxy/ceflib.tar.gz ceflib.tar.gz
RUN tar -xvf ceflib.tar.gz

# Clone and install google test
RUN git clone https://github.com/google/googletest.git
RUN git checkout tags/release-1.7.0
WORKDIR /home/roxxy/googletest
RUN cmake .
RUN sudo make install

# Clone and install proxygen
WORKDIR /home/roxxy/
RUN git clone https://github.com/facebook/proxygen.git
RUN git checkout tags/v0.32.0
WORKDIR /home/roxxy/proxygen/proxygen
RUN ./deps.sh && ./reinstall.sh

# Get the gradle dependencies
WORKDIR /home/roxxy/
RUN ./gradlew


RUN gradlew build
