#include <stdio.h>
#include <string.h>
#include "decode.h"
#include "types.h"
#include "common.h"

/*Function Definations*/

/* Get File pointers for i/p and o/p files */
Status open_dfiles(DecodeInfo *decInfo)
{
    //Encoded stego img file
    decInfo -> fptr_stego_info = fopen(decInfo -> stego_info_fname, "r");
    // Do Error handling
    if(decInfo -> fptr_stego_info == NULL)
    {
	perror("fopen");
	fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo -> stego_info_fname);

	return e_failure;
    }

    //Output file
    decInfo -> fptr_output_file = fopen(decInfo -> output_file_name, "w");
    if(decInfo -> fptr_output_file == NULL)
    {
	perror("fopen");
	fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo -> output_file_name);

	return e_failure;
    }
    //no failure return esuccess
    return e_success;

}

/* Read and validating the argument pass from command line */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    // Validate stego_info_fname (argv[2])
    if (strstr(argv[2], ".") != NULL)
    {
	if (strcmp(strstr(argv[2], "."), ".bmp") == 0)
	{
	    decInfo -> stego_info_fname = argv[2];
	}
	else
	{
	    return e_failure;
	}
    }
    else
    {
	return e_failure;
    }

    // Validate file_name (argv[3])
    if (argv[3] != NULL)
    {
	if (strstr(argv[3], ".") != NULL)
	{
	    if (strcmp(strstr(argv[3], "."), ".txt") == 0)
	    {
		decInfo -> output_file_name = argv[3];
	    }
	    else
	    {
		return e_failure;
	    }
	}
	else
	{
	    return e_failure;
	}
    }
    else
    {
	// Default file_name if not provided
	decInfo -> output_file_name = "Output.txt";
    }

    return e_success;
}

/*Skip 54 byte header data form stego.bmp img file*/
Status skip_header(DecodeInfo *decInfo)
{
    fseek(decInfo -> fptr_stego_info , 54 , SEEK_SET);
    return e_success;
}

/* Decode magic string form stego.bmp img file */
Status decode_magic_string(const char *magic_string, DecodeInfo *decInfo)
{
    decode_data_from_image(MAGIC_STRING, strlen(MAGIC_STRING), decInfo);
    {
	return e_success;
    }
}

/* Read 8 byte of RGB for decoding magic string form stego.bmp img file */
Status decode_data_from_image(char *data, int size, DecodeInfo *decInfo)
{
    char *magic_string = MAGIC_STRING ;
    for(int i = 0; i < size; i++)
    {
	char temp_ch = 0;
	fread(decInfo -> image_data, 8 , sizeof(char) , decInfo -> fptr_stego_info);
	decode_byte_from_lsb(&temp_ch, decInfo -> image_data);
	if(temp_ch != magic_string[i])
	{
	    return e_failure;
	}
    }
    return e_success;
}

/* Decode form LSB of stego.bmp img file */
Status decode_byte_from_lsb(char *data, char *image_buffer)
{
	/* 
     * It will perform decoding on 8 byte of RGB
     * image_buffer[i] & 0X01) will get encode bit at LSB
	 * ((image_buffer[i] & 0X01) << mask - i) will shift bit 
     * *data = (*data | ((image_buffer[i] & 0X01) << mask - i)) store the bit.
     */

    int mask = 7;
    for(int i = 0; i < 8; i++)
    {
	*data = (*data | ((image_buffer[i] & 0X01) << mask - i));

    }
    return e_success;
}

/* Read 32 byte of RGB from stego.bmp img file to decode secret file extension size */
Status decode_secret_file_extn_size(DecodeInfo *decInfo)
{
    char temp_buffer[32];
    fread(temp_buffer, sizeof(char), 32, decInfo -> fptr_stego_info);
    decode_size_from_lsb(temp_buffer, decInfo);
}


/* Decode the size of extension from LSB of stego.bmp img file */
Status decode_size_from_lsb(char *image_buffer, DecodeInfo *decInfo)
{
    uint mask = 31;
    decInfo -> secret_file_extn_size = 0;
    for(int i = 0; i < 32; i++)
    {
	decInfo -> secret_file_extn_size  = (decInfo -> secret_file_extn_size | ( (image_buffer[i] & 0X01) << mask - i));
    }
    return e_success;
}

/* Comparing the encoded extension and passed extension */
Status decode_secret_file_extn( int size, DecodeInfo *decInfo)
{
    decode_secret_file_extn_data(size,  decInfo);
    {
	if(strcmp(strstr(decInfo -> output_file_name,"."),decInfo -> secret_decode_file_extn) == 0);
	{
	    return e_success;
	}

    }
}

/* Decode secret file extension from stego.bmp img file */
Status decode_secret_file_extn_data(int size, DecodeInfo *decInfo)
{
    for ( int i = 0 ; i < size ; i++)
    {
	char ch = 0;
	//Read 8 byte of RGB data form stego.bmp.
	fread(decInfo -> image_data, sizeof(char), 8, decInfo -> fptr_stego_info);
	//Function call to decode data from LSB
	decode_byte_from_lsb(&ch,  decInfo -> image_data);
	decInfo -> secret_decode_file_extn[i] = ch;
    }

    return e_success;
}

/* Read 32 byte RGB form Stego.bmp img file for decoding */
Status decode_secret_file_data_size(DecodeInfo *decInfo)
{
    char temp_buffer[32];
    fread(temp_buffer, sizeof(char), 32, decInfo -> fptr_stego_info);
    decode_secret_file_data_size_lsb(temp_buffer, decInfo);

    return e_success;
}

/* Decode secret data size from LSB of stego.bmp img file */
Status decode_secret_file_data_size_lsb(char *image_buffer, DecodeInfo *decInfo)
{
    uint mask = 31;
    //Initializing secret_data_size as 0
    decInfo -> decode_secret_data_size = 0;
    for(int i = 0; i < 32; i++)
    {
	//Logic for decoding the secret data size form .bmp img file.
	decInfo -> decode_secret_data_size  = (decInfo -> decode_secret_data_size | ( (image_buffer[i] & 0X01) << mask - i));
    }

    return e_success;
}

/* Decode secrate data form stego.bmp img file */
Status decode_secret_data(int size, DecodeInfo *decInfo)
{
    for(int i = 0; i < size; i++)
    {
	char ch = 0;
	//Reading 8 byte of RGB form form stego.bmp 
	fread(decInfo -> image_data, 8, sizeof(char), decInfo -> fptr_stego_info);
	//Function call to decode data from LSB
	decode_byte_from_lsb(&ch,  decInfo -> image_data);
	//Storing decoded data in output file.
	fwrite(&ch, sizeof(char), 1, decInfo -> fptr_output_file);
    }
    return e_success;

}

/* All function call required to decode the information from stego.bmp file */
Status do_decoding(DecodeInfo *decInfo) 
{
	// Function call for opening all require file for decoding 
    if (open_dfiles(decInfo) == e_success)	
    {
	printf("All required files are successfully opened\n");

	//Function call for skip 54 byte header data
	if (skip_header(decInfo) == e_success) 
	{
	    printf("Header data skipped successfully\n");

		//Function call for decode magic string form .bmp file
	    if (decode_magic_string(MAGIC_STRING, decInfo) == e_success)
	    {
		printf("Magic string decoded successfully\n");

		//Function call for decode secret file extn size from .bmp file 
		if (decode_secret_file_extn_size(decInfo) == e_success)
		{
		    printf("Secret file extn size decoded successfully\n");

			//Function call for decode secret file extn from .bmp file
		    if (decode_secret_file_extn(decInfo -> secret_file_extn_size, decInfo) == e_success)
		    {
			printf("Secret file extn decoded successfully\n");

			//Function call for decode secret file extn from .bmp file
			if(decode_secret_file_data_size(decInfo) == e_success)
			{
			    printf("Secret file data size decoded successfully\n");

				//Function call for decode secret file data size from .bmp file
			    if(decode_secret_data(decInfo -> decode_secret_data_size, decInfo) == e_success) {
				printf("Secret data decoded successfully\n");
				} 

			    else 
			    {
				printf("Error: Fail to decode secret data\n");
				return e_failure;
			    }
			} 
			else 
			{
			    printf("Error: Fail to decode secret file data size\n");
			    return e_failure;
			}

		    } 
		    else 
		    {
			printf("Error: Fail to decode secret file data size\n");
			return e_failure;
		    }
		} 
		else 
		{
		    printf("Error: Fail to decode secret file extn\n");
		    return e_failure;
		}
	    } 
	    else 
	    {
		printf("Error: Fail to decode magic string\n");
		return e_failure;
	    }
	} 
	else 
	{
	    printf("Error: Fail to skip header data\n");
	    return e_failure;
	}
    } 
    else 
    {
	printf("Error: Failed to open all required files\n");
	return e_failure;
    }

    return e_success;
}
