# Use i386 to ensure 16-bit tool compatibility and bionic for stable bcc packages
FROM i386/ubuntu:bionic

# Avoid stuck installs with a "noninteractive" flag
ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    bcc \
    bin86 \
    nasm \
    make \
    build-essential \
    && rm -rf /var/lib/apt/lists/*

# Set the working directory
WORKDIR /work

# Start in a bash shell
CMD ["/bin/bash"]

