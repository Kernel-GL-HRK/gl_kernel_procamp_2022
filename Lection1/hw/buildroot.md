Setup Buildroot environment
---------------------------

1. Get all packages installed from https://buildroot.org/downloads/manual/manual.html#requirement-mandatory and https://buildroot.org/downloads/manual/manual.html#requirement-optional lists.
2. Get buildroot at https://buildroot.org/download.html.
3. Unpack it and get into the unpacked folder.
4. Do `make qemu_x86_64_defconfig`.
5. Do `make menuconfig` and check `Target packages -> Networking applications -> dropbear`.
6. Do `make` and wait for finish.
7. Start it with `./output/images/start-qemu.sh`
8. Replace `-net user` with `-net user,hostfwd=tcp::10022-:22` in `./output/images/start-qemu.sh`.
9. Restart the emulator using previous script.
10. Login in emulator as `root`.
11. Execute in shell:
```
# mkdir -p /home/user 
# adduser user
# passwd user (set password “user”)
# chown user:user /home/user
```
12. Execute `ssh -p 10022 user@localhost`
