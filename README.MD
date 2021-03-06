### Note:

Version 1.0 of the executables, dated April 19, 2019, contained a bug that could have resulted in flashing the contents of a file which was marked as deleted in the directory.  Those executables should no longer be used.  In this Version 2.0, that bug has been fixed, and functional changes have also been made to accomodate "always-on" devices.

## SD Card Bootloader for MSP430

This is a very small bootloader which can be used to update firmware in the field from an SD or microSD card containing the update file.  The bootloader occupies 1K at the beginning of MAIN memory.  This BSL option is an alternative to other firmware update methods requiring interface devices and cables/jumpers, and appropriate software and drivers.

Bootloaders for the MSP430G2452 (USI) and MSP430G2553 (USCI-B) are included.  There is also a Windows console program that converts a normal Intel-HEX or TI-TXT file into a binary file in the form needed by the bootloader.  The user in the field would download the binary file from the developer, copy it to the SD card, insert the card in the device, and power up.  The bootloader would detect the card, then read in data from the file and write it to Main memory.  SD and SDHC cards are supported, formatted with FAT16 or FAT32.

Further information is included in the PDF file.
