# Telink Matter SDK Release Note

[![Version](https://img.shields.io/badge/Version-tl__v1.5.1--beta--v1.5-blue?style=flat-square)](https://github.com/telink-semi/tl_matter/releases/tag/tl_v1.5.1-beta-v1.5)
[![License](https://img.shields.io/badge/License-Apache%202.0-red?style=flat-square)](../../../../LICENSE)
[![Matter](https://img.shields.io/badge/Matter-v1.5-green?style=flat-square)](https://github.com/project-chip/connectedhomeip/commit/f4a8cf98ada4ad4f439b45e360800693cc5f1391)

---

-   **Release Type:** Beta Release
-   **Tag Version:**
    [tl_v1.5.1-beta-v1.5](https://github.com/telink-semi/tl_matter/releases/tag/tl_v1.5.1-beta-v1.5)

---

## 📖 Introduction

This patch release is the follow-up to `tl_v1.5.0-v1.5` and backports the Telink
Matter changes from `dev-tlk_v1.5` onto `release-v1.5-v1.5-branch`:

-   Enables the **BLE + Thread concurrent power management** on **TL323X** and
    optimizes the static RAM usage of the concurrent build (unified 160 KB RAM
    layout, smaller OpenThread/IPv6/heap footprints).
-   Moves the underlying Telink Zephyr SDK to the **tl_v1.5.1-beta-v4.1.0** line
    (release branch commit `5d72a63a9c13`) and the Telink BLE SDK to
    `release/V4.0.4.8`.
-   Fixes the system-workqueue and OpenThread stack overflows observed during
    boot and Amazon ecosystem commissioning.

---

## 🆕 Changes in tl_v1.5.1

The following changes land on top of tl_v1.5.0:

-   **TL323x concurrent RAM optimization:** Use the unified 160 KB SRAM layout
    for BLE + Thread coexistence builds and trim the static RAM footprint (IPv6
    multicast tables, OpenThread children, kernel heap). The concurrent build
    now needs 153,468 B of RAM (93.67% of 160 KB) and 915,558 B of ROM
    ([#102](https://github.com/telink-semi/tl_matter/pull/102)).
-   **BLE off/resume on TL323X:** The post-commissioning BLE teardown and the
    KEY3 on-demand BLE resume are now enabled for the TL323x concurrent build.
    Amazon ecosystem commissioning has been validated with this configuration
    ([#102](https://github.com/telink-semi/tl_matter/pull/102)).
-   **Stack fixes:** Raise the concurrent-mode system workqueue stack to 2048
    (PM lowers the default to 664, which overflows on boot) and restore the
    OpenThread thread stack to the project default of 2400 (Amazon ecosystem
    commissioning overflows it when trimmed to 1888)
    ([#102](https://github.com/telink-semi/tl_matter/pull/102)).
-   **Commissioning regression fix:** Restore the network buffer pools to 12
    RX/TX buffers (with 8 TX buffers a full-size Matter packet cannot be sent)
    and re-enable the ADC in the concurrent configuration
    ([#102](https://github.com/telink-semi/tl_matter/pull/102)).
-   **Dependencies:** Update the Telink Zephyr SDK to
    [`5d72a63a`](https://github.com/telink-semi/tl_zephyr/commit/5d72a63a9c13a09c06b4f7776a6563308b3149fa)
    (`tl_v1.5.1-beta-v4.1.0`), the Telink BLE SDK to
    [`a508cc0`](https://github.com/telink-semi/tl_ble_sdk_zephyr/commit/a508cc070bee70d5596624377e0b078b29c29fc2)
    (`release/V4.0.4.8`), and `hal_telink` to `78c97208`.
-   **CI:** Point the Telink examples and Chef workflows at the Zephyr
    `release-v1.5-v4.1-branch` revision, and extend the Mergify backport rules
    ([#100](https://github.com/telink-semi/tl_matter/pull/100)).

---

## ✨ Highlights

| Category            | Details                                                   |
| ------------------- | --------------------------------------------------------- |
| **Matter Support**  | Matter 1.5 protocol stack                                 |
| **Supported Chips** | TL323X (validated)                                        |
| **Sample Apps**     | light-switch                                              |
| **OTA**             | OTA requestor and compress-LZMA support                   |
| **Factory Data**    | Factory data provisioning support                         |
| **Low Power**       | BLE + Thread concurrent BLE off/resume (TL323X validated) |
| **Dual Mode**       | Matter + Zigbee dual mode on 4 MB flash                   |

---

## 🆕 New Features

-   ✅ TL323x unified 160 KB RAM layout for concurrent BLE + Thread builds, with
    a trimmed static RAM footprint
    ([#102](https://github.com/telink-semi/tl_matter/pull/102))
-   ✅ BLE post-commissioning teardown and KEY3 on-demand BLE resume on TL323X;
    Amazon ecosystem commissioning validated
    ([#102](https://github.com/telink-semi/tl_matter/pull/102))
-   ✅ TL3228X SoC platform support with 2 MB flash and LZMA overlays
-   ✅ smoke-CO-alarm example on TL3228X (retention build)
-   ✅ window-covering example on TL3228X with OTA, LZMA compression and factory
    data
-   ✅ Concurrent-mode BLE teardown after commissioning (**TL323X validated in
    this release**): BLE stack is disabled and the RF is handed permanently to
    802.15.4 for the lowest SED power state
    ([#84](https://github.com/telink-semi/tl_matter/pull/84))
-   ✅ On-demand BLE resume for BLE OTA on a commissioned Thread device
    (**TL323X validated in this release**): the existing "start BLE advertising"
    button restores the BLE/802.15.4 RF time-sharing, re-initializes the BLE
    stack and starts a connectable advertisement, while Matter commissioning
    data is untouched ([#84](https://github.com/telink-semi/tl_matter/pull/84))
-   ✅ LZMA compression support for OTA images across the supported TLX boards
-   ✅ Factory data provisioning support across the supported TLX boards
-   ✅ Matter + Zigbee dual-mode support on 4 MB flash

> **Note:** The BLE off/resume power-management options are validated on
> **TL323X** in this release (see `CHIP_CONCURRENT_BLE_OFF_AFTER_COMMISSIONING`
> and `CHIP_CONCURRENT_BLE_RESUME_AFTER_COMMISSIONING` in
> `config/telink/chip-module/Kconfig`), through the
> `boards/tl3238x_retention_*.conf` board configurations. The resume path
> additionally requires a `hal_telink` revision providing
> `tlx_bt_802154_dual_mode_resume()`.

---

## 🐛 Bug Fixes

| Issue                                                     | Component  | Description                                                                                                                                     |
| --------------------------------------------------------- | ---------- | ----------------------------------------------------------------------------------------------------------------------------------------------- |
| [#102](https://github.com/telink-semi/tl_matter/pull/102) | BLE/PM     | Raise the concurrent-mode system workqueue stack to 2048 (PM default 664 overflows on boot)                                                     |
| [#102](https://github.com/telink-semi/tl_matter/pull/102) | OpenThread | Restore the OpenThread thread stack to 2400 (Amazon ecosystem commissioning overflows at 1888)                                                  |
| [#102](https://github.com/telink-semi/tl_matter/pull/102) | Networking | Restore 12 RX/TX network buffers; 8 TX buffers cause `SendMessage() to UDP ... failed: 200000c` (ENOMEM) and flaky commissioning                |
| [#102](https://github.com/telink-semi/tl_matter/pull/102) | ADC        | Re-enable the ADC in the concurrent configuration (`ota_lzma.conf` was overriding `CONFIG_ADC=n`)                                               |
| [#54](https://github.com/telink-semi/tl_matter/pull/54)   | CI         | Fix invalid `chef.yaml` YAML (`if:false` → `if: false`) and incorrect disabled workflow file suffixes                                           |
| [#84](https://github.com/telink-semi/tl_matter/pull/84)   | BLE/PM     | Restore `InitBLEMACAddress()` in `_Init()` so non-concurrent builds keep creating the BLE identity from the controller MAC before `bt_enable()` |
| TL322X                                                    | SoC/Build  | Fix TL322X build issues, including the IEEE 802.15.4 driver, in the Telink Zephyr SDK                                                           |

---

## 📦 Updates / Dependencies

| Component                 | Repository                                                                          | Commit                                                                                                         | Notes                                                       |
| ------------------------- | ----------------------------------------------------------------------------------- | -------------------------------------------------------------------------------------------------------------- | ----------------------------------------------------------- |
| **Telink Zephyr SDK**     | [telink-semi/tl_zephyr](https://github.com/telink-semi/tl_zephyr)                   | [`5d72a63a`](https://github.com/telink-semi/tl_zephyr/commit/5d72a63a9c13a09c06b4f7776a6563308b3149fa)         | `tl_v1.5.1-beta-v4.1.0`, Zephyr v4.1.0 line                 |
| **Telink BLE SDK**        | [telink-semi/tl_ble_sdk_zephyr](https://github.com/telink-semi/tl_ble_sdk_zephyr)   | [`a508cc0`](https://github.com/telink-semi/tl_ble_sdk_zephyr/commit/a508cc070bee70d5596624377e0b078b29c29fc2)  | `tl_v1.5.1-beta-v4.0.4.8`, paired with the Zephyr SDK above |
| **Telink HAL Zephyr**     | [telink-semi/hal_telink](https://github.com/telink-semi/hal_telink)                 | [`78c97208`](https://github.com/telink-semi/hal_telink/commit/78c97208022e655c8fc547aed7e19ce6f314b88c)        | Updated from `81ded24`; contains hal_v1 and hal_v2 sources  |
| **MCUBoot**               | [telink-semi/tl_mcuboot](https://github.com/telink-semi/tl_mcuboot)                 | [`ce0da85`](https://github.com/telink-semi/tl_mcuboot/commit/ce0da85c39c749df49b0ec62b33d2ecdea24c927)         | Unchanged since tl_v1.4.1; needed for OTA                   |
| **OpenThread Telink**     | [telink-semi/tl_openthread](https://github.com/telink-semi/tl_openthread)           | [`542aaab`](https://github.com/telink-semi/tl_openthread/commit/542aaab44e1308e1a8a24573dfbd413fade342ee)      | Unchanged since tl_v1.4.1                                   |
| **OpenThread Telink Lib** | [telink-semi/tl_openthread_libs](https://github.com/telink-semi/tl_openthread_libs) | [`f69c186`](https://github.com/telink-semi/tl_openthread_libs/commit/f69c186d65a41259480e87ccf9d2a7f665249778) | Unchanged since tl_v1.4.1                                   |
| **Telink XZ (LZMA)**      | [telink-semi/tl_xz](https://github.com/telink-semi/tl_xz)                           | [`831f338`](https://github.com/telink-semi/tl_xz/commit/831f338fd6784661d3bec62fd01060ee4d7d373d)              | Unchanged since tl_v1.4.1                                   |

### Dependency Changes Since tl_v1.5.0

-   `telink_zephyr`: `3b7996ad` → `5d72a63a9c13` (`tl_v1.5.1-beta-v4.1.0`)
-   `telink_ble_sdk`: `eb0d1206` → `a508cc070` (`release/V4.0.4.8`)
-   `hal_telink`: `81ded240` → `78c97208`
-   `tl_mcuboot`, `tl_openthread`, `tl_openthread_libs`, and `tl_xz` remain
    unchanged from tl_v1.5.0.

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

| Property         | Version                                                                                          |
| ---------------- | ------------------------------------------------------------------------------------------------ |
| **Tag Name**     | [tl_v1.5.1-beta-v1.5](https://github.com/telink-semi/tl_matter/releases/tag/tl_v1.5.1-beta-v1.5) |
| **Release Type** | Beta Release                                                                                     |

### Telink Zephyr SDK

| Property   | Version                                                                                              |
| ---------- | ---------------------------------------------------------------------------------------------------- |
| **Commit** | [5d72a63a](https://github.com/telink-semi/tl_zephyr/commit/5d72a63a9c13a09c06b4f7776a6563308b3149fa) |

### Chip &amp; Hardware Versions

📦 **Chip Versions**

| Chip Family | Versions |
| ----------- | -------- |
| TL323X      | A1       |

🔧 **Hardware EVK Versions**

| Chip   | EVK Version    |
| ------ | -------------- |
| TL323X | C1T388A20_V1_2 |

> **Note:** TL323X was validated in this release.

---

## 📱 Matter Community Examples

This release provides a Telink platform port for the following Matter community
example applications. They are located under `examples/<app-name>/telink/` in
the repository. The table below lists the boards covered by this release (TL323X
validated) (see
[Telink Examples YAML file](../../../../.github/workflows/examples-telink.yaml)).

| Example      | Boards                                        |
| ------------ | --------------------------------------------- |
| light-switch | tl3238x_retention (validated in this release) |

### Notes on Platform Support

-   The `*_retention` targets enable the retention-RAM low-power layout and are
    the recommended configuration for battery-powered SED/ICD devices.
-   The concurrent-mode BLE off/resume power management is validated on
    **TL323X** in this release, through the light-switch `tl3238x_retention`
    target; Amazon ecosystem commissioning was validated on TL323X.
-   For build commands per board/app, refer to the per-board `*_README.md` files
    inside each example's `boards/` directory.

---

## 📊 Resource Usage (Code Size)

This section shows the RAM and ROM usage for the supported Matter example on the
Telink TL323X platform, built with the Matter SDK and Zephyr RTOS.

### Supported Boards

| Board             | Chip Family |
| ----------------- | ----------- |
| tl3238x_retention | TL323X      |

### TL323X (tl3238x_retention) — Light Switch App Code Size

📈 **Resource Usage Details**

| App              | Build Target                                  | RAM_ILM_N                 | ROM                          | RAM                         |
| ---------------- | --------------------------------------------- | ------------------------- | ---------------------------- | --------------------------- |
| **light-switch** | `build_tl3238x_retention_ota_lzma_v1`         | 63794 B (97.34% of 64 KB) | 907314 B (76.91% of 1152 KB) | 85356 B (52.10% of 160 KB)  |
| **light-switch** | `build_tl3238x_retention_concurrent_ota_lzma` | 0 B (0.00% of 64 KB)      | 915698 B (77.62% of 1152 KB) | 153468 B (93.67% of 160 KB) |

> 📌 **Note:** Memory usage data is extracted from the linker output (Memory
> region summary) of the build logs. For TL323X, `RAM_ILM_N` is 64 KB, the
> concurrent build uses the unified 160 KB RAM layout (no `RAM_ILM_N` usage),
> and `ROM` is the 1152 KB slot0 partition. The complete per-board table for all
> release targets is generated by
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
