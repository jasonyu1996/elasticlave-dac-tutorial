# Elasticlave Tutorial

## Set it up

**Prerequisite: Docker**


```
git clone --recursive https://github.com/jasonyu1996/elasticlave-dac-tutorial.git
cd elasticlave-dac-tutorial
./make
```

## Run the tutorials

`./run` boots a Linux system on QEMU with the tutorial files in their
respective directories.

To directly run a specific tutorial task, use `./run tut<task-no>`
(e.g., `./run tut2` runs the second task).

## Tutorial tasks

There are so far four tasks in the tutorial. Their respective topis:
  1. How to create, manage, and destroy an enclave;
  2. Inter-enclave communication through public memory;
  3. Elasticlave instructions for creating memory regions and adjusting their access permissions;
  4. Inter-enclave communication with Elasticlave.


