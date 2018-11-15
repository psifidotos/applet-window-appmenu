if ! [ -a build ] ; then
    mkdir build
fi
cd build
cmake -DCMAKE_INSTALL_PREFIX=/usr .. -Wnodev
make
sudo make install
