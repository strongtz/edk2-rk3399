# EDK2 UEFI Firmware For rk3399 SoC

This repo is based on edk2-sdm845 and rk3399-edk2, check out those if you have questions.

Telegram group: https://t.me/UEFIonRK3399

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
git clone https://github.com/edk2-porting/edk2-rk3399.git
```

Workspace tree like:

```
workspace-edk2
├── edk2
├── edk2-platforms
└── edk2-sdm845
```

3.Build this project

```bash
bash build.sh --device polaris
```

4.check out edk2-sdm845/workspace/Build/sdm845Pkg/DEBUG_GCC5/FV/SDM845PKG_UEFI.fd

## Boot

This edk2 build is a second stage boot image which needs to be loaded by u-boot sysboot (extlinux).

In order to make SimpleFB working properly, the u-boot should initialize the framebuffer, which you can determine by whether a logo showing during u-boot stage or not.

### Change FB base & resolution

Vendor u-boot fb base is `0xF5F00000` and resolution is `800x480`.

Change FB base in sdm845Pkg.dsc#L140 and resolution in polaris.dsc

Also, if you are using Rockchip DRM driver in U-boot, you can also do some hack to alter the resolution of framebuffer.

```diff
--- a/drivers/video/drm/rockchip_display.c
+++ b/drivers/video/drm/rockchip_display.c
@@ -759,6 +759,8 @@ static int display_logo(struct display_state *state)
        if (!state->is_init)
                return -ENODEV;
 
+       printf("logo bpp: = %d\n", logo->bpp);
+       logo->bpp = 32; // force bpp into 32
        switch (logo->bpp) {
        case 16:
                crtc_state->format = ROCKCHIP_FMT_RGB565;
@@ -775,17 +777,28 @@ static int display_logo(struct display_state *state)
        }
        crtc_state->rb_swap = logo->bpp != 32;
        hdisplay = conn_state->mode.hdisplay;
+       printf("hdisplay: = %d\n", hdisplay);
        vdisplay = conn_state->mode.vdisplay;
-       crtc_state->src_w = logo->width;
-       crtc_state->src_h = logo->height;
+       printf("vdisplay: = %d\n", vdisplay);
+       // crtc_state->src_w = logo->width;
+       printf("Force display into 1440x900\n");
+       crtc_state->src_w = 1440;
+       printf("logo width: = %d\n", logo->width);
+       // crtc_state->src_h = logo->height;
+       crtc_state->src_h = 900;
+       printf("logo height: = %d\n", logo->height);
        crtc_state->src_x = 0;
        crtc_state->src_y = 0;
        crtc_state->ymirror = logo->ymirror;
 
-       crtc_state->dma_addr = (u32)(unsigned long)logo->mem + logo->offset;
+       crtc_state->dma_addr = (u32)(unsigned long)logo->mem; // + logo->offset;
+       printf("dma_addr: = 0x%08x\n", crtc_state->dma_addr);
+       printf("logo addr: = 0x%08x\n", (u32)(unsigned long)logo->mem);
        crtc_state->xvir = ALIGN(crtc_state->src_w * logo->bpp, 32) >> 5;
 
+       logo->mode = ROCKCHIP_DISPLAY_FULLSCREEN; // Force into FULLSCREEN
        if (logo->mode == ROCKCHIP_DISPLAY_FULLSCREEN) {
                crtc_state->crtc_x = 0;
                crtc_state->crtc_y = 0;
                crtc_state->crtc_w = hdisplay;
```

### Create extlinux.conf

```
label whatever
        kernel /SDM845PKG_UEFI.fd
```

### Put extlinux.conf & edk2 into sdcard

```
mkdir efi
sudo mount /dev/sdb4 efi # replace /dev/sdb4 to the partition of your sdcard
sudo cp extlinux.conf efi
sudo cp SDM845PKG_UEFI.fd efi
sudo umount efi
```

### Boot edk2 via u-boot

U-boot can automatically boot extlinux, and you can also boot manually.

```
# change mmc 1:4 to your sdcard device & partition
sysboot mmc 1:4 any 0x00500000 /extlinux.conf
```

## Known issues

* edk2 input not working in serial

## Credits

SimpleFbDxe driver is from imbushuo's [Lumia950XLPkg](https://github.com/WOA-Project/Lumia950XLPkg).

