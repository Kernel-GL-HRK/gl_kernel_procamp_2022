# Home Task 1

Build your own kernel module for buildroot project

# Solution

## Build environment setup

### Build of buildroot

1) Download buildroot [sources](https://buildroot.org/download.html).
2) Unpack it and get into the unpacked folder
3) Do `make qemu_x86_64_defconfig`
4) Do `make menuconfig` and check `Target packages -> Networking applications -> dropbear`
5) Do `make` and wait for finish
6) Start it with `./output/images/start-qemu.sh`

> **NOTE**: replace `-net user` with `-net user,hostfwd=tcp::10022-:22` in `./output/images/start-qemu.sh`.

> **NOTE**: after each rebuilding of `qemu`, `./output/images/start-qemu.sh` need to be modified again.

### Setting-up QEMU

For creating user follow the next steps:

```sh
mkdir -p /home/user
adduser user
passwd <password>
chown user:user /home/user
```

## Building Kernel module

To build kernel module execute:
```sh
make
```

## Running

### Remote connection to QEMU

For remote connection execute:
```sh
ssh -p 10022 user@localhost
```

### Transferring files to QEMU

For secure copying files to QEMU host use the following command:
```sh
# scp -P [remote port] [local file] [user]@[host]:[remote path]
scp -P 10022 test-mod.ko user@localhost:~/
```

### Executing Kernel module

1. Run QEMU (see [Build of buildroot](#build-of-buildroot) - 6)
2. Copy module file to QEMU host ([Transferring files to QEMU](#transferring-files-to-qemu))
3. Load Kernel module:
    ```sh
    modprobe test-module.ko
    ```
4. Unload Kernel module:
    ```sh
    rmmod test-module
    ```

    > **TODO**: There is an error executing `modprobe` under the restricted user account.

    > **WORKAROUND**: for loading Kernel module run `insmod <module-name.ko>` under root account. 