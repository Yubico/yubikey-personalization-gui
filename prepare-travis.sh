#!/bin/sh

set -e
set -x

if [ "x$TRAVIS_OS_NAME" != "xosx" ]; then
  sudo add-apt-repository -y ppa:yubico/stable
  sudo apt-get update -qq
  sudo apt-get install -qq -y --no-install-recommends libyubikey-dev libykpers-1-dev qt4-qmake libqt4-dev
  QMAKE=qmake
else
  brew update
  brew install qt5
  brew install libyubikey
  brew install ykpers
  QMAKE=/usr/local/opt/qt5/bin/qmake
  CXXFLAGS="-I/usr/local/include"
  LDFLAGS="-L/usr/local/lib"
fi

$QMAKE 'CONFIG += debug nosilent force_pkgconfig' "CXXFLAGS=$CXXFLAGS" "LDFLAGS=$LDFLAGS"
