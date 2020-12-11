# EDK2 UEFI Firmware For rk3399 SoC

This repo is based on edk2-sdm845 and rk3399-edk2, check out those if you have questions.

## Dependencies

Ubuntu 18.04:

```bash
sudo apt update
sudo apt install build-essential uuid-dev iasl git nasm python3-distutils gcc-aarch64-linux-gnu
```
Ubuntu 20.04 is also proved to be fine.


## Building

1.Clone edk2 and edk2-platforms (Place three directories side by side.)

edk2:
```
commit:3a3713e62cfad00d78bb938b0d9fb1eedaeff314
```

edk2-platforms:
```
commit:cfdc7f907d545b14302295b819ea078bc36c6a40
```

```bash
mkdir workspace-edk2
cd workspace-edk2
git clone https://github.com/tianocore/edk2.git -o 3a3713e62cfad00d78bb938b0d9fb1eedaeff314 --recursive --depth=1
git clone https://github.com/tianocore/edk2-platforms.git -o cfdc7f907d545b14302295b819ea078bc36c6a40 --recursive --depth=1
```

2.Clone this project(doesn't exist yet)

```bash
git clone https://github.com/edk2-porting/edk2-sdm845.git
```

3.Build this project
```bash
bash build.sh --device polaris
```

4.check out ./out/

## Credits

SimpleFbDxe driver is from imbushuo's [Lumia950XLPkg](https://github.com/WOA-Project/Lumia950XLPkg).
