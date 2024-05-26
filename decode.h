#include "types.h" // Contains user defined types

#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4

/* 
 * Structure to store information required for
 * decoding .bmp file to output.txt file
 * Info about output and required data for decoding
 * is also stored.
 */


typedef struct _DecodeInfo
{
    /*Stego.bmp file info*/
    char *stego_info_fname;                        //Stego.bmp file name.
    FILE *fptr_stego_info;                         //Stego.bmp file poiter.
    char image_data[MAX_IMAGE_BUF_SIZE];           //Reusable char buffer to store RGB data.
    int secret_file_extn_size;                     //Secret file extension size.
    char secret_decode_file_extn[5];               //Secret file extension size.
    int decode_secret_data_size;                   //Secret file data size.

    /*Output file info*/
    char *output_file_name;                        //Output file name.
    FILE *fptr_output_file;                        //Output file pointer.

} DecodeInfo;

/*Decoding function definations*/

/* Check operation type */
OperationType check_operation_type(char *argv[]);

/* Read and validate Encode args from argv */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* Perform the decoding */
Status do_decoding(DecodeInfo *decInfo);

/* Get File pointers for i/p and o/p files */
Status open_dfiles(DecodeInfo *decInfo);

/*Skip 54 byte of header file*/
Status skip_header(DecodeInfo *decInfo);

/* Decode Magic String */
Status decode_magic_string(const char *magic_string, DecodeInfo *decInfo);

/* decode function, which does the real decoding */
Status decode_data_from_image(char *data, int size, DecodeInfo *decInfo);

/* decode a byte from LSB of stego img data */
Status decode_byte_from_lsb(char *data, char *image_buffer);

/* decode extension size of secret file extension */
Status decode_secret_file_extn_size(DecodeInfo *decInfo);

/* decode size of secret file extn from stego img data */
Status decode_size_from_lsb(char *image_buffer, DecodeInfo *decInfo);

/* decode secret file extenstion */
Status decode_secret_file_extn(int size, DecodeInfo *decInfo);

/* decode secret file extn data */
Status decode_secret_file_extn_data(int size, DecodeInfo *decInfo);

/* decode secret file data size */
Status decode_secret_file_data_size(DecodeInfo *decInfo);

/* decode secret file data size from LSB */
Status decode_secret_file_data_size_lsb(char *image_buffer, DecodeInfo *decInfo);

/* decode secret data from stego.bmp img */
Status decode_secret_data(int size, DecodeInfo *decInfo);
