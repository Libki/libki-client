#!/bin/bash

version=$(grep FileVersion ../../libki.rc | grep -Eo '[0-9]+\.[0-9]+\.[0-9]+')

arch=$(uname -m)

if [[ $arch == "x86_64" ]]; then
  arch="amd64"
fi

buildname="libkiclient_${version}_${arch}"

mkdir build
cd build

qmake ../../../Libki.pro
make

cd ..

mkdir -p deb/usr/bin
mkdir -p deb/etc/xdg

mv build/libkiclient deb/usr/bin/libkiclient

cp ../../example.ini deb/etc/xdg/Libki.ini

rm build -r

cp deb/DEBIAN/control deb/DEBIAN/control.bak

sed -i "s/VERSION/$version/g" deb/DEBIAN/control
sed -i "s/ARCH/$arch/g" deb/DEBIAN/control

dpkg -b deb $buildname.deb

rm deb/usr/bin/libkiclient
rm deb/etc/xdg/Libki.ini

mv deb/DEBIAN/control.bak deb/DEBIAN/control
