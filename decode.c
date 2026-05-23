#include <stdio.h>
#include <string.h>
#include "decode.h"
#include "types.h"
#include "common.h"

Status do_decoding(DecodeInfo *decInfo)
{

    printf("\n\t<----------DECODING---------->\n\n");
    if (open_files_for_decoding(decInfo) == e_success)
    {
        printf("INFO: Successfully opened files\n");
        if (skip_bmp_header(decInfo) == e_success)
        {
            printf("INFO: Successfully skipped header\n");
            if (decode_magic_string(decInfo) == e_success)
            {
                printf("INFO: Successfully decoded magic string\n");
                if (decode_secret_file_extn(decInfo) == e_success)
                {
                    printf("INFO: Successfully decoded secret file extension\n");
                    if (decode_secret_file_size(decInfo) == e_success)
                    {
                        printf("INFO: Successfully decoded size of secret file\n");
                        if (decode_secret_file_data(decInfo) == e_success)
                        {
                            printf("INFO: Successfully decoded secret file data\n");
                            return e_success;
                        }
                        else
                        {
                            printf("ERROR: Decoding failed\n");
                            return e_failure;
                        }
                    }
                    else
                    {
                        printf("ERROR: Failed to decode size of secret file\n");
                        return e_failure;
                    }
                }
                else
                {
                    printf("ERROR: Failed to decode secret file extension\n");
                    return e_failure;
                }
            }
            else
            {
                printf("ERROR: Failed to decode magic string\n");
                return e_failure;
            }
        }
        else
        {
            printf("ERROR: Failed to skip bmp header\n");
            return e_failure;
        }
    }
    else
    {
        printf("ERROR: Failed to open files for decoding\n");
        return e_failure;
    }
    return e_success;
}

Status open_files_for_decoding(DecodeInfo *decInfo)
{
    
    // Stego Image file
    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "r");
    // Do Error handling
    if (decInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->stego_image_fname);

        return e_failure;
    }

    // No failure return e_success
    return e_success;
}


Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    char *stego_ext = strrchr(argv[2], '.');
    if (stego_ext != NULL && strcmp(stego_ext, ".bmp") == 0)
    {
        decInfo->stego_image_fname = argv[2];
    }
    else
    {
        printf("ERROR: Stego image file must be .bmp\n");
        return e_failure;

    }
    if (argv[3] != NULL)
    {
        strcpy(decInfo->secret_fname , argv[3]);
    }
    else
    {
        strcpy(decInfo->secret_fname , "output");
    }

    return e_success;
}

Status skip_bmp_header(DecodeInfo *decInfo)
{
    fseek(decInfo->fptr_stego_image, 54, SEEK_SET);
    return e_success;
}

Status decode_magic_string(DecodeInfo *decInfo)
{
    char buffer[32];
    fread(buffer, 32 , 1, decInfo->fptr_stego_image);
    int size_of_magic_string = decode_size_from_lsb(buffer);
    
    char imagebuffer[8];
    char decode_char;
    char decoded_magic_string[size_of_magic_string+1];
    int i;
    for(i=0; i < size_of_magic_string; i++)
    {
        fread(imagebuffer, 8, 1, decInfo->fptr_stego_image);
        decode_byte_from_lsb(imagebuffer, &decode_char);
        decoded_magic_string[i] = decode_char;
    }
    decoded_magic_string[i] = '\0';

    if(strcmp(decoded_magic_string, MAGIC_STRING) == 0)
    {
        return e_success;
    }
}

Status decode_secret_file_extn(DecodeInfo *decInfo)
{
    char buffer[32];
    fread(buffer, 32 , 1, decInfo->fptr_stego_image);
    int size_of_secret_file_extn = decode_size_from_lsb(buffer);
    
    char imagebuffer[8];
    char decode_char;
    char decoded_secret_file_extn[size_of_secret_file_extn + 1];
    int i;
    for(i=0; i < size_of_secret_file_extn; i++)
    {
        fread(imagebuffer, 8, 1, decInfo->fptr_stego_image);
        decode_byte_from_lsb(imagebuffer, &decode_char);
        decoded_secret_file_extn[i] = decode_char;
    }
    decoded_secret_file_extn[i] = '\0';
    char *user_ext = strrchr(decInfo->secret_fname, '.');
    if (user_ext != NULL)
    {
        // If an extension is present, validate it against the decoded one
        if (strcmp(user_ext, decoded_secret_file_extn) != 0)
        {
            printf("ERROR: The provided output file extension does not match the encoded extension\n");
            return e_failure;
        }
    }
    else // If no extension was provided by the user, append the correct one
    {
        strcat(decInfo->secret_fname, decoded_secret_file_extn);
    }
    return e_success;
}

Status decode_secret_file_size(DecodeInfo *decInfo)
{
    char buffer[32];
    fread(buffer, 32, 1, decInfo->fptr_stego_image);
    decInfo->size_secret_file = decode_size_from_lsb(buffer);
    return e_success;
}

Status decode_secret_file_data(DecodeInfo *decInfo)
{
    decInfo->fptr_secret = fopen(decInfo->secret_fname, "w");
    if (decInfo->fptr_secret == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->secret_fname);
        return e_failure;
    }

    char imagebuffer[8];
    char decode_char;
    for(int i=0; i < decInfo->size_secret_file; i++)
    {
        fread(imagebuffer, 8, 1, decInfo->fptr_stego_image);
        decode_byte_from_lsb(imagebuffer, &decode_char);
        fputc(decode_char, decInfo->fptr_secret);

    }
    // decoded_secret_file[size_of_secret_file] = '\0';
    return e_success;

    

}

int decode_size_from_lsb(char *buffer)
{
    int size = 0;
    for (int i = 0; i < 32; i++)
    {
        size = ((buffer[i] & 1) << (31 - i)) | size;
    }
    return size;
}

Status decode_byte_from_lsb(char buffer[], char *data)
{
    *data = 0;
    for (int i = 0; i < 8; i++)
    {
        *data = ((buffer[i] & 1)  << (7 - i)) | *data;
    }
} 
