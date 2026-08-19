# RZ/N2L xSPI Boot：独立 Loader 与 App 拆分实施记录

## 1. 目标

本次工作将原始融合式 `rzn2l_xspi_boot` 工程拆分为两个独立构建单元：

- **独立 Loader**：由 Boot ROM 加载，完成最小 CPU 启动、xSPI/QSPI 初始化、外部 RAM 初始化、App 镜像搬运与清零，然后交接给 App。
- **独立 App**：保留 Profinet、FreeRTOS 和运行期 QSPI Flash 功能；可由 Loader 启动，也可通过 `SPLIT_LOADER_APP=0` 以原融合方式单独调试。

最终目标是保持 FSP 2.0.0 / GCC 12.2.1 兼容性，同时使 Loader→App 的交接稳定、App 快速启动。

## 2. 硬件与地址空间

| 区域 | 起始地址 | 长度 | 用途 |
|---|---:|---:|---|
| ATCM | `0x00000000` | `0x20000` | 向量表及 ATCM 代码 |
| BTCM | `0x00100000` | `0x20000` | Loader / App 早期运行域及异常栈 |
| System RAM | `0x10000000` | `0x180000` | App 普通代码和初始化数据 |
| System RAM mirror | `0x30000000` | `0x180000` | 非缓存数据及共享缓冲 |
| xSPI0 CS0 | `0x60000000` | `0x4000000` | Loader 和嵌入式 App Flash 镜像 |

Boot ROM Loader 参数从 `0x60000000` 开始，Loader payload 从 `0x6000004C` 开始。

## 3. 拆分前的原始融合设计

原始融合 App 链接脚本将以下内容一起放入一个启动镜像：

1. Boot ROM Loader 参数；
2. Loader `.loader_text` 与 `.loader_data`；
3. App vector、 `.text`、`.data`、BSS；
4. App 六类异常栈。

原始 `.loader_text` 的选择范围较大，包含 CMSIS 启动代码、`startup.o`、`system.o`、BSP、I/O 端口、QSPI 以及部分库函数。它保证从复位到 C 运行时建立的早期依赖在低延迟 BTCM 中可执行。

## 4. 独立 Loader 的职责

Loader 工程位于 `rzn2l_xspi_boot_loader`，其职责如下：

1. 使用正常 FSP 2.0 启动路径进入可运行状态；
2. 配置引脚和 QSPI Flash 协议；
3. 初始化 SDRAM 或 HyperRAM；
4. 从 `0x60100000` 读取 App manifest；
5. 按 manifest 拷贝 App 初始化段，清零 App BSS 段；
6. 在内存屏障后跳转到 App vector entry；
7. 不包含 App 编号、App 特定地址或 App 业务逻辑。

### 4.1 Loader BTCM 布局

最终构建 map 的 Loader 布局如下：

| 内容 | 地址范围 | 大小 |
|---|---:|---:|
| Loader `.loader_text` | `0x00102000–0x00103600` | `0x1600` |
| Loader `.loader_data` | `0x00103600–0x00103C60` | `0x660` |
| Loader 异常栈 | `0x00103C60–0x00105460` | `0x1800` |

因此 App BTCM 域从 `0x00108000` 开始，和 Loader 最终占用之间仍保留安全间隔。

## 5. App 镜像 Manifest

### 5.1 ABI 演进

早期拆分版本的 manifest 仅包含 5 项：vector、普通 text、data、non-cache data、普通 BSS。

实际调试表明：仅拷贝普通 App 段并不足以建立完整早期启动环境。为支持 App BTCM 早期运行域，manifest 升级为：

- magic：`0x41505049`（`APPI`）
- format version：`3`
- entry count：`8`
- entry point：`_fvector_start`

每项均由以下四个 32-bit 字组成：

| 字段 | 含义 |
|---|---|
| `source_address` | xSPI Flash 源地址 |
| `destination_address` | 运行时目标地址 |
| `size` | 拷贝或清零字节数 |
| `enable_flag` | `1` 为 copy，`2` 为 clear |

### 5.2 Manifest v3 条目

| 序号 | 操作 | 内容 |
|---:|---|---|
| 1 | copy | App BTCM `.app_loader_text` |
| 2 | copy | App BTCM `.app_loader_data` |
| 3 | clear | App BTCM `.app_loader_bss` |
| 4 | copy | App vector 到 ATCM |
| 5 | copy | App 普通 `.text` 到 System RAM |
| 6 | copy | App `.data` 到 System RAM |
| 7 | copy | App non-cache initialized data 到 mirror RAM |
| 8 | clear | App 普通 BSS |

Loader 端的 `APP_IMAGE_FORMAT_VERSION` 和 `TABLE_ENTRY_NUM` 必须同步为 `3` 和 `8`，否则 Loader 会拒绝镜像。

## 6. App BTCM 早期运行域

### 6.1 设计原因



新的设计将 App 的完整早期运行依赖放到独立的 BTCM 窗口：

```text
0x00108000 ─┬─ .app_loader_text
            ├─ .app_loader_data
            ├─ .app_loader_bss (NOLOAD, Loader manifest clear)
            ├─ sys stack
            ├─ svc stack
            ├─ irq stack
            ├─ fiq stack
            ├─ und stack
            └─ abt stack
0x00120000 ─┘
```

### 6.2 链接脚本关键规则

App 链接脚本使用：

```ld
APP_LOADER_START = 0x00108000;
APP_LOADER_END   = 0x00120000;
LOADER_START     = APP_LOADER_START;
LOADER_LENGTH    = APP_LOADER_END - APP_LOADER_START;
```

`.app_loader_text` 显式收集：

- `*(.loader_text)`；
- `Reset_Handler`；
- CMSIS startup / system core；
- App `startup.o` 和 `system.o`；
- BSP early functions；
- 必需的 C 库函数；
- IOPORT、QSPI 和 warm-start 函数。

`.app_loader_data` 放置早期函数所需的 initialized data 与相关只读数据；`.app_loader_bss (NOLOAD)` 放置相应 `.bss*` 和 `COMMON` 符号。

`ASSERT(. <= APP_LOADER_END, ...)` 在链接阶段检查早期运行域不超过 96 KiB。


## 7. 最终 App5 BTCM 容量

App5 已成功链接，map 给出的实际布局：

| 内容 | 地址范围 | 大小 |
|---|---:|---:|
| `.app_loader_text` | `0x00108000–0x0010BE00` | `0x3E00`（15,872 B） |
| `.app_loader_data` | `0x0010BE00–0x0010C598` | `0x798`（1,944 B） |
| `.app_loader_bss` | `0x0010C598–0x0010D018` | `0xA80`（2,688 B） |
| 六类异常栈 | `0x0010D018–0x0010E818` | `0x1800`（6,144 B） |
| **总计** | `0x00108000–0x0010E818` | **`0x6818`（26,648 B）** |

可用 App BTCM 窗口为 `0x18000`（96 KiB），剩余：

$$
0x18000 - 0x6818 = 0x117E8 = 71,656\text{ B}
$$

## 8. 启动交接路径

最终启动路径为：

```text
Boot ROM
  → 独立 Loader（BTCM）
  → QSPI / 外部 RAM 初始化
  → 读取 App manifest
  → copy App BTCM text/data
  → clear App BTCM BSS
  → copy App vector/text/data
  → clear App normal BSS
  → App vector（ATCM）
  → Reset_Handler（BTCM）
  → system_init / stack_init（BTCM）
  → SystemInit / BSP early startup（BTCM）
  → main / FreeRTOS / Profinet
```

在 split App 分支中，`system_init()` 设置 VBAR 后使用 `DSB`、`ISB`，然后直接跳转到 `stack_init()`。该路径避免返回到已交接完成的 Loader 调用链。

## 9. `SPLIT_LOADER_APP` 双模式

`split_loader_app.h` 中：

```c
#define SPLIT_LOADER_APP (1)
```

| 值 | 行为 |
|---:|---|
| `1` | App 由独立 Loader 启动；App 跳过自身的 Flash→RAM copy、BSS clear、重复 QSPI 配置和外部 RAM 物理初始化。 |
| `0` | App 使用原始融合式自加载启动流程，可用于直接 J-Link 调试。 |



## 10. QSPI 与外部 RAM 的所有权

- Loader 必须先初始化 QSPI，才能读取嵌入式 App image。
- Loader 必须先初始化 SDRAM/HyperRAM，才能搬运或清零位于外部 RAM 的 App data/BSS。
- split App 不应重复执行 Loader 已完成的 QSPI Quad Enable、外部 RAM 物理初始化、App image copy 与 App BSS clear。
- App 仍应保留运行期 QSPI driver 和其 Flash 读写/擦除功能，不能为了避免重复初始化而删除 QSPI runtime driver。

## 11. App 二进制、Loader 嵌入与调试

### 11.1 当前有效的嵌入文件

当前 Loader 的 `Flash_section.s` 直接嵌入：

```asm
.incbin "../../rzn2l_xspi_boot/Debug_App5_FAILSAFE_PSD/rzn2l_xspi_boot_App5.bin"
```

所以当前有效流程为：

```text
Build App5
  → Debug_App5_FAILSAFE_PSD/rzn2l_xspi_boot_App5.bin
  → Build Loader（Flash_section.s 通过 .incbin 嵌入 App binary）
  → Loader ELF / SREC
```

### 11.2 `export_loader_image.bat`

该脚本从 App ELF 导出紧凑的 `build/loader_input/application.bin`，并排除 BSS、heap、异常栈和其他非初始化运行区。它适用于未来 Loader 改为嵌入统一公共输出文件的方案。

**当前 `Flash_section.s` 不引用 `build/loader_input/application.bin`，因此该脚本不是当前 App5 调试/启动链路的一部分。** 保留该脚本不会影响当前流程。

### 11.3 J-Link / e² studio Debug Configuration

建议在 Loader debug configuration 中加载两个模块：

1. Loader ELF / 目标镜像；
2. App5 ELF，类型为“映像和符号（Map and Symbols）”，偏移为 `0`。

第二个模块的作用是为调试器提供 App symbols 和真实 VMA 地址；它不改变 Loader 构建时嵌入的 App binary。

### 11.4 重要：`.incbin` 依赖

GNU Make 通常无法从 `.incbin` 自动推导 `Flash_section.o` 对 App `.bin` 的依赖。因此 App binary 更新后，即使 Loader 的汇编源未变，也必须确保 `Flash_section.o` 被重新汇编，然后重新链接 Loader。可采用 clean Loader 或手工删除该对象文件后重新构建。


## 12. 最终验证状态

已完成：

- App5 使用 manifest v3 / 8 条目成功链接；
- App BTCM 96 KiB 容量断言通过；
- App5 BTCM 实际占用仅约 26 KiB；
- Loader 使用 v3 / 8 条目 manifest 成功链接；
- Loader 已重新汇编 `Flash_section.s` 并确认嵌入新 App5 binary；
- J-Link 进入 App 后，App、FreeRTOS 与 Profinet 的启动速度恢复正常；
- App5 ELF 作为“映像和符号”加载后，可以在 App 中调试。

## 13. 使用 `configuration.xml` 重新生成 FSP 代码的影响

App 的 [configuration.xml](../rzn2l_xspi_boot/configuration.xml) 仍使用 FSP `2.0.0`、RZ/N2L `xspi0_x1_boot` 板级配置与 GCC `12.2.1`。重新生成 FSP Project Content 时，通常会更新：

| 位置 | 是否可能更新 | 对拆分方案的影响 |
|---|---|---|
| `rzn_gen/` | 是 | 高：`hal_data`、`pin_data`、`common_data`、时钟和线程配置会变化 |
| `rzn_cfg/` | 是 | 中：BSP、driver、FreeRTOS 配置会变化 |
| `rzn/board/` | 可能 | 中：板级引脚、LED、PHY 等配置可能变化 |
| `script/fsp_xspi0_boot.ld` | 是 | 低：这是 FSP 默认生成脚本，但不是当前 App 的实际链接脚本 |
| `script/fsp_xspi0_boot_app.ld` | 通常否 | 高：这是拆分 App 的自定义脚本，必须保留 |

当前所有 App build configuration 在 [`.cproject`](../rzn2l_xspi_boot/.cproject) 中显式链接 `fsp_xspi0_boot_app.ld`。因此正常 FSP 生成覆盖默认的 `fsp_xspi0_boot.ld` 时，不会直接覆盖 App 的 manifest v3、App BTCM 域或八个 copy/clear 表项。

但 FSP 配置模型并不了解下列手工设计，无法自动恢复：

- manifest v3 / 8 项 ABI；
- `0x00108000–0x00120000` App BTCM early-runtime domain；
- `.app_loader_text`、`.app_loader_data`、`.app_loader_bss` 与 BTCM 异常栈；
- Loader 的 manifest 解析、外部 RAM 初始化和 App image handoff；
- `SPLIT_LOADER_APP` 启动分支。

因此，App FSP 重新生成后必须执行：

```text
重新生成 App FSP 代码
  → 检查 rzn_gen / rzn_cfg / rzn/board 的差异
  → 确认 .cproject 仍使用 fsp_xspi0_boot_app.ld
  → 重新构建 App 并检查 App map 的 BTCM ASSERT 与 manifest
  → 强制重建 Loader 的 Flash_section.o
  → 重新链接 Loader，检查 IMAGE_APP_FLASH_section 长度
  → J-Link 启动验证
```

若 App FSP 配置改变 QSPI、pin、时钟、SDRAM/HyperRAM 类型或时序，必须同步检查 Loader 的对应配置。原因是 Loader 先初始化这些硬件；若 Loader 与 App 的配置不一致，App 可能在启动后访问到不符合预期的 Flash 或外部 RAM 状态。

### 13.1 Loader 工程的额外风险

Loader 的 FSP 设置将 `script/fsp_xspi0_boot.ld` 设为默认链接脚本，同时该文件也是当前 Loader 实际使用的自定义拆分脚本。直接对 Loader 执行 FSP Generate Project Content 可能覆盖其中的 `.IMAGE_APP_FLASH_section`、Loader BTCM 选段和 `loader_table.o` 布局。

在需要重新生成 Loader FSP 内容前，应先备份该脚本；更安全的长期做法是将它重命名为独立的 custom linker script，并在 Loader `.cproject` 中显式通过 `-T` 使用 custom script，让 FSP 只覆盖默认文件。

## 14. 上级目录 `profinet_sdk` 依赖

App 工程不是自包含工程。它通过 [`.cproject`](../rzn2l_xspi_boot/.cproject) 中的相对路径依赖项目上级目录中的 Profinet SDK：

```text
<项目根目录>/
├─ gcc_project/
│  ├─ rzn2l_xspi_boot/
│  └─ rzn2l_xspi_boot_loader/
└─ profinet_sdk/
```

从 App 工程看，该 SDK 的基准路径是：

```text
${ProjDirPath}/../../profinet_sdk
```

该外部目录提供的并不只是头文件，还包括实际参与 App 编译和链接的 Profinet 协议栈、以太网驱动适配、FreeRTOS/PNIO 适配、App common 代码和各 App 业务源文件。典型路径包括：

- `profinet_sdk/rsk/v3/rz/fsp/...`；
- `profinet_sdk/rsk/v3/rz_cfg/...`；
- `profinet_sdk/src/ext/EK47/...`；
- `profinet_sdk/src/application/App_common/...`；
- `profinet_sdk/src/application/App5_FAILSAFE_PSD/...`。

因此：

1. 移动、复制或重新导入工程时，必须保持 `gcc_project` 与 `profinet_sdk` 的相对层级不变，或者同步修改 `.cproject` 的 include path、linked resources 和 source path；
2. 仅复制 `gcc_project` 目录不能得到可独立构建的 App；缺少 SDK 会导致头文件、源文件或链接对象缺失；
3. FSP Generate Project Content 不会生成或更新 `profinet_sdk`；FSP 只管理本工程的 FSP 配置输出；
4. 更新 Profinet SDK 版本后，应重新全量构建 App、检查 map 中的 text/BSS 增长，并确认 App BTCM 96 KiB 容量断言仍通过；
5. Loader 不应依赖 Profinet SDK 的运行期业务代码；Profinet SDK 主要属于 App 侧依赖。

## 15. 后续建议

1. 为所有 App 配置使用同一 manifest ABI 和 App BTCM 布局；
2. 若切换嵌入 App，统一更新 `Flash_section.s` 的 `.incbin` 输入或改为公共 `application.bin`；
3. 若采用公共 binary，给 Loader 构建增加显式 App binary 依赖，避免 `.incbin` 生成旧镜像；
4. 保持独立 Loader 的 BTCM 区和 App BTCM 区严格不重叠；
5. 每次修改链接脚本后，检查 App map 的 BTCM 断言、manifest 源范围和 Loader 的嵌入区大小；
