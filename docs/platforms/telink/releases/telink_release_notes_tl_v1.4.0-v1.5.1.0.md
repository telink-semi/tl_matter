# Telink Matter SDK Release Note

[![Version](https://img.shields.io/badge/Version-tl_v1.4.0--v1.5-blue?style=flat-square)](https://github.com/telink-semi/tl_matter/releases/tag/tl_v1.4.0-v1.5)
[![License](https://img.shields.io/badge/License-Apache%202.0-red?style=flat-square)](LICENSE)
[![Matter](https://img.shields.io/badge/Matter-v1.5-green?style=flat-square)](https://github.com/project-chip/connectedhomeip/commit/f4a8cf98ada4ad4f439b45e360800693cc5f1391)

---

-   **Release Type:** Release
-   **Branch:**
    [dev-tlk_v1.5](https://github.com/telink-semi/tl_matter/tree/dev-tlk_v1.5)
-   **Tag Version:**
    [tl_v1.4.0-v1.5](https://github.com/telink-semi/tl_matter/releases/tag/tl_v1.4.0-v1.5)
-   **Target Commit:**
    [9511ac48](https://github.com/telink-semi/tl_matter/commit/9511ac48780c25c99eb0d8713fa995df8ecf547c)

---

## 📖 Introduction

This release is based on the target commit of `dev-tlk_v1.5` branch, providing
Matter protocol support for the Telink TL521X SoC platform. It integrates the
Matter SDK with the Telink Zephyr SDK to enable a Matter-over-Thread device on
TL521X (A0) with the lighting-app.

---

## ✨ Highlights

| Category           | Details                                    |
| ------------------ | ------------------------------------------ |
| **Matter Support** | Matter 1.5.1 protocol stack                |
| **Supported Chips**| TL521X                                     |
| **Sample Apps**    | Lighting (lighting-app)                    |
| **OTA**            | OTA requestor and compress-LZMA support    |
| **Factory Data**   | Factory data provisioning support          |
| **Dual Mode**      | Matter + Zigbee dual mode on 4MB flash     |

---

## 🆕 New Features

-   ✅ TL521X SoC platform support with lighting-app
-   ✅ Add flash overlays for TL5218X 2MB and 4MB flash variants, including LZMA
    compression and backup overlays
-   ✅ LZMA compression support for OTA images
-   ✅ Factory data provisioning support
-   ✅ Matter + Zigbee dual-mode support on 4MB flash

---

## 🐛 Bug Fixes

| Issue                    | Description                                                  |
| ------------------------ | ------------------------------------------------------------ |

---

## 📦 Updates

-   Updated Telink Zephyr SDK (tl_zephyr) to support the TL521x SoC family
    split and track the `dev-tlk_v4.1` branch
    ([commit:0858e43f05a91d84ab2fed77f3849ff589510b24](https://github.com/telink-semi/tl_zephyr/commit/0858e43f05a91d84ab2fed77f3849ff589510b24))
-   Updated Telink BLE SDK
    ([commit:3bddf885c0a1e4342393b7c6b668ece104c12102](https://github.com/telink-semi/tl_ble_sdk_zephyr/commit/3bddf885c0a1e4342393b7c6b668ece104c12102))
-   Updated Zephyr SDK references from `telink-semi/zephyr` to
    `telink-semi/tl_zephyr` (repo renamed, links updated accordingly)

---

### Telink Matter ↔ Telink Zephyr Dependency

The Telink Matter SDK is built **on top of** the Telink Zephyr SDK, and the two
are tightly coupled — they must be used as a matched pair.

The Telink Zephyr SDK provides the foundational layers required to bring up a
Matter device on Telink silicon: the Zephyr RTOS core, the Telink hardware
abstraction layer (HAL), the BLE stack, MCUBoot bootloader, OpenThread
networking stack, the WEST tool and build toolchain.

The Telink Matter SDK (this repository) builds upon that foundation to deliver
the Matter protocol stack itself together with a set of Telink Matter example
applications. Both components are required: the Matter SDK cannot be built
without the Zephyr SDK, and the Zephyr SDK alone does not provide Matter
support.

> ⚠️ **Version pairing:** This Telink Matter release is validated against a
> specific Telink Zephyr SDK tag (see the
> [Version Information](#version-information) table below). Using a different
> Zephyr revision may cause build failures or runtime issues.

---

## 📋 Version Information

### Matter SDK &amp; Toolchain

| Component              | Version                                                                                                                                                                                                                                                                                   |
| ---------------------- | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| **Matter SDK Version** | Telink Matter v1.5.1                                                                                                                                                                                                                                                                      |
| **Matter Branch**      | master                                                                                                                                                                                                                                                                                    |
| **Commit**             | [f4a8cf9](https://github.com/project-chip/connectedhomeip/commit/f4a8cf98ada4ad4f439b45e360800693cc5f1391) (the previous one of [15a68d7](https://github.com/project-chip/connectedhomeip/commit/15a68d7026b1cd34a0d4cf35dadc7558e503d2cb) that community upgraded Matter version to 1.6) |
| **Toolchain**          | Zephyr SDK v0.17.0 riscv64-zephyr-elf                                                                                                                                                                                                                                                     |

### Telink Matter SDK

| Property          | Version                                                                               |
| ----------------- | ------------------------------------------------------------------------------------- |
| **Branch**        | [dev-tlk_v1.5](https://github.com/telink-semi/tl_matter/tree/dev-tlk_v1.5)            |
| **Target Commit** | [9511ac48](https://github.com/telink-semi/tl_matter/commit/9511ac48780c25c99eb0d8713fa995df8ecf547c) |
| **Tag Name**      | [tl_v1.4.0-v1.5](https://github.com/telink-semi/tl_matter/releases/tag/tl_v1.4.0-v1.5) |
| **Release Type**  | Release                                                                           |

### Chip &amp; Hardware Versions

📦 **Chip Versions**

| Chip Family | Versions |
| ----------- | -------- |
| TL521X      | A0       |

🔧 **Hardware EVK Versions**

| Chip   | EVK Version    |
| ------ | -------------- |
| TL521X | C1T416A20_V1.0 |

---

## 📱 Matter Community Examples

This release provides a Telink platform port for the following Matter community
example application. It is located under `examples/<app-name>/telink/` in the
repository. The table below summarizes the example and the Telink chip platform
that supports it (see
[Telink Examples YAML file](.github/workflows/examples-telink.yaml)).

> ✅ = Supported and Tested

| Example      | Description                             | TL521X |
| ------------ | --------------------------------------- | :----: |
| lighting-app | Lighting (On/Off, Level, Color Control) |   ✅   |

### Notes on Platform Support

-   TL521X is the latest Telink RISC-V SoC family. This release supports the
    lighting-app on TL521X only.
-   For build commands per board/app, refer to the per-board `*_README.md` files
    inside each example's `boards/` directory.

---

## 📊 Resource Usage (Code Size)

This section shows the RAM and ROM usage for the supported Matter example on the
Telink TL521X platform, built with the Matter SDK and Zephyr RTOS.

### Supported Boards

| Board   | Chip Family |
| ------- | ----------- |
| tl5218x | TL521X      |

### TL521X (tl5218x) — Lighting App Code Size

#### TL521X (tl5218x)

📈 **Resource Usage Details**

| App              | Build Target                 | RAM_ILM_N                  | ROM                          | RAM                        |
| ---------------- | ---------------------------- | -------------------------- | ---------------------------- | -------------------------- |
| **lighting-app** | `build_tl5218x_4m_dual_mode` | 51390 B (39.21% of 128 KB) | 935562 B (47.09% of 1940 KB) | 87712 B (66.92% of 128 KB) |

> 📌 **Note:** TL521X currently only supports lighting-app. Memory usage data is
> extracted from the linker output (`Memory region` summary) of the build logs.
> `RAM_ILM_N` (instruction RAM) and `RAM` (data RAM) are 128 KB each; `ROM` is
> the application slot0 partition (1940 KB with the 4MB flash dual-mode overlay).
> For a detailed RAM/ROM symbol breakdown, run `west build -t ram_report` /
> `west build -t rom_report` in the build directory.

---

### 📝 Additional Notes

-   **Memory Regions:** May vary between chip variants; check individual board
    configurations
-   **Build Config:** Matter builds use `west build` directly from each
    example's `telink` directory; CI builds use the Matter `build_examples.py`
    system with Telink targets
-   **Production Optimizations:** For production builds, disable debug logging
    and enable appropriate optimizations to reduce RAM/ROM usage
-   **Bluetooth &amp; OpenThread:** Matter uses OpenThread for Thread networking
    and BLE for commissioning
-   **OTA Images:** Signed OTA images (`zephyr.signed.bin`) are generated when
    OTA is enabled

---

---

Made by Telink Semiconductor

-   [Website](https://www.telink-semi.com/)
-   [Forum](https://forum.telink-semi.cn/)
-   [Documentation](https://doc.telink-semi.cn/)
