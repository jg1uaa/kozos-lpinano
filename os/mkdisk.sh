#!/bin/sh

# check root
username=`whoami`
if [ $username != root ]
then
   echo "must be root"
   exit
fi

# check arguments
if [ $# -lt 2 ]
then
    echo "usage: $0 [u-boot] [output_image]"
    exit
fi
current_directory=$PWD
uboot_file=$1
image_file=$2

# create empty disk image
dd if=/dev/zero of=$image_file bs=1M count=20

# attach disk image
vnconfig vnd0 $image_file

# setup u-boot
dd if=$uboot_file of=/dev/vnd0c bs=1024 seek=8

# create partition
echo "edit 0\n 4\n n\n 2048\n 38400\n p\n w\n q\n" | fdisk -e vnd0
newfs_msdos /dev/rvnd0i

# copy files
mount_msdos -l /dev/vnd0i /mnt
cp kozos /mnt
umount /mnt

# detach disk image
vnconfig -u vnd0
