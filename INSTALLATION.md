Installation
============

## Using installation script

**Before running the installation script you have to install the dependencies needed for compiling.**


### KDE Neon / Ubuntu

```
sudo apt install make cmake extra-cmake-modules qtdeclarative5-dev libkf5plasma-dev libqt5x11extras5-dev g++ libsm-dev libkf5configwidgets-dev libkdecorations2-dev libxcb-randr0-dev
```

### openSUSE Leap 15 / Tumbleweed

```
sudo zypper in -y xrandr cmake make gcc gcc-c++ extra-cmake-modules libqt5-qtbase-devel libqt5-qtdeclarative-devel libKF5WindowSystem5 plasma-framework-devel libSM-devel libqt5-qtx11extras-devel libkdecoration2-devel kconfigwidgets-devel kwidgetsaddons-devel kdeclarative-devel
```

### Fedora
```
sudo dnf install make cmake extra-cmake-modules qt5-qtdeclarative-devel kf5-plasma-devel qt5-qtx11extras-devel gcc-c++ libSM-devel kf5-kconfigwidgets-devel kdecoration-devel
```

### Building and Installing

**Now you can run the installation script.**

```
sh install.sh
```

