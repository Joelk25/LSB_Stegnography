/*Name: Joel K Sam
  Date: 15/12/2025
  Description: Stegnography project

  Sample input: ./a.out -e beautiful.bmp secret.txt
  Sample output: 
                    <----------ENCODING---------->

                INFO: Successfully opened files
                Wiidth = 1024
                Height = 768
                INFO: Capacity is sufficient
                INFO: Successfully copied header
                INFO: Successfully encoded magic string size
                INFO: Successfully encoded magic string
                INFO: Successfully encoded secret file extension size
                INFO: Successfully encoded secret file extension
                INFO: Successfully encoded secret file size
                INFO: Successfully encoded secret file data
                INFO: Successfully copied remaining data

                        <-----------ENCODING SUCCESSFUL----------->


  Sample input: ./a.out -d stego.bmp output.txt
  Sample output: 
                        <----------DECODING---------->

                INFO: Successfully opened files
                INFO: Successfully skipped header
                INFO: Successfully decoded magic string
                INFO: Successfully decoded secret file extension
                INFO: Successfully decoded size of secret file
                INFO: Successfully decoded secret file data

                        <-----------DECODING SUCCESSFUL----------->

*/

#include <stdio.h>
#include<string.h>
#include "encode.h"
#include "decode.h"
#include "types.h"
OperationType check_operation_type(char *);

int main(int argc, char *argv[])
{

    if(argc == 1)
    {
        printf("ERROR: Invalid no. of arguments\n");
        return 0;
    }
    OperationType oper_type = check_operation_type(argv[1]);
    if(oper_type == e_encode)
    {
        if(argc < 4 || argc > 5)
        {
            printf("ERROR: Invalid arguments for encoding\n");
            return 0;
        }
        EncodeInfo enc_info;
        if(read_and_validate_encode_args(argv,&enc_info)== e_success)
        {
            if(do_encoding(&enc_info) == e_success)
            {
                printf("\n\t<-----------ENCODING SUCCESSFUL----------->\n");
                return e_success;
            }
            else
            {
                printf("ERROR: Encoding failed\n");
                return 0;
            }
        }
        
    }
    else if(oper_type  == e_decode)
    {
        if(argc < 3 || argc> 4)
        {
            printf("ERROR: Invalid arguments for decoding\n");
            return 0;
        }
        DecodeInfo decinfo;
        if(read_and_validate_decode_args(argv,&decinfo)== e_success)
        {
            if(do_decoding(&decinfo) == e_success)
            {
            printf("\n\t<-----------DECODING SUCCESSFUL----------->\n");
                return e_success;
            }
            else
            {
                printf("ERROR: Decoding failed\n");
                return 0;
            }
        }
       
    }
    else if(oper_type == e_unsupported)
    {
        printf("ERROR: Unsupported operation\n");
        return 0;
    }
    
}

OperationType check_operation_type(char *symbol)
{
    // Step 1 : Check whether the symbol is -e or not true - > return e_encode false -> Step 2
    // Step 2 : Check whether the symbol is -d or not true - > return e_decode false -> return e_unsupported

    if(strcmp(symbol , "-e") == 0)
    {
        return e_encode;
    }
    else if(strcmp(symbol , "-d") == 0)
    {
        return e_decode;
    }
    else
    {
        return e_unsupported;
    }
}
