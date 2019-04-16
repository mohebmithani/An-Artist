#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef uint8_t  BYTE;
typedef uint32_t DWORD;
typedef int32_t  LONG;
typedef uint16_t WORD;


/*********************************************************STRUCTS***********************************************************/
#pragma pack(push, 1)
typedef struct tagBITMAPFILEHEADER
{
    WORD bfType;  //specifies the file type
    DWORD bfSize;  //specifies the size in bytes of the bitmap file
    WORD bfReserved1;  //reserved; must be 0
    WORD bfReserved2;  //reserved; must be 0
    DWORD bfOffBits;  //species the offset in bytes from the bitmapfileheader to the bitmap bits
}BITMAPFILEHEADER;


typedef struct tagBITMAPINFOHEADER
{
    DWORD biSize;  //specifies the number of bytes required by the struct
    LONG biWidth;  //specifies width in pixels
    LONG biHeight;  //species height in pixels
    WORD biPlanes; //specifies the number of color planes, must be 1
    WORD biBitCount; //specifies the number of bit per pixel
    DWORD biCompression;//specifies the type of compression
    DWORD biSizeImage;  //size of image in bytes
    LONG biXPelsPerMeter;  //number of pixels per meter in x axis
    LONG biYPelsPerMeter;  //number of pixels per meter in y axis
    DWORD biClrUsed;  //number of colors used by the bitmap
    DWORD biClrImportant;  //number of colors that are important
}BITMAPINFOHEADER;

typedef struct
{
    BYTE  rgbtBlue;
    BYTE  rgbtGreen;
    BYTE  rgbtRed;
}RGBTRIPLE;

typedef struct
{
	BYTE hslHue;
	BYTE hslSaturation;
	BYTE hslLightness;
}HSL;
#pragma pack(pop)
/*********************************************************STRUCTS***********************************************************/

#define filterWidth 3
#define filterHeight 3

/********************************************************PROTOTYPES*********************************************************/
int CopyBitmapFile(char *filename, BITMAPINFOHEADER *bitmapInfoHeader, char *copy_filename, int edit);
void Filters(int biHeight, int biWidth, RGBTRIPLE **triple, HSL **hsl);
void RGBtoHSL(int biHeight, int biWidth, RGBTRIPLE **triple, HSL **hsl);
void HSLtoRGB (int biHeight, int biWidth, HSL **hsl, RGBTRIPLE **triple);
float max(float a, float b);
float min(float a, float b);
void delay();
void about();
/********************************************************PROTOTYPES*********************************************************/

int main(int argc, char *argv[]) 
{
	// variable for input and output image file
	char image[80], image_copy[80];
	char greetings[] = "Welcome To My Application", project_name[] = "An Artist";
	int n, i, j, choice, edit;
	n = strlen(greetings);
	
	printf("\t\t\t   ");
	for(i = 0; i < n; i++)
	{
		
		printf("%c", greetings[i]);
		delay();	
	}
	delay();
	delay();
	printf("\n\t\t\t\t   ");
	printf("\"");
	n = strlen(project_name);
	
	for (i = 0; i < n; i++)
		{
			printf("%c", project_name[i]);
			delay();
		}
	printf("\"");
	
	delay();
	delay();
	delay();
	mainhome:
	system("cls");
	printf("\n\n");
	printf("Press the Appropriate Number.........\n");
	printf("1. Edit\n");
	printf("2. About the Application\n");
	printf("3. Exit\n");
	printf("Your Choice : ");
	scanf("%d", &choice);
	
	if (choice == 1)
	{
		system("cls");
		printf("\n\n");
		printf("Which Image would you like to edit : ");
		scanf("%s", image);
		
		printf("Name of your new edited image file : ");
		scanf("%s", image_copy);
		
		system("cls");
		printf("\n\n");
		printf("Which Feature of Edit would you like to perform.........\n");
		printf("1. Copy.\n");
		printf("2. Add Filters.\n");
		printf("Your Choice : ");
		scanf("%d", &edit);
		
		BITMAPINFOHEADER bitmapInfoHeader;
		//unsigned char *bitmapData;
		memset(&bitmapInfoHeader,0,sizeof(BITMAPINFOHEADER));
	
		CopyBitmapFile(image,&bitmapInfoHeader, image_copy, edit);
		printf("\n\n\n");
		printf("Done......");
		delay();
		delay();
		delay();
		
		goto mainhome;
	}
	else if (choice == 2)
	{
		about();
		getch();
		goto mainhome;
	}
	else
	{
		return 0;
	}
	return 0;
}





int CopyBitmapFile(char *filename, BITMAPINFOHEADER *bitmapInfoHeader, char *copy_filename, int edit)
{
	int i, j, k;
    FILE *filePtr; //our file pointer
    BITMAPFILEHEADER bitmapFileHeader; //our bitmap file header
	memset(&bitmapFileHeader,0,sizeof(BITMAPFILEHEADER));

    //open filename in read binary mode
    filePtr = fopen(filename,"rb");
    if (filePtr == NULL)
        return 1;
        
    FILE *outptr = fopen(copy_filename, "wb");
    if (outptr == NULL)
    {
        fclose(filePtr);
        fprintf(stderr, "Could not create %s.\n", copy_filename);
        return 2;
    }

    //read the bitmap file header
    fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER),1,filePtr);

    //verify that this is a bmp file by check bitmap id
    if (bitmapFileHeader.bfType !=0x4D42)
    {
    	printf("no bitmap image found");
        fclose(filePtr);
        fclose(outptr);
        return 3;
    }

    //read the bitmap info header
    fread(bitmapInfoHeader, sizeof(BITMAPINFOHEADER),1,filePtr);

    
    // write outfile's BITMAPFILEHEADER
    fwrite(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, outptr);

    // write outfile's BITMAPINFOHEADER
    fwrite(bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, outptr);

    // determine padding for scanlines
    int padding =  (4 - (bitmapInfoHeader->biWidth * sizeof(RGBTRIPLE)) % 4) % 4;

	int biHeight = abs(bitmapInfoHeader->biHeight);
	int biWidth = bitmapInfoHeader->biWidth;
	
	//declaring an array of struct for pixels
	RGBTRIPLE **triple;
	triple = (RGBTRIPLE **) malloc(sizeof(RGBTRIPLE *) * biHeight);
	int ii;
	for(ii = 0; ii < biHeight; ii++) 
	{
		triple[ii] = (RGBTRIPLE *) malloc(sizeof(RGBTRIPLE) * biWidth);
	}
	
	HSL **hsl;
	hsl = (HSL **) malloc(sizeof(HSL *) * biHeight);
	for(ii = 0; ii < biHeight; ii++) 
	{
		hsl[ii] = (HSL *) malloc(sizeof(HSL) * biWidth);
	}
	
	
    // iterate over infile's scanlines
    for (i = 0; i < biHeight; i++)
    {
        // iterate over pixels in scanline
        for (j = 0; j < biWidth; j++)
        {
            // read RGB triple from infile
            fread(&triple[i][j], sizeof(RGBTRIPLE), 1, filePtr);
        }
    }
    
    
    if (edit == 2)
    	Filters(biHeight, biWidth, triple, hsl);
    
	
    
    for (i = 0; i < biHeight; i++)
    {
        // iterate over pixels in scanline
        for (j = 0; j < biWidth; j++)
        {

            // write RGB triple to outfile
            fwrite(&triple[i][j], sizeof(RGBTRIPLE), 1, outptr);
        }

        // skip over padding, if any
        fseek(filePtr, padding, SEEK_CUR);

        // then add it back (to demonstrate how)
        for (k = 0; k < padding; k++)
        {
            fputc(0x00, outptr);
        }
    }

    // close infile
    fclose(filePtr);

    // close outfile
    fclose(outptr);
	free(triple);
}


 void Filters(int biHeight, int biWidth, RGBTRIPLE **triple, HSL **hsl)
 {
	int i, j, choice;
	
	system("cls");
	printf("\n\n");
	printf("Which Filters Do You Want To Try.........\n");
	printf("1. GreyScale.\n");
	printf("2. Negative.\n");
	printf("3. Brighter Image.\n");
	printf("4. Darker Image.\n");
	printf("5. Removing Some Color Channels.\n");
	printf("6. Swap The Color Channels.\n");
	printf("7. Blur.\n");
	printf("8. Sharpen.\n");
	printf("9. Emboss.\n");
	printf("10. Changing Hue.\n");
	printf("11. Changing Saturation.\n");
	printf("Your Choice : ");
	scanf("%d", &choice);
	
	/*double filter[filterHeight][filterWidth];
	double factor = 1.0;
	double bias = 0.0;;*/
	switch(choice)
	{
		case 1:
			for (i = 0; i < biHeight; i++)
			{
				for(j = 0; j < biWidth; j++)
				{
					//grayscale
					int x = 0.2126*(triple[i][j].rgbtRed) + 0.7152*(triple[i][j].rgbtGreen) + 0.0722*(triple[i][j].rgbtBlue);
					triple[i][j].rgbtBlue = x;
					triple[i][j].rgbtRed = x;
					triple[i][j].rgbtGreen = x;
				}
			}
		break;
		case 2:
			for (i = 0; i < biHeight; i++)
			{
				for(j = 0; j < biWidth; j++)
				{
					// negative
					triple[i][j].rgbtBlue = 255 - triple[i][j].rgbtBlue;
					triple[i][j].rgbtGreen = 255 - triple[i][j].rgbtGreen;
					triple[i][j].rgbtRed = 255 - triple[i][j].rgbtRed;
				}
			}
		break;
		case 3:
			for (i = 0; i < biHeight; i++)
			{
				for(j = 0; j < biWidth; j++)
				{
					// brighter image
					int x, y, z;
					x = triple[i][j].rgbtBlue * 2;
					y = triple[i][j].rgbtGreen * 2;
					z = triple[i][j].rgbtRed * 2;
					triple[i][j].rgbtBlue = ((x > 255)? 255 : x);
					triple[i][j].rgbtGreen = ((y > 255)? 255 : x);
					triple[i][j].rgbtRed = ((z > 255)? 255 : x);
				}
			}
		break;
		case 4:
			for (i = 0; i < biHeight; i++)
			{
				for(j = 0; j < biWidth; j++)
				{
					// darker image
					int x, y, z;
					x = triple[i][j].rgbtBlue / 1.2;
					y = triple[i][j].rgbtGreen / 1.2;
					z = triple[i][j].rgbtRed / 1.2;
					triple[i][j].rgbtBlue = ((x < 0)? 0 : x);
					triple[i][j].rgbtGreen = ((y < 0)? 0 : x);
					triple[i][j].rgbtRed = ((z < 0)? 0 : x);
				}
			}
		break;
		case 5:
			system("cls");
			printf("\n\n");
			printf("How many Color Do You Want To Remove......\n");
			printf("1 or 2 ?\n");
			int c;
			printf("Your Choice : ");
			scanf("%d", &c);
			
			if (c == 1)
			{
				char c;
				system("cls");
				printf("\n\n");
				printf("Name of Color.........\n");
				printf("\"R\" for RED.\n");
				printf("\"G\" for Green.\n");
				printf("\"B\" for Blue.\n");
				printf("Your Choice : ");
				c = toupper(getch());
				
				if (c == 'R')
				{
					for (i = 0; i < biHeight; i++)
					{
						for(j = 0; j < biWidth; j++)
						{
							// removing  color channel
			   				triple[i][j].rgbtRed = 0;
						}
					}
				}
				else if (c == 'G')
				{
					for (i = 0; i < biHeight; i++)
					{
						for(j = 0; j < biWidth; j++)
						{
							// removing  color channel
			   				triple[i][j].rgbtGreen = 0;
						}
					}
				}
				else
				{
					for (i = 0; i < biHeight; i++)
					{
						for(j = 0; j < biWidth; j++)
						{
							// removing  color channel
			   				triple[i][j].rgbtBlue = 0;
						}
					}
				}
				
			}
			else
			{
				char c, d;
				system("cls");
				printf("\n\n");
				printf("Name of Colors.........\n");
				printf("\"R\" for RED.\n");
				printf("\"G\" for Green.\n");
				printf("\"B\" for Blue.\n");
				printf("Your Choices (1st color) : ");
				c = toupper(getch());
				printf("\n");
				printf("Your Choices (1st color) : ");
				d = toupper(getch());
				
				if ((c == 'R' && d == 'G') || (c == 'G' && d == 'R')) 
				{
					for (i = 0; i < biHeight; i++)
					{
						for(j = 0; j < biWidth; j++)
						{
							// removing  color channel
			   				triple[i][j].rgbtRed = 0;
			   				triple[i][j].rgbtGreen = 0;
						}
					}
				}
				else if ((c == 'R' && d == 'B') || (c == 'B' && d == 'R'))
				{
					for (i = 0; i < biHeight; i++)
					{
						for(j = 0; j < biWidth; j++)
						{
							// removing  color channel
			   				triple[i][j].rgbtRed = 0;
			   				triple[i][j].rgbtBlue = 0;
						}
					}
				}
				else if ((c == 'G' && d == 'B') || (c == 'B' && d == 'G'))
				{
					for (i = 0; i < biHeight; i++)
					{
						for(j = 0; j < biWidth; j++)
						{
							// removing  color channel
			   				triple[i][j].rgbtBlue = 0;
			   				triple[i][j].rgbtGreen = 0;
						}
					}
				}
			}
		break;
		
		case 6:
				system("cls");
				printf("\n\n");
				printf("Which Colors To Swap.........\n");
				printf("\"R\" for RED.\n");
				printf("\"G\" for Green.\n");
				printf("\"B\" for Blue.\n");
				char ch, dh;
				printf("Your Choices (1st color) : ");
				ch = toupper(getch());
				printf("\n");
				printf("Your Choices (2nd color) : ");
				dh = toupper(getch());
				
				if ((ch == 'R' && dh == 'G') || (ch == 'G' && dh == 'R')) 
				{
					for (i = 0; i < biHeight; i++)
					{
						for(j = 0; j < biWidth; j++)
						{
							// swapping the color channels
							int temp;
							temp = triple[i][j].rgbtRed;
							triple[i][j].rgbtRed = triple[i][j].rgbtGreen;
							triple[i][j].rgbtGreen = temp;
						}
					}
				}
				else if ((ch == 'R' && dh == 'B') || (ch == 'B' && dh == 'R'))
				{
					for (i = 0; i < biHeight; i++)
					{
						for(j = 0; j < biWidth; j++)
						{
							// swapping the color channels
							int temp;
							temp = triple[i][j].rgbtBlue;
							triple[i][j].rgbtBlue = triple[i][j].rgbtRed;
							triple[i][j].rgbtRed = temp;
						}
					}
				}
				else if ((ch == 'G' && dh == 'B') || (ch == 'B' && dh == 'G'))
				{
					for (i = 0; i < biHeight; i++)
					{
						for(j = 0; j < biWidth; j++)
						{
							// swapping the color channels
							int temp;
							temp = triple[i][j].rgbtBlue;
							triple[i][j].rgbtBlue = triple[i][j].rgbtGreen;
							triple[i][j].rgbtGreen = temp;
						}
					}
				}
		break;
		
		case 7: ;
			
			
			// for blur
		 	double filter[filterHeight][filterWidth] =
			{
		   		0.0625, 0.125,  0.0625,
		   		0.125, 0.25,  0.125,
		   		0.0625, 0.125,  0.0625
			};
			
			double factor = 1.0;
			double bias = 0.0;
			int k = 0;
		while (k < 4)
		{
			
			for(i = 0; i < biHeight; i++)
			{
				for(j = 0; j < biWidth; j++)
				{
					// border black
					if((i == 0) || (i == (biHeight - 1)) || (j == 0) || (j == (biWidth - 1)))
					{	
						triple[i][j].rgbtBlue = 0;
             			triple[i][j].rgbtGreen = 0;
						triple[i][j].rgbtRed = 0;
					}
					else
					{
						double red = 0, green = 0, blue = 0;								
						red = (triple[i][j].rgbtRed * filter[1][1]) + (triple[i][j + 1].rgbtRed * filter[1][2]) + (triple[i][j - 1].rgbtRed * filter[1][0]) + (triple[i - 1][j].rgbtRed * filter[0][1]) + (triple[i + 1][j].rgbtRed * filter[2][1]) + (triple[i - 1][j - 1].rgbtRed * filter[0][0]) + (triple[i - 1][j + 1].rgbtRed * filter[0][2]) + (triple[i + 1][j - 1].rgbtRed * filter[2][0]) + (triple[i + 1][j + 1].rgbtRed * filter[2][2]); 
						green = (triple[i][j].rgbtGreen * filter[1][1]) + (triple[i][j + 1].rgbtGreen * filter[1][2]) + (triple[i][j - 1].rgbtGreen * filter[1][0]) + (triple[i - 1][j].rgbtGreen * filter[0][1]) + (triple[i + 1][j].rgbtGreen * filter[2][1]) + (triple[i - 1][j - 1].rgbtGreen * filter[0][0]) + (triple[i - 1][j + 1].rgbtGreen * filter[0][2]) + (triple[i + 1][j - 1].rgbtGreen * filter[2][0]) + (triple[i + 1][j + 1].rgbtGreen * filter[2][2]);
						blue = (triple[i][j].rgbtBlue * filter[1][1]) + (triple[i][j + 1].rgbtBlue * filter[1][2]) + (triple[i][j - 1].rgbtBlue * filter[1][0]) + (triple[i - 1][j].rgbtBlue * filter[0][1]) + (triple[i + 1][j].rgbtBlue * filter[2][1]) + (triple[i - 1][j - 1].rgbtBlue * filter[0][0]) + (triple[i - 1][j + 1].rgbtBlue * filter[0][2]) + (triple[i + 1][j - 1].rgbtBlue * filter[2][0]) + (triple[i + 1][j + 1].rgbtBlue * filter[2][2]);	
						
						triple[i][j].rgbtRed = min(max((int)(factor * red + bias), 0), 255);
						triple[i][j].rgbtGreen = min(max((int)(factor * green + bias), 0), 255);
						triple[i][j].rgbtBlue = min(max((int)(factor * blue + bias), 0), 255);
					}
			
				}
			
			}
			
			k++;
		}
		break;
		case 8 : ;
			// sharpen
			double filters[filterHeight][filterWidth] = 
		{
			-1,-1,-1,
			-1,9,-1,
			-1,-1,-1
		};
			
			
			
			factor = 1.0;
			bias = 0.0;
			
			for(i = 0; i < biHeight; i++)
			{
				for(j = 0; j < biWidth; j++)
				{
					// border black
					if((i == 0) || (i == (biHeight - 1)) || (j == 0) || (j == (biWidth - 1)))
					{	
						triple[i][j].rgbtBlue = 0;
             			triple[i][j].rgbtGreen = 0;
						triple[i][j].rgbtRed = 0;
					}
					else
					{
						double red = 0, green = 0, blue = 0;								
						red = (triple[i][j].rgbtRed * filters[1][1]) + (triple[i][j + 1].rgbtRed * filters[1][2]) + (triple[i][j - 1].rgbtRed * filters[1][0]) + (triple[i - 1][j].rgbtRed * filters[0][1]) + (triple[i + 1][j].rgbtRed * filters[2][1]) + (triple[i - 1][j - 1].rgbtRed * filters[0][0]) + (triple[i - 1][j + 1].rgbtRed * filters[0][2]) + (triple[i + 1][j - 1].rgbtRed * filters[2][0]) + (triple[i + 1][j + 1].rgbtRed * filters[2][2]); 
						green = (triple[i][j].rgbtGreen * filters[1][1]) + (triple[i][j + 1].rgbtGreen * filters[1][2]) + (triple[i][j - 1].rgbtGreen * filters[1][0]) + (triple[i - 1][j].rgbtGreen * filters[0][1]) + (triple[i + 1][j].rgbtGreen * filters[2][1]) + (triple[i - 1][j - 1].rgbtGreen * filters[0][0]) + (triple[i - 1][j + 1].rgbtGreen * filters[0][2]) + (triple[i + 1][j - 1].rgbtGreen * filters[2][0]) + (triple[i + 1][j + 1].rgbtGreen * filters[2][2]);
						blue = (triple[i][j].rgbtBlue * filters[1][1]) + (triple[i][j + 1].rgbtBlue * filters[1][2]) + (triple[i][j - 1].rgbtBlue * filters[1][0]) + (triple[i - 1][j].rgbtBlue * filters[0][1]) + (triple[i + 1][j].rgbtBlue * filters[2][1]) + (triple[i - 1][j - 1].rgbtBlue * filters[0][0]) + (triple[i - 1][j + 1].rgbtBlue * filters[0][2]) + (triple[i + 1][j - 1].rgbtBlue * filters[2][0]) + (triple[i + 1][j + 1].rgbtBlue * filters[2][2]);	
						
						triple[i][j].rgbtRed = min(max((int)(factor * red + bias), 0), 255);
						triple[i][j].rgbtGreen = min(max((int)(factor * green + bias), 0), 255);
						triple[i][j].rgbtBlue = min(max((int)(factor * blue + bias), 0), 255);
					}
			
				}
			
			}
			
		break;
		case 9: ;
			k = 0;
			// emboss
			double filterss[filterHeight][filterWidth] =
			{
			 -2, -1,  0,
		   		-1, 1,  1,
		   		0, 1,  2
			};
			factor = 1.0;bias = 0.0;
		while(k < 2)
		{
			for(i = 0; i < biHeight; i++)
			{
				for(j = 0; j < biWidth; j++)
				{
					// border black
					if((i == 0) || (i == (biHeight - 1)) || (j == 0) || (j == (biWidth - 1)))
					{	
						triple[i][j].rgbtBlue = 0;
             			triple[i][j].rgbtGreen = 0;
						triple[i][j].rgbtRed = 0;
					}
					else
					{
						double red = 0, green = 0, blue = 0;								
						red = (triple[i][j].rgbtRed * filterss[1][1]) + (triple[i][j + 1].rgbtRed * filterss[1][2]) + (triple[i][j - 1].rgbtRed * filterss[1][0]) + (triple[i - 1][j].rgbtRed * filterss[0][1]) + (triple[i + 1][j].rgbtRed * filterss[2][1]) + (triple[i - 1][j - 1].rgbtRed * filterss[0][0]) + (triple[i - 1][j + 1].rgbtRed * filterss[0][2]) + (triple[i + 1][j - 1].rgbtRed * filterss[2][0]) + (triple[i + 1][j + 1].rgbtRed * filterss[2][2]); 
						green = (triple[i][j].rgbtGreen * filterss[1][1]) + (triple[i][j + 1].rgbtGreen * filterss[1][2]) + (triple[i][j - 1].rgbtGreen * filterss[1][0]) + (triple[i - 1][j].rgbtGreen * filterss[0][1]) + (triple[i + 1][j].rgbtGreen * filterss[2][1]) + (triple[i - 1][j - 1].rgbtGreen * filterss[0][0]) + (triple[i - 1][j + 1].rgbtGreen * filterss[0][2]) + (triple[i + 1][j - 1].rgbtGreen * filterss[2][0]) + (triple[i + 1][j + 1].rgbtGreen * filterss[2][2]);
						blue = (triple[i][j].rgbtBlue * filterss[1][1]) + (triple[i][j + 1].rgbtBlue * filterss[1][2]) + (triple[i][j - 1].rgbtBlue * filterss[1][0]) + (triple[i - 1][j].rgbtBlue * filterss[0][1]) + (triple[i + 1][j].rgbtBlue * filterss[2][1]) + (triple[i - 1][j - 1].rgbtBlue * filterss[0][0]) + (triple[i - 1][j + 1].rgbtBlue * filters[0][2]) + (triple[i + 1][j - 1].rgbtBlue * filterss[2][0]) + (triple[i + 1][j + 1].rgbtBlue * filterss[2][2]);	
						
						triple[i][j].rgbtRed = min(max((int)(factor * red + bias), 0), 255);
						triple[i][j].rgbtGreen = min(max((int)(factor * green + bias), 0), 255);
						triple[i][j].rgbtBlue = min(max((int)(factor * blue + bias), 0), 255);
					}
			
				}
			
			}
			k++;
		}
		break;
		case 10: ;
			int a;
			RGBtoHSL(biHeight, biWidth, triple, hsl);
			system("cls");
			printf("\n\n");
			printf("To Which Degree you have to change the Hue......\n");
			printf("1. 0 Degree.\n");
			printf("2. 60 Degree.\n");
			printf("3. 180 Degree.\n");
			printf("4. 315 Degree.\n");
			printf("Your Choice : ");
			scanf("%d", &a);
    
		    for (i = 0; i < biHeight; i++)
		    {
		    	for (j = 0; j < biWidth; j++)
		    	{
		    		if (a == 1)
		    		{
		    			//change Hue
				        hsl[i][j].hslHue += (int)(255 * 1);
				        hsl[i][j].hslHue %= 255;
					}
					else if (a == 2)
					{
						//change Hue
				        hsl[i][j].hslHue += (int)(42.5 * 1);
				        hsl[i][j].hslHue %= 255;
					}
					else if (a == 3)
					{
						//change Hue
				        hsl[i][j].hslHue += (int)(127.5 * 1);
				        hsl[i][j].hslHue %= 255;
					}
					else
					{
						//change Hue
				        hsl[i][j].hslHue += (int)(223.125 * 1);
				        hsl[i][j].hslHue %= 255;
					}
				}
			}
			HSLtoRGB (biHeight, biWidth, hsl, triple);
		break;
		case 11: ;
			int choice;
			system("cls");
			printf("\n\n");
			printf("1. Increase Saturation.\n");
			printf("2. Decrease Saturation.\n");
			printf("Your Choice : ");
			scanf("%d", &choice);
			RGBtoHSL(biHeight, biWidth, triple, hsl);
    		
    		
    		if (choice == 1)
    		{
    			for (i = 0; i < biHeight; i++)
			    {
			    	for (j = 0; j < biWidth; j++)
			    	{
				        // change saturation
				        hsl[i][j].hslSaturation = (int)(hsl[i][j].hslSaturation * 2.5);
			    		if(hsl[i][j].hslSaturation > 255) 
						hsl[i][j].hslSaturation = 255;
					}
				}	
			}
			else
			{
				for (i = 0; i < biHeight; i++)
			    {
			    	for (j = 0; j < biWidth; j++)
			    	{
				        // change saturation
				        hsl[i][j].hslSaturation = (int)(hsl[i][j].hslSaturation * 0.5);
			    		if(hsl[i][j].hslSaturation < 0) 
							hsl[i][j].hslSaturation = 0;
					}
				}	
			}
		    
			HSLtoRGB (biHeight, biWidth, hsl, triple);
			break;		
	}
}
 float max(float a, float b)
 {
 	return (a > b ? a : b);
 }
 
 float min(float a, float b)
 {
 	return (a < b ? a : b);
 }
 
 
 void RGBtoHSL(int biHeight, int biWidth, RGBTRIPLE **triple, HSL **hsl)
 {
 	/*HSL **hsl;
	hsl = (HSL **) malloc(sizeof(HSL *) * biHeight);
	int ii;
	for(ii = 0; ii < biHeight; ii++) 
	{
		hsl[ii] = (HSL *) malloc(sizeof(HSL) * biWidth);
	}*/
 	
 	
 	int i, j;
	float r, g, b, h, s, l; //this function works with floats between 0 and 1
	
	for(i = 0; i < biHeight; i++)
	{
		for(j = 0; j < biWidth; j++)
		{
			r = triple[i][j].rgbtRed / 256.0;
    		g = triple[i][j].rgbtGreen / 256.0;
    		b = triple[i][j].rgbtBlue / 256.0;
    		
    		float maxColor = max(r, max(g, b));
    		float minColor = min(r, min(g, b));
    		
    		if (maxColor == minColor)
    		{
    			h = 0.0; //it doesn't matter what value it has
        		s = 0.0;
        		l = r; //doesn't matter if you pick r, g, or b
			}
			else
			{
				l = (minColor + maxColor) / 2;

		        if(l < 0.5) 
					s = (maxColor - minColor) / (maxColor + minColor);
		        else 
					s = (maxColor - minColor) / (2.0 - maxColor - minColor);
		
		        if
					(r == maxColor) h = (g - b) / (maxColor - minColor);
		        else if
					(g == maxColor) h = 2.0 + (b - r) / (maxColor - minColor);
		        else 
					h = 4.0 + (r - g) / (maxColor - minColor);
		
		        h /= 6; //to bring it to a number between 0 and 1
		        if(h < 0) 
					h++;
			}
			
			hsl[i][j].hslHue = (int)(h * 255.0);
    		hsl[i][j].hslSaturation = (int)(s * 255.0);
    		hsl[i][j].hslLightness = (int)(l * 255.0);
		}
	}
    
 }
 
 void HSLtoRGB (int biHeight, int biWidth, HSL **hsl, RGBTRIPLE **triple)
 {
 	int i, j;
	 
	for (i = 0; i < biHeight; i++)
	{
		for (j = 0; j < biWidth; j++)
		{
			float r, g, b, h, s, l; //this function works with floats between 0 and 1
		    float temp1, temp2, tempr, tempg, tempb;
		    h = hsl[i][j].hslHue / 256.0;
		    s = hsl[i][j].hslSaturation / 256.0;
		    l = hsl[i][j].hslLightness / 256.0;
			
			//If saturation is 0, the color is a shade of gray
    		if(s == 0) 
				r = g = b = l;
			else
			{
				//Set the temporary values
		        if(l < 0.5) 
					temp2 = l * (1 + s);
		        else 
					temp2 = (l + s) - (l * s);
					
		        temp1 = 2 * l - temp2;
		        tempr = h + 1.0 / 3.0;
		        
		        if(tempr > 1) 
					tempr--;
		        tempg = h;
		        tempb = h - 1.0 / 3.0;
		        
		        if(tempb < 0) 
					tempb++;
		
		        //Red
		        if(tempr < 1.0 / 6.0) 
					r = temp1 + (temp2 - temp1) * 6.0 * tempr;
		        else if(tempr < 0.5) 
					r = temp2;
		        else if(tempr < 2.0 / 3.0) 
					r = temp1 + (temp2 - temp1) * ((2.0 / 3.0) - tempr) * 6.0;
		        else 
					r = temp1;
		
		        //Green
		        if(tempg < 1.0 / 6.0) 
					g = temp1 + (temp2 - temp1) * 6.0 * tempg;
		        else if(tempg < 0.5) 
					g = temp2;
		        else if(tempg < 2.0 / 3.0) 
					g = temp1 + (temp2 - temp1) * ((2.0 / 3.0) - tempg) * 6.0;
		        else 
					g = temp1;
		
		        //Blue
		        if(tempb < 1.0 / 6.0) 
					b = temp1 + (temp2 - temp1) * 6.0 * tempb;
		        else if(tempb < 0.5) 
					b = temp2;
		        else if(tempb < 2.0 / 3.0) 
					b = temp1 + (temp2 - temp1) * ((2.0 / 3.0) - tempb) * 6.0;
		        else 
					b = temp1;	
			}
			
			triple[i][j].rgbtRed = (int)(r * 255.0);
		    triple[i][j].rgbtGreen = (int)(g * 255.0);
		    triple[i][j].rgbtBlue = (int)(b * 255.0);	
		}
	}	
 }



 void delay()
 {
 	int c = 1, d = 1;
 
   for ( c = 1 ; c <= 3000 ; c++ )
   {
   
       for ( d = 1 ; d <= 30000 ; d++ )
       {}
   }
 
   return;
 }
 
  void about()
 {
 	system("cls");
 	printf("\n\n");
 	printf("\tThis is a simple photo editor program.\n");
 	printf("\tThis Program only focuses on \"BITMAP\" images of 24 bit.\n");
 	printf("\tThis uses some basic filtering process like \"GreyScale\",\n");
 	printf("\t\"Hue Changing\", \"Blurring\" and \"etc.\n");
 	printf("\t\t\t\tand don't forget it is ");
 	int i, n;
 	for (i = 0; i < 12; i++)
 	{
 		delay();
	}
 	char c[] = "An Artist";
 	for (i = 0; i < 9; i++)
 	{
 		printf("%c", c[i]);
 		delay();
	 }
 	return;
 }
 
 
 

