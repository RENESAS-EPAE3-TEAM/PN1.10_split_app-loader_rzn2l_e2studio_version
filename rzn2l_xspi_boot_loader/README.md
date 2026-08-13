# Generic xSPI Loader

This project is intentionally independent of App1-App5. It embeds the current
application image from `../rzn2l_xspi_boot/build/loader_input/application.bin`
and reads the image manifest at the start of that binary.

## Build order

1. Build any `rzn2l_xspi_boot` configuration (App1, App3, App44, App5, with or without `USE_HRAM`).
2. Its post-build step creates `build/loader_input/application.bin`.
3. Build this Loader project. It embeds that file and copies the manifest's enabled sections before jumping to its entry point.

The current manifest contains four generic section entries: vector table, program text, initialized data, and non-cache initialized data. The Loader contains no App-number-specific address, size, or selection logic.

`application.bin` is generated output and is intentionally excluded from Git.
