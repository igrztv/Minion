#include "readFile.h"

bool read_file(char *file_name, vector<Point> &points)
{
	FILE *pFile;
	pFile = fopen (file_name,"r");

	if (pFile == NULL)
	{
		perror ("Error opening file");
		return false;
	}

	while(!feof(pFile))
	{
		int x = 0, y = 0;
		if(fscanf(pFile,"%d %d\n",&x,&y))
		{
			points.push_back(Point(x,y));
			printf("%d %d\n",x,y);
		}else{
			printf("text format is incorrect!\n");
			fclose(pFile);
			return false;
		}
	}

	fclose(pFile);

	return true;
}