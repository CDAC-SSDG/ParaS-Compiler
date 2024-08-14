# Compiling codes for Nvidia GPU device
---

## Command for Compilation
```bash
parascc convolution.cpp -parasdevice cuda:sm_x 
```
where x is compute capability of NVIDIA GPU

## Executing the Compiled binary
```bash
./a.out
```
