#include <stdio.h>
#include <string.h>
#include "decode.h"
#include "types.h"

#include "colour.h"

/*
 * Open stego image and output file
 */
Status open_decode_files(DecodeInfo *decInfo)
{
    // Src Image file
    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "r");
    // Do Error handling = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (decInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, RED "ERROR: Unable to open file %s\n" RESET, decInfo->stego_image_fname);

    	return e_failure;
    }

    decInfo->fptr_output = fopen(decInfo->output_fname, "w");
    if (decInfo->fptr_output == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, RED "ERROR: Unable to open file %s\n" RESET, decInfo->stego_image_fname);

    	return e_failure;
    }
    // No failure return e_success
    return e_success;
}

Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    if(argv[2] != NULL && strcmp(strstr(argv[2], "."), ".bmp")==0)
    {
        decInfo->stego_image_fname = argv[2];
    }
    else
    {
        return e_failure;
    }
    if(argv[3] != NULL)
    {
        decInfo->output_fname = argv[3];
        //printf("<--%s",decInfo->output_fname);
    }
    else
    {
        decInfo->output_fname = "outputFile.txt";
    }

    //optional output image file
    
    
    return e_success;
}

char decode_byte_from_lsb(char *image_buffer)
{
    char data = 0;

    for(int i = 0; i < 8; i++)
    {
        data = data << 1;

        data = data | (image_buffer[i] & 1);
    }

    return data;
}

Status decode_magic_string(DecodeInfo *decInfo)
{
    char image_buffer[8];
    char magic_str[20];

    /* Move file pointer after BMP header */
    fseek(decInfo->fptr_stego_image, 54, SEEK_SET);

    int magic_size = strlen(MAGIC_STRING);

    for(int i = 0; i < magic_size; i++)
    {
        fread(image_buffer, 1, 8,
              decInfo->fptr_stego_image);

        magic_str[i] =
            decode_byte_from_lsb(image_buffer);
    }

    magic_str[magic_size] = '\0';
    printf(CYAN"\n<------------------------------------------------------->\n"RESET);
    printf( BLUE "✓ " RESET MAGENTA"Decoded Magic String = "YELLOW"%s\n"RESET, magic_str);

    if(strcmp(magic_str, MAGIC_STRING) == 0)
    {
        printf( BLUE "✓ " RESET MAGENTA"Magic string matched\n"RESET);

        return e_success;
    }

    printf(RED "Magic string mismatch\n" RESET);
    return e_failure;
}


int decode_size_from_lsb(DecodeInfo *decInfo)
{
    char image_buffer[32];
    unsigned int data = 0;

    fread(image_buffer, 1, 32,decInfo->fptr_stego_image);

    for(int i = 0; i < 32; i++)
    {
        data = data << 1;
        data = data | (image_buffer[i] & 1);
    }

    return data;
}

Status decode_secret_file_extn_size(DecodeInfo *decInfo)
{
    decInfo->extn_size = decode_size_from_lsb(decInfo);
printf(CYAN"\n<------------------------------------------------------->\n"RESET);
    printf(BLUE "✓ " RESET MAGENTA"Extension size = %d\n"RESET, decInfo->extn_size);

    return e_success;
}

Status decode_secret_file_extn(DecodeInfo *decInfo)
{
    char image_buffer[8];

    for(int i = 0; i < decInfo->extn_size; i++)
    {
        fread(image_buffer, 1, 8,
              decInfo->fptr_stego_image);

        decInfo->extn[i] =
            decode_byte_from_lsb(image_buffer);
    }

    decInfo->extn[decInfo->extn_size] = '\0';
    printf(BLUE "✓ " RESET MAGENTA"Secret file extension = "YELLOW"%s"RESET"\n" RESET, decInfo->extn);

    return e_success;
}

Status decode_secret_file_size(DecodeInfo *decInfo)
{
    decInfo->secret_file_size =
        decode_size_from_lsb(decInfo);

    printf(CYAN"\n<------------------------------------------------------->\n"RESET);
    printf(BLUE"✓ " RESET MAGENTA"Secret file size = %d bytes\n" RESET,decInfo->secret_file_size);

    return e_success;
}

Status decode_secret_file_data(DecodeInfo *decInfo)
{
    char image_buffer[8];
    char ch;

   // decInfo->fptr_output = fopen("outputFile.txt", "w");


    printf(CYAN"\n<------------------------------------------------------->\n"RESET);
    printf(BLUE "✓ " RESET MAGENTA"YOUR SECRET DATA IS: " RESET);
    for(int i = 0; i < decInfo->secret_file_size; i++)
    {
        fread(image_buffer, 1, 8, decInfo->fptr_stego_image);

        ch = decode_byte_from_lsb(image_buffer);
        printf(YELLOW"%c"RESET,ch);

       fwrite(&ch, 1, 1, decInfo->fptr_output);
    }
   

    return e_success;
}

/*
 * Main decoding function
 */
Status do_decoding(DecodeInfo *decInfo)
{
    
    if(open_decode_files(decInfo) == e_success)
    {
        printf(GREEN);
         printf("Opened all the files successfully\n");
        printf("Started decoding.......\n");
        printf(CYAN"<------------------------------------------------------->\n"RESET);
        printf(RESET);

      if(decode_magic_string(decInfo) == e_success)
      {
        printf(GREEN "Magic String decoded successfully\n" RESET);
        printf(CYAN"<------------------------------------------------------->\n"RESET);

        if(decode_secret_file_extn_size(decInfo) == e_success)
        {
            printf(GREEN "Successfully decodded the secret file extension size\n" RESET);
            if(decode_secret_file_extn(decInfo)== e_success)
            {
                printf(GREEN "Successfully decodded the secret file extension\n" RESET);
                printf(CYAN"<------------------------------------------------------->\n"RESET);

                if(decode_secret_file_size(decInfo)==e_success)
                {
                    printf(GREEN "Successfully decodded the secret file size\n" RESET);
                    printf(CYAN"<------------------------------------------------------->\n"RESET);

                    if(decode_secret_file_data(decInfo)==e_success)
                    {
                    
                        printf(CYAN"\nSuccessfully decodded the secret file data\n"RESET);
                        printf(CYAN"Secret data stored in file: "BLUE" \"%s\" "RESET"\n",decInfo->output_fname);
                         printf(CYAN"<------------------------------------------------------->\n"RESET);
                    
                    }
                    else
                    {
                        printf(RED "Failed to decode the secret file data\n" RESET);
                    }
                }
                else
                {
                    printf(RED"Failed to decodded the secret file size\n"RESET);

                }
            }
            else
            {
                printf(RED"Failed to decode the extension of secret file\n"RESET);
            }
           
        }
        else
        {
            printf(RED"Failed to decode the extension size of secret file\n"RESET);
        }
    }
      else
      {
        printf(RED"Failed to encode the magic string\n"RESET);
        return e_failure;
      }
    }
    else
    {
        printf(RED"Failed to open the files\n"RESET);
        return e_failure;
    }
    return e_success; 
}