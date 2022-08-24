[![Ubuntu](https://github.com/hyoklee/vol-log-based/actions/workflows/ubuntu_mpich.yml/badge.svg)](https://github.com/hyoklee/vol-log-based/actions/workflows/ubuntu_mpich.yml)
[![Linux HDF5 develop](https://github.com/hyoklee/vol-log-based/actions/workflows/linux.yml/badge.svg)](https://github.com/hyoklee/vol-log-based/actions/workflows/linux.yml)
[![Linux HDF5 1.12.1](https://github.com/hyoklee/vol-log-based/actions/workflows/linux_1.12.yml/badge.svg)](https://github.com/hyoklee/vol-log-based/actions/workflows/linux_1.12.yml)

## Log-layout based VOL - an HDF5 VOL Plugin for storing datasets in a log-based layout

This software repository contains source codes implementing an
[HDF5](https://www.hdfgroup.org) Virtual Object Layer
([VOL](https://portal.hdfgroup.org/display/HDF5/Virtual+Object+Layer)) plugin
that stores HDF5 datasets in a log-based storage layout in files. When using
this VOL, multiple write requests from an MPI process are appended one after
another (as logs) and stored in a contiguous block in the file. The contiguous
blocks of multiple processes are appended one another following the processes'
MPI rank order. Such log-based I/O strategy avoids the expensive inter-process
communication which is required when storing data in the canonical order.
Thus, exist HDF5 programs, through the log-based VOL, can achieve a better
parallel write performance with no changes to their codes. Files created by
this VOL conform with the HDF5 file format, but require this VOL to read them
back.

* Current build status:
  [![Ubuntu_mpich](https://github.com/DataLib-ECP/vol-log-based/actions/workflows/ubuntu_mpich.yml/badge.svg)](https://github.com/DataLib-ECP/vol-log-based/actions/workflows/ubuntu_mpich.yml)

<<<<<<< HEAD
### Building Steps
* Build HDF5 with VOL and parallel I/O support
  + Clone the develop branch from the HDF5 repository
  + Run command ./autogen.sh
  + Configure HDF5 with parallel I/O enabled
  + Run make install
  + Example commands are given below. This example will install
    the HD5 library under the folder `${HOME}/HDF5`.
    ```
    % git clone https://github.com/HDFGroup/hdf5.git
    % cd hdf5
    % git checkout hdf5-1_12_0
    % ./autogen
    % ./configure --prefix=${HOME}/HDF5 --enable-parallel CC=mpicc
    % make -j4 install
    ```
* Build this VOL plugin, `log-based vol.`
  + Clone this VOL plugin repository
  + Run command autoreconf -i
  + Configure log-based VOL 
    + Shared library is required to enable log-based VOL by environment variables
    + Compile with zlib library to enable metadata compression
  + Example commands are given below.
    ```
    % git clone https://github.com/DataLib-ECP/vol-log-based.git
    % cd vol-log-based
    % autoreconf -i
    % ./configure --prefix=${HOME}/Log_IO_VOL --with-hdf5=${HOME}/HDF5 --enable-shared --enable-zlib
    % make -j 4 install
    ```
    The VOL plugin library is now installed under the folder `${HOME}/Log_IO_VOL.`
=======
### HDF5 VOL Connector ID
* This log-based VOL has been registered with the HDF group with
  [Connector Identifier 514](https://portal.hdfgroup.org/display/support/Registered+VOL+Connectors).
 
### Documents
* [doc/userguide.md](doc/userguide.md) contains the compile and run instructions.
* [doc/design.md](doc/design.md) outlines the design of log-based VOL.
* [doc/api.md](doc/api.md) describes the new APIs introduced in this VOL.
>>>>>>> upstream/master

### Developers
* Wei-keng Liao <<wkliao@northwestern.edu>>
* Kai-yuan Hou <<kai-yuanhou2020@u.northwestern.edu>>

Copyright (C) 2022, Northwestern University.
See [COPYRIGHT](COPYRIGHT) notice in top-level directory.

### Project funding supports:
<<<<<<< HEAD
<<<<<<< HEAD
Ongoing development and maintenance of Log-based VOL are supported by the Exascale Computing Project (17-SC-20-SC), a joint project of the U.S. Department of Energy's Office of Science and National Nuclear Security Administration, responsible for delivering a capable exascale ecosystem, including software, applications, and hardware technology, to support the nation's exascale computing imperative.
=======
Ongoing development and maintenance of Log-based VOL are supported by the
Exascale Computing Project (17-SC-20-SC), a joint project of the U.S.
Department of Energy's Office of Science and National Nuclear Security
Administration, responsible for delivering a capable exascale ecosystem,
including software, applications, and hardware technology, to support the
nation's exascale computing imperative.

>>>>>>> upstream/master
