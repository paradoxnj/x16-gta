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

typedef struct _MYBITMAPFILEHEADER {
	uint16_t signature;
	uint32_t file_size;
	uint16_t reserved1;
	uint16_t reserved2;
	uint32_t dataOffset;
} MYBITMAPFILEHEADER;

typedef struct _MYBITMAPINFOHEADER {
	uint32_t Size;            /* Size of this header in bytes */
	int32_t  Width;           /* Image width in pixels */
	int32_t  Height;          /* Image height in pixels */
	uint16_t  Planes;          /* Number of color planes */
	uint16_t  BitsPerPixel;    /* Number of bits per pixel */
	uint32_t Compression;     /* Compression methods used */
	uint32_t SizeOfBitmap;    /* Size of bitmap in bytes */
	int32_t  HorzResolution;  /* Horizontal resolution in pixels per meter */
	int32_t  VertResolution;  /* Vertical resolution in pixels per meter */
	uint32_t ColorsUsed;      /* Number of colors in the image */
	uint32_t ColorsImportant; /* Minimum number of important colors */
	/* Fields added for Windows 4.x follow this line */
} MYBITMAPINFOHEADER;

typedef struct _MYRGB {
	uint8_t b;
	uint8_t g;
	uint8_t r;
	uint8_t reserved;
} MYRGB;

void usage()
{
	printf("Usage: bmp2cx16 <bmpfile> <outpalfile> <outbmp>\n\n");
}

char szBmpFile[_MAX_PATH];
char szPalFile[_MAX_PATH];
char szOutFile[_MAX_PATH];
int showBmpInfoOnly = 0;

void parseArgs(int argc, char** argv)
{
	for (int i = 1; i < argc; ++i)
	{
		if (!_stricmp(argv[i], "-in"))
		{
			i++;
			strcpy(szBmpFile, argv[i]);
		}
		else if (!_stricmp(argv[i], "-pal"))
		{
			i++;
			strcpy(szPalFile, argv[i]);
		}
		else if (!_stricmp(argv[i], "-out"))
		{
			i++;
			strcpy(szOutFile, argv[i]);
		}
		else if (!_stricmp(argv[i], "-info"))
		{
			showBmpInfoOnly = 1;
		}
	}
}
int main(int argc, char** argv) {
	FILE* stream = 0;
	FILE* outfile = 0;
	MYBITMAPFILEHEADER fheader;
	MYBITMAPINFOHEADER infoHeader;
	size_t bytes_read = 0;
	uint16_t magic = 0;

	memset(&fheader, 0, sizeof(MYBITMAPFILEHEADER));
	memset(&infoHeader, 0, sizeof(MYBITMAPINFOHEADER));
	memset(szBmpFile, 0, _MAX_PATH);
	memset(szOutFile, 0, _MAX_PATH);
	memset(szPalFile, 0, _MAX_PATH);

	if (argc < 2)
	{
		usage();
		return 0;
	}

	parseArgs(argc, argv);

	printf("\n\n");

	if (szBmpFile[0] != '\0')
		printf("Bitmap File Name:  %s\n", szBmpFile);

	if (szPalFile[0] != '\0')
		printf("Palette Output File Name:  %s\n", szPalFile);

	if (szOutFile[0] != '\0')
		printf("CX16 Output File Name:  %s\n", szOutFile);

	if (showBmpInfoOnly == 1)
		printf("*** Showing bitmap info only ***\n\n");

	if (szBmpFile[0] == '\0')
	{
		printf("ERROR:  Bitmap file name required!!\n\n");
		return 0;
	}

	stream = fopen(szBmpFile, "rb");
	if (!stream)
	{
		printf("ERROR:  Could not open bitmap file!!\n\n");
		return 1;
	}
	
	fseek(stream, 0, SEEK_SET);

	bytes_read = fread(&fheader, 1, sizeof(MYBITMAPFILEHEADER), stream);
	if (bytes_read != sizeof(MYBITMAPFILEHEADER)) {
		printf("File header not right size!!\n");
	}

	printf("Bitmap File Header\n");
	printf("------------------\n");
	printf("Signature:  %04x\n", fheader.signature);
	printf("File Size: %d\n", fheader.file_size);
	printf("Reserved 1: %d\n", fheader.reserved1);
	printf("Reserved 2: %d\n", fheader.reserved2);
	printf("Data Offset: %d\n\n", fheader.dataOffset);

	fread(&infoHeader, sizeof(MYBITMAPINFOHEADER), 1, stream);

	printf("Bitmap Info Header\n");
	printf("------------------\n");
	printf("Header Size:  %d\n", infoHeader.Size);
	printf("Header shoud be:  %d\n", sizeof(MYBITMAPINFOHEADER));
	printf("Width:  %d\n", infoHeader.Width);
	printf("Height: %d\n", infoHeader.Height);
	printf("BPP: %d\n", infoHeader.BitsPerPixel);
	printf("Planes:  %d\n", infoHeader.Planes);
	printf("Compression:  %d\n", infoHeader.Compression);
	printf("Image Data Size:  %d\n", infoHeader.SizeOfBitmap);
	printf("X Resolution:  %d\n", infoHeader.HorzResolution);
	printf("Y Resolution:  %d\n", infoHeader.VertResolution);
	printf("Num Colors:  %d\n", infoHeader.ColorsUsed);
	printf("Important Colors: %d\n", infoHeader.ColorsImportant);

	int num_pal_entries = 1 << infoHeader.BitsPerPixel;
	printf("Num Palette Entries:  %d\n", num_pal_entries);

	if (showBmpInfoOnly)
	{
		fclose(stream);
		stream = 0;
		return 0;
	}

	outfile = fopen(szPalFile, "wb");
	if (!outfile)
	{
		printf("ERROR:  Could not open palette file:  %s\n", argv[2]);
		fclose(stream);
		stream = 0;
		return 0;
	}

	fwrite(&magic, sizeof(uint16_t), 1, szPalFile);

	MYRGB* palette = (MYRGB*)malloc(sizeof(MYRGB) * num_pal_entries);
	if (!palette)
	{
		printf("Could not allocate palette RAM!!\n");
	}

	
	bytes_read = fread(palette, sizeof(MYRGB), num_pal_entries, stream);
	if (bytes_read != num_pal_entries)
		printf("ERROR:  Did not read enough palette entries!!");

	for (int i = 0; i < num_pal_entries; i++)
	{
		uint8_t gb;
		uint8_t g, b;
		uint8_t r;
		
		printf("B: ");
		printf(BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(palette[i].b));
		printf("  G: ");
		printf(BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(palette[i].g));
		printf("  R: ");
		printf(BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(palette[i].r));

		b = (palette[i].b & 0xF0) >> 4;
		g = (palette[i].g) & 0xF0;

		printf("  B Shifted: ");
		printf(BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(b));
		printf("  G Shifted: ");
		printf(BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(g));

		gb = g | b;
		r = (palette[i].r & 0xF0) >> 4;

		printf("  R Shifted: ");
		printf(BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(r));

		fwrite(&gb, sizeof(uint8_t), 1, outfile);
		fwrite(&r, sizeof(uint8_t), 1, outfile);

		printf("  Bin gb: ");
		printf(BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(gb));
		printf("  Bin r: ");
		printf(BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(r));

		printf("  %d: RGB { %d %d %d }\t\tVERA { %02x %02x }\n", i, palette[i].r, palette[i].g, palette[i].b, gb, r);
	}

	fclose(outfile);
	outfile = 0;

	if (szOutFile[0] == '\0')
	{
		fclose(stream);
		stream = 0;
		return 0;
	}

	// Calculate the end of the data
	fseek(stream, 0, SEEK_END);
	int end = ftell(stream);
	fseek(stream, fheader.dataOffset, SEEK_SET);
	uint32_t datasize = end - ftell(stream);

	// Open output bin
	outfile = fopen(szOutFile, "wb");
	if (!outfile)
	{
		printf("ERROR:  Could not open bitmap output binary file %s!!\n", argv[3]);
		fclose(stream);
		stream = 0;
		return 0;
	}

	fwrite(&magic, sizeof(uint16_t), 1, szOutFile);

	uint8_t* bitmap = (uint8_t*)malloc(sizeof(uint8_t) * datasize);
	if (!bitmap)
	{
		printf("ERROR:  Out of memory!!\n");
		fclose(outfile);
		outfile = 0;

		fclose(stream);
		stream = 0;

		return 0;
	}

	uint8_t* flipped = (uint8_t*)malloc(sizeof(uint8_t) * datasize);
	if (!flipped)
	{
		printf("ERROR:  Out of memory!!\n");
		free(bitmap);
		bitmap = 0;

		fclose(outfile);
		outfile = 0;

		fclose(stream);
		stream = 0;

		return 0;
	}

	fread(bitmap, sizeof(uint8_t), datasize, stream);

	/*for (uint32_t i = 0, j = infoHeader.SizeOfBitmap - infoHeader.Width; i < infoHeader.SizeOfBitmap; i += infoHeader.Width, j -= infoHeader.Width) {
		for (uint32_t k = 0; k < infoHeader.Width; k++) {
			flipped[i + k] = bitmap[j + k];
		}
	}*/

	fwrite(bitmap, sizeof(uint8_t), datasize, outfile);

	/*for (int i = 0; i < infoHeader.SizeOfBitmap; i+=8)
	{
		printf("%d %d %d %d %d %d %d %d\n", flipped[i], flipped[i + 1], flipped[i + 2], flipped[i + 3], flipped[i + 4], flipped[i + 5], flipped[i + 6], flipped[i + 7]);
	}*/

	free(flipped);
	flipped = 0;

	free(bitmap);
	bitmap = 0;

	fclose(outfile);
	outfile = 0;

	free(palette);
	palette = 0;

	fclose(stream);
	stream = 0;

	return 0;
}