#include <stdio.h>
#include "encode.h"
#include "types.h"
#include "common.h"
#include <string.h>

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
	perror("fopen");
	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

	return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
	perror("fopen");
	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

	return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
	perror("fopen");
	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

	return e_failure;
    }

    // No failure return e_success
    return e_success;
} n  

/*Read and validating the argument defination*/
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    if(strstr(argv[2], ".") != NULL) 
    {

	if(strcmp (strstr (argv[2], "."), ".bmp") == 0) {
	    encInfo -> src_image_fname = argv[2];
	} 
	else {
	    return e_failure;
	} 
    } 

    else {
	return e_failure;
    }

    if(argv[3] != NULL)
    {

	if(strstr(argv[3], ".") != NULL) {

	    if(strcmp (strstr (argv[3], "."), ".txt") == 0) {
		encInfo -> secret_fname = argv[3];
	    } 
	    else  {
		return e_failure;
	    }
	}
	else {
	    return e_failure;
	}
    } 

    else {
	return e_failure;
    }

    if(argv[4] != NULL)  {

	encInfo -> stego_image_fname = argv[4];
    }
    else {
	encInfo -> stego_image_fname = "Stego.bmp";
    }

    return e_success;
}

/*Function return size secret.txt file*/
uint get_file_size(FILE *fptr_secret)
{
    fseek(fptr_secret , 0 , SEEK_END);
    return ftell(fptr_secret);
}

/*check_capacity() defination*/
Status check_capacity(EncodeInfo *encInfo)
{
    encInfo -> image_capacity = get_image_size_for_bmp(encInfo -> fptr_src_image);
    encInfo ->  size_secret_file = get_file_size(encInfo -> fptr_secret);

    /*
     * Total size of stego.bmp
     * 54 --> Header data of beautiful.bmp
     * 32 --> Size of magic string in bytes
     * 32 --> Size of .txt extension in int
     * 32 --> Size of .txt extension in char
     * 64 --> size of secrate data.
     */

    if(encInfo -> image_capacity > (54 + (strlen(MAGIC_STRING) * 8) + 32 + 32 + 32 + (encInfo -> size_secret_file * 8))) {
	return e_success;
    }
    else {
	return e_failure;
    }
}

/*copy_bmp_header() defination*/
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_stego_image)
{
    fseek(fptr_src_image, 0, SEEK_SET);
    char temp_header[54];
    fread(temp_header, sizeof(char), 54, fptr_src_image);
    fwrite(temp_header, sizeof(char), 54, fptr_stego_image);
    return e_success;
}

/*encode_byte_to_lsb() defination*/
Status encode_byte_to_lsb(char data, char *image_buffer)
{
    uint mask = 1 << 7;
    for(int i = 0; i < 8; i++)
    {
	image_buffer[i] = (image_buffer[i] & 0xFE) | ((data & mask) >> (7 - i)); //Clear the LSB bit of RGB 
	mask = mask >> 1; //Changing the mask.
    }
    return e_success;
}

/*encode_data_to_image() defination*/
Status encode_data_to_image(char *data, int size, EncodeInfo *encInfo)
{
    for(int i = 0; i < size; i++)
    {
	fread(encInfo -> image_data, sizeof(char), 8, encInfo -> fptr_src_image);
	encode_byte_to_lsb(data[i], encInfo -> image_data);
	fwrite(encInfo -> image_data, sizeof(char), 8, encInfo -> fptr_stego_image);
    } 
    return e_success;
}


/*encode_magic_string() defination*/
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    encode_data_to_image(MAGIC_STRING, strlen(MAGIC_STRING), encInfo);
    return e_success;
}

/*encode_size_to_lsb() defination*/
Status encode_size_to_lsb(int size, char *image_buffer)
{
    uint mask = 1 << 31;
    for(int i = 0; i < 32; i++)
    {
	image_buffer[i] = (image_buffer[i] & 0XFE) | ((size & mask) >> (31 - i));  //Clear the LSB bit of RGB 
	mask = mask >> 1; //Changing the mask.        
    }
    return e_success;

}

/*encode_secret_file_extn_size() defination*/
Status encode_secret_file_extn_size(int file_size, EncodeInfo *encInfo)
{
    char temp_buffer[32];
    fread(temp_buffer, sizeof(char), 32, encInfo -> fptr_src_image);
    encode_size_to_lsb(file_size, temp_buffer);
    fwrite(temp_buffer, sizeof(char), 32, encInfo -> fptr_stego_image);
    return e_success;

}

/*encode_secret_file_extn() defination*/
Status encode_secret_file_extn(char *file_extn, EncodeInfo *encInfo)
{
    file_extn = ".txt";
    encode_data_to_image(file_extn, strlen(file_extn), encInfo);
    return e_success;
}

/*e_encode_secret_file_size() defination*/
Status e_encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    char temp_buffer[32];
    fread(temp_buffer, sizeof(char), 32, encInfo -> fptr_src_image);
    encode_size_to_lsb(file_size, temp_buffer);
    fwrite(temp_buffer, sizeof(char), 32, encInfo -> fptr_stego_image);
    return e_success;
}

/*encode_secret_file_data() defination*/
Status encode_secret_file_data(EncodeInfo *encInfo)
{
    char secret_ch;
    fseek(encInfo -> fptr_secret, 0, SEEK_SET);
    for(int i = 0; i < encInfo -> size_secret_file; i++)
    {
	fread(encInfo -> image_data, 8, sizeof(char), encInfo -> fptr_src_image);
	fread(&secret_ch, sizeof(char), 1, encInfo -> fptr_secret);
	encode_byte_to_lsb(secret_ch, encInfo -> image_data);
	fwrite(encInfo -> image_data, sizeof(char), 8, encInfo -> fptr_stego_image);
    }
    return e_success;
}

/*copy_remaining_img_data() defination*/
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    char temp_ch;
    while(fread(&temp_ch, sizeof(char), 1, fptr_src) > 0)
    {
	fwrite(&temp_ch, sizeof(char), 1, fptr_dest);
    }
    return e_success;
}


/*do_encoding() defination*/
Status do_encoding(EncodeInfo *encInfo)
{
    if (open_files(encInfo) == e_success) {
	printf("All required files are successfully opened\n");

	if (check_capacity(encInfo) == e_success) {
	    printf("Encoding is possible with .bmp\n");

	    if (copy_bmp_header(encInfo -> fptr_src_image, encInfo -> fptr_stego_image) == e_success) {
		printf("The Header content copied successfully\n");

		if (encode_magic_string(MAGIC_STRING, encInfo) == e_success) {
		    printf("Magic string encoded successfully\n");

		    if (encode_secret_file_extn_size(strlen(".txt"), encInfo) == e_success) {
			printf("Secret file extension size encoded successfully\n");

			if (encode_secret_file_extn(encInfo -> extn_secret_file, encInfo) == e_success) {
			    printf("Secret file extension encoded successfully\n");

			    if(e_encode_secret_file_size(encInfo -> size_secret_file, encInfo) == e_success) {
				printf("Secret file size encoded successfully\n");

				if(encode_secret_file_data(encInfo) == e_success) {
				    printf("Secret file data encoded successfully\n");

				    if(copy_remaining_img_data(encInfo -> fptr_src_image, encInfo -> fptr_stego_image) == e_success) {
					printf("Remaining image data encoded successfully\n");

				    } else {
					printf("Error: Fail to encode remaining image data\n");
					return e_failure;
				    }

				} else {
				    printf("Error: Fail to encode secret data\n");
				    return e_failure;
				}

			    } else {
				printf("Error: Fail to encode size of secret file\n");
				return e_failure;
			    }
			} else {
			    printf("Error: Fail to encode secret file extension\n");
			    return e_failure;
			}
		    } else {
			printf("Error: Fail to encode secret file extension size\n");
			return e_failure;
		    }
		} else {
		    printf("Error: Fail to encode magic string\n");
		    return e_failure;
		}
	    } else {
		printf("Error: unsuccessful to copy header\n");
		return e_failure;
	    }
	} else {
	    printf("Error: Encoding is not possible with .bmp\n");
	    return e_failure;
	}
    } else {
	printf("Error: Failed to open all required files\n");
	return e_failure;
    }

    return e_success;
}


