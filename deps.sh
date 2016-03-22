#!/bin/bash

echo "Installing cef lib binaries"
# Firstly pick up the compiled libs from s3!
wget https://s3.amazonaws.com/bam4d-experiments/roxxy/ceflib.tar.gz ceflib.tar.gz
tar -xvf ceflib.tar.gz


echo "Installing pxygen dependencies"
# Install lib dependencies for proxygen
apt-get update
apt-get install -yq git
apt-get install -yq curl
apt-get install -yq cmake build-essential
apt-get install -yq \
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

echo "Installing gtest"
# Clone the gtest repo from github.
git clone https://github.com/google/googletest.git

cd googletest
cmake
make install


echo "Installing proxygen"
# Clone the proxygen repo from github.
git clone https://github.com/facebook/proxygen.git

cd proxygen/proxygen

echo "Installing proxygen.."
# build proxygen stuff
./deps.sh && ./reinstall.sh

cd ../../
echo "Installing gradle..."
./gradlew

echo "Building Roxxy.."
gradle build
