# Create the random.txt file that will be used as the seed to the programs

apt update
apt-get -y install libyaml-dev # Required for testing code to read .response.yaml file
apt -y install gcc-aarch64-linux-gnu # Required for cross compiling
apt -y install qemu-user # Required to run cross compiled binary

# This so is required to be placed in /lib for this arm64 code to run under qemu
wget https://ughe.github.io/data/2018/ld-linux-aarch64.so.1
cp ld-linux-aarch64.so.1 /lib

# This so is required but since it has the same name as the so on the system it
# is left in the current directory and the LD_LIBRARY_PATH variable is used to
# point to the current directory to find it

wget https://ughe.github.io/data/2018/libc.so.6
