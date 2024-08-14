# ParaS-Compiler
---

## Table of Contents
+ [About](https://github.com/CDAC-SSDG/ParaS-Compiler/edit/main/README.md#about)
+ [Requirements](https://github.com/CDAC-SSDG/ParaS-Compiler/edit/main/README.md#requirements)
+ [Installation](https://github.com/CDAC-SSDG/ParaS-Compiler/edit/main/README.md#installation)
+ [Usage](https://github.com/CDAC-SSDG/ParaS-Compiler/edit/main/README.md#usage)
+ [Issues](https://github.com/CDAC-SSDG/ParaS-Compiler/edit/main/README.md#issues)
+ [Contact Us](https://github.com/CDAC-SSDG/ParaS-Compiler/edit/main/README.md#contact-us) 

## About 
ParaS (sounds like **paa-ruhs**) is an implementation of the [SYCL 2020 specification](https://registry.khronos.org/SYCL/specs/sycl-2020/html/sycl-2020.html), developed to facilitate architectural and vendor neutral programming. This initial version enables seamless execution across Intel and AMD (x86) and ARM based CPUs and CUDA enabled Nvidia GPUs, this truly embodies the principle of ***Code Once and Execute on All***.


## Requirements
+ Linux OS
+ Singularity
+ OpenSSL

## Installation

**Step-1: Git clone**
```bash
git clone git@github.com:CDAC-SSDG/ParaS-Compiler.git
```
**Step-2: Run the installer script**
```bash
bash install_paras.sh
```
## Usage: 
**Run the singularity container**
```bash
singularity run ParaS_v0.5_x86.sif
```
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
Issues can be raised in the issues section of the git repositery

## Contact Us
Contact us at *paras@cdac.in*
