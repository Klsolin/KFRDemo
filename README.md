# KFRDemo

Before compiling the project, make sure your Linux system has the necessary dependencies installed. For Debian/Ubuntu based systems, use the following command:
sudo apt-get update

sudo apt-get install cmake libglm-dev libglfw3-dev libgl1-mesa-dev

For Fedora/RedHat/CentOS based systems, use the following command:
sudo yum install cmake glm-devel glfw-devel mesa-libGL-devel # Or use dnf instead of yum

When compiling the project, please first enter the KFRDemo directory on the command line, and then execute the following commands in sequence:
mkdir build

cd build

cmake ..

make

cd bin

./KFRDemo
