#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"
#include "decode.h"

int main( int argc , char *argv[])
{
    /* Check operation types */
    if(argc >= 3)
    {
    if(check_operation_type(argv) == e_encode) {
	printf("---------------Started Encoding---------------\n\n");

	EncodeInfo encInfo; // Created encode structure variable.

	if(read_and_validate_encode_args(argv , &encInfo) == e_success) {
	    printf("Successful: Read and validate argument\n");

	    if(do_encoding(&encInfo) == e_success) {
		printf("\nEncoding is successfully done\n");
	    }
	    else {
		printf("Error: Failed to do encoding\n");
	    }
	}
	else {
	    printf("Error: Failed to read and validate argument\n");
	}
    }
    else if(check_operation_type(argv) == e_decode) {
	printf("---------------Started Decoding---------------\n\n");

	DecodeInfo decInfo; //Created decode structure variable.

	if(read_and_validate_decode_args(argv, &decInfo) == e_success) {
		printf("Successful: Read and validate argument\n");

		if(do_decoding(&decInfo) == e_success) {
			printf("\nDecoding is successfully done.\n");
		}
		else {
			printf("Error: Failed to do decoding\n");
		}
	}
    }
    }
    else {
	printf("Error: Invalid Option.");
	printf("\nUsage:");
	printf("\nEncoding --> ./a.out -e beautiful.bmp secrete.txt stego.bmp");
	printf("\nDecoding --> ./a.out -d stego.bmp output.txt\n");
    }
    return e_success;

    return 0;
}

/* check_operation_type() defination */
OperationType check_operation_type(char *argv[])
{
    if(strcmp(argv[1] , "-e") == 0) {
	return e_encode;
    }
    else if(strcmp(argv[1] , "-d") == 0) {
	return e_decode;
    }
    else {
	return e_unsupported;
    }
}

