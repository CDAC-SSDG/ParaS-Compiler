# Compiling Codes for NVIDIA GPU device
---

## Command for Compilation
```bash
parascc convolution.cpp -parasdevice cuda:sm_x -O3 -ffast-math
```
where x is compute capability of NVIDIA GPU

## Executing the Compiled binary
```bash
./a.out
```
