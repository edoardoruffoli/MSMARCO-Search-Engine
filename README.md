# MSMARCO-Search-Engine

Project developed for the MIRVC course of the Master of Artificial Intelligence and Data Engineering at the University of Pisa.

This project consists in the design and implementation of a Search Engine for MSMARCO dataset.

To run this project you need to download in the main folder the [MSMARCO](https://msmarco.blob.core.windows.net/msmarcoranking/collection.tar.gz) dataset.

## Compiling on Windows
1. Import the project in Visual Studio/Visual Studio Code

2. Build the project using Cmake

3. Execute app.exe

## Compiling on UNIX
1. Install the required software
```bash
$ sudo apt-get install git cmake build-essential zlib1g-dev libboost-all-dev
```
2. Download the source code
```bash
$ git clone --recursive https://github.com/edoardoruffoli/MSMARCO-Search-Engine
```
3. Generate the build files
```bash
$ cd MSMARCO-Search-Engine
$ mkdir build && cd build
$ cmake ..
```
4. Build
```bash
$ make
```

5. Run
```bash
$ cd bin
$ ./app
```

## Running
```
*** Started MSMARCO Search Engine ***
Available commands:
  help - display a list of commands
  query - perform a query
  eval - execute a queries dataset, saving the result file for trec_eval
  index - create the inverted index
  exit - exit the program

Enter a command:
>query
Enter the query execution mode:
    0 : CONJUNCTIVE_MODE
    1 : DISJUNCTIVE_MODE
    2 : DISJUNCTIVE_MODE_MAX_SCORE

>2
Select how many documents return:
>10
Enter the query:

>manhattan project

Results for: "manhattan project"
The elapsed time was 15 milliseconds, 15293700 nanoseconds.

RESULTS:
Doc Id  Score
2036644 4.31715
3870080 4.30079
2       4.29498
3615618 4.28213
2395250 4.27013
4404039 4.25136
3607205 4.23599
7243450 4.20026
3689999 4.1146
3870082 4.09159

```

## Repository

The repository is organized as follows:
- *apps/* contains the main of the programs
- *docs/* contains the project report and the assignment
- *evaluation/* contains the dataset used to evaluate the search engine with trec_eval
- *include/* contains the header files
- *src/* contains the source files
- *tests/* contains the unit tests
- *thirdparty/* contains the thirdparty dependencies

## Contributors
- Francesco Hudema [@MrFransis](https://github.com/mrfransis)
- Edoardo Ruffoli [@edoardoruffoli](https://github.com/edoardoruffoli)
