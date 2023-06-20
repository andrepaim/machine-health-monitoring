# Use the official Ubuntu image as the base
FROM ubuntu:20.04

# Install necessary dependencies
RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt-get install -y \
      cmake \
      systemctl \
      build-essential \
      pkg-config \
      gdb \
      git \
      netcat \
      sudo \
      libboost-all-dev \
      libmongoc-dev \
      libbson-dev \
      mosquitto \
      libssl-dev \
      doxygen \
      graphviz \
      && rm -rf /var/lib/apt/lists/*

# Go back to parent directory
WORKDIR /

# Clone the Paho MQTT C library
RUN git clone https://github.com/eclipse/paho.mqtt.c.git
WORKDIR /paho.mqtt.c
RUN git checkout v1.3.12

# Make and install the Paho MQTT C library
RUN cmake -Bbuild -H. -DPAHO_ENABLE_TESTING=OFF -DPAHO_BUILD_STATIC=ON -DPAHO_WITH_SSL=ON -DPAHO_INSTALL_CMAKE=ON
RUN sudo cmake --build build/ --target install
RUN sudo ldconfig

# Go back to parent directory
WORKDIR /

# Clone the Paho MQTT C++ library
RUN git clone https://github.com/eclipse/paho.mqtt.cpp
WORKDIR /paho.mqtt.cpp

# Make and install the Paho MQTT C++ library
RUN cmake -Bbuild -H. -DPAHO_BUILD_STATIC=ON -DPAHO_BUILD_DOCUMENTATION=TRUE -DPAHO_BUILD_SAMPLES=TRUE
RUN sudo cmake --build build/ --target install
RUN sudo ldconfig

# Add a non-root user
ARG USERNAME=developer
ARG USER_UID=1000
ARG USER_GID=$USER_UID
RUN groupadd --gid $USER_GID $USERNAME \
&& useradd -s /bin/bash --uid $USER_UID --gid $USER_GID -m $USERNAME \
&& echo $USERNAME ALL=\(root\) NOPASSWD:ALL > /etc/sudoers.d/$USERNAME \
&& chmod 0440 /etc/sudoers.d/$USERNAME

# Switch to the non-root user
USER $USERNAME

# Set up the working directory
WORKDIR /workspace
