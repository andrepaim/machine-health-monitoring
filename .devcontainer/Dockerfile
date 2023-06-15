# Use the official Ubuntu image as the base
FROM ubuntu:20.04

# Install necessary dependencies
RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt-get install -y \
      cmake \
      build-essential \
      gdb \
      git \
      libboost-all-dev \
      netcat \
      sudo \
      && rm -rf /var/lib/apt/lists/*

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