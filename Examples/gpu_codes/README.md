# Compiling codes for Nvidia GPU device
---

## Command for Compilation
```bash
parascc convolution.cpp -parasdevice cuda:sm_<x> 
```
***where x is compute capability of Nvidia GPU***

## Executing the Compiled binary
```bash
./a.out
```
