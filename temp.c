#include <stdio.h>
int main()
{
	FILE *fp=NULL;
	char ch;
	fp=fopen("sample.txt","r");
	while((ch = fgetc(fp)) != EOF)
	     printf("%c", ch);

	return 0;
}
