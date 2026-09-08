# Telink Matter SDK

- [Chinese README](README_cn.md)

[![Telink Website](https://img.shields.io/badge/Website-Telink-blue?style=flat-square)](https://www.telink-semi.com/)
[![Forum](https://img.shields.io/badge/Forum-Telink-green?style=flat-square)](https://forum.telink-semi.cn/)
[![License](https://img.shields.io/badge/License-Apache%202.0-red?style=flat-square)](LICENSE)
[![Matter](https://img.shields.io/badge/Matter-v1.5-green?style=flat-square)](https://github.com/project-chip/connectedhomeip)

---

> 📖 This is the README for the **Telink Matter SDK** (fork). For the upstream
> Matter (Project CHIP) README, see [MATTER_README.md](MATTER_README.md).

**A Matter protocol implementation for Telink RISC-V SoC platforms based on the
Connected Home over IP (CHIP) project**

- For development environment setup, SDK acquisition, and quick-start
  instructions, refer to the
  [Telink Matter Getting Started Guide](docs/platforms/telink/telink_getting_started.md).
- For a deeper walkthrough, see the online
  [Telink Matter Developer Guide](https://doc.telink-semi.cn/doc/en/software/res/sdk/matter/telink_matter_developer_guide_en/)
  (Chapter **Obtaining Matter Source Code** covers the initial setup).
- For a detailed list of supported devices and resource usage, refer to the
  [Release Note](docs/platforms/telink/releases/telink_release_notes.md)

---

## 📖 SDK Introduction

Telink Matter SDK is a software development platform that implements the Matter
protocol on Telink RISC-V SoC platforms. It is built on top of the Connected
Home over IP (CHIP) project and integrates with the Telink Zephyr SDK to provide
complete Matter-over-Thread support for Telink chips.

### SDK Core Capabilities

| Category            | Capability                                                         |
| ------------------- | ------------------------------------------------------------------ |
| Matter              | Matter (connectedhomeip) protocol stack                            |
| RTOS                | Telink Zephyr RTOS integration                                     |
| Thread networking   | Matter over Thread via OpenThread                                  |
| BLE commissioning   | BLE commissioning for device provisioning                          |
| OTA                 | Remote firmware update support                                     |
| Factory data        | Factory data provisioning                                          |
| Power management    | Retention RAM power management for low-power devices               |
| Boot management     | MCUboot bootloader integration                                     |
| Sample applications | Multiple Matter sample applications for validation and development |

You can use this SDK to develop Telink Matter smart-home end devices with
cross-vendor interoperability, covering the full flow from prototyping and
device development to production testing.

### Supported Examples

The SDK provides Telink ports for the following Matter example applications. See
the [Release Note](docs/platforms/telink/releases/telink_release_notes.md) for
the chip/EVK support matrix validated in each release.

| Category           | Examples                                                                                                                                                                                                 |
| ------------------ | -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| Matter over Thread | lighting-app, light-switch-app, lock-app, contact-sensor-app, smoke-co-alarm-app, temperature-measurement-app, thermostat, air-quality-sensor-app, window-app, pump-app, pump-controller-app, bridge-app |
| Development tools  | all-clusters-app, all-clusters-minimal-app, shell, ota-requestor-app, chef                                                                                                                               |

---

## 🚀 Quick Reference

| Resource              | Description                                | Link                                                                                                                         |
| --------------------- | ------------------------------------------ | ---------------------------------------------------------------------------------------------------------------------------- |
| **Release Note**      | Telink Matter SDK Changelog & New Features | [Telink Matter SDK Release Note](docs/platforms/telink/releases/telink_release_notes.md)                                     |
| **Get Started Guide** | SDK Quick Start Guide                      | [Telink Matter Getting Started](docs/platforms/telink/telink_getting_started.md)                                             |
| **Developer Guide**   | Telink Matter Developer Guide (online)     | [Telink Matter Developer Guide](https://doc.telink-semi.cn/doc/en/software/res/sdk/matter/telink_matter_developer_guide_en/) |
| **Examples**          | Matter Sample Applications                 | See [examples](examples/) (filter by `telink`)                                                                               |
| **Dependency**        | Telink Zephyr SDK                          | [Telink Zephyr SDK](https://github.com/telink-semi/tl_zephyr/blob/dev-tlk_v4.1/README.md)                                    |

---

## 📚 Additional Resources

| Type                            | Resource                                                                                                                     |
| ------------------------------- | ---------------------------------------------------------------------------------------------------------------------------- |
| 🌐 **Telink Official Website**  | [Telink - Chips for a Smarter IoT](https://www.telink-semi.com/)                                                             |
| 💬 **Telink Forum**             | [Telink Technical Support](https://forum.telink-semi.cn/)                                                                    |
| 📖 **Documentation**            | [Telink Matter Developer Guide](https://doc.telink-semi.cn/doc/en/software/res/sdk/matter/telink_matter_developer_guide_en/) |
| 📦 **Matter Community Project** | [Connected Home over IP](https://github.com/project-chip/connectedhomeip)                                                    |

---

## 📝 Release Information

For version history and detailed changelog, refer to the
[Release Note](docs/platforms/telink/releases/telink_release_notes.md).

---

## 📄 License

```
Apache License, Version 2.0

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at:

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
```

---

Made by Telink Semiconductor
