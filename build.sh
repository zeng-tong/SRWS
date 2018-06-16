#!/bin/sh
find . -iwholename '*cmake*' -not -name CMakeLists.txt -delete
cmake CMakeLists.txt
make && make test