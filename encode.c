#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"
#include "common.h"

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
    printf("Wiidth = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("Height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

uint get_file_size(FILE *fptr)
{
    // Find the size of secret file data
    fseek(fptr, 0, SEEK_END);
    return ftell(fptr);
}

  /*
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */

Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    
    char *source_ext = strrchr(argv[2], '.');
    if (source_ext != NULL && strcmp(source_ext, ".bmp") == 0)
    {
        encInfo->src_image_fname = argv[2];
    }
    else
    {
        printf("ERROR: Source image file should be a .bmp file\n");
        return e_failure;
    }

    char *secret_ext = strrchr(argv[3], '.');
    if (secret_ext != NULL && secret_ext != argv[3])
    {
        encInfo->secret_fname = argv[3];
        strcpy(encInfo->extn_secret_file, secret_ext);
    }
    else
    {
        printf("ERROR: Secret file must have an extension(eg-> .txt, .c, .py)\n");
        return e_failure;
    }

    if (argv[4] != NULL)
    {
        if(strstr(argv[4], ".bmp") != NULL)
        {
            encInfo->stego_image_fname = argv[4];
        }
        else
        {
            printf("ERROR: Stego image file should be a .bmp file\n");
            return e_failure;
        }
    }
    else
    {
        encInfo->stego_image_fname = "stego.bmp";
    }
    


    return e_success;
}

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
}

Status check_capacity(EncodeInfo *encInfo)
{
    encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);
    encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);
    long capacity = (4 * 8 + 2 * 8 + 4 * 8 + 4 * 8 + 4 * 8 + (encInfo->size_secret_file * 8));
    long image_capacity_bits = encInfo->image_capacity * 8;
    if (image_capacity_bits < capacity)
    {
        return e_failure;
    }
    return e_success;
}

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    /*fseek(src_file, 0, SEEK_SET) //we get 18th offset as width
      resad   */
    char buffer[54];
    fseek(fptr_src_image, 0, SEEK_SET);
    fread(buffer,54,1,fptr_src_image);
    fwrite(buffer,54,1,fptr_dest_image);
    if(ftell(fptr_src_image) != ftell(fptr_dest_image))
    {
        return e_failure;
    }
    return e_success;

}

Status encode_magic_string_size(int size, EncodeInfo *encInfo)
{
    // decalar a  buff as size as 32
    /*read 32 bytes from src_file
    call encode_size of lsb ()*/
    char buffer[32];
    fread(buffer,32,1,encInfo->fptr_src_image);
    encode_size_to_lsb(size,buffer);
    fwrite(buffer,32,1,encInfo->fptr_stego_image);
    return e_success;

}

Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    char buffer[8];
    for(int i=0; i < strlen(magic_string);i++)
    {
        fread(buffer,8,1,encInfo->fptr_src_image);
        encode_byte_to_lsb(magic_string[i],buffer);
        fwrite(buffer,8,1,encInfo->fptr_stego_image); 
    }
    return e_success;
}

Status encode_secret_file_extn_size(int size, EncodeInfo *encInfo)
{
    char buffer[32];
    fread(buffer,32,1,encInfo->fptr_src_image);
    encode_size_to_lsb(size, buffer);
    fwrite(buffer,32,1,encInfo->fptr_stego_image);
    return e_success;
}

Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    int size = strlen(file_extn);
    char buffer[8];
    for(int i=0; i < size;i++)
    {
        fread(buffer,8,1,encInfo->fptr_src_image);
        encode_byte_to_lsb(encInfo->extn_secret_file[i],buffer);
        fwrite(buffer,8,1,encInfo->fptr_stego_image); 
    }
    return e_success;
}

Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    char buffer[32];
    fread(buffer,32,1,encInfo->fptr_src_image);
    encode_size_to_lsb(file_size, buffer);
    fwrite(buffer,32,1,encInfo->fptr_stego_image);
    return e_success;
}

Status encode_secret_file_data(EncodeInfo *encInfo)
{
    fseek(encInfo->fptr_secret, 0, SEEK_SET);
    char buffer[8];
    for(int i=0; i < encInfo->size_secret_file; i++)
    {
        char ch = fgetc(encInfo->fptr_secret);
        fread(buffer,8,1,encInfo->fptr_src_image);
        encode_byte_to_lsb(ch,buffer);
        fwrite(buffer,8,1,encInfo->fptr_stego_image); 
    }
    return e_success;
}

Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    char ch;
    while ((fread(&ch, 1, 1, fptr_src)) > 0)
    {
        fwrite(&ch, 1, 1, fptr_dest);
    }
    return e_success;
}

Status encode_byte_to_lsb(char data, char *image_buffer)
{
    for (int i = 0; i < 8; i++)
    {
        image_buffer[i] = (image_buffer[i] & ~(1)) | ((data >> (7-i)) & 1);
    }
    return e_success;
}

Status encode_size_to_lsb(int size, char *imageBuffer)
{
    for(int i=0; i < 32; i++)
    {
        imageBuffer[i] = (imageBuffer[i] & ~(1)) | ((size >> (31-i)) & 1);
    }
    return e_success;
}

Status do_encoding(EncodeInfo *encInfo)
{

    printf("\n\t<----------ENCODING---------->\n\n");
    if (open_files(encInfo) == e_success)
    {
        printf("INFO: Successfully opened files\n");
        if (check_capacity(encInfo) == e_success)
        {
            printf("INFO: Capacity is sufficient\n");
            if (copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image))
            {
                printf("INFO: Successfully copied header\n");
                if (encode_magic_string_size(strlen(MAGIC_STRING), encInfo) == e_success)
                {
                    printf("INFO: Successfully encoded magic string size\n");
                    if (encode_magic_string(MAGIC_STRING, encInfo) == e_success)
                    {
                        printf("INFO: Successfully encoded magic string\n");
                        if (encode_secret_file_extn_size(strlen(encInfo->extn_secret_file), encInfo) == e_success)
                        {
                            printf("INFO: Successfully encoded secret file extension size\n");
                            if (encode_secret_file_extn(encInfo->extn_secret_file, encInfo) == e_success)
                            {
                                printf("INFO: Successfully encoded secret file extension\n");
                                if (encode_secret_file_size(encInfo->size_secret_file, encInfo) == e_success)
                                {
                                    printf("INFO: Successfully encoded secret file size\n");
                                    if (encode_secret_file_data(encInfo) == e_success)
                                    {
                                        printf("INFO: Successfully encoded secret file data\n");
                                        if (copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
                                        {
                                            printf("INFO: Successfully copied remaining data\n");
                                            return e_success;
                                        }
                                        else
                                        {
                                            printf("ERROR: Failed to copy remaining data\n");
                                            return e_failure;
                                        }
                                    }
                                    else
                                    {
                                        printf("ERROR: Failed to encode secret file\n");
                                        return e_failure;
                                    }
                                }
                                else
                                {
                                    printf("ERROR: Failed to encode size of secret file\n");
                                    return e_failure;
                                }
                            }
                            else
                            {
                                printf("ERROR: Failed to encode secret file extension\n");
                                return e_failure;
                            }
                        }
                        else
                        {
                            printf("ERROR: Failed to encode size of the secret file extension\n");
                            return e_failure;
                        }
                    }
                    else
                    {
                        printf("ERROR: Failed to encode magic string\n");
                        return e_failure;
                    }
                }
                else
                {
                    printf("ERROR: Failed to encode magic string size\n");
                    return e_failure;
                }
            }
            else
            {
                printf("ERROR: Failed to copy header\n");
                return e_failure;
            }
        }
        else
        {
            printf("ERROR: Capacity of secret file exceeds the capacity of original file\n");
            return e_failure;
        }
    }
    else
    {
        printf("ERROR: Failed to open file\n");
        return e_failure;
    }
}

