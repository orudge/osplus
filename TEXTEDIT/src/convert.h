/* Revision History:
 *
 * 05/01/2002: Cleaned up indentation (orudge)
 * 27/12/2004: Finished new converter system (orudge)
 */

// Converters

#ifdef __cplusplus
	extern "C" {
#endif

typedef struct CONVERTER_INFO
{
   int id;          // ID number
   char *ext;       // File extension (eg, "RTF")
   char *load_fn;   // Converter to load file
   char *params;    // Any extra parameters to pass
   struct CONVERTER_INFO *next;
} CONVERTER_INFO;

int convert_file(char *fn_in, char *fn_out, char *converter, char *params, char *error_out);
void register_converter_file_type(char *ext, char *load_fn, char *params);
void register_microsoft_converters();
int convert_text_file(char *filename_in, char *filename_out, char *error_out, int start_from);

#define CONVERT_DELETE_FILE           2

#define ERROR_CNV_OK                  1
#define ERROR_CNV_CANNOT_OPEN_SRC     4
#define ERROR_CNV_CANNOT_OPEN_DEST    8
#define ERROR_CNV_PARSE_ERROR         16
#define ERROR_CNV_INCORRECT_FORMAT    32
#define ERROR_CNV_FROM_RTF            64
#define ERROR_CNV_RTF_ERROR           128
#define ERROR_CNV_LOAD                256
#define ERROR_CNV_INIT_FAILED         512
#define ERROR_CNV_FAILED              1024
#define ERROR_CNV_PARSE_ERROR_RTF     2048

#ifdef WIN32_DLL
   #define CONV_EXPORT      __declspec(dllexport)
#else
   #define CONV_EXPORT
#endif

#ifdef __cplusplus
	}
#endif
