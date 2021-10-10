# ES-5-Pipewire

A Pipewire client for the Expert Sleepers ES-5 eurorack module. Based upon [ES-5 Linx](https://github.com/CarlColglazier/ES-5Linx). This module takes eight gate inputs and converts them to a single output.

## Setup

### Requirements
 - GCC
 - Pipewire Development
 
 #### Fedora & RHEL
 ```sh
 sudo dnf install gcc pipewire-devel
 ```
 #### Debian & Ubuntu
 ```sh
 sudo apt install gcc pipewire-dev
 ```
 
 ## Building
 ```sh
 make
 ```
 ### Installing
 ```sh
 make install
 ```
 
 This will build the binary and copy it over to the `/opt/es-5-pipewire` directory.
