# BuildIt Array DSL

This repo is the skeleton code for the CGO 2024 handson tutorial - [Building DSLs made easy with the BuildIt Framework](https://buildit.so/tutorial). 
This repo can be run entirely on a Linux system (including WSL). You can also run it on MacOS, except for the debugging section, since D2X is currently not supported for MacOS.

We will be implementing a BuildIt Array DSL which supports collective operations on array types just like numpy. Basic operations include pointwise additions, multiplications and cross products. More operations can be added in a similar way. The DSL supports code generation for CPUs and GPUs and supports debugging using D2X. We will implement simple whole program static analysis and transformations like correctness checks for GPU vs Host memory and constant propagation on arrays all the while writing a pure library implementation. 

The skeleton code has several TODOs that you can find by grepping for `TODO`. What needs to be done for each of them has been explained below. 

## Getting started

To start, you can clone the entire repo and the submodules using the command - 

    git clone --recursive https://github.com/BuildIt-lang/buildit-array.git

If you have already cloned it without the `--recursive` flag, the submodules can be fetched with the following commands -

    git submodule init
    git submodule update

You are now ready to build the repo and run the first sample.

## Building the DSL

Building the skeleton code and the DSL is as simple as running the command - 

    make -j$(nproc)

This will compile all the requried dependencies (BuildIt), the library and the samples. Please make sure you are using GNU make for this. If the above step runs successfully, you can run the first sample using the command - 

    ./build/sample1

The command should return successfully and print out the code to the standard output. You can read the DSL input code under `samples/sample1.cpp`. You can notice that there are some blatant correctness issues in the generated code. All these correctness issues will be fixed when we fix all the TODOs below. So let's get into them. 

