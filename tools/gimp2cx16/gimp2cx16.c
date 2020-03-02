#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#ifndef _MAX_PATH
#define _MAX_PATH				256
#endif

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0') 

void usage()
{
	printf("Usage: gimp2cx16 <gimpfile> <outfile>\n\n");
}

char szBmpFile[_MAX_PATH];
char szOutFile[_MAX_PATH];
char szPalFile[_MAX_PATH];

void parseArgs(int argc, char** argv)
{
	for (int i = 1; i < argc; ++i)
	{
		if (!strcmp(argv[i], "-in"))
		{
			i++;
			strcpy(szBmpFile, argv[i]);
		}
		else if (!strcmp(argv[i], "-out"))
		{
			i++;
			strcpy(szOutFile, argv[i]);
		}
	}
}

int main(int argc, char** argv) {
	FILE* stream = 0;
	FILE* outfile = 0;
	size_t bytes_read = 0;
	uint16_t magic = 0;
    long datasize = 0;
    uint8_t palette[16 * 3];
    uint8_t idata[2];
    uint8_t odata[2];

	memset(szBmpFile, 0, _MAX_PATH);
	memset(szOutFile, 0, _MAX_PATH);
	
	if (argc < 2)
	{
		usage();
		return 0;
	}

	parseArgs(argc, argv);

	printf("\n\n");

	if (szBmpFile[0] != '\0')
		printf("Bitmap File Name:  %s\n", szBmpFile);

	if (szOutFile[0] != '\0')
		printf("CX16 Output File Name:  %s\n", szOutFile);

	if (szBmpFile[0] == '\0')
	{
		printf("ERROR:  Bitmap file name required!!\n\n");
		return 0;
	}

    printf("\n\n");

    sprintf(szPalFile, "%s.pal", szBmpFile);
	stream = fopen(szPalFile, "rb");
	if (!stream)
	{
		printf("ERROR:  Could not open source palette file!!\n\n");
		return 1;
	}
	
	fseek(stream, 0, SEEK_SET);
    sprintf(szPalFile, "%s.x16.pal", szBmpFile);
	outfile = fopen(szPalFile, "wb");
	if (!outfile)
	{
		printf("ERROR:  Could not open destination palette file:  %s\n", szPalFile);
		fclose(stream);
		stream = 0;
		return 0;
	}

	fwrite(&magic, sizeof(uint16_t), 1, stream);
    
	bytes_read = fread(palette, sizeof(uint8_t), sizeof(palette), stream);
	if (bytes_read != sizeof(palette))
		printf("ERROR:  Did not read enough palette entries!!");

	for (int i = 0; i < (16 * 3); i+=3)
	{
		uint8_t gb;
		uint8_t g, b;
		uint8_t r;
		
		printf("R: ");
		printf(BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(palette[i]));
		printf("  G: ");
		printf(BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(palette[i+1]));
		printf("  B: ");
		printf(BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(palette[i+2]));

		b = (palette[i+2] & 0xF0) >> 4;
		g = (palette[i+1]) & 0xF0;

		printf("  B Shifted: ");
		printf(BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(b));
		printf("  G Shifted: ");
		printf(BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(g));

		gb = g | b;
		r = (palette[i] & 0xF0) >> 4;

		printf("  R Shifted: ");
		printf(BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(r));

		fwrite(&gb, sizeof(uint8_t), 1, outfile);
		fwrite(&r, sizeof(uint8_t), 1, outfile);

		printf("  Bin gb: ");
		printf(BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(gb));
		printf("  Bin r: ");
		printf(BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(r));

		printf("  %d: RGB { %d %d %d }\t\tVERA { %02x %02x }\n", i, palette[i], palette[i+1], palette[i+2], gb, r);
	}

	fclose(outfile);
	outfile = 0;

	if (szOutFile[0] == '\0')
	{
		fclose(stream);
		stream = 0;
		return 0;
	}
	
    fclose(stream);
    stream = 0;

    stream = fopen(szBmpFile, "rb");
    if (!stream)
    {
        printf("ERROR:  Could not open source bitmap file!!\n\n");
		return 1;
    }

    fseek(stream, 0, SEEK_END);
    datasize = ftell(stream);
    fseek(stream, 0, SEEK_SET);

	// Open output bin
	outfile = fopen(szOutFile, "wb");
	if (!outfile)
	{
		printf("ERROR:  Could not open bitmap output binary file %s!!\n", szOutFile);
		fclose(stream);
		stream = 0;
		return 0;
	}

	fwrite(&magic, sizeof(uint16_t), 1, outfile);

	while (!feof(stream))
    {
        if (fread(idata, 1, 2, stream) > 0)
        {
            odata[0] = (idata[0] & 0xF) << 4;
            odata[0] |= idata[1] & 0xF;
            fwrite(odata, 1, 1, outfile);
        }
    }

	fclose(outfile);
	outfile = 0;

	fclose(stream);
	stream = 0;

	return 0;
}