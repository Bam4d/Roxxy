FROM ubuntu:14.04

# Install common dependencies
RUN sudo apt-get update
RUN sudo apt-get install -yq \
	git \
 	cmake \
 	build-essential
 	
# Generate the working directory for installing Roxxy
WORKDIR /home/roxxy

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

# Get the gyp dependencies
WORKDIR /home/roxxy/
RUN git clone https://chromium.googlesource.com/external/gyp.git
WORKDIR /home/roxxy/gyp
RUN sudo pip install setuptools
RUN sudo python setup.py install

RUN sudo apt-get install -yq \
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

RUN sudo apt-get -yq autoremove

COPY build.gyp build.gyp
COPY build.sh build.sh

COPY assets/ assets/
COPY src/ src/

# Get the pre-built cef binary
RUN wget http://opensource.spotify.com/cefbuilds/cef_binary_3.2743.1442.ge29124d_linux64.tar.bz2 cef_binary_3.2743.1442.ge29124d_linux64.tar.bz2
RUN tar -xvf cef_binary_3.2743.1442.ge29124d_linux64.tar.bz2

WORKDIR /home/roxxy/cef_binary_3.2743.1442.ge29124d_linux64 
RUN cmake .
WORKDIR /home/roxxy/cef_binary_3.2743.1442.ge29124d_linux64/libcev_dll_wrapper
RUN make 


WORKDIR /home/roxxy/
RUN ./build.sh

WORKDIR out/Release/
CMD ./Roxxy --filters filters --browsers 10
