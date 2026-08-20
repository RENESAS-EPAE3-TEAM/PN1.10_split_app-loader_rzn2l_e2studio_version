/*
 * This file includes the application image selected by the application build.
 * Every App1-App5 configuration produces this common input path; the Loader
 * identifies the image layout from its manifest rather than its App number.
 */

.section .IMAGE_APP_FLASH_section, "a", %progbits
.incbin "../../rzn2l_xspi_boot/Debug_App5_FAILSAFE_PSD/rzn2l_xspi_boot_App5.bin"
//.incbin "../../rzn2l_xspi_boot/Debug_App1_STANDARD/rzn2l_xspi_boot_App1.bin"
