#define arg_size 1024

int hexstr_to_str(char *input, unsigned char *output);
void str_to_hexstr(unsigned char input[], int in_len, char *output);
int alloc_args(int argc, char ***argv);
void dealloc_args(int argc, char ***argv);
void memset_args(int argc, char ***argv);
char **copy_args(int argc, ...);
