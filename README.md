# BuildIt DSL Template 

This is a template repository for creating a DSL with BuildIt. The repository has BuildIt as a submodule and the appropriate Makefiles to compile the DSL. 

Follow the following steps to create a new DSL repo - 

1. Click "Use this template" to create a new repo with the template files
2. Clone the created repo with the recursive flag -

   `git clone --recursive <remote url for your new repo>`

3. Update BuildIt version if not up to date by navigating to `deps/buildit` and running `git pull`
4. Edit Makefile to set the LIBRARY_NAME variable. The library for your DSL will be created with the name lib<name>.a
5. Edit the files under include/ and src/ to implement your DSL.
6. Working samples can be added under samples/ which will be compiled and linked appropriately.
7. The build system takes a single make variable DEBUG=0 or DEBUG=1 and recompiles BuildIt appropriately if needed. This variable can either be added at the command line as `make DEBUG=1` or can be added to a file named Makefile.inc in the top level directory.


This template does not contain a LICENSE file. BuildIt is available under the MIT License and is our recommendation for your DSL. 
