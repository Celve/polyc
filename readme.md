# Polyc

This is a part of *A Practical Automatic Polyhedral Parallelizer and Locality Optimizer*. The whole process could be regarded as: 

C code → Polyhedral extraction (clan) → Dependence analysis (candl) → **Loop Transformation(polyc)** → Code generation (CLooG) → C code

The bold part is what this project does. It takes the output of candl and transform the loops to parallel loops according to the comments in the code. Hence, it could not judge automatically whether a loop could be parallelized or not. The user should give the information about the parallelization.

## Run

```bash
cmake -B build
cmake --build build
./build/poc <filename>
```

For example, 
```bash
./build/poc test/fuse/fuse.c
```

To see how it fuse two loops inside the `test/fuse/fuse.c`. 