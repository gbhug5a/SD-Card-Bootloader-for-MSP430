### Note:

On April 19, 2019, the XOR checksum used to test for errors in the file read from the SD card was modified.  Anything downloaded before that date, as well as any binary files prepared by SDBSL.exe, should be updated to the new versions.  The modification was to change the initial XOR seed from zero to 8.  The zero seed would allow a file consisting of all zeroes to be interpreted as valid, which could happen if an error caused reading from SD card sectors which had been fully erased to zeroes.


## SD Card Bootloader for MSP430

Presented here is a very small bootloader which can be used to update firmware in the field from an SD or microSD card containing the update file.  The bootloader uses 1K at the beginning of Main memory.  This option is an alternative to other update methods requiring interface devices and cables/jumpers, and appropriate software and drivers.

Bootloaders for the MSP430G2452 (USI) and MSP430G2553 (USCI-B) are included.  There is also a Windows console program that converts a normal Intel-HEX or TI-TXT file into a binary file in the form needed by the bootloader.  The user in the field would download the binary file from the developer, copy it to the SD card, insert the card in the device, and power up.  The bootloader would detect the card, then read in data from the file and write it to Main memory.  SD and SDHC cards are supported, formatted with FAT16 or FAT32.

Further information is included in the PDF file.
