#include <stdarg.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <stdio.h>

#include "misc.h"
#include "err.h"

int hexstr_to_str(char *input, unsigned char *output)
{
	char code[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };
	int loop, flag;
	int i;

	i = 0;
	loop = 0;
	int num = 0;
	while (input[loop] != '\0') {
		flag = 0;
		for (int j = 0; j < 16; j++) {
			if (code[j] == input[loop]) {
				if ((loop % 2) == 0)
					num = j * 16;
				else {
					num = num + j;
					//sprintf((char *)(output + i), "%c", num);
					output[i]=(char)num;
					i++;
					num = 0;
				}
				flag = 1;
				break;
			}
		}
		loop++;
		if (flag == 0)
			break;
	}
	output[i++] = '\0';
	return flag;
}

void str_to_hexstr(unsigned char input[], int in_len, char *output)
{
	int loop;
	int i, j = 0;

	i = 0;
	loop = 0;

	while (j++ < in_len) {
		sprintf((char *)(output + i), "%02x", input[loop]);
		loop++;
		i += 2;
	}
	output[i++] = '\0';
}

int alloc_args(int argc, char ***argv)
{
	int ret = OK;

	*argv = calloc(argc, sizeof(char *));
	if (*argv == NULL)
		return ERR_MEMORY_ALLOC;
	for (int i = 0; i < argc; i++) {
		(*argv)[i] = calloc(arg_size, sizeof(char));
		if ((*argv)[i] == NULL)
			return ERR_MEMORY_ALLOC;
	}
	return ret;
}

void dealloc_args(int argc, char ***argv)
{
	if (*argv) {
		for (int i = 0; i < argc; i++) {
			if ((*argv)[i])
				free((*argv)[i]);
		}
		free(*argv);
	}
}

void memset_args(int argc, char ***argv)
{
	if (*argv) {
		for (int i = 0; i < argc; i++) {
			if ((*argv)[i])
				memset((*argv)[i], 0x00, arg_size);
		}
	}
}

char **copy_args(int argc, ...)
{
	int32_t ret = OK;
	char **argv = NULL;
	va_list valist, va_copy;

	ret = alloc_args(argc, &argv);
	if (ret != OK) {
		printf("error :: %d", ret);
		return NULL;
	}

	va_start(valist, argc);
	va_copy(va_copy, valist);

	for (int i = 0; i < argc; i++) {
		if (strlen(va_arg(va_copy, char *)) > arg_size) {
			ret = ERR_ARG_SIZE_TOO_LONG;
			printf("Error::%d::%s", ret, va_arg(valist, char *));
			printf("Error in running the command::%s", argv[1]);
			goto end;
		}
		strcpy(argv[i], va_arg(valist, char *));
	}
end:
	va_end(valist);
	va_end(va_copy);
	if (ret != OK) {
		dealloc_args(argc, &argv);
		return NULL;
	}
	return argv;
}

