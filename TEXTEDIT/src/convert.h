// Converters

#ifdef __cplusplus
	extern "C" {
#endif

typedef struct CONVERTER_INFO
{
	char *ext;       // File extension (eg, "RTF")
	char *load_fn;   // Converter to load file
	struct CONVERTER_INFO *next;
} CONVERTER_INFO;

int convert_file(char *fn_in, char *fn_out, char *converter, char *error_out, int *delete_file);
void register_converter_file_type(char *ext, char *load_fn);
int convert_text_file(char *filename_in, char *filename_out, char *error_out, int *delete_file);

#ifdef __cplusplus
	}
#endif
