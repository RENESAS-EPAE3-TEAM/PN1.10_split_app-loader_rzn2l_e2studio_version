/*
 * RZ/N2L xSPI boot Loader linker layout for FSP 2.0.0 / GCC 12.
 *
 * This is intentionally based on the original project's FSP 2.0 layout.
 * The Loader runs from BTCM; the embedded application is data at 0x60100000.
 */

INCLUDE memory_regions.ld

ATCM_PRV_START = DEFINED(ATCM_START) ? ATCM_START : 0;
ATCM_PRV_LENGTH = DEFINED(ATCM_LENGTH) ? ATCM_LENGTH : 0;
BTCM_PRV_START = DEFINED(BTCM_START) ? BTCM_START : 0;
BTCM_PRV_LENGTH = DEFINED(BTCM_LENGTH) ? BTCM_LENGTH : 0;
SYSTEM_RAM_PRV_START = DEFINED(SYSTEM_RAM_START) ? SYSTEM_RAM_START : 0;
SYSTEM_RAM_PRV_LENGTH = DEFINED(SYSTEM_RAM_LENGTH) ? SYSTEM_RAM_LENGTH : 0;
SYSTEM_RAM_MIRROR_PRV_START = DEFINED(SYSTEM_RAM_MIRROR_START) ? SYSTEM_RAM_MIRROR_START : 0;
SYSTEM_RAM_MIRROR_PRV_LENGTH = DEFINED(SYSTEM_RAM_MIRROR_LENGTH) ? SYSTEM_RAM_MIRROR_LENGTH : 0;
xSPI0_CS0_SPACE_PRV_START = DEFINED(xSPI0_CS0_SPACE_START) ? xSPI0_CS0_SPACE_START : 0;
xSPI0_CS0_SPACE_PRV_LENGTH = DEFINED(xSPI0_CS0_SPACE_LENGTH) ? xSPI0_CS0_SPACE_LENGTH : 0;

LOADER_PARAM_ADDRESS = xSPI0_CS0_SPACE_PRV_START;
FLASH_CONTENTS_ADDRESS = LOADER_PARAM_ADDRESS + 0x4C;
LOADER_TEXT_ADDRESS = BTCM_PRV_START + 0x2000;
INTVEC_ADDRESS = ATCM_PRV_START;
DATA_NONCACHE_LENGTH = 0x4000;
DATA_NONCACHE_START = SYSTEM_RAM_MIRROR_PRV_START + SYSTEM_RAM_MIRROR_PRV_LENGTH - 0x48000;
SHARED_NONCACHE_BUFFER_START = SYSTEM_RAM_MIRROR_PRV_START + SYSTEM_RAM_MIRROR_PRV_LENGTH - 0x40000;
SHARED_NONCACHE_BUFFER_LENGTH = 0x20000;

MEMORY
{
    ATCM : ORIGIN = ATCM_PRV_START, LENGTH = ATCM_PRV_LENGTH
    BTCM : ORIGIN = BTCM_PRV_START, LENGTH = BTCM_PRV_LENGTH
    SYSTEM_RAM : ORIGIN = SYSTEM_RAM_PRV_START, LENGTH = SYSTEM_RAM_PRV_LENGTH
    SYSTEM_RAM_MIRROR : ORIGIN = SYSTEM_RAM_MIRROR_PRV_START, LENGTH = SYSTEM_RAM_MIRROR_PRV_LENGTH
    xSPI0_CS0_SPACE : ORIGIN = xSPI0_CS0_SPACE_PRV_START, LENGTH = xSPI0_CS0_SPACE_PRV_LENGTH
    DATA_NONCACHE : ORIGIN = DATA_NONCACHE_START, LENGTH = DATA_NONCACHE_LENGTH
    SHARED_NONCACHE_BUFFER : ORIGIN = SHARED_NONCACHE_BUFFER_START, LENGTH = SHARED_NONCACHE_BUFFER_LENGTH
}

SECTIONS
{
    .loader_param LOADER_PARAM_ADDRESS :
    {
        KEEP(*(.loader_param))
    } > xSPI0_CS0_SPACE

    /* Load images for all Loader runtime sections. */
    .flash_contents FLASH_CONTENTS_ADDRESS :
    {
        _mloader_text = .;
        . += (_loader_text_end - _loader_text_start);
        _mloader_data = .;
        . += (_loader_data_end - _loader_data_start);
        _mfvector = .;
        . += (_fvector_end - _fvector_start);
        _mtext = .;
        . += (_text_end - _text_start);
        _mdata = .;
        . += (_data_end - _data_start);
        _mdata_noncache = .;
        . += (_data_noncache_end - _data_noncache_start);
        flash_contents_end = .;
    } > xSPI0_CS0_SPACE

    /* Application binary only: it is copied by Loader code using its manifest. */
    .IMAGE_APP_FLASH_section 0x60100000 :
    {
        IMAGE_APP_FLASH_section_start = .;
        KEEP(*(.IMAGE_APP_FLASH_section))
        IMAGE_APP_FLASH_section_end = .;
    } > xSPI0_CS0_SPACE

    .loader_text LOADER_TEXT_ADDRESS : AT (_mloader_text)
    {
        _loader_text_start = .;
        *(.loader_text)
        *(.text*)
        KEEP(*(.warm_start))
        KEEP(*(.reset_handler))
        KEEP(*(.init))
        KEEP(*(.fini))
        *(.ARM.extab* .gnu.linkonce.armextab.*)
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
        *(.ctors*)
        *(.dtors*)
        *(.eh_frame*)
        _loader_text_end = .;
    } > BTCM

    .loader_data : AT (_mloader_data)
    {
        _loader_data_start = .;
        __loader_data_start = .;
        *(vtable)
        *(.data*)
        *(.rodata*)
        *(.got*)
        KEEP(*(.preinit_array))
        KEEP(*(.init_array*))
        KEEP(*(.fini_array*))
        . = ALIGN(8);
        __loader_data_end = .;
        __loader_bss_start = .;
        *(.bss*)
        *(COMMON)
        . = ALIGN(8);
        __loader_bss_end = .;
        _loader_data_end = .;
    } > BTCM

    .intvec INTVEC_ADDRESS : AT (_mfvector)
    {
        _fvector_start = .;
        KEEP(*(.intvec))
        KEEP(*(.intvec_*))
        _fvector_end = .;
    } > ATCM

    /* Empty application-copy ranges: this project is only the first-stage Loader. */
    .text (NOLOAD) :
    {
        _text_start = .;
        _text_end = .;
    } > ATCM

    .data (NOLOAD) :
    {
        _data_start = .;
        _data_end = .;
    } > ATCM

    .data_noncache DATA_NONCACHE_START : AT (_mdata_noncache)
    {
        _data_noncache_start = .;
        KEEP(*(.data_noncache*))
        . = ALIGN(8);
        _data_noncache_end = .;
    } > DATA_NONCACHE

    .shared_noncache_buffer (NOLOAD) :
    {
        _sncbuffer_start = .;
        KEEP(*(.shared_noncache_buffer*))
        _sncbuffer_end = .;
    } > SHARED_NONCACHE_BUFFER

    .heap (NOLOAD) :
    {
        . = ALIGN(8);
        __HeapBase = .;
        KEEP(*(.heap))
        __HeapLimit = .;
    } > SYSTEM_RAM

    .thread_stack (NOLOAD) :
    {
        . = ALIGN(8);
        __ThreadStackBase = .;
        KEEP(*(.stack*))
        __ThreadStackLimit = .;
    } > BTCM

    .loader_stack (NOLOAD) :
    {
        . = ALIGN(8); __SysStackBase = .; KEEP(*(.sys_stack)) __SysStackLimit = .;
        . = ALIGN(8); __SvcStackBase = .; KEEP(*(.svc_stack)) __SvcStackLimit = .;
        . = ALIGN(8); __IrqStackBase = .; KEEP(*(.irq_stack)) __IrqStackLimit = .;
        . = ALIGN(8); __FiqStackBase = .; KEEP(*(.fiq_stack)) __FiqStackLimit = .;
        . = ALIGN(8); __UndStackBase = .; KEEP(*(.und_stack)) __UndStackLimit = .;
        . = ALIGN(8); __AbtStackBase = .; KEEP(*(.abt_stack)) __AbtStackLimit = .;
    } > BTCM

    __bss_start__ = __loader_bss_start;
    __bss_end__ = __loader_bss_end;
    _bss = __bss_start__;
    _ebss = __bss_end__;
    _end = __bss_end__;
    __exidx_start = _loader_text_start;
    __exidx_end = _loader_text_end;
    _rvectors_start = _fvector_start;
    _rvectors_end = _fvector_end;
    IMAGE_APP_FLASH_section_size = SIZEOF(.IMAGE_APP_FLASH_section);
}
