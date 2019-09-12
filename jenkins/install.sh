mkdir -p /ioplacer/build
cd /ioplacer/build
cmake -DCMAKE_BUILD_TYPE=Release -DBOOST_ROOT=/opt/rh/rh-mongodb32/root/usr ..  && \
    make