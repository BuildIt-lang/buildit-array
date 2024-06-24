# BuildIt Array DSL

This repo is the skeleton code for the PLDI/CGO 2024 handson tutorial - [Building DSLs made easy with the BuildIt Framework](https://buildit.so/tutorial). 
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



### TODO 1: Generate code to initialize the elements

If you open the sample input under `samples/sample1.cpp` you can notice that we have 3 arrays - `y`, `z` and `w`. `z` and `w` are not "bound" to any existing buffers and hence have their memory allocated in the generated code with calls to `malloc`. `y` is bound to the `int*` array passed as argument to the function. Since this array is declared as `dyn_var<int*>` it is supplied at runtime in the generated code instead of being supplied in the first stage in the compiler. 

Next we initialize the arrays `z` to `0` and `w` to `1`. This is supposed to initialize all the elements, but no code is being generated for it. Let us fix that by writing code for the first TODO. Search for `TODO 1` in `include/barray_impl.h`. It should be under the implementation for of `operator=(const T& value)`. We have calculated the size of the array in the first stage by a call to the `get_total_size()` function which just multiplies all the array dimensions. We need a loop to iterate over all these elements and set them to `value`. 

Since we want this code to be generated in the output, we would need to write this loop using a `dyn_var<int>` index. Fill in this TODO and compile the code again with `make`. Try running sample1 again to check if it produces the correct code to initialize all elements. 

Remember that the `barray` class stores the buffer as a `dyn_var<T*>` member called `m_arr`.  

### TODO 2: Induce loops for all dimensions for computations

Now that we have a flat initialization working correctly, let's look at the expresison `y = z + w`. This operation is supposed to be performed on each element of the array but is currently being performed only on the 0th element. For this operation, we would like to create a loop nest, one loop for each output dimension so that each array can use each index as it wishes. A flat loop approach would not be feasible in the long run. We have already created a helper function called `induce_loop_at` which is supposed to create a loop for each dimension and call itself recursively for the next dimension. Currently we have the recursive loop, but no loop for the dimension. Seach for `TODO 2` and fill in the loop. Notice how we keep track of references to these indices in a vector and finally use them to compute the value of the RHS in the inner most level. Try to reason about what the type of the loop and the loop bounds should be. 

Build and run the sample again, if everything is filled in correctly, the generated code should have a 3 level nested loop to compute values at each element and store in `y`. 

### TODO 3: Analyze and assert that computations are valid

We have correct loop nests being generated, but what would happen if the sizes of dimensions of the individual arrays being computed on are not the same. We will end with an out of bounds access. Let's test this out by changing the dimensions of one of the arrays say `z` from `{dim1, dim2, dim3}` to `{dim1 - 1, dim2, dim3}`. Build and run the sample again. We notice that the DSL just generates the same loop nest. This is incorrect because it would lead to an out of bounds access. We should analyze and assert that at every operation, the dimensions of the sub arrays. For pointwise operations this simply means the arrays are of the same dimensions. 

If we scroll down to the `barray_expr_add` constructor, we notice that we call the function `match_expr_sizes` to compare the sizes of the two subexpressions and compute the dimensions of the output expression. This funciton is implemented under `src/barray.cpp` which is where we have our 3rd TODO. Fill in this TODO to assert that the respective dimensions of the array are equal. Since this comparison can be performed entirely in the first stage (compiler), we should not use a `dyn_var` loop, but rather a `static_var` loop (or a regular C++ loop). Fill in this TODO to assert that the dimensions are the same. Build and run the sample. This time we will notice that the compiler errors out with a message saying the array dimensions don't agree. We can revert back the dimensions of `z` and check that the `match_expr_sizes` does not fail this time. 

### TODO 4: Optimizing constant initializations

Let us run the code for the sample1 again. We notice that we have 3 loop nests. One to intialize `z`, second to initialize `w` and third to initialize `y`. However, `z` and `y` are simply initialized to a constant. All the elements are guaranteed to be the same when they are read. We should be able to optimize it so that when the array is read, we directly use the constant. 

To allow this optimization, we have already created two `static_var`s `is_constant` and `constant_val` initialized to `false` and `0` respectively. If we know that the array is being set to a constant, we can simply set this flag and save the value. When accessing the array, we can simply return this value. Search for `TODO 4` in `include/barray_impl.h`. There should be two of them. The first one should be inside the `operator=(const T& val)` where we implemented the first TODO. We can now comment the old implementation and simply set the `is_constant` to `true` and `constant_val` to `value`. 

Next search for `TODO 4.2`. Here we can check if `is_constant` is set, and if it is, we can simply return `constant_val`. Make the changes and build and run sample1. We will see that the generated code only has a single loop nest and the values for `w` and `z` are directly embedded in the expression. 

To further verify this, let's make sure the values are used properly, if the arrays are changed. Add the `y = z + w` expression inside a loop and add a second statement `w = y;` after the first one. Run and make sure the generated code is correct and the code is not using stale values for `w`. This works because BuildIt keeps track of all static values and merges loops only if they are exactly the same as the previous iteration. 


### TODO 5: Map computations to GPU

Next, we will map computations to the GPU to improve the performance of the code. BuildIt allows automatically mapping doubly nested loops to CUDA code by using a simple annotation. Check the BuildIt sample under `deps/buildit/sample/sample37.cpp` to see how this annotation works. Next search for `TODO 5`. We can see that this is right before the loop for a dimension is induced. If we add the annotation here for the first level, BuildIt will turn the next two loop levels into GPU blocks and threads. To decide whether compute is running on GPU or Host, we have a defined an enum `currect_device` which the user can set. Check this enum and conditionally add the annotation only on the outermost loop. 

We have provided a helper routine `run_on_gpu` which accepts a lambda to run on GPU. Wrap the `y = z + w` in this function under a lambda and build and run the code. We can see that the compute is moved to a CUDA kernel and an appropriate call to the kernel is inserted in the host code. 

We have also provided the funcitons `barray.to_device()` and `barray.to_host()` to move the data between host and GPU for compute. Insert `z.to_device()` and `w.to_device()` before the call to allocate buffers and move the data. 

### TODO 6: Ensuring data is moved to the right device before compute

What happens if we forget to move a particular array to the GPU before operating on it on the GPU. Comment out `z.to_device()` and check the generated code. We notice that BuildIt still generates the code but it is wrong. We need to assert that the data is in the right place just like before. Search for `TODO 6`. The `barrays` have a `static_var`, `current_storage` to check where they are currently stored. Assert that the arrays and the execution is in the right place. Build and run the sample to check that the assert should fail. 
Uncomment the `z.to_device()` to check that the compiler now generates correct code. 

### TODO 7: Implement cross product for 2D arrays
For this TODO, we would want you to implement the cross product operator for arbitrary 2D barray expressions. The skeleton code and the `TODO 7` has been provided. A `samples/sample2.cpp` has also been provided to test the kernel. Fill in the implementation and check if the compiler generates correct code. 

## D2X Debugging
[D2X](https://buildit.so/d2x) is a companion library in BuildIt that allows debugging code through stages. D2X allows adding debugging support without a single line of code change. So there are no TODOs for this section. But the proces to enable D2X will be discussed and demonstrated in the tutorial. 
