# Generic xSPI Loader

This project is intentionally independent of App1-App5. It embeds the current
application image from `../rzn2l_xspi_boot/build/loader_input/application.bin`
and reads the image manifest at the start of that binary.

## FSP baseline

`configuration.xml`, `rzn_cfg`, `rzn_gen`, `rzn`, and the e² studio FSP
metadata are migrated from `rzn2l_xspi_boot`. The Loader therefore uses the
same FSP 2.0.0 / GCC 12.2.1 baseline, xSPI0 x1 Boot board settings, and Boot
ROM Loader parameters as the original project (`0x6000004C`, `0x6000`, and
`0x00102000`). Do not regenerate this project from a newer FSP package.

## Build order

1. Build any `rzn2l_xspi_boot` configuration (App1, App3, App44, App5, with or without `USE_HRAM`).
2. Its post-build step creates `build/loader_input/application.bin`.
3. Build this Loader project. It embeds that file and copies the manifest's enabled sections before jumping to its entry point.

The current manifest contains four generic section entries: vector table, program text, initialized data, and non-cache initialized data. The Loader contains no App-number-specific address, size, or selection logic.

`application.bin` is generated output and is intentionally excluded from Git.

## Required FSP generation

After importing the Loader into e2 studio, run **Generate Project Content** before building. This regenerates zn_gen for the retained IOPORT, xSPI-QSPI, HyperRAM, and memory-configuration modules; stale App/FreeRTOS generated files have intentionally been removed.

