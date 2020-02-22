if ! [ -a build ] ; then
    mkdir build
fi
cd build
cmake -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=Release  .. -Wnodev
make -j$(nproc)
sudo make install
