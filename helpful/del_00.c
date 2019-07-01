#include <stdio.h>
#include <fcntl.h>
int main(int argc,char **argv)
{
	if(argc<3)
	{
		printf("usage:%s inputFile OutputFile\n",argv[0]);
		return -1;
	}
	FILE *fp=fopen(argv[1],"r");
	FILE *fp2=fopen(argv[2],"w");
	if(NULL==fp)
	{
		printf("failed to open input file for reading\n");
		return -1;
	}
	if(NULL==fp2)
	{
		printf("failed to open output file for writing\n");
		return -1;
	}
	while(!feof(fp))
	{
		char buffer[4];
		if(fread(buffer,sizeof(buffer),1,fp)!=1)
		{
			printf("failed to fread.\n");
			break;
		}
		fwrite(buffer,sizeof(buffer)-1,1,fp2);
	}
	fclose(fp);
	fclose(fp2);
	printf("done\n");
	return 0;
}
