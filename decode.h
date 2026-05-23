#ifndef DECODE_H
#define DECODE_H
#include <stdio.h>

#include "types.h" // Contains user defined types

/*
 * Structure to store information required for
 * encoding secret file to source Image
 * Info about output and intermediate data is
 * also stored
 */

typedef struct _DecodeInfo
{

    /* Output File Info */
    char secret_fname[20];       // To store the output file name
    FILE *fptr_secret;        // To store the output file address
    char extn_secret_file[5]; // To store the output file extension
    long size_secret_file;    // To store the size of the output data
    
    /* Stego Image Info */
    char *stego_image_fname; // To store the stego file name
    FILE *fptr_stego_image;  // To store the address of stego image file
    
    /* */
    // char decode_char;
    // char sizebuffer[32];
    // char databuffer[8];
    

} DecodeInfo;

/* Decoding function prototype */

/* Read and validate Decode args from argv */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* Perform the encoding */
Status do_decoding(DecodeInfo *decInfo);

/* Get File pointers for i/p and o/p files */
Status open_files_for_decoding(DecodeInfo *decInfo);

/* check capacity */
//Status check_capacity(DecodeInfo *decInfo);

/* Get image size */
uint get_image_size_for_bmp(FILE *fptr_image);

/* Get file size */
uint get_file_size(FILE *fptr);

/* Skip bmp image header */
Status skip_bmp_header(DecodeInfo *decInfo);

/* Copy magic string size to stego */
// Status decode_magic_string_size(DecodeInfo *decInfo);

/* Store Magic String */
Status decode_magic_string(DecodeInfo *decInfo);

/*decode extension size*/
//Status decode_secret_file_extn_size(int* size, DecodeInfo *decInfo);

/* decode secret file extenstion */
Status decode_secret_file_extn(DecodeInfo *decInfo);

/* decode secret file size */
Status decode_secret_file_size(DecodeInfo *decInfo);

/* decode secret file data*/
Status decode_secret_file_data(DecodeInfo *decInfo);

/* decode a byte from LSB of image data array */
Status decode_byte_from_lsb(char *buffer, char* data);

// decode a size from lsb
int decode_size_from_lsb(char *imageBuffer);
 

#endif