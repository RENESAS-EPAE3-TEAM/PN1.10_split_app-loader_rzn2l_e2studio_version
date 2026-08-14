/*
                  Linker File for Renesas RZ/N2L FSP
*/

INCLUDE memory_regions.ld

/* The memory information for each device is done in memory regions file.
 * The starting address and length of memory not defined in memory regions file are defined as 0. */

ATCM_PRV_START = DEFINED(ATCM_START) ? ATCM_START : 0;
ATCM_PRV_LENGTH = DEFINED(ATCM_LENGTH) ? ATCM_LENGTH : 0;
BTCM_PRV_START = DEFINED(BTCM_START) ? BTCM_START : 0;
BTCM_PRV_LENGTH = DEFINED(BTCM_LENGTH) ? BTCM_LENGTH : 0;
SYSTEM_RAM_PRV_START = DEFINED(SYSTEM_RAM_START) ? SYSTEM_RAM_START : 0;
SYSTEM_RAM_PRV_LENGTH = DEFINED(SYSTEM_RAM_LENGTH) ? SYSTEM_RAM_LENGTH : 0;
SYSTEM_RAM_MIRROR_PRV_START = DEFINED(SYSTEM_RAM_MIRROR_START) ? SYSTEM_RAM_MIRROR_START : 0;
SYSTEM_RAM_MIRROR_PRV_LENGTH = DEFINED(SYSTEM_RAM_MIRROR_LENGTH) ? SYSTEM_RAM_MIRROR_LENGTH : 0;
xSPI0_CS0_SPACE_MIRROR_PRV_START = DEFINED(xSPI0_CS0_SPACE_MIRROR_START) ? xSPI0_CS0_SPACE_MIRROR_START : 0;
xSPI0_CS0_SPACE_MIRROR_PRV_LENGTH = DEFINED(xSPI0_CS0_SPACE_MIRROR_LENGTH) ? xSPI0_CS0_SPACE_MIRROR_LENGTH : 0;
xSPI0_CS1_SPACE_MIRROR_PRV_START = DEFINED(xSPI0_CS1_SPACE_MIRROR_START) ? xSPI0_CS1_SPACE_MIRROR_START : 0;
xSPI0_CS1_SPACE_MIRROR_PRV_LENGTH = DEFINED(xSPI0_CS1_SPACE_MIRROR_LENGTH) ? xSPI0_CS1_SPACE_MIRROR_LENGTH : 0;
xSPI1_CS0_SPACE_MIRROR_PRV_START = DEFINED(xSPI1_CS0_SPACE_MIRROR_START) ? xSPI1_CS0_SPACE_MIRROR_START : 0;
xSPI1_CS0_SPACE_MIRROR_PRV_LENGTH = DEFINED(xSPI1_CS0_SPACE_MIRROR_LENGTH) ? xSPI1_CS0_SPACE_MIRROR_LENGTH : 0;
xSPI1_CS1_SPACE_MIRROR_PRV_START = DEFINED(xSPI1_CS1_SPACE_MIRROR_START) ? xSPI1_CS1_SPACE_MIRROR_START : 0;
xSPI1_CS1_SPACE_MIRROR_PRV_LENGTH = DEFINED(xSPI1_CS1_SPACE_MIRROR_LENGTH) ? xSPI1_CS1_SPACE_MIRROR_LENGTH : 0;
CS0_SPACE_MIRROR_PRV_START = DEFINED(CS0_SPACE_MIRROR_START) ? CS0_SPACE_MIRROR_START : 0;
CS0_SPACE_MIRROR_PRV_LENGTH = DEFINED(CS0_SPACE_MIRROR_LENGTH) ? CS0_SPACE_MIRROR_LENGTH : 0;
CS2_SPACE_MIRROR_PRV_START = DEFINED(CS2_SPACE_MIRROR_START) ? CS2_SPACE_MIRROR_START : 0;
CS2_SPACE_MIRROR_PRV_LENGTH = DEFINED(CS2_SPACE_MIRROR_LENGTH) ? CS2_SPACE_MIRROR_LENGTH : 0;
CS3_SPACE_MIRROR_PRV_START = DEFINED(CS3_SPACE_MIRROR_START) ? CS3_SPACE_MIRROR_START : 0;
CS3_SPACE_MIRROR_PRV_LENGTH = DEFINED(CS3_SPACE_MIRROR_LENGTH) ? CS3_SPACE_MIRROR_LENGTH : 0;
CS5_SPACE_MIRROR_PRV_START = DEFINED(CS5_SPACE_MIRROR_START) ? CS5_SPACE_MIRROR_START : 0;
CS5_SPACE_MIRROR_PRV_LENGTH = DEFINED(CS5_SPACE_MIRROR_LENGTH) ? CS5_SPACE_MIRROR_LENGTH : 0;
xSPI0_CS0_SPACE_PRV_START = DEFINED(xSPI0_CS0_SPACE_START) ? xSPI0_CS0_SPACE_START : 0;
xSPI0_CS0_SPACE_PRV_LENGTH = DEFINED(xSPI0_CS0_SPACE_LENGTH) ? xSPI0_CS0_SPACE_LENGTH : 0;
xSPI0_CS1_SPACE_PRV_START = DEFINED(xSPI0_CS1_SPACE_START) ? xSPI0_CS1_SPACE_START : 0;
xSPI0_CS1_SPACE_PRV_LENGTH = DEFINED(xSPI0_CS1_SPACE_LENGTH) ? xSPI0_CS1_SPACE_LENGTH : 0;
xSPI1_CS0_SPACE_PRV_START = DEFINED(xSPI1_CS0_SPACE_START) ? xSPI1_CS0_SPACE_START : 0;
xSPI1_CS0_SPACE_PRV_LENGTH = DEFINED(xSPI1_CS0_SPACE_LENGTH) ? xSPI1_CS0_SPACE_LENGTH : 0;
xSPI1_CS1_SPACE_PRV_START = DEFINED(xSPI1_CS1_SPACE_START) ? xSPI1_CS1_SPACE_START : 0;
xSPI1_CS1_SPACE_PRV_LENGTH = DEFINED(xSPI1_CS1_SPACE_LENGTH) ? xSPI1_CS1_SPACE_LENGTH : 0;
CS0_SPACE_PRV_START = DEFINED(CS0_SPACE_START) ? CS0_SPACE_START : 0;
CS0_SPACE_PRV_LENGTH = DEFINED(CS0_SPACE_LENGTH) ? CS0_SPACE_LENGTH : 0;
CS2_SPACE_PRV_START = DEFINED(CS2_SPACE_START) ? CS2_SPACE_START : 0;
CS2_SPACE_PRV_LENGTH = DEFINED(CS2_SPACE_LENGTH) ? CS2_SPACE_LENGTH : 0;
CS3_SPACE_PRV_START = DEFINED(CS3_SPACE_START) ? CS3_SPACE_START : 0;
CS3_SPACE_PRV_LENGTH = DEFINED(CS3_SPACE_LENGTH) ? CS3_SPACE_LENGTH : 0;
CS5_SPACE_PRV_START = DEFINED(CS5_SPACE_START) ? CS5_SPACE_START : 0;
CS5_SPACE_PRV_LENGTH = DEFINED(CS5_SPACE_LENGTH) ? CS5_SPACE_LENGTH : 0;

/* The Boot ROM parameter block belongs to the separate Loader project. */
APP_IMAGE_ADDRESS          = xSPI0_CS0_SPACE_PRV_START + 0x00100000;
FLASH_CONTENTS_ADDRESS     = APP_IMAGE_ADDRESS + 0x00000060;
LOADER_TEXT_ADDRESS        = 0x00102000;
INTVEC_ADDRESS             = 0x00000000;
TEXT_ADDRESS               = 0x10000100;
NONCACHE_BUFFER_OFFSET     = 0x00020000;
DMAC_LINK_MODE_OFFSET      = 0x00044000;
DATA_NONCACHE_OFFSET       = 0x00048000;
RAM_START                  = ATCM_PRV_START;
RAM_LENGTH                 = ATCM_PRV_LENGTH;
LOADER_START               = BTCM_PRV_START;
LOADER_LENGTH              = BTCM_PRV_LENGTH;

SDRAM_NC_START             = DEFINED(USE_HRAM) ? 0x44500000 : 0x54500000;
SDRAM_NC_LENGTH            = 0x201000;  
HEAP_START                 = DEFINED(USE_HRAM) ? 0x44200000 : 0x55000000;
HEAP_LENGTH                = 0x1fffff;  
BSS_SPACE_START            = DEFINED(USE_HRAM) ? 0x64400100 : 0x74400100;
BSS_SPACE_LENGTH           = 0x100000;  

/* Define starting addresses and length for data_noncache, DMAC link mode data, CPU-shared non-cache, and CPU-specific non-cache areas. */
DATA_NONCACHE_START = DEFINED(SYSTEM_RAM_MIRROR_START) ? SYSTEM_RAM_MIRROR_START + SYSTEM_RAM_MIRROR_LENGTH - DATA_NONCACHE_OFFSET : 0;
DATA_NONCACHE_LENGTH = DEFINED(SYSTEM_RAM_MIRROR_LENGTH) ? 0x00004000 : 0;
DMAC_LINK_MODE_START = DEFINED(SYSTEM_RAM_MIRROR_START) ? SYSTEM_RAM_MIRROR_START + SYSTEM_RAM_MIRROR_LENGTH - DMAC_LINK_MODE_OFFSET : 0;
DMAC_LINK_MODE_LENGTH = DEFINED(SYSTEM_RAM_MIRROR_LENGTH) ? 0x00004000 : 0;
SHARED_NONCACHE_BUFFER_START = DEFINED(SYSTEM_RAM_MIRROR_START) ? SYSTEM_RAM_MIRROR_START + SYSTEM_RAM_MIRROR_LENGTH - 0x00040000 : 0;
SHARED_NONCACHE_BUFFER_LENGTH = DEFINED(SYSTEM_RAM_MIRROR_LENGTH) ? 0x00020000 : 0;
NONCACHE_BUFFER_START = DEFINED(SYSTEM_RAM_MIRROR_START) ? SYSTEM_RAM_MIRROR_START + SYSTEM_RAM_MIRROR_LENGTH - NONCACHE_BUFFER_OFFSET : 0;
NONCACHE_BUFFER_LENGTH = DEFINED(SYSTEM_RAM_MIRROR_LENGTH) ? 0x00020000 : 0;

MEMORY
{
    ATCM : ORIGIN = ATCM_PRV_START, LENGTH = ATCM_PRV_LENGTH
    BTCM : ORIGIN = BTCM_PRV_START, LENGTH = BTCM_PRV_LENGTH
    SYSTEM_RAM : ORIGIN = SYSTEM_RAM_PRV_START, LENGTH = SYSTEM_RAM_PRV_LENGTH
    SYSTEM_RAM_MIRROR : ORIGIN = SYSTEM_RAM_MIRROR_PRV_START, LENGTH = SYSTEM_RAM_MIRROR_PRV_LENGTH
    xSPI0_CS0_SPACE_MIRROR : ORIGIN = xSPI0_CS0_SPACE_MIRROR_PRV_START, LENGTH = xSPI0_CS0_SPACE_MIRROR_PRV_LENGTH
    xSPI0_CS1_SPACE_MIRROR : ORIGIN = xSPI0_CS1_SPACE_MIRROR_PRV_START, LENGTH = xSPI0_CS1_SPACE_MIRROR_PRV_LENGTH
    xSPI1_CS0_SPACE_MIRROR : ORIGIN = xSPI1_CS0_SPACE_MIRROR_PRV_START, LENGTH = xSPI1_CS0_SPACE_MIRROR_PRV_LENGTH
    xSPI1_CS1_SPACE_MIRROR : ORIGIN = xSPI1_CS1_SPACE_MIRROR_PRV_START, LENGTH = xSPI1_CS1_SPACE_MIRROR_PRV_LENGTH
    CS0_SPACE_MIRROR : ORIGIN = CS0_SPACE_MIRROR_PRV_START, LENGTH = CS0_SPACE_MIRROR_PRV_LENGTH
    CS2_SPACE_MIRROR : ORIGIN = CS2_SPACE_MIRROR_PRV_START, LENGTH = CS2_SPACE_MIRROR_PRV_LENGTH
    CS3_SPACE_MIRROR : ORIGIN = CS3_SPACE_MIRROR_PRV_START, LENGTH = CS3_SPACE_MIRROR_PRV_LENGTH
    CS5_SPACE_MIRROR : ORIGIN = CS5_SPACE_MIRROR_PRV_START, LENGTH = CS5_SPACE_MIRROR_PRV_LENGTH
    xSPI0_CS0_SPACE : ORIGIN = xSPI0_CS0_SPACE_PRV_START, LENGTH = xSPI0_CS0_SPACE_PRV_LENGTH
    xSPI0_CS1_SPACE : ORIGIN = xSPI0_CS1_SPACE_PRV_START, LENGTH = xSPI0_CS1_SPACE_PRV_LENGTH
    xSPI1_CS0_SPACE : ORIGIN = xSPI1_CS0_SPACE_PRV_START, LENGTH = xSPI1_CS0_SPACE_PRV_LENGTH
    xSPI1_CS1_SPACE : ORIGIN = xSPI1_CS1_SPACE_PRV_START, LENGTH = xSPI1_CS1_SPACE_PRV_LENGTH
    CS0_SPACE : ORIGIN = CS0_SPACE_PRV_START, LENGTH = CS0_SPACE_PRV_LENGTH
    CS2_SPACE : ORIGIN = CS2_SPACE_PRV_START, LENGTH = CS2_SPACE_PRV_LENGTH
    CS3_SPACE : ORIGIN = CS3_SPACE_PRV_START, LENGTH = CS3_SPACE_PRV_LENGTH
    CS5_SPACE : ORIGIN = CS5_SPACE_PRV_START, LENGTH = CS5_SPACE_PRV_LENGTH
    RAM : ORIGIN = RAM_START, LENGTH = RAM_LENGTH
    LOADER_STACK : ORIGIN = LOADER_START, LENGTH = LOADER_LENGTH
    DUMMY : ORIGIN = RAM_START, LENGTH = RAM_LENGTH
    DATA_NONCACHE : ORIGIN = DATA_NONCACHE_START, LENGTH = DATA_NONCACHE_LENGTH
    DMAC_LINK_MODE : ORIGIN = DMAC_LINK_MODE_START, LENGTH = DMAC_LINK_MODE_LENGTH
    SHARED_NONCACHE_BUFFER : ORIGIN = SHARED_NONCACHE_BUFFER_START, LENGTH = SHARED_NONCACHE_BUFFER_LENGTH
    NONCACHE_BUFFER : ORIGIN = NONCACHE_BUFFER_START, LENGTH = NONCACHE_BUFFER_LENGTH
	SDRAM_NC_SPACE : ORIGIN = SDRAM_NC_START, LENGTH = SDRAM_NC_LENGTH
	HEAP_SPACE : ORIGIN = HEAP_START, LENGTH = HEAP_LENGTH
	BSS_SPACE : ORIGIN = BSS_SPACE_START, LENGTH = BSS_SPACE_LENGTH
}

SECTIONS
{
    /*
    * Loader-readable manifest. Entries are vector, text, data, non-cache
    * initialized data, and the BSS range to clear. Its layout is defined in
    * the Loader project's loader_table.h.
     */
    .app_image_manifest APP_IMAGE_ADDRESS : AT (APP_IMAGE_ADDRESS)
    {
        LONG(0x41505049); /* APP_IMAGE_MAGIC */
        LONG(2);          /* APP_IMAGE_FORMAT_VERSION */
        LONG(5);          /* table entry count */
        LONG(_fvector_start);
        LONG(_mfvector); LONG(_fvector_start); LONG(_fvector_end - _fvector_start); LONG(1);
        LONG(_mtext); LONG(_text_start); LONG(_text_end - _text_start); LONG(1);
        LONG(_mdata); LONG(_data_start); LONG(_data_end - _data_start); LONG(1);
        LONG(_mdata_noncache); LONG(_data_noncache_start); LONG(_data_noncache_end - _data_noncache_start); LONG(1);
        LONG(0); LONG(__bss_start__); LONG(__bss_end__ - __bss_start__); LONG(2);
    } > xSPI0_CS0_SPACE
    .flash_contents FLASH_CONTENTS_ADDRESS : AT (FLASH_CONTENTS_ADDRESS)
    {
        _mfvector = .;
        . = . + (_fvector_end - _fvector_start);
        _mtext = .;
        . = . + (_text_end - _text_start);
        _mdummy = .;
        . = . + (_dummy_end - _dummy_start);
        _mdata = .;
        . = . + (_data_end - _data_start);
        _mdata_noncache = .;
        . = . + (_data_noncache_end - _data_noncache_start);
        flash_contents_end = .;
    } > xSPI0_CS0_SPACE
    .intvec INTVEC_ADDRESS : AT (_mfvector)
    {
        _fvector_start = .;
        KEEP(*(.intvec))
        _fvector_end = .;
    } > RAM
    .text TEXT_ADDRESS : AT (_mtext)
    {
        _text_start = .;
        *(.loader_text)
        *(.text*)

        KEEP(*(.reset_handler))
        KEEP(*(.init))
        KEEP(*(.fini))

        /* .ctors */
        *crtbegin.o(.ctors)
        *crtbegin?.o(.ctors)
        *(EXCLUDE_FILE(*crtend?.o *crtend.o) .ctors)
        *(SORT(.ctors.*))
        *(.ctors)
        _ctor_end = .;

        /* .dtors */
        *crtbegin.o(.dtors)
        *crtbegin?.o(.dtors)
        *(EXCLUDE_FILE(*crtend?.o *crtend.o) .dtors)
        *(SORT(.dtors.*))
        *(.dtors)
        _dtor_end = .;

        KEEP(*(.eh_frame*))
    } > SYSTEM_RAM
    .rvectors :
    {
        _rvectors_start = .;
        KEEP(*(.rvectors))
        _rvectors_end = .;
    } > SYSTEM_RAM
    .ARM.extab :
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } > SYSTEM_RAM
    __exidx_start = .;
    .ARM.exidx :
    {
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    } > SYSTEM_RAM
    __exidx_end = .;
    .got :
    {
        *(.got)
        *(.got.plt)
        . = ALIGN(4);
        _text_end = .;
    } > SYSTEM_RAM
    .dummy _fvector_end : AT (_mdummy)
    {
        _dummy_start = .;
        KEEP(*(.dummy));
        _dummy_end = .;
    } > DUMMY
    .data :  AT (_mdata)
    {
        _data_start = .;

        *(vtable)
        *(.data.*)
        *(.data)

        *(.rodata*)
        _erodata = .;

        . = ALIGN(4);
        /* preinit data */
        PROVIDE_HIDDEN (__preinit_array_start = .);
        KEEP(*(.preinit_array))
        PROVIDE_HIDDEN (__preinit_array_end = .);

        . = ALIGN(4);
        /* init data */
        PROVIDE_HIDDEN (__init_array_start = .);
        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array))
        PROVIDE_HIDDEN (__init_array_end = .);

        . = ALIGN(4);
        /* finit data */
        PROVIDE_HIDDEN (__fini_array_start = .);
        KEEP(*(SORT(.fini_array.*)))
        KEEP(*(.fini_array))
        PROVIDE_HIDDEN (__fini_array_end = .);

        KEEP(*(.jcr*))

        . = ALIGN(4);

        /* All data end */
        _data_end = .;
    } > SYSTEM_RAM
    .sys_stack (NOLOAD) :
    {
        . = ALIGN(8);
        __SysStackBase = .;
        /* Place the sys_stack here. */
        KEEP(*(.sys_stack))
        __SysStackLimit = .;
    } > LOADER_STACK
    .svc_stack (NOLOAD) :
    {
        . = ALIGN(8);
        __SvcStackBase = .;
        /* Place the svc_stack here. */
        KEEP(*(.svc_stack))
        __SvcStackLimit = .;
    } > LOADER_STACK
    .irq_stack (NOLOAD) :
    {
        . = ALIGN(8);
        __IrqStackBase = .;
        /* Place the irq_stack here. */
        KEEP(*(.irq_stack))
        __IrqStackLimit = .;
    } > LOADER_STACK
    .fiq_stack (NOLOAD) :
    {
        . = ALIGN(8);
        __FiqStackBase = .;
        /* Place the fiq_stack here. */
        KEEP(*(.fiq_stack))
        __FiqStackLimit = .;
    } > LOADER_STACK
    .und_stack (NOLOAD) :
    {
        . = ALIGN(8);
        __UndStackBase = .;
        /* Place the und_stack here. */
        KEEP(*(.und_stack))
        __UndStackLimit = .;
    } > LOADER_STACK
    .abt_stack (NOLOAD) :
    {
        . = ALIGN(8);
        __AbtStackBase = .;
        /* Place the abt_stack here. */
        KEEP(*(.abt_stack))
        __AbtStackLimit = .;
    } > LOADER_STACK
    .data_noncache DATA_NONCACHE_START : AT (_mdata_noncache)
    {
        . = ALIGN(4);
        _data_noncache_start = .;
        KEEP(*(.data_noncache*))
        _data_noncache_end = .;
    } > DATA_NONCACHE
    .dmac_link_mode DMAC_LINK_MODE_START : AT (DMAC_LINK_MODE_START)
    {
        . = ALIGN(4);
        _DmacLinkMode_start = .;
        KEEP(*(.dmac_link_mode*))
        _DmacLinkMode_end = .;
    } > DMAC_LINK_MODE
    .shared_noncache_buffer SHARED_NONCACHE_BUFFER_START (NOLOAD) : AT (SHARED_NONCACHE_BUFFER_START)
    {
        . = ALIGN(32);
        _sncbuffer_start = .;
        KEEP(*(.shared_noncache_buffer*))
        _sncbuffer_end = .;
        KEEP(*(task_stack*))
    } > SHARED_NONCACHE_BUFFER
    .noncache_buffer NONCACHE_BUFFER_START (NOLOAD) : AT (NONCACHE_BUFFER_START)
    {
        . = ALIGN(32);
        _ncbuffer_start = .;
        KEEP(*(.noncache_buffer*))
        _ncbuffer_end = .;
    } > NONCACHE_BUFFER
    .sdram_nc SDRAM_NC_START : AT (SDRAM_NC_START)
    {
        . = ALIGN(32);
        __sdnc_start__ = .;
        KEEP(*(uncached_mem*))
        KEEP(*(nocache*))
        __sdnc_end__ = .;
    } > SDRAM_NC_SPACE
    .heap HEAP_START : AT (HEAP_START)
    {
        . = ALIGN(8);
        __HeapBase = .;
        /* Place the STD heap here. */
        KEEP(*(.heap))
        __HeapLimit = .;
        
        . = ALIGN(4);
        __Heap4_start__ = .;
        */rzn/aws/amazon-freertos/freertos_kernel/portable/MemMang/heap_4.o(.bss*)
        __Heap4_end__ = .;
    } > HEAP_SPACE
    .bss BSS_SPACE_START :
    {
        . = ALIGN(4);
        __bss_start__ = .;
        _bss = .;
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        __bss_end__ = .;
        _ebss = .;
        _end = .;
    } > BSS_SPACE
    .thread_stack (NOLOAD):
    {
        . = ALIGN(8);
        __ThreadStackBase = .;
        /* Place the Thread stacks here. */
        KEEP(*(.stack*))
        __ThreadStackLimit = .;
    } > BSS_SPACE
}
