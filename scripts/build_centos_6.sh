#!/bin/bash
set -e

# Centos Specific
#https://fedoraproject.org/wiki/EPEL
#pkill yum
yum install -y epel-release
yum install -y git cmake gcc gcc-c++ bison flex
#yum install -y git cmake gcc gcc-c++ gnu-fortran bison flex wget


#download install Miniconda
cd /root
curl -O https://repo.continuum.io/miniconda/Miniconda2-latest-Linux-x86_64.sh
bash ~/Miniconda2-latest-Linux-x86_64.sh -b -p ${HOME}/anaconda
${HOME}/anaconda/bin/conda install -y numpy mkl

#git clone https://github.com/devsim/symdiff
#cd symdiff
#bash scripts/setup_centos_6.sh
#cd linux_x86_64_release
#make -j2

cd /root
# add max depth or copy over
git clone https://github.com/devsim/devsim
#git clone git@github.com:devsim/devsim
cd devsim
git submodule init
git submodule update

# SuperLU and CGNS Download
(cd external && curl -O http://crd-legacy.lbl.gov/~xiaoye/SuperLU/superlu_4.3.tar.gz && tar xzf superlu_4.3.tar.gz)
(cd external && curl -L -O http://downloads.sourceforge.net/project/cgns/cgnslib_3.1/cgnslib_3.1.4-2.tar.gz && tar xzf cgnslib_3.1.4-2.tar.gz)

# SYMDIFF build
(cd external/symdiff && bash scripts/setup_centos_6.sh && cd linux_x86_64_release && make -j2);
# CGNSLIB build
(cd external && mkdir -p cgnslib_3.1.4/build && cd cgnslib_3.1.4/build && cmake -DBUILD_CGNSTOOLS=OFF -DCMAKE_INSTALL_PREFIX=$PWD/../../cgnslib .. && make -j2 && make install)
# SUPERLU build
(cd external/SuperLU_4.3 && sh ../superlu_redhat_6.5.sh)

bash scripts/setup_centos_6.sh
(cd linux_x86_64_release && make -j2)
(cd dist && bash package_linux.sh devsim_linux)


