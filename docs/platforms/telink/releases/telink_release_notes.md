# Telink Matter SDK Release Note

[![Version](https://img.shields.io/badge/Version-tl__v1.5.0--v1.5-blue?style=flat-square)](https://github.com/telink-semi/tl_matter/releases/tag/tl_v1.5.0-v1.5)
[![License](https://img.shields.io/badge/License-Apache%202.0-red?style=flat-square)](../../../../LICENSE)
[![Matter](https://img.shields.io/badge/Matter-v1.5-green?style=flat-square)](https://github.com/project-chip/connectedhomeip/commit/f4a8cf98ada4ad4f439b45e360800693cc5f1391)

---

-   **Release Type:** Release
-   **Tag Version:**
    [tl_v1.5.0-v1.5](https://github.com/telink-semi/tl_matter/releases/tag/tl_v1.5.0-v1.5)

---

## 📖 Introduction

This release is the follow-up to `tl_v1.4.1-v1.5` and keeps the Matter 1.5
protocol stack while extending the Telink Matter platform:

-   Adds **BLE + Thread concurrent power management** (**TL721X only**): BLE is
    fully torn down after commissioning (RF handed permanently to 802.15.4 for
    the lowest SED power), with an optional on-demand BLE resume used for BLE
    OTA.
-   Moves the underlying Telink Zephyr SDK to the **tl_v1.5.0-v4.1.0** line
    (based on upstream Zephyr v4.1.0).

---

## 🆕 Changes in tl_v1.5.0

The following changes land on top of tl_v1.4.1:

-   **New SoC:** TL3228X platform support with `tl3228x_2m_flash` and
    `tl3228x_2m_flash_lzma` overlays, SoC-family Kconfig clean-up and build
    targets for the smoke-CO-alarm and window-covering examples.
-   **Power management (TL721X only):** Add concurrent-mode BLE off/resume after
    commissioning ([#84](https://github.com/telink-semi/tl_matter/pull/84)):
    `CHIP_CONCURRENT_BLE_OFF_AFTER_COMMISSIONING` tears down the BLE stack once
    the device is commissioned and running on Thread, and
    `CHIP_CONCURRENT_BLE_RESUME_AFTER_COMMISSIONING` allows an on-demand BLE
    restart (button press) for BLE OTA while Thread stays attached. Both options
    are currently only supported on TL721X.
-   **Dependencies:** Update the Telink Zephyr SDK to
    [`3cd5848c`](https://github.com/telink-semi/tl_zephyr/commit/3cd5848c2a76f029097e582ccb3d27211f9cae9d)
    (`tl_v1.5.0-v4.1.0`) and the Telink BLE SDK to
    [`eb0d1206`](https://github.com/telink-semi/tl_ble_sdk_zephyr/commit/eb0d12063db5606db8bca5526ce9b54c77c45797).
-   **CI:** Fix the Chef workflow and the disabled workflow file suffixes
    (`disalbed`/`dsiabled` → `disabled`), and refresh the CI build matrix for
    the TL321X/TL322X/TL323X/TL521X/TL721X targets
    ([#54](https://github.com/telink-semi/tl_matter/pull/54)).
-   **Docs:** Refresh the getting started links and the release notes structure
    ([#57](https://github.com/telink-semi/tl_matter/pull/57)).

---

## ✨ Highlights

| Category            | Details                                              |
| ------------------- | ---------------------------------------------------- |
| **Matter Support**  | Matter 1.5 protocol stack                            |
| **Supported Chips** | TL721X                                               |
| **Sample Apps**     | light-switch                                         |
| **OTA**             | OTA requestor and compress-LZMA support              |
| **Factory Data**    | Factory data provisioning support                    |
| **Low Power**       | BLE + Thread concurrent BLE off/resume (TL721X only) |
| **Dual Mode**       | Matter + Zigbee dual mode on 4 MB flash              |

---

## 🆕 New Features

-   ✅ TL3228X SoC platform support with 2 MB flash and LZMA overlays
-   ✅ smoke-CO-alarm example on TL3228X (retention build)
-   ✅ window-covering example on TL3228X with OTA, LZMA compression and factory
    data
-   ✅ Concurrent-mode BLE teardown after commissioning (**TL721X only**): BLE
    stack is disabled and the RF is handed permanently to 802.15.4 for the
    lowest SED power state
    ([#84](https://github.com/telink-semi/tl_matter/pull/84))
-   ✅ On-demand BLE resume for BLE OTA on a commissioned Thread device
    (**TL721X only**): the existing "start BLE advertising" button restores the
    BLE/802.15.4 RF time-sharing, re-initializes the BLE stack and starts a
    connectable advertisement, while Matter commissioning data is untouched
    ([#84](https://github.com/telink-semi/tl_matter/pull/84))
-   ✅ LZMA compression support for OTA images across the supported TLX boards
-   ✅ Factory data provisioning support across the supported TLX boards
-   ✅ Matter + Zigbee dual-mode support on 4 MB flash

> **Note:** The BLE off/resume power-management options are currently only
> supported on **TL721X** (see `CHIP_CONCURRENT_BLE_OFF_AFTER_COMMISSIONING` and
> `CHIP_CONCURRENT_BLE_RESUME_AFTER_COMMISSIONING` in
> `config/telink/chip-module/Kconfig`), matching the
> `boards/tl7218x_retention_concurrent.conf` and
> `boards/tl7218x_retention_ble_off.conf` board configurations. The resume path
> additionally requires a `hal_telink` revision providing
> `tlx_bt_802154_dual_mode_resume()`.

---

## 🐛 Bug Fixes

| Issue                                                   | Component | Description                                                                                                                                     |
| ------------------------------------------------------- | --------- | ----------------------------------------------------------------------------------------------------------------------------------------------- |
| [#54](https://github.com/telink-semi/tl_matter/pull/54) | CI        | Fix invalid `chef.yaml` YAML (`if:false` → `if: false`) and incorrect disabled workflow file suffixes                                           |
| [#84](https://github.com/telink-semi/tl_matter/pull/84) | BLE/PM    | Restore `InitBLEMACAddress()` in `_Init()` so non-concurrent builds keep creating the BLE identity from the controller MAC before `bt_enable()` |
| TL322X                                                  | SoC/Build | Fix TL322X build issues, including the IEEE 802.15.4 driver, in the `tl_v1.5.0-v4.1.0` Telink Zephyr SDK                                        |

---

## 📦 Updates / Dependencies

| Component                 | Repository                                                                          | Commit                                                                                                         | Notes                                     |
| ------------------------- | ----------------------------------------------------------------------------------- | -------------------------------------------------------------------------------------------------------------- | ----------------------------------------- |
| **Telink Zephyr SDK**     | [telink-semi/tl_zephyr](https://github.com/telink-semi/tl_zephyr)                   | [`3cd5848c`](https://github.com/telink-semi/tl_zephyr/commit/3cd5848c2a76f029097e582ccb3d27211f9cae9d)         | `tl_v1.5.0-v4.1.0`, Zephyr v4.1.0 line    |
| **Telink BLE SDK**        | [telink-semi/tl_ble_sdk_zephyr](https://github.com/telink-semi/tl_ble_sdk_zephyr)   | [`eb0d1206`](https://github.com/telink-semi/tl_ble_sdk_zephyr/commit/eb0d12063db5606db8bca5526ce9b54c77c45797) | Paired with the Zephyr SDK above          |
| **Telink HAL Zephyr**     | [telink-semi/hal_telink](https://github.com/telink-semi/hal_telink)                 | [`81ded240`](https://github.com/telink-semi/hal_telink/commit/81ded2409cb3d1461818bf3899f0c76d57f4a4d0)        | Contains hal_v1 and hal_v2 sources        |
| **MCUBoot**               | [telink-semi/tl_mcuboot](https://github.com/telink-semi/tl_mcuboot)                 | [`ce0da85`](https://github.com/telink-semi/tl_mcuboot/commit/ce0da85c39c749df49b0ec62b33d2ecdea24c927)         | Unchanged since tl_v1.4.1; needed for OTA |
| **OpenThread Telink**     | [telink-semi/tl_openthread](https://github.com/telink-semi/tl_openthread)           | [`542aaab`](https://github.com/telink-semi/tl_openthread/commit/542aaab44e1308e1a8a24573dfbd413fade342ee)      | Unchanged since tl_v1.4.1                 |
| **OpenThread Telink Lib** | [telink-semi/tl_openthread_libs](https://github.com/telink-semi/tl_openthread_libs) | [`f69c186`](https://github.com/telink-semi/tl_openthread_libs/commit/f69c186d65a41259480e87ccf9d2a7f665249778) | Unchanged since tl_v1.4.1                 |
| **Telink XZ (LZMA)**      | [telink-semi/tl_xz](https://github.com/telink-semi/tl_xz)                           | [`831f338`](https://github.com/telink-semi/tl_xz/commit/831f338fd6784661d3bec62fd01060ee4d7d373d)              | Unchanged since tl_v1.4.1                 |

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

| Component              | Version                                                                                                                                                                                                                                                                                   |
| ---------------------- | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| **Matter SDK Version** | Telink Matter v1.5                                                                                                                                                                                                                                                                        |
| **Commit**             | [f4a8cf9](https://github.com/project-chip/connectedhomeip/commit/f4a8cf98ada4ad4f439b45e360800693cc5f1391) (the previous one of [15a68d7](https://github.com/project-chip/connectedhomeip/commit/15a68d7026b1cd34a0d4cf35dadc7558e503d2cb) that community upgraded Matter version to 1.6) |
| **Toolchain**          | Zephyr SDK v0.17.0 riscv64-zephyr-elf                                                                                                                                                                                                                                                     |

### Telink Matter SDK

| Property         | Version                                                                                |
| ---------------- | -------------------------------------------------------------------------------------- |
| **Tag Name**     | [tl_v1.5.0-v1.5](https://github.com/telink-semi/tl_matter/releases/tag/tl_v1.5.0-v1.5) |
| **Release Type** | Release                                                                                |

### Telink Zephyr SDK

| Property   | Version                                                                                              |
| ---------- | ---------------------------------------------------------------------------------------------------- |
| **Commit** | [3cd5848c](https://github.com/telink-semi/tl_zephyr/commit/3cd5848c2a76f029097e582ccb3d27211f9cae9d) |

### Chip &amp; Hardware Versions

📦 **Chip Versions**

| Chip Family | Versions |
| ----------- | -------- |
| TL721X      | A3       |

🔧 **Hardware EVK Versions**

| Chip   | EVK Version    |
| ------ | -------------- |
| TL721X | C1T315A20_V1.5 |

---

## 📱 Matter Community Examples

This release provides a Telink platform port for the following Matter community
example applications. They are located under `examples/<app-name>/telink/` in
the repository. The table below lists the TL721X boards validated in this
release (see
[Telink Examples YAML file](../../../../.github/workflows/examples-telink.yaml)).

| Example      | Boards            |
| ------------ | ----------------- |
| light-switch | tl7218x_retention |

### Notes on Platform Support

-   The `*_retention` targets enable the retention-RAM low-power layout and are
    the recommended configuration for battery powered SED/ICD devices.
-   The concurrent-mode BLE off/resume power management described above is
    **only supported on TL721X**, through the light-switch `tl7218x_retention`
    target. Other TLX retention targets (for example TL323X) do not support this
    feature in this release.
-   For build commands per board/app, refer to the per-board `*_README.md` files
    inside each example's `boards/` directory.

---

## 📊 Resource Usage (Code Size)

This section shows the RAM and ROM usage for the supported Matter example on the
Telink TL721X platform, built with the Matter SDK and Zephyr RTOS.

### Supported Boards

| Board             | Chip Family |
| ----------------- | ----------- |
| tl7218x_retention | TL721X      |

### TL721X (tl7218x_retention) — Light Switch App Code Size

📈 **Resource Usage Details**

| App              | Build Target                             | RAM_ILM_N                  | ROM                       | RAM                         |
| ---------------- | ---------------------------------------- | -------------------------- | ------------------------- | --------------------------- |
| **light-switch** | `build_tl7218x_retention_lzma_v1`        | 54974 B (41.94% of 128 KB) | 884538 B (42.18% of 2 MB) | 104796 B (79.95% of 128 KB) |
| **light-switch** | `build_tl7218x_retention_concurrent_off` | 61458 B (46.89% of 128 KB) | 844745 B (40.28% of 2 MB) | 101532 B (77.46% of 128 KB) |

> 📌 **Note:** Memory usage data is extracted from the linker output (Memory
> region summary) of the build logs. `RAM_ILM_N` (instruction RAM) and `RAM`
> (data RAM) are 128 KB each; `ROM` is the application slot0 partition. The
> complete per-board table for all release targets is generated by
> `docs/platforms/telink/releases/generate_and_update_matter_notes.py` when the
> release package is produced. For a detailed RAM/ROM symbol breakdown, run
> `west build -t ram_report` / `west build -t rom_report` in the build
> directory.

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

Made by Telink Semiconductor

-   [Website](https://www.telink-semi.com/)
-   [Forum](https://forum.telink-semi.cn/)
-   [Documentation](https://doc.telink-semi.cn/)
