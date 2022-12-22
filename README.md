# MSMARCO-Search-Engine

Project developed for the MIRVC course of the Master of Artificial Intelligence and Data Engineering at the University of Pisa.

This project consists in the design and implementation of a Search Engine for MSMARCO dataset.

To run this project you need to download in the main folder the [@MSMARCO](https://msmarco.blob.core.windows.net/msmarcoranking/collection.tar.gz) dataset.
## Compiling on UNIX

### 1. Install the required software
```bash
$ sudo apt-get install git cmake build-essential zlib1g-dev libboost-all-dev
```
### 2. Download the source code
```bash
$ git clone --recursive https://github.com/edoardoruffoli/MSMARCO-Search-Engine
```
### 3. Generate the build files
```bash
$ cd MSMARCO-Search-Engine
$ mkdir build && cd build
$ cmake ..
```
### 4. Build
```bash
$ make
```

### 5. Run
```bash
$ cd bin
$ ./app
```

## Compiling on Windows
### a. Install and run with Visual Studio
Clone the reposiotry, generate the build file with cmake, build and run!
### b. Run without an IDE

## Repository

The repository is organized as follows:
- *apps/* contains 
- *docs/* contains the report and the assignment
- *evaluation/* contains the IMDb dataset stored in *film_ratings.txt*
- *include/* contains 
- *src/* contains
- *tests/* contains 
- *thirdparty/* contains 

## Contributors
- Francesco Hudema [@MrFransis](https://github.com/mrfransis)
- Edoardo Ruffoli [@edoardoruffoli](https://github.com/edoardoruffoli)
