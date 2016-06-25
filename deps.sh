#!/bin/bash

echo "Installing cef lib binaries"
# Firstly pick up the compiled libs from s3!
wget https://s3.amazonaws.com/bam4d-experiments/roxxy/ceflib.tar.gz ceflib.tar.gz
tar -xvf ceflib.tar.gz


echo "Installing pxygen dependencies"
# Install lib dependencies for proxygen
sudo apt-get update
sudo apt-get install -yq git
sudo apt-get install -yq curl
sudo apt-get install -yq cmake build-essential
sudo apt-get install -yq \
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

echo "Installing gtest"
# Clone and install google test
git clone https://github.com/google/googletest.git
cd googletest
cmake .
make
sudo cp -a googletest/include/gtest /usr/include && \ 
	sudo cp -a googlemock/gtest/libgtest_main.a googlemock/gtest/libgtest.a /usr/lib/
sudo cp -a googlemock/include/gmock /usr/include && \ 
	sudo cp -a googlemock/libgmock_main.a googlemock/libgmock.a /usr/lib/

cd ../

echo "Installing proxygen"
# Clone the proxygen repo from github.
git clone https://github.com/facebook/proxygen.git
cd proxygen/proxygen

echo "Installing proxygen.."
# build proxygen stuff
./deps.sh && ./reinstall.sh

echo "Installing gyp"
git clone https://chromium.googlesource.com/external/gyp.git
cd gyp
sudo pip install setuptools
sudo python setup.py install


echo "Dependencies installed!!"
