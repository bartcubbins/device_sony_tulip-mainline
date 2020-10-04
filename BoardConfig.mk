#
# Copyright 2020, Pavel Dubrova <pashadubrova@gmail.com>
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# 

TARGET_BOARD_PLATFORM := msm8939

# Primary Arch
TARGET_ARCH := arm64
TARGET_ARCH_VARIANT := armv8-a
TARGET_CPU_VARIANT := cortex-a53
TARGET_CPU_ABI := arm64-v8a
TARGET_CPU_ABI2 :=

# Secondary Arch
TARGET_2ND_ARCH := arm
TARGET_2ND_ARCH_VARIANT := armv8-a
TARGET_2ND_CPU_VARIANT := cortex-a53
TARGET_2ND_CPU_ABI := armeabi-v7a
TARGET_2ND_CPU_ABI2 := armeabi

# Kernel cmdline parameters
BOARD_KERNEL_CMDLINE += androidboot.hardware=tulip androidboot.boot_devices=soc/7824900.sdhci
BOARD_KERNEL_CMDLINE += androidboot.memcg=1 cgroup.memory=nokmem
BOARD_KERNEL_CMDLINE += androidboot.selinux=permissive

BOARD_KERNEL_BASE        := 0x80000000
BOARD_KERNEL_PAGESIZE    := 2048
BOARD_KERNEL_TAGS_OFFSET := 0x01E00000
BOARD_RAMDISK_OFFSET     := 0x02000000

TARGET_NO_BOOTLOADER := true
TARGET_NO_KERNEL := false
TARGET_NO_RECOVERY := false

# boot.img
BOARD_BOOTIMAGE_PARTITION_SIZE := 41943040

# system.img
BOARD_SYSTEMIMAGE_PARTITION_SIZE := 2675703808
BOARD_SYSTEMIMAGE_FILE_SYSTEM_TYPE := ext4

# userdata.img
# 16 Gb
# Reserve space for data encryption (12213813248-16384)
BOARD_USERDATAIMAGE_PARTITION_SIZE := 12213796864
# 8 Gb
# Reserve space for data encryption (4399824896-16384)
# BOARD_USERDATAIMAGE_PARTITION_SIZE := 4399808512
TARGET_USERIMAGES_USE_EXT4 := true

# vendor.img
BOARD_VENDORIMAGE_PARTITION_SIZE := 209715200
BOARD_VENDORIMAGE_FILE_SYSTEM_TYPE := ext4
TARGET_COPY_OUT_VENDOR := vendor

BOARD_FLASH_BLOCK_SIZE := 131072 # (BOARD_KERNEL_PAGESIZE * 64)

# Use mke2fs to create ext4 images
TARGET_USES_MKE2FS := true

# Mesa DRM hwcomposer
BOARD_USES_DRM_HWCOMPOSER := true
BOARD_GPU_DRIVERS := freedreno virgl
TARGET_USES_HWC2 := true

# BT
BOARD_HAVE_BLUETOOTH := true

# WiFi
WPA_SUPPLICANT_VERSION := VER_0_8_X
BOARD_WPA_SUPPLICANT_DRIVER := NL80211
BOARD_HOSTAPD_DRIVER := NL80211
BOARD_WLAN_DEVICE := qcwcn
BOARD_HOSTAPD_PRIVATE_LIB := lib_driver_cmd_$(BOARD_WLAN_DEVICE)

BOARD_SEPOLICY_DIRS += device/sony/tulip-mainline/sepolicy

DEVICE_MANIFEST_FILE := device/sony/tulip-mainline/vintf/manifest.xml
DEVICE_MATRIX_FILE := device/sony/tulip-mainline/vintf/compatibility_matrix.xml

# Enable dex pre-opt to speed up initial boot
ifeq ($(HOST_OS),linux)
  ifeq ($(WITH_DEXPREOPT),)
    WITH_DEXPREOPT := true
  endif
endif
