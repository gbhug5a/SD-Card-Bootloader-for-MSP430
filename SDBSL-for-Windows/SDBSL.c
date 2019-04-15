/*

SDBSL.exe  {Infile}  {Outfile}  {NewMain hex address}

This code was compiled by the LCC-Win32 compiler.

The program accompanies the SD card bootloaders for the
MSP430G2553/2452.  The bootloader requires that the file
containing the firware update be in binary form, and
that it include the entire contents of the MAIN memory
space not occupied by the bootloader itself.

This program takes as input a normal Intel-HEX or TI-TXT
file and converts it to the appropriate binary file.
It requires as input the input .hex or .txt filename,
the output filename (presumably a .bin file, but not
required), and the address in hex of the new beginning
of MAIN memory. As currently configured, the bootloader
occupies 1K of flash memory at the beginning of MAIN
memory, which means that for the G2553, the new main
address is 0xC400.  Programs must be written to begin
at that address, including setting the reset vector at
0xFFFE to 0xC400. The program will check to make sure
that is the case.

The NewMain address is included in the command line without
a "0x" prefix or "h" suffix. So an example command line
might be:

SDBSL.exe  v14Project.hex  v14Project.bin  C400

The the related .PDF file for further explanation of the
SD card bootloader.

*/

#include <stdio.h>
#include <conio.h>
#include <stdlib.h>

unsigned int firmwarelen;
int infilearg = 0;
int outfilearg = 0;
int filelen = 0;
unsigned int NewMain = 0;
char *eptr;


/*======== Display proper usage of program if /? or error. ===========*/

void Usage(char *programName)			//print out command line format
{
	printf("\n%s usage:\n",programName);
	printf("%s {Infile} {Outfile} {NewMain hex address} \n",programName);
	printf("\nExample for G2553 (MAIN normally at 0xC000): \n");
	printf("%s v14project.hex v14project.bin C400 \n",programName);
}

/*======== Process command line arguments. ==========*/

void HandleOptions(int argc,char *argv[])
{
	int i;

	for (i=1; i< argc;i++)
	{
		if (argv[i][0] == '/' || argv[i][0] == '-')
		{
			continue;
		}
		else if (strlen(argv[i]) > 4)
		{
			if (infilearg == 0) infilearg = i;
			else outfilearg = i;
		}
		else
		{
			NewMain = strtoul(argv[i], &eptr, 16);			//input hex value
			if (*eptr != '\0')
			{
            	printf("Error in hex value \n");
		    	NewMain = 0;
		 	}

		}
	}
}
/*============MAIN==========*/

int main(int argc,char *argv[])
{

	/*handle the program options*/
	HandleOptions(argc,argv);
	if ((infilearg==0) || (outfilearg==0) || (NewMain==0))
	{
		Usage(argv[0]);
		return 0;
	}

	/*Create buffer and init buffer to all FFs */

	int i;
	int j;

	firmwarelen = 0xFFFF - NewMain;			//Main - reset voctor + XORsum
	unsigned char *buf = NULL;
	buf = malloc(firmwarelen+1);

	if(buf == NULL)
	{
		printf("Memory Error \n");
		exit(1);
	}

	for (i = 0; i < firmwarelen+1; i++)
    {
		buf[i] = 0xff;						//fill with FF's
	}

	/*Open firmware new firmware file for reading, process contents*/

	int linelen= 0;
	int linepos= 0;
	int	recordtype;
	unsigned int currentAddr = NewMain;
	unsigned int netAddr = 0;
	unsigned char strdata[128];
	unsigned char xorsum = 0;
	unsigned int resetAdr = 0;
	unsigned char temp = 0;
	unsigned char temp1 = 0;

	FILE *infile;						//open the file

	infile = fopen(argv[infilearg], "rb");
	if (infile == NULL)
	{
	    printf("File Not Found.\n");
		Usage(argv[0]);
		goto CloseExit;
	}

	/* Convert data for MSP430, file is parsed line by line: */
	while (TRUE)
	{
		/* Read one line: */
		if (fgets(strdata, 127, infile) == NULL)    /* if End Of File */
		{
			fclose(infile);
			break;
		}

		if (strdata[0] == ':')                  /* is this a hex file */
		{									    /* yes - process the lines */

			sscanf(&strdata[7], "%02x", &recordtype);  /*record type of this line */
			if(recordtype > 0)
			{
				continue;						// back to while, get next line
			}

			sscanf(&strdata[1], "%02x", &linelen);  /*number of data bytes in line */

			sscanf(&strdata[3], "%04x", &currentAddr);  /* the address for this line's data */
			if (currentAddr < NewMain)				/* must not be below NewMain */
			{
		    	printf("File locates data below %X \n", NewMain);
				fclose(infile);
				goto CloseExit;
			}

			netAddr = currentAddr - NewMain;			/* calculate position in binary image */

			for (linepos= 9; linepos < 9+(linelen*2); linepos+= 2, netAddr++)
			{
				xorsum = xorsum ^ buf[netAddr];		/* take out existing byte */

				temp = strdata[linepos] - 0x30;		/* replace FF with new byte */
				if (temp > 9) temp -= 7;			/* hex to binary */
				temp1 = strdata[linepos+1] - 0x30;
				if (temp1 > 9) temp1 -= 7;
				buf[netAddr] = (temp << 4) + temp1;

				xorsum = xorsum ^ buf[netAddr];		/* add in replacement byte */
			}
			continue;								/* go back to *while* */
		}

		else										/* if a TI-TXT file */
		{
			linelen= strlen(strdata);				/* basically same process */

			if (strdata[0] == 'q') continue;		/* "q" = end of file - back to while */

			if (strdata[0] == '@')
			{
				sscanf(&strdata[1], "%lx\n", &currentAddr);
				if (currentAddr < NewMain)
				{
			    	printf("File locates data below %X \n", NewMain);
					fclose(infile);
					goto CloseExit;
				}
				netAddr = currentAddr - NewMain;
				continue;
			}

			/* Transfer data in line into binary inage: */
			for (linepos= 0; linepos < linelen-3; linepos+= 3, netAddr++)
			{
				xorsum = xorsum ^ buf[netAddr];    /* take out existing byte */

				temp = strdata[linepos] - 0x30;
				if (temp > 9) temp -= 7;
				temp1 = strdata[linepos+1] - 0x30;
				if (temp1 > 9) temp1 -= 7;
				buf[netAddr] = (temp << 4) + temp1;

				xorsum = xorsum ^ buf[netAddr];    /* add in replacement byte */
			}
		}
	}			/* end of While */

	/* "break" goes here */

	/* now check to see if data makes sense */

	resetAdr = buf[firmwarelen-1] + (buf[firmwarelen] * 256);
	temp = 0xFF;

	if (resetAdr != NewMain)
	{
		printf("Reset vector %X must show program starts at %X \n",
				resetAdr, NewMain);
		goto CloseExit;
	}

	for (i=0; i<4; i++)
	{
		temp = temp & buf[i];
	}

	if (temp == 0xFF)
	{
		printf("No code stored where reset vector points - %X \n", resetAdr);
		goto CloseExit;
	}


	xorsum = xorsum ^ buf[firmwarelen-1] ^ buf[firmwarelen];	/* Remove reset vector from checksum */
	buf[firmwarelen-1] = xorsum;								/* place checksum as last byte */
	filelen = firmwarelen;


	FILE *fp;										//write binary image to outfile
	fp = fopen(argv[outfilearg], "wb" );
	fwrite(buf, 1 , filelen, fp );
	fclose(fp);

	printf("\n %d bytes written to %s \n",filelen,argv[outfilearg]);

CloseExit:

	free(buf);
	return 0;
}
