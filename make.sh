#!/bin/bash

cd ./build || { echo "无法进入 build 目录"; exit 1; }

rm -rf ./* || { echo "清空 build 目录失败"; exit 1; }

cmake .. || { echo "cmake 执行失败"; exit 1; }

make || { echo  "make 执行失败";exit 1;}
