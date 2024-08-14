# Compiling Codes for NVIDIA GPU device
---

## Command for Compilation
```bash
parascc convolution.cpp -O3 -ffast-math -parasdevice cuda:sm_x 
```
where x is compute capability NVIDIA GPU

## Executing the Compiled binary
```bash
./a.out
```
