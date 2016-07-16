#!/bin/bash

CEFTAR="cef_binary_3.2743.1442.ge29124d_linux64.tar.bz2"

sudo apt-get update
sudo apt-get install -yq 
	git \
	cmake \
	build-essential \
	python-pip

echo "Installing gtest"
# Clone and install google test
git clone https://github.com/google/googletest.git
cd googletest
git fetch --all
git checkout 0a439623f75c029912728d80cb7f1b8b48739ca4
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
git fetch --all
git checkout 00612209fa827f1a6ad0dea498435a9cfd449624

echo "Installing proxygen.."
# build proxygen stuff
./deps.sh && ./reinstall.sh

echo "Installing gyp"
git clone https://chromium.googlesource.com/external/gyp.git
cd gyp
sudo pip install setuptools
sudo python setup.py install

echo "Installing cef lib binaries"
# Firstly pick up the compiled libs from s3!
if [ ! -f $CEFTAR ]; then
    echo "Downloading cef build $CEFTAR"
    wget "http://opensource.spotify.com/cefbuilds/$CEFTAR" $CEFTAR
fi

tar -xvf $CEFTAR

cd $CEFTAR
cmake .
cd libcev_dll_wrapper
make 
cd ../../

echo "Installing CEF dependencies"
sudo apt-get install -yq \
    python-pip \
    ninja-build \
    libpng-dev \
    libfontconfig \
    libfreetype6 \
    libpangocairo-1.0-0 \
    libcairo2 \
    libpango-1.0-0 \
    libnss3 \
    libnspr4 \
    libgconf-2-4 \
    libxi6 \
    libxcursor1 \
    libxfixes3 \
    libxss1 \
    libxcomposite1 \
    libasound2 \
    libxdamage1 \
    libxtst6 \
    libatk1.0-0 \
    libxrandr2 \
    libcups2
    
sudo apt-get -yq autoremove 


echo "Dependencies installed!!"
