<h1>
<p align="center">
<img src="https://github.com/CDAC-SSDG/ParaS-Compiler/blob/main/PARAS_logo.png" width="100" height="100" style="">
<br>
  ParaS-Compiler
</p>
</h1>


---

## Table of Contents
+ [About](https://github.com/CDAC-SSDG/ParaS-Compiler/blob/main/README.md#about)
+ [Requirements](https://github.com/CDAC-SSDG/ParaS-Compiler/blob/main/README.md#requirements)
+ [Platforms Supported](https://github.com/CDAC-SSDG/ParaS-Compiler/blob/main/README.md#platforms-supported)
+ [Installation](https://github.com/CDAC-SSDG/ParaS-Compiler/blob/main/README.md#installation)
+ [Usage](https://github.com/CDAC-SSDG/ParaS-Compiler/blob/main/README.md#usage)
+ [Issues](https://github.com/CDAC-SSDG/ParaS-Compiler/blob/main/README.md#issues)
+ [Contact Us](https://github.com/CDAC-SSDG/ParaS-Compiler/blob/main/README.md#contact-us) 

## About 
ParaS (sounds like **paa-ruhs**) is an implementation of the [SYCL 2020 specification](https://registry.khronos.org/SYCL/specs/sycl-2020/html/sycl-2020.html), developed to facilitate architectural and vendor neutral programming. It aims to achieve the principle of ***Code Once and Execute on All***.
The current version enables seamless execution across CPUs of x86 (Intel and AMD) and ARM as well as CUDA enabled NVIDIA GPUs.


## Requirements
+ Linux OS
+ Singularity
+ OpenSSL

## Platforms Supported

**ParaS is supported on the following architectures:** 
  + CPU: *X86, ARM, Power*
  + GPU: *CUDA*
    
> [!NOTE]
> *ParaS has been tested on the following processors*
>  + *CPUs: Intel (Skylake, CascadeLake, Sapphire Rapids), AMD Gen 4 EPYC processors, NVIDIA Grace, Altra, Fujitsu A64FX, IBM Power 10*
>  + *GPUs: Tesla V100, Ampere A100, Hopper (H100, H200)*

## Installation

**Step-1: Git clone**
```bash
git clone https://github.com/CDAC-SSDG/ParaS-Compiler.git
```
**Step-2: Run the installer script**

> [!NOTE]
> If using an **Intel processor** use the following command
> ```bash
> bash install_paras.sh intel
> ```

> [!NOTE]
> If using an **AMD processor** use the following command
>  ```bash
> bash install_paras.sh amd
> ```

## Usage: 
**To enter in the singularity container environment**
+ For CPU
```bash
singularity run --bind /etc:/etc ParaS_v0.5.1_x86.sif
```

+ For Nvidia GPU
```bash
singularity run --nv --bind /etc:/etc ParaS_v0.5.1_x86.sif
```
***where, --nv is to load CUDA drivers***

**For compilation**

+ For CPU
```bash
parascc <program.cpp> [Compiler Optimization Flags] -o <executable_file>
```
+ For Nvidia GPU
```bash
parascc <program.cpp> -parasdevice cuda:sm_<x> [Compiler Optimization Flags] -o <executable_file>
```
***where, x is the compute capability of Nvidia GPU***

## Issues
Issues can be raised in the issues section of the git repositery. In case of issues with the download, contact paras@cdac.in. 

## Contact Us
Contact us at *paras@cdac.in*
