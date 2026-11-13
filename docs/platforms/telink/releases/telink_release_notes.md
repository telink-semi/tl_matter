# Telink Matter SDK Release Note

[![Version](https://img.shields.io/badge/Version-tl__v1.6.0--v1.6.1-blue?style=flat-square)](https://github.com/telink-semi/tl_matter/releases/tag/tl_v1.6.0-v1.6.1)
[![License](https://img.shields.io/badge/License-Apache%202.0-red?style=flat-square)](../../../../LICENSE)
[![Matter](https://img.shields.io/badge/Matter-v1.6.1-green?style=flat-square)](https://github.com/project-chip/connectedhomeip/commit/3bcdd56ba54fb88b2afb4bfef575014671df7aa7)

---

-   **Release Type:** Public Release
-   **Branch:**
    [dev-tlk_v1.6.1](https://github.com/telink-semi/tl_matter/tree/dev-tlk_v1.6.1)
-   **Tag Version:**
    [tl_v1.6.0-v1.6.1](https://github.com/telink-semi/tl_matter/releases/tag/tl_v1.6.0-v1.6.1)

---

## 📖 Introduction

This release is based on the `dev-tlk_v1.6.1` branch, providing Matter protocol
support for Telink RISC-V SoC platforms. It integrates the Matter SDK (based on
project-chip/connectedhomeip v1.6.1) with the Telink Zephyr SDK to enable
Matter-over-Thread devices on Telink chips including TL323X, TL521X, and
TL721X.

This release focuses on validated always-on (non-power-managed) support for
the TL521X platform, with the lighting-app as the primary tested application.

---

## 🆕 Changes in tl_v1.6.0-v1.6.1

This release is rebased from the previous tl\_v1.4.1-v1.5 branch onto the
upstream Matter v1.6.1 branch
([`3bcdd56`](https://github.com/project-chip/connectedhomeip/commit/3bcdd56ba54fb88b2afb4bfef575014671df7aa7)).
It incorporates all changes from tl\_v1.5.0 (including TL521X initial support,
dual-mode, and dependency updates) and brings the Matter stack from v1.5 to
v1.6.1.

Key changes on top of tl\_v1.4.1-v1.5:

- **Matter SDK:** Upgrade from upstream Matter v1.5 to v1.6.1 (commit `3bcdd56`)
- **Platform:** Add TL3228X SoC support ([#83](https://github.com/telink-semi/tl_matter/pull/83))
- **Platform:** Retarget dual-mode fixes onto dev-tlk_v1.6.1 — split BLEManager, concurrent PM feature ([#83](https://github.com/telink-semi/tl_matter/pull/83))
- **Platform:** Unify concurrent-mode config macros and harden BLE teardown/resume ([#89](https://github.com/telink-semi/tl_matter/pull/89))
- **Platform:** Bump tl_zephyr to latest dev-tlk_v4.1 revision (`0f52556`) ([#92](https://github.com/telink-semi/tl_matter/pull/92))
- **Bug fixes:**
  - SNTP get time from OpenThread network
  - Fix NFC device advertisement ([#74607](https://github.com/project-chip/connectedhomeip/pull/74607))
  - Fix IPV6_RECVPKTINFO socket option
  - Add multi-endpoint Identify support ([#73754](https://github.com/project-chip/connectedhomeip/pull/73754))
  - Graceful Detach during Thread attach if attached ([#73600](https://github.com/project-chip/connectedhomeip/pull/73600))
  - Fix re-adding SRP service pending removal ([#74456](https://github.com/project-chip/connectedhomeip/pull/74456))
  - Fix LZMA/bootloader mismatch, key_pool glitch and packaging gaps ([#88](https://github.com/telink-semi/tl_matter/pull/88))
  - Align BLE adv button handler naming with upstream master
  - Bump cryptography from 48.0.1 to 50.0.0 in /scripts/tools/telink
- **CI:** Add CHIP_ENABLE_GROUPCAST config for lighting-app
- **Docs:** Update Zephyr SDK references to tl_zephyr; add backport rules

---

## ✨ Highlights

| Category                  | Details                                                |
| ------------------------- | ------------------------------------------------------ |
| **Matter Support**        | Matter 1.6.1 protocol stack                            |
| **Supported Chips**       | TL323X, TL521X, TL721X                                 |
| **Tested Chip & App**     | TL521X lighting-app (always-on, non-PM)                |
| **Sample Apps**           | Lighting, Light Switch, Lock, Bridge, Thermostat, etc. |
| **OTA**                   | OTA requestor and compress-LZMA support                |
| **Factory Data**          | Factory data provisioning support                      |
| **Dual Mode**             | Matter + Zigbee dual-mode on 4 MB flash                |
<!-- | **BLE/Thread Concurrent** | BLE + Thread concurrent mode on TL323X and TL721X      | -->
<!-- | **Channel Sounding**      | CS RAS reflector support on TL721X                     | -->
<!-- | **Power Management**      | Retention RAM support for low-power operation           | -->

---

## 🆕 New Features

-   ✅ Matter v1.6.1 protocol support (upgraded from v1.5)
-   ✅ TL3228X SoC platform support ([#83](https://github.com/telink-semi/tl_matter/pull/83))
-   ✅ BLE/Thread concurrent mode with split BLEManager architecture ([#83](https://github.com/telink-semi/tl_matter/pull/83))
-   ✅ Unified concurrent-mode config macros and hardened BLE teardown/resume ([#89](https://github.com/telink-semi/tl_matter/pull/89))
-   ✅ Channel Sounding (CS) RAS reflector (CsReflector) for TL7218X
-   ✅ LZMA compression support for OTA images
-   ✅ Factory data provisioning support
-   ✅ Matter + Zigbee dual-mode support on 4MB flash
-   ✅ Power management with retention RAM support
-   ✅ SNTP time synchronization from OpenThread network
-   ✅ Add CHIP_ENABLE_GROUPCAST config for lighting-app

---

## 🐛 Bug Fixes

| Issue | Component | Description |
| ----- | --------- | ----------- |
| [#88](https://github.com/telink-semi/tl_matter/pull/88) | Build/OTA | Fix LZMA/bootloader mismatch, key_pool glitch and packaging gaps |
| [#89](https://github.com/telink-semi/tl_matter/pull/89) | BLE/PM | Unify concurrent-mode config macros and harden BLE teardown/resume |
| [#74607](https://github.com/project-chip/connectedhomeip/pull/74607) | NFC | Fix NFC device advertisement |
| [#73754](https://github.com/project-chip/connectedhomeip/pull/73754) | Identify | Add multi-endpoint Identify support |
| [#73600](https://github.com/project-chip/connectedhomeip/pull/73600) | Thread | Graceful Detach during Thread attach if attached |
| [#74456](https://github.com/project-chip/connectedhomeip/pull/74456) | OpenThread | Fix re-adding SRP service pending removal |
| — | Network | Fix IPV6_RECVPKTINFO socket option |
| — | BLE | Align BLE adv button handler naming with upstream master |
| — | Dependencies | Bump cryptography from 48.0.1 to 50.0.0 in /scripts/tools/telink |

---

## 📦 Updates / Dependencies

| Component                 | Repository                                                                          | Commit                                                                                                         | Notes                                                      |
| ------------------------- | ----------------------------------------------------------------------------------- | -------------------------------------------------------------------------------------------------------------- | ---------------------------------------------------------- |
| **Telink Zephyr SDK**     | [telink-semi/tl_zephyr](https://github.com/telink-semi/tl_zephyr)                   | [`0f52556`](https://github.com/telink-semi/tl_zephyr/commit/0f52556eb0f7a1f45d64c37a3afe06c566bb3991)          | Tag: `tl_v1.6.0-beta-v4.1.0`; TL523X BLE, central, GPIO fix, SWS fix |
| **Telink BLE SDK**        | [telink-semi/tl_ble_sdk_zephyr](https://github.com/telink-semi/tl_ble_sdk_zephyr)   | [`8c43a9d`](https://github.com/telink-semi/tl_ble_sdk_zephyr/commit/8c43a9d44b146ce58e23ffcb29a483aa06b29383) | Tag: `tl_v1.6.0-beta-v4.0.4.8`; peak BTR update           |
| **Telink HAL Zephyr**     | [telink-semi/hal_telink](https://github.com/telink-semi/hal_telink)                 | [`3cc9054`](https://github.com/telink-semi/hal_telink/commit/3cc9054edd5edcaa1a8b5870099af81433afaf58)         | Tag: `tl_v1.6.0-beta-v4.0.4.8`; peripheral_ht + central   |
| **MCUBoot**               | [telink-semi/tl_mcuboot](https://github.com/telink-semi/tl_mcuboot)                 | [`ce0da85`](https://github.com/telink-semi/tl_mcuboot/commit/ce0da85c39c749df49b0ec62b33d2ecdea24c927)         | Unchanged since tl\_v1.4.1-v1.5; bootloader; required for OTA/DFU |
| **OpenThread Telink**     | [telink-semi/tl_openthread](https://github.com/telink-semi/tl_openthread)           | [`542aaab`](https://github.com/telink-semi/tl_openthread/commit/542aaab44e1308e1a8a24573dfbd413fade342ee)      | Unchanged since tl\_v1.4.1-v1.5; OpenThread source adapted for Telink |
| **OpenThread Telink Lib** | [telink-semi/tl_openthread_libs](https://github.com/telink-semi/tl_openthread_libs) | [`bb57bac`](https://github.com/telink-semi/tl_openthread_libs/commit/bb57bac0857cdf4f408894ffdd9c8469d25c66cb) | Tag: `tl_v1.6.0-v1.4`; pre-built OpenThread library for Telink |
| **Telink XZ (LZMA)**      | [telink-semi/tl_xz](https://github.com/telink-semi/tl_xz)                           | [`831f338`](https://github.com/telink-semi/tl_xz/commit/831f338fd6784661d3bec62fd01060ee4d7d373d)              | Unchanged since tl\_v1.4.1-v1.5; LZMA compression library module |

### Dependency Changes Since tl\_v1.4.1-v1.5

- `tl_zephyr`: `37a3359e` → `0f52556` (tag: `tl_v1.6.0-beta-v4.1.0`)
- `tl_ble_sdk_zephyr`: `aff054a7` → `8c43a9d` (tag: `tl_v1.6.0-beta-v4.0.4.8`)
- `hal_telink`: `31f167f` → `3cc9054` (tag: `tl_v1.6.0-beta-v4.0.4.8`)
- `tl_openthread_libs`: `f69c186` → `bb57bac` (tag: `tl_v1.6.0-v1.4`, retroactive)
- `tl_mcuboot`, `tl_openthread`, and `tl_xz` remain unchanged from tl\_v1.4.1-v1.5.

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
> specific Telink Zephyr SDK revision (see the
> [Version Information](#version-information) table below). Using a different
> Zephyr revision may cause build failures or runtime issues.

---

## Version Information {#version-information}

### Matter SDK &amp; Toolchain

| Component              | Version                                                                                                                                                       |
| ---------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| **Matter SDK Version** | Telink Matter v1.6.1                                                                                                                                          |
| **Upstream Base**      | [3bcdd56](https://github.com/project-chip/connectedhomeip/commit/3bcdd56ba54fb88b2afb4bfef575014671df7aa7) (upstream Matter v1.6.1.0)                         |
| **Toolchain**          | Zephyr SDK v0.17.0 riscv64-zephyr-elf                                                                                                                         |

### Telink Matter SDK

| Property          | Version                                                                                            |
| ----------------- | -------------------------------------------------------------------------------------------------- |
| **Branch**        | [dev-tlk_v1.6.1](https://github.com/telink-semi/tl_matter/tree/dev-tlk_v1.6.1)                     |
| **Tag Name**      | [tl_v1.6.0-v1.6.1](https://github.com/telink-semi/tl_matter/releases/tag/tl_v1.6.0-v1.6.1)         |
| **Release Type**  | Public Release                                                                                    |

### Telink Zephyr SDK

| Property   | Version                                                                                             |
| ---------- | --------------------------------------------------------------------------------------------------- |
| **Commit** | [0f52556](https://github.com/telink-semi/tl_zephyr/commit/0f52556eb0f7a1f45d64c37a3afe06c566bb3991) |
| **Tag**    | [tl_v1.6.0-beta-v4.1.0](https://github.com/telink-semi/tl_zephyr/releases/tag/tl_v1.6.0-beta-v4.1.0) |

### Chip &amp; Hardware Versions

📦 **Chip Versions**

| Chip Family | Versions |
| ----------- | -------- |
| TL323X      | A0/A1    |
| TL521X      | A0       |
| TL721X      | A2/A3    |

🔧 **Hardware EVK Versions**

| Chip   | EVK Version    |
| ------ | -------------- |
| TL323X | C1T388A20_V1.1 |
| TL521X | C1T416A20_V1.0/C1T417A20_V1.0 |
| TL721X | C1T315A20_V1.5 |

---

## ⚠️ Important Notes

**This is a public release version for demonstration and testing purposes.**

---

## ⚠️ Known Issues and Limitations

-   **Power Management:** PM (retention RAM) mode on TL521X is not validated in
    this release; only always-on (non-PM) mode is tested.
-   **BLE/Thread Concurrent Mode:** Concurrent mode on TL323X and TL721X is
    implemented but not fully validated in this release.
-   **Channel Sounding:** CS RAS reflector support is implemented for TL7218X
    but not tested.
-   **TL3228X:** Platform support is new and builds are pending functional
    validation.
-   **Multi-device Commissioning:** Demo apps share the same default Setup
    discriminator and passcode. See
    [Getting Started Guide](../telink_getting_started.md#multi-device-commissioning)
    for multi-device setup instructions.
-   **Factory Data:** Demo mode (without factory data) uses compile-time default
    credentials. Production devices must provision unique credentials via
    factory data.

---

## 📱 Matter Community Examples

This release provides Telink platform ports for the following Matter community
example applications. Each example is located under
`examples/<app-name>/telink/` in the repository.

> ✅ = Supported and Tested &nbsp;&nbsp; 🟡 = Compiles but Untested
> &nbsp;&nbsp; · = Not Built

| Example          | Description                                                | TL323X | TL521X | TL721X |
| ---------------- | ---------------------------------------------------------- | :----: | :----: | :----: |
| lighting-app     | Lighting (On/Off, Level, Color Control)                    |   🟡   |   ✅   |   🟡   |
| light-switch-app | Light Switch (controller, switches a bound lighting-app)   |   🟡   |   ·    |   🟡   |
| bridge-app       | Bridge / Aggregator (bridges non-Matter devices to Matter) |   ·    |   ·    |   🟡   |
| window-app       | Window Covering (shades, blinds)                           |   ·    |   ·    |   🟡   |

### Notes on Platform Support

-   **Tested combination (✅):** TL521X lighting-app (always-on, non-PM) has been
    fully tested in this release.
-   **Compiles but untested (🟡):** The following chip/app combinations compile
    successfully but have not been functionally validated in this release:
    -   TL323X lighting-app and light-switch-app
    -   TL721X lighting-app, light-switch-app, bridge-app, and window-app
-   **Not built (·):** Combinations not listed are not built or validated.
-   TL323X / TL521X / TL721X are the latest Telink RISC-V SoC families.
-   The light-switch-app uses the `*_retention` board target (power management
    with retention RAM).
-   For build commands per board/app, refer to the per-board `*_README.md` files
    inside each example's `boards/` directory.

---

## 📊 Resource Usage (Code Size)

The following resource usage data is extracted from actual builds using the
`package_telink_firmware.sh` script. Memory regions may vary between chip
variants and build configurations.

### Supported Boards

| Board             | Chip Family |
| ----------------- | ----------- |
| `tl5218x`         | TL521X      |
| `tl3238x`         | TL323X      |
| `tl7218x`         | TL721X      |
| `tl3238x_retention` | TL323X    |
| `tl7218x_retention` | TL721X    |

### Lighting App — TL5218X (Always-on, 2 MB Flash)

| Configuration                | RAM_ILM_N  | ROM          | RAM          |
| ---------------------------- | ---------- | ------------ | ------------ |
| Default (no OTA, no MCUBoot) | 51,356 B   | 909,656 B    | 85,472 B     |
| OTA + LZMA v1                | 51,216 B   | 963,840 B    | 91,752 B     |
| OTA + LZMA v2                | 51,216 B   | 963,840 B    | 91,752 B     |
| Dual-mode (4 MB Flash)       | 51,216 B   | 964,656 B    | 91,816 B     |

> Region sizes for TL5218X: RAM_ILM_N = 128 KB, ROM = 2 MB (default) or 1940 KB
> (dual-mode with MCUBoot), RAM = 128 KB.

### Lighting App — TL3238X (2 MB Flash)

| Configuration                | RAM_ILM_N  | ROM          | RAM          |
| ---------------------------- | ---------- | ------------ | ------------ |
| Default (no OTA, no MCUBoot) | 53,820 B   | 933,764 B    | 86,008 B     |
| OTA + LZMA v1                | 53,820 B   | 987,668 B    | 92,224 B     |
| OTA + LZMA v2                | 53,820 B   | 987,668 B    | 92,224 B     |
| Dual-mode (4 MB Flash)       | 53,820 B   | 988,484 B    | 92,288 B     |

> Region sizes for TL3238X: RAM_ILM_N = 64 KB, ROM = 2 MB (default) or 1940 KB
> (dual-mode with MCUBoot), RAM = 96 KB.

### Lighting App — TL7218X (2 MB Flash)

| Configuration                | RAMILM     | ROM          | RAM          |
| ---------------------------- | ---------- | ------------ | ------------ |
| Default (no OTA, no MCUBoot) | 88,816 B   | 912,036 B    | 55,448 B     |
| OTA + LZMA v1                | 92,512 B   | 966,290 B    | 57,960 B     |
| OTA + LZMA v2                | 92,512 B   | 966,290 B    | 57,960 B     |
| Concurrent (BLE always-on)   | 93,184 B   | 921,432 B    | 55,592 B     |

> Region sizes for TL7218X: RAMILM = 256 KB, ROM = 2 MB (default) or 1152 KB
> (with MCUBoot), RAM = 256 KB.

### Light Switch App — TL3238X Retention (2 MB Flash)

| Configuration                | RAM_ILM_N  | ROM          | RAM          |
| ---------------------------- | ---------- | ------------ | ------------ |
| Default (no OTA, no MCUBoot) | 63,716 B   | 893,234 B    | 80,792 B     |
| OTA + LZMA v1                | 63,794 B   | 941,402 B    | 87,000 B     |
| OTA + LZMA v2                | 63,794 B   | 941,402 B    | 87,000 B     |
| Dual-mode (4 MB Flash)       | 63,794 B   | 941,662 B    | 87,000 B     |

> Region sizes for TL3238X retention: RAM_ILM_N = 64 KB, ROM = 2 MB (default)
> or 1152 KB (with MCUBoot), RAM = 96 KB.

### Light Switch App — TL7218X Retention (2 MB Flash)

| Configuration                       | RAM_ILM_N  | ROM          | RAM          |
| ----------------------------------- | ---------- | ------------ | ------------ |
| Default (no OTA, no MCUBoot)        | 54,862 B   | 871,109 B    | 100,264 B    |
| OTA + LZMA v1                       | 54,958 B   | 919,318 B    | 106,480 B    |
| OTA + LZMA v2                       | 54,958 B   | 919,318 B    | 106,480 B    |
| Concurrent BLE-off + OTA v1         | 61,506 B   | 929,477 B    | 105,064 B    |
| Concurrent BLE-off + OTA v2         | 61,506 B   | 929,477 B    | 105,064 B    |

> Region sizes for TL7218X retention: RAM_ILM_N = 128 KB, ROM = 2 MB (default)
> or 1152 KB (with MCUBoot), RAM = 128 KB.

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
-   **Multi-device Commissioning:** Each demo app uses the same default Setup
    discriminator (0xF00) and passcode (20202021). To commission multiple
    devices simultaneously, each device must have a unique discriminator and
    Serial Number. See the
    [Getting Started Guide](../telink_getting_started.md#multi-device-commissioning)
    for details.
-   **Detailed RAM/ROM Breakdown:** For a per-symbol memory breakdown, run
    `west build -t ram_report` or `west build -t rom_report` in the build
    directory.

---

---

Made by Telink Semiconductor

-   [Website](https://www.telink-semi.com/)
-   [Forum](https://forum.telink-semi.cn/)
-   [Documentation](https://doc.telink-semi.cn/)
