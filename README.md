# PSI From Lightweight OPRF
This is the implementation of our paper **Private Set Intersection in the Internet Setting From Lightweight Oblivious PRF**.

## Installation
The implementation has been tested on Linux. To download the code and compile, run the following commands. It will also download and compile the libraries of [`libOTe`](https://github.com/osu-crypto/libOTe), [`Boost`](https://sourceforge.net/projects/boost/), and [`Miracl`](https://github.com/miracl/MIRACL).
```
$ git clone --recursive git@github.com:peihanmiao/OPRF-PSI.git
$ cd OPRF-PSI
$ bash compile
```

## Running the Code
### Parameters:
```
-r 0/1   to run a sender/receiver.
-ss      log of the set size on sender side.
-rs      log of the set size on receiver side.
-w       width of the matrix.
-h       log of the height of the matrix.
-hash    hash output length in bytes.
-ip      ip address (and port).
```
### Examples:
```
$ ./bin/PSI_test -r 0 -ss 16 -rs 16 -w 609 -h 16 -hash 9 -ip 127.0.0.1
  & ./bin/PSI_test -r 1 -ss 16 -rs 16 -w 609 -h 16 -hash 9 -ip 127.0.0.1

$ ./bin/PSI_test -r 0 -ss 20 -rs 20 -w 621 -h 20 -hash 10 -ip 127.0.0.1
  & ./bin/PSI_test -r 1 -ss 20 -rs 20 -w 621 -h 20 -hash 10 -ip 127.0.0.1
```

## Help
For any questions on building or running the library, please contact [`Peihan Miao`](https://sites.google.com/view/peihanmiao) at peihan at berkeley dot edu
