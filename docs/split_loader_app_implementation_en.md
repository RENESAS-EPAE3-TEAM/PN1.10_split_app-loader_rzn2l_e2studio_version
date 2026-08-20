# RZ/N2L xSPI Boot: Standalone Loader and App Split Implementation Record

## 1. Objective

The original fused `rzn2l_xspi_boot` project was split into two independently built units:

- **Standalone Loader**: loaded by Boot ROM; performs minimal CPU startup, xSPI/QSPI setup, external RAM initialization, App image copy/clear operations, and handoff to the App.
- **Standalone App**: retains Profinet, FreeRTOS, and runtime QSPI Flash functionality; can run from the Loader or be debugged in the original fused mode with `SPLIT_LOADER_APP=0`.

The final design remains compatible with FSP 2.0.0 and GCC 12.2.1 while providing stable Loader-to-App handoff, fast App startup, and source-level debugging.

## 2. Hardware Address Space

| Region | Start | Length | Purpose |
|---|---:|---:|---|
| ATCM | `0x00000000` | `0x20000` | Vector table and ATCM code |
| BTCM | `0x00100000` | `0x20000` | Loader/App early-runtime domains and exception stacks |
| System RAM | `0x10000000` | `0x180000` | Normal App code and initialized data |
| System RAM mirror | `0x30000000` | `0x180000` | Non-cache data and shared buffers |
| xSPI0 CS0 | `0x60000000` | `0x4000000` | Loader and embedded App Flash images |

The Boot ROM Loader parameter starts at `0x60000000`; Loader payload starts at `0x6000004C`.

### 2.1 Hardware and Software Environment

The following table records the environment baseline for this split design. Confirm these conditions first when rebuilding, importing the projects, or diagnosing startup issues.

| Category | Item | Environment / Configuration | Notes |
|---|---|---|---|
| Hardware | SoC | Renesas RZ/N2L, `R9A07G084M04GBG` | Target device in the App `configuration.xml`. |
| Hardware | Board configuration | RZ/N2L RSK, `xspi0_x1_boot` | FSP board: `board.rzn2lrsk.xspi0_x1`. |
| Hardware | Boot storage | External Flash on xSPI0 CS0 | Used by the Boot ROM, Loader, and embedded App image startup path. |
| Hardware | External runtime memory | SDRAM or HyperRAM | The Loader must initialize the memory fitted to the hardware; normal App data and BSS can reside there. |
| Software | IDE / configuration generation | Renesas e² studio 2401.1 + FSP `2.0.0` | `configuration.xml` manages board, pin, driver, and generated-code settings. |
| Software | Industrial Ethernet protocol stack | Renesas PN SDK `1.10.0` | This project is based on the `Renesas_PROFINET_IRT_DEVKIT_V1.10.0` release package; protocol-stack and App business sources are in the parent `profinet_sdk` directory. |

Keep the following directory relationship so that the App can resolve the Profinet SDK include paths, linked resources, and source files:

```text
<project root>/
├─ gcc_project/
│  ├─ rzn2l_xspi_boot/
│  └─ rzn2l_xspi_boot_loader/
└─ profinet_sdk/
```

The App accesses Renesas PN SDK `1.10.0` through `${ProjDirPath}/../../profinet_sdk`. The PN SDK is an App build-time and runtime dependency; the Loader must not link its protocol stack or business code. When the board, Flash type, external-RAM type, clocks, pins, PN SDK, or FSP/GCC version changes, review the Loader and App configuration and startup sequence together; do not update only the App-side configuration.

## 3. Original Fused Design

The original fused App linker script placed the following in one startup image:

1. Boot ROM Loader parameters;
2. Loader `.loader_text` and `.loader_data`;
3. App vector, normal `.text`, `.data`, and BSS;
4. The six App exception stacks.

The original `.loader_text` selection was broad and included CMSIS startup code, `startup.o`, `system.o`, BSP functions, I/O port support, QSPI, and selected library functions. This ensured that early startup dependencies were available in low-latency BTCM.

## 4. Standalone Loader Responsibilities

The Loader project is located in `rzn2l_xspi_boot_loader`. It:

1. follows the normal FSP 2.0 startup path;
2. configures pins and the QSPI Flash protocol;
3. initializes SDRAM or HyperRAM;
4. reads the App manifest at `0x60100000`;
5. copies initialized App sections and clears App BSS sections according to the manifest;
6. issues memory barriers and jumps to the App vector entry;
7. contains no App-number-specific layout or business logic.

### 4.1 Loader BTCM Layout

The final Loader map reports:

| Content | Address range | Size |
|---|---:|---:|
| Loader `.loader_text` | `0x00102000–0x00103600` | `0x1600` |
| Loader `.loader_data` | `0x00103600–0x00103C60` | `0x660` |
| Loader exception stacks | `0x00103C60–0x00105460` | `0x1800` |

The App BTCM domain therefore begins at `0x00108000`, leaving a safe gap after Loader usage.

## 5. App Image Manifest

### 5.1 ABI Evolution

The initial split manifest had five entries: vector, normal text, data, non-cache data, and normal BSS.

Debugging showed that copying only normal App sections did not establish a complete early startup environment. To support an App BTCM early-runtime domain, the manifest was upgraded to:

- magic: `0x41505049` (`APPI`)
- format version: `3`
- entry count: `8`
- entry point: `_fvector_start`

Each entry contains four 32-bit fields:

| Field | Meaning |
|---|---|
| `source_address` | xSPI Flash source address |
| `destination_address` | Runtime destination address |
| `size` | Byte count to copy or clear |
| `enable_flag` | `1` = copy; `2` = clear |

### 5.2 Manifest v3 Entries

| No. | Operation | Content |
|---:|---|---|
| 1 | copy | App BTCM `.app_loader_text` |
| 2 | copy | App BTCM `.app_loader_data` |
| 3 | clear | App BTCM `.app_loader_bss` |
| 4 | copy | App vector to ATCM |
| 5 | copy | Normal App `.text` to System RAM |
| 6 | copy | App `.data` to System RAM |
| 7 | copy | Non-cache initialized data to mirror RAM |
| 8 | clear | Normal App BSS |

The Loader-side `APP_IMAGE_FORMAT_VERSION` and `TABLE_ENTRY_NUM` must also be changed to `3` and `8`, or the Loader will reject the App image.

## 6. App BTCM Early-Runtime Domain

### 6.1 Rationale

In the earlier split design, only the tiny `system_init()` `.loader_text` remained in an early region. `stack_init()`, `SystemInit()`, BSP early functions, their data, and the exception stacks could still be distributed across normal App regions or the original low BTCM stack range. That made the early execution path fragmented and introduced an overlap risk with the standalone Loader's BTCM code and stacks.

The final design places the complete App early-runtime dependency set in an independent BTCM window:

```text
0x00108000 ─┬─ .app_loader_text
            ├─ .app_loader_data
            ├─ .app_loader_bss (NOLOAD; cleared by Loader manifest)
            ├─ sys stack
            ├─ svc stack
            ├─ irq stack
            ├─ fiq stack
            ├─ und stack
            └─ abt stack
0x00120000 ─┘
```

### 6.2 Key Linker Rules

The App linker script defines:

```ld
APP_LOADER_START = 0x00108000;
APP_LOADER_END   = 0x00120000;
LOADER_START     = APP_LOADER_START;
LOADER_LENGTH    = APP_LOADER_END - APP_LOADER_START;
```

`.app_loader_text` explicitly collects:

- `*(.loader_text)`;
- `Reset_Handler`;
- CMSIS startup/system-core code;
- App `startup.o` and `system.o`;
- BSP early functions;
- required C library functions;
- IOPORT, QSPI, and warm-start functions.

`.app_loader_data` contains initialized data and associated read-only data needed by early functions. `.app_loader_bss (NOLOAD)` contains the related `.bss*` and `COMMON` symbols.

`ASSERT(. <= APP_LOADER_END, ...)` enforces the 96 KiB BTCM capacity during linking.

## 7. Final App5 BTCM Capacity

App5 linked successfully with this map layout:

| Content | Address range | Size |
|---|---:|---:|
| `.app_loader_text` | `0x00108000–0x0010BE00` | `0x3E00` (15,872 B) |
| `.app_loader_data` | `0x0010BE00–0x0010C598` | `0x798` (1,944 B) |
| `.app_loader_bss` | `0x0010C598–0x0010D018` | `0xA80` (2,688 B) |
| Six exception stacks | `0x0010D018–0x0010E818` | `0x1800` (6,144 B) |
| **Total** | `0x00108000–0x0010E818` | **`0x6818` (26,648 B)** |

The App BTCM window is `0x18000` (96 KiB), leaving:

$$
0x18000 - 0x6818 = 0x117E8 = 71,656\text{ B}
$$

## 8. Startup Handoff Path

The final path is:

```text
Boot ROM
  → Standalone Loader (BTCM)
  → QSPI / external RAM initialization
  → Read App manifest
  → Copy App BTCM text/data
  → Clear App BTCM BSS
  → Copy App vector/text/data
  → Clear normal App BSS
  → App vector (ATCM)
  → Reset_Handler (BTCM)
  → system_init / stack_init (BTCM)
  → SystemInit / BSP early startup (BTCM)
  → main / FreeRTOS / Profinet
```

In the split-App branch, `system_init()` programs VBAR, executes `DSB` and `ISB`, then branches directly to `stack_init()`. This avoids returning to the already-completed Loader handoff call chain.

## 9. `SPLIT_LOADER_APP` Dual Mode

`split_loader_app.h` defines:

```c
#define SPLIT_LOADER_APP (1)
```

| Value | Behavior |
|---:|---|
| `1` | The App is entered by the standalone Loader and skips its own Flash-to-RAM copy, BSS clear, duplicate QSPI configuration, and external-RAM physical initialization. |
| `0` | The App uses the original fused self-loading startup flow for direct J-Link debugging. |

Conditional compilation must test the value with `#if SPLIT_LOADER_APP` or `#if !SPLIT_LOADER_APP`, not `defined(SPLIT_LOADER_APP)`. A macro defined as `0` is still considered defined.

## 10. QSPI and External RAM Ownership

- The Loader must initialize QSPI before it can read the embedded App image.
- The Loader must initialize SDRAM/HyperRAM before copying or clearing App data/BSS located in external RAM.
- A split App must not repeat Loader-owned Quad Enable, external-RAM physical initialization, App image copy, or App BSS clear.
- The App must retain the runtime QSPI driver and its Flash read/write/erase functionality; avoiding duplicate initialization must not remove QSPI runtime support.

## 11. App Binary, Loader Embedding, and Debugging

### 11.1 Current Embedded Input

The current Loader `Flash_section.s` directly embeds:

```asm
.incbin "../../rzn2l_xspi_boot/Debug_App5_FAILSAFE_PSD/rzn2l_xspi_boot_App5.bin"
```

The active workflow is therefore:

```text
Build App5
  → Debug_App5_FAILSAFE_PSD/rzn2l_xspi_boot_App5.bin
  → Build Loader (Flash_section.s embeds the App binary using .incbin)
  → Loader ELF / SREC
```

### 11.2 `export_loader_image.bat`

This script exports a compact `build/loader_input/application.bin` from an App ELF and excludes BSS, heap, exception stacks, and other non-initialized runtime regions. It is intended for a future design in which the Loader embeds a common output binary.

**The current `Flash_section.s` does not reference `build/loader_input/application.bin`; therefore this batch file is not part of the current App5 debug or boot chain.** Keeping it does not affect the current workflow.

### 11.3 J-Link / e² studio Debug Configuration

The Loader debug configuration should load two modules:

1. Loader ELF / target image;
2. App5 ELF as “Map and Symbols”, with offset `0`.

The second module gives the debugger App symbols and the ELF's real VMAs. It does not replace the App binary embedded when the Loader was built.

### 11.4 Important: `.incbin` Dependency

GNU Make normally cannot infer the dependency of `Flash_section.o` on the App `.bin` included by `.incbin`. After updating an App binary, ensure `Flash_section.o` is reassembled and the Loader is relinked. Cleaning the Loader or manually deleting this object before rebuilding are valid approaches.

## 12. Final Verification Status

Completed:

- App5 links with manifest v3 and eight entries;
- the App 96 KiB BTCM capacity assertion passes;
- actual App5 BTCM usage is approximately 26 KiB;
- Loader links with the v3/eight-entry manifest ABI;
- `Flash_section.s` was reassembled and verified to embed the updated App5 binary;
- after J-Link enters the App, App, FreeRTOS, and Profinet startup speed is normal;
- the App5 ELF can be loaded as “Map and Symbols” for App source debugging.

## 13. Impact of Regenerating FSP Code from `configuration.xml`

The App [configuration.xml](../rzn2l_xspi_boot/configuration.xml) remains configured for FSP `2.0.0`, the RZ/N2L `xspi0_x1_boot` board, and GCC `12.2.1`. Generating FSP Project Content can normally update the following locations:

| Location | Can be regenerated | Impact on the split design |
|---|---|---|
| `rzn_gen/` | Yes | High: `hal_data`, `pin_data`, `common_data`, clock, and thread configuration can change |
| `rzn_cfg/` | Yes | Medium: BSP, driver, and FreeRTOS configuration can change |
| `rzn/board/` | Possibly | Medium: board pins, LEDs, PHY, and related configuration can change |
| `script/fsp_xspi0_boot.ld` | Yes | Low: this is the FSP-generated default script, not the active App linker script |
| `script/fsp_xspi0_boot_app.ld` | Normally no | High: this is the custom split-App script and must be retained |

Every App build configuration explicitly links `fsp_xspi0_boot_app.ld` in [`.cproject`](../rzn2l_xspi_boot/.cproject). Consequently, ordinary FSP generation replacing the default `fsp_xspi0_boot.ld` does not directly overwrite the App manifest v3, App BTCM domain, or eight copy/clear entries.

However, the FSP configuration model does not understand and cannot recreate these manual design elements:

- manifest v3 / eight-entry ABI;
- the `0x00108000–0x00120000` App BTCM early-runtime domain;
- `.app_loader_text`, `.app_loader_data`, `.app_loader_bss`, and BTCM exception stacks;
- Loader manifest parsing, external-RAM initialization, and App-image handoff;
- `SPLIT_LOADER_APP` startup branches.

After regenerating App FSP code, always perform:

```text
Regenerate App FSP code
  → inspect rzn_gen / rzn_cfg / rzn/board changes
  → confirm .cproject still selects fsp_xspi0_boot_app.ld
  → rebuild App and inspect its BTCM ASSERT and manifest in the map
  → force rebuild of Loader Flash_section.o
  → relink Loader and inspect IMAGE_APP_FLASH_section size
  → validate startup with J-Link
```

If the App FSP configuration changes QSPI, pins, clocks, SDRAM/HyperRAM type, or timing, inspect and synchronize the corresponding Loader configuration. The Loader initializes this hardware first; mismatched Loader/App assumptions can leave Flash or external RAM in an unexpected state.

### 13.1 Additional Loader Risk

The Loader FSP settings identify `script/fsp_xspi0_boot.ld` as the default linker script, and that file is also the current custom split-Loader script. Running FSP Generate Project Content for the Loader can overwrite its `.IMAGE_APP_FLASH_section`, Loader BTCM selection rules, and `loader_table.o` placement.

Before regenerating Loader FSP content, back up that linker script. The safer long-term approach is to rename it to a custom linker script and explicitly select it with `-T` in the Loader `.cproject`, allowing FSP to overwrite only its default script.

## 14. Renesas PN SDK `1.10.0` Parent-Level `profinet_sdk` Dependency

The App project is not self-contained. This implementation record applies to the `Renesas_PROFINET_IRT_DEVKIT_V1.10.0` release package, which contains Renesas PN SDK `1.10.0`. Its [`.cproject`](../rzn2l_xspi_boot/.cproject) contains relative paths to a Profinet SDK located next to the `gcc_project` directory:

```text
<project root>/
├─ gcc_project/
│  ├─ rzn2l_xspi_boot/
│  └─ rzn2l_xspi_boot_loader/
└─ profinet_sdk/
```

From the App project, the SDK base path is:

```text
${ProjDirPath}/../../profinet_sdk
```

This external directory provides more than headers. It contains actual App build inputs: the Profinet protocol stack, Ethernet-driver adaptation, FreeRTOS/PNIO adaptation, common App code, and App-specific sources. Typical paths include:

- `profinet_sdk/rsk/v3/rz/fsp/...`;
- `profinet_sdk/rsk/v3/rz_cfg/...`;
- `profinet_sdk/src/ext/EK47/...`;
- `profinet_sdk/src/application/App_common/...`;
- `profinet_sdk/src/application/App5_FAILSAFE_PSD/...`.

Therefore:

1. When moving, copying, or importing the project, preserve the relative relationship between `gcc_project` and `profinet_sdk`, or update `.cproject` include paths, linked resources, and source paths together.
2. Copying only `gcc_project` cannot produce a standalone-buildable App; absent SDK content causes missing headers, sources, or objects.
3. FSP Generate Project Content does not generate or update `profinet_sdk`; FSP only manages this project's own generated configuration output.
4. After changing the Profinet SDK version, perform a full App rebuild, inspect text/BSS growth in the map, and confirm the App BTCM 96 KiB assertion still passes.
5. The Loader should not depend on Profinet SDK runtime application code; the SDK is primarily an App-side dependency.

## 15. Recommendations

1. Use one manifest ABI and App BTCM layout for all App configurations.
2. When switching the embedded App, update the `.incbin` input in `Flash_section.s` or migrate to a common `application.bin`.
3. If adopting a common binary, add an explicit App-binary dependency to the Loader build to avoid stale `.incbin` images.
4. Keep standalone Loader BTCM and App BTCM strictly non-overlapping.
5. After linker-script changes, inspect the App BTCM assertion, manifest source ranges, and Loader embedded-image size.
6. Do not create Git commits unless explicitly requested.
