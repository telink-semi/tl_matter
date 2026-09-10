# Telink Matter SDK (tl_matter) 介绍

-   [英文 README](README.md)

[![Telink Website](https://img.shields.io/badge/Website-Telink-blue?style=flat-square)](https://www.telink-semi.cn/)
[![Forum](https://img.shields.io/badge/Forum-Telink-green?style=flat-square)](https://forum.telink-semi.cn/)
[![License](https://img.shields.io/badge/License-Apache%202.0-red?style=flat-square)](LICENSE)
[![Matter](https://img.shields.io/badge/Matter-v1.5-green?style=flat-square)](https://github.com/project-chip/connectedhomeip)

---

> 📖 本文档是 **Telink Matter SDK**（fork）的 README。上游 Matter（Project
> CHIP）的 README 请参阅 [MATTER_README.md](MATTER_README.md)。

**基于 Connected Home over IP（CHIP）项目的 Telink RISC-V SoC 平台 Matter
协议实现**

-   开发环境搭建、SDK 获取及快速上手说明，请参阅
    [Telink Matter Getting Started Guide](docs/platforms/telink/telink_getting_started.md)。
-   更深入的讲解，请参阅在线
    [Telink Matter Developer Guide](https://doc.telink-semi.cn/doc/zh/software/res/sdk/matter/telink_matter_developer_guide_cn/)
    （**获取 Matter 源代码** 章节包含初始环境搭建）。
-   支持的设备及资源占用的详细列表，请参阅
    [Release Note](docs/platforms/telink/releases/telink_release_notes.md)

---

## 📖 SDK 介绍

Telink Matter SDK 是一个在 Telink RISC-V SoC 平台上实现 Matter 协议的软件开发平台。
它基于 Connected Home over IP（CHIP）项目构建，并集成 Telink Zephyr SDK，
为 Telink 芯片提供完整的 Matter over Thread 支持。

### SDK 核心能力

| 类别        | 能力说明                                |
| ----------- | --------------------------------------- |
| Matter 协议 | Matter（connectedhomeip）协议栈         |
| RTOS        | Telink Zephyr RTOS 集成                 |
| Thread 网络 | 基于 OpenThread 的 Matter over Thread   |
| BLE 配网    | 用于设备配网的 BLE Commissioning        |
| OTA         | 远程固件升级支持                        |
| 工厂数据    | 工厂数据配置                            |
| 电源管理    | 面向低功耗设备的 Retention RAM 电源管理 |
| 启动管理    | MCUboot 引导加载程序集成                |
| 示例应用    | 多个 Matter 示例应用，用于验证与开发    |

您可以基于本 SDK 开发具有跨厂商互操作能力的 Telink Matter 智能家居终端设备，
覆盖从原型验证、设备开发到量产测试的完整流程。

### 典型应用

| 应用领域 | 设备示例                        |
| -------- | ------------------------------- |
| 智能照明 | 智能灯具、智能开关等            |
| 智能安防 | 智能门锁、烟雾/一氧化碳报警器等 |
| 环境监测 | 空气质量传感器等环境传感设备    |
| 智能控制 | 恒温器、窗帘控制器、水泵等      |
| 设备互联 | 网桥等                          |

### 支持的示例

本 SDK 为以下 Matter 示例应用提供 Telink 平台移植。每个 release 实际验证过的
芯片/EVK 支持矩阵，请参阅
[Release Note](docs/platforms/telink/releases/telink_release_notes.md)。

| 类别               | 示例                                                                                                                                                                                                     |
| ------------------ | -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| Matter over Thread | lighting-app、light-switch-app、lock-app、contact-sensor-app、smoke-co-alarm-app、temperature-measurement-app、thermostat、air-quality-sensor-app、window-app、pump-app、pump-controller-app、bridge-app |
| 开发工具           | all-clusters-app、all-clusters-minimal-app、shell、ota-requestor-app、chef                                                                                                                               |

### 支持信息

关于完整、准确的芯片型号、对应的开发板、开发平台、工具链以及 SDK 版本的详细信息，
请参阅
[Release Notes](docs/platforms/telink/releases/telink_release_notes.md)。打开
Release Notes 页面后，通过左侧下拉列表，选择与您当前使用的 SDK 版本对应的
Release Notes 查看。

---

## 🚀 快速参考

| 资源                  | 说明                               | 链接                                                                                                                         |
| --------------------- | ---------------------------------- | ---------------------------------------------------------------------------------------------------------------------------- |
| **Release Note**      | Telink Matter SDK 变更日志与新特性 | [Telink Matter SDK Release Note](docs/platforms/telink/releases/telink_release_notes.md)                                     |
| **Get Started Guide** | SDK 快速入门指南                   | [Telink Matter Getting Started](docs/platforms/telink/telink_getting_started.md)                                             |
| **Developer Guide**   | Telink Matter 开发手册（在线）     | [Telink Matter Developer Guide](https://doc.telink-semi.cn/doc/zh/software/res/sdk/matter/telink_matter_developer_guide_cn/) |
| **Examples**          | Matter 示例应用                    | 参见 [examples](examples/)（按 `telink` 筛选）                                                                               |
| **Dependency**        | Telink Zephyr SDK                  | [Telink Zephyr SDK](https://github.com/telink-semi/tl_zephyr/blob/dev-tlk_v4.1/README.md)                                    |

---

## 📚 更多资源

| 类型                   | 资源                                                                                                                         |
| ---------------------- | ---------------------------------------------------------------------------------------------------------------------------- |
| 🌐 **Telink 官方网站** | [Telink - Chips for a Smarter IoT](https://www.telink-semi.cn/)                                                              |
| 💬 **Telink 论坛**     | [Telink Technical Support](https://forum.telink-semi.cn/)                                                                    |
| 📖 **文档**            | [Telink Matter Developer Guide](https://doc.telink-semi.cn/doc/zh/software/res/sdk/matter/telink_matter_developer_guide_cn/) |
| 📦 **Matter 社区项目** | [Connected Home over IP](https://github.com/project-chip/connectedhomeip)                                                    |

---

## 📝 发布信息

版本历史及详细变更日志，请参阅
[Release Note](docs/platforms/telink/releases/telink_release_notes.md)。

---

## 🤝 贡献指南

提交 Issue、贡献代码及开发规范，请参阅 [Contribution Guide](CONTRIBUTING.md)。

---

## 📄 许可证

```text
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
