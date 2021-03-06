/*
Name: Varun Praveen
CUID: C15618128
Email: pravee2@clemson.edu
Course: ECE-6680
Description:This code implements LZW compression
*/



#include"header.h"


int main(int argc, char *argv[])
{



FILE 			*fpt_in,*fpt_out;	
int 			i,j;				
int 			count, iter;
int 			fileSize;					//filesize of file to be compressed
int 			dictSize;					//number of dictionary entries
unsigned char 		byte;						//buffer byte	
unsigned short		code[MAXCOUNT];					//dictionary table containing codes for corresponding dictionary entries
unsigned short 		outcode;					//output code variable written to compressed file
unsigned char		**dictionary;					//table containing dictionary entries
unsigned char 		*data;						//data read from file
unsigned char 		p,c;			
unsigned char		*pstring,*pPlusCstring;				//temporary p string and p+c string
int 			dlength[MAXCOUNT];				//table containing length of dictionary entries
int 			plength,pPlusClength;				//length of strings
int 			k,l,m;
int			elemindict;

//program usage check
if(argc!=3)
{
	printf("Invalid Usage!\nUsage: lzwcomp [input filename] [output filename]\n");
	exit(0);
}

fpt_in = fopen(argv[1], "rb");		
//invalid input file check
if(fpt_in == NULL)
{
	printf("Invalid file name for input file: %s \n", argv[1]);
	exit(0);
}


fileSize = 0;
// computing file size
while(j = fread(&byte,1, 1,fpt_in)!=0)
{ 
	fileSize++;
}	
fseek(fpt_in,0, SEEK_SET);
printf("Filesize : %d\n", fileSize);
fileSize;
//reading data from file
data = (unsigned char*)malloc(fileSize*sizeof(unsigned char));
fread(data, sizeof(unsigned char), fileSize, fpt_in);
fpt_out = fopen(argv[2],"wb");


/****************************** INTIALIZING DICTIONARY WITH ROOTS **********************************/
dictionary = (unsigned char**)malloc(65536*sizeof(unsigned char*));

for(i=0;i<256;i++)
{
	dictionary[i] = (unsigned char*)calloc(1,sizeof(unsigned char));
	dictionary[i][0] = i;
//	printf("dictionary[%d] = %s\n", i, dictionary[i]);
	dlength[i] = 1;
	code[i] = i;
}

iter = 1;

dictSize = 256;

pstring = (unsigned char*)malloc(1);
*pstring = '\0';

for(j=0;j<fileSize;j++)
{
	c = data[j];
//	printf("c read:%cend\nj=%d\n",c,j);
	pPlusCstring = concat(pstring,c);
//	printf("concatenated String: %s\n",pPlusCstring);

	for(i=0;pPlusCstring[i]!='\0';i++);
	pPlusClength = i;
//	printf("concatenated length: %d\n",pPlusClength);
	
	for(i=0;pstring[i]!='\0';i++);
	plength = i;
//	printf("pstring length: %d\n",plength);
	
	i=0;
	//checking if concatenated string is in dictionary
	for(i=0;i<dictSize;i++)
	{
			
		if(pPlusClength==dlength[i])
		{	
//			printf("dlength = %d\n",dlength[i]);
//			printf("length equal at dictionary entry : %d\n",i);
			for(k=0;k<pPlusClength;k++)
			{
				if(pPlusCstring[k]!=dictionary[i][k])
				{
					break;
				}
			}	
		}
//		printf("k before match: %d\n",k);
//		if so check for code of the string and P = P+C		
		if(k==pPlusClength)
		{
			outcode = code[i];
			if(j==fileSize-1)
			{	
				fwrite(&outcode,2,1,fpt_out);
				break;
			}
			else
			{
				free(pstring);
				pstring = (unsigned char*)malloc(pPlusClength*sizeof(unsigned char));
//				printf("l=%d\n",l);
				for(l=0;l<pPlusClength;l++)
					pstring[l] = pPlusCstring[l];
				plength = pPlusClength;
				outcode = code[i];
//				printf("match detected at index %d\n",i);
/*				if(j==fileSize-1)
				fwrite(&outcode,sizeof(unsigned short),1,fpt_out);
*/				break;
			}	
		}
	}
	l=0;	
//	printf("k value: %d\n",k);

// 	if string is not present in the dictionary		
	if(k!=pPlusClength)
	{
//		printf("Pstring at Dict Search at iteration %d : %send\n", iter, pstring);
//		printf("Plength in Dict Search: %d\n", plength);		
		for(i=0;i<dictSize;i++)
		{
			if(plength == dlength[i])
			{
//				printf("output code length equal at: %d\n",i);
				for(l=0;l<plength;l++)
				{
					if(pstring[l] != dictionary[i][l])
						break;
				}
			
			}
			if(l==plength)
			{
//				printf("L = %d\n",l);
//				printf("index for code search: %d\n",i);
//				printf("outcode in if: %d\n", code[i]);	
				outcode = code[i];
				break;
			}
			l=0;
		}
		if(j!=fileSize-1)
		{
			dictSize++;
			code[dictSize-1] = dictSize-1;
			dlength[dictSize-1] = pPlusClength;
			dictionary[dictSize-1] = (unsigned char*)malloc(pPlusClength*sizeof(unsigned char));
			for(m=0;m<pPlusClength;m++)
				dictionary[dictSize-1][m] = pPlusCstring[m];
			free(pstring);	
			fwrite(&outcode,2,1,fpt_out);	
			pstring = (unsigned char*)calloc(1,1);
			*pstring = pPlusCstring[pPlusClength-1];
		}
		else
		{
			fwrite(&outcode,2,1,fpt_out);
			for(m=0; m<255; m++)
			{
				if(c == dictionary[m][0])
				{
					fwrite(&code[m],2,1,fpt_out);
					break;
				}
			}	
		}
	}
	k=0;l=0;m=0;	
}


fclose(fpt_out);
fpt_out = fopen("dictionary.txt","wb");
for(i=0;i<dictSize;i++)
{
	fwrite(dictionary[i],sizeof(unsigned char), dlength[i], fpt_out);
	fprintf(fpt_out,"\n");	
}

fclose(fpt_out);
fclose(fpt_in);
free(data);
free(dictionary);

return 0;
}
