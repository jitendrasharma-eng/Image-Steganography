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
}

Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    if(argv[2] != NULL && strcmp(strstr(argv[2], "."), ".bmp")==0)
    {
        encInfo->src_image_fname = argv[2];
    }
    else
    {
        return e_failure;
    }

    if(argv[3] != NULL && strcmp(strstr(argv[3], "."), ".txt")==0)
    {
        encInfo->secret_fname = argv[3];
    }
    else
    {
        return e_failure;
    }
    //optional output image file
    if(argv[4] != NULL)
    {
        encInfo->stego_image_fname = argv[4];
    }
    else
    {
        encInfo -> stego_image_fname = "stego.bmp";
    }
    return e_success;
}


OperationType check_operation_type(char *argv[])
{
    if(strcmp(argv[1], "-e") == 0)
    {
        return e_encode;
    }
    else if(strcmp(argv[1], "-d")==0)
    {
        return e_decode;
    }
    else
    {
        return e_unsupported;
    }
}


uint get_file_size(FILE *fptr_secr)
{
    fseek(fptr_secr, 0, SEEK_END);
    return ftell(fptr_secr);
}


Status check_capacity(EncodeInfo *encInfo)
{
    // get the size of image file
    encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);
    //get the size of secret file
    encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);
    if(encInfo->image_capacity >(54+(2+4+4+4+encInfo->size_secret_file)*8))   //54-bmp header, 2-magic string, 4-screat file extension, 4- secret file size, 4-another metadata/extension related information.
    {
        return e_success;
    }
    else
    {
        return e_failure;
    }
}


//copy the header form source to destination
Status copy_bmp_header(FILE *fptr_src, FILE *fptr_stego)
{
    char header[54];
    fseek(fptr_src, 0, SEEK_SET);
    //read 54 byte header from source
    fread(header, 54, sizeof(char), fptr_src);
    //write 54 byte to stego image
    fwrite(header, 54, sizeof(char), fptr_stego);
    return e_success;

}

//Encode the character in the LSB of RGB
Status encode_byte_to_lsb(char data, char *image_buffe)
{
    unsigned char mask = 1<<7;
    for(int i=0; i<8; i++)
    {
        image_buffe[i] = (image_buffe[i] & 0xFE) | ((data & mask) >> (7-i));
        mask = mask >> 1;
    }
    return e_success;
}

//charcater encoding
Status encode_data_to_image(const char *data, int size, FILE *fptr_src, FILE *fptr_stego, EncodeInfo *encInfo)
{
    //calll encode byte_to_lsb to encode the data one after the other
    for(int i=0; i< size; i++)
    {
        //read 8 bytes of rgb from source image
        fread(encInfo ->image_data, 8, sizeof(char), fptr_src);
        encode_byte_to_lsb(data[i], encInfo->image_data);
        fwrite(encInfo->image_data, 8, sizeof(char), fptr_stego);
    }
    return e_success;
}

Status encode_size_to_lsb(int data, char *image_buffe)
{
    unsigned int mask = 1<<31;
    for(int i=0; i<32; i++)
    {
        image_buffe[i] = (image_buffe[i] & 0xFE) | ((data & mask) >> (31-i));
        mask = mask >> 1;
    }
    return e_success;
}
//encode magic string
Status encode_magic_string(const char *magic_str, EncodeInfo *encInfo)
{
    //every character encoding will have to call this function
    encode_data_to_image(magic_str, strlen(magic_str),encInfo->fptr_src_image, encInfo->fptr_stego_image, encInfo);
    return e_success;
}

Status encode_size(int size, FILE *fptr_src, FILE *fptr_stego)
{
    char str[32];  // to encode 4byte int 
    fread(str, 32, sizeof(char), fptr_src);
    encode_size_to_lsb(size, str);
    fwrite(str, 32, sizeof(char), fptr_stego);
    return e_success;
}

Status encode_secret_file_size(int size, EncodeInfo *encInfo)
{
    char str[32];  // to encode 4byte int 
    fread(str, 32, sizeof(char), encInfo->fptr_src_image);
    encode_size_to_lsb(size, str);
    fwrite(str, 32, sizeof(char), encInfo->fptr_stego_image);
    return e_success;
}

Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    file_extn = ".txt";
    encode_data_to_image(file_extn, strlen(file_extn), encInfo->fptr_src_image, encInfo->fptr_stego_image, encInfo);
    return e_success;
}

Status encode_secret_file_data(EncodeInfo *encInfo)
{
    char ch;
    fseek(encInfo->fptr_secret, 0, SEEK_SET);
    for(int i=0; i<encInfo->size_secret_file; i++)
    {
        fread(encInfo->image_data, 8, sizeof(char), encInfo->fptr_src_image);
        fread(&ch, 1, 1, encInfo->fptr_secret);
        encode_byte_to_lsb(ch, encInfo->image_data);
        fwrite(encInfo->image_data, 8, sizeof(char), encInfo->fptr_stego_image);
    }
    return e_success;
}

 Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
    {
        char ch;
        while(fread(&ch, 1, 1, fptr_src)>0)
        {
            fwrite(&ch, 1, 1, fptr_dest);
        }
        return e_success;
    }

//rest of the encoding function is called here
Status do_encoding(EncodeInfo *encInfo)
{
    //open all the file which is requiree for encoding
    if(open_files(encInfo) == e_success)
    {
        printf("Opened all the files successfully\n");
        printf("Started Encoding.......\n");
        if(check_capacity(encInfo) == e_success)
        {
            printf("Image has enough capacity to encode\n");
            if(copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
            {
                printf("Successfully copied the header\n");
                if(encode_magic_string(MAGIC_STRING, encInfo) == e_success)
                {
                    printf("Magic String encoded successfully\n");
                    if(encode_size(strlen(".txt"), encInfo->fptr_src_image, encInfo -> fptr_stego_image) == e_success)
                    {
                        printf("Successfully encodded the secret file extension size\n");
                        if(encode_secret_file_extn(encInfo ->extn_secret_file, encInfo) == e_success)
                        {
                            printf("Successfully encoded the secret file extension\n");
                            if(encode_secret_file_size(encInfo->size_secret_file, encInfo) == e_success)
                            {
                                printf("Successfully encoded the secret file size\n");
                                if(encode_secret_file_data(encInfo) == e_success)
                                {
                                    printf("Encoded secret data successfully\n");
                                    if(copy_remaining_img_data(encInfo-> fptr_src_image, encInfo->fptr_stego_image) == e_success)
                                    {
                                        printf("Copied remaining RGB data successfully\n");
                                    }
                                    else
                                    {
                                        printf("Failed to copy remaining data\n");
                                        return e_failure;
                                    }
                                }
                                else
                                {
                                    printf("Failed to encode the secret data\n");
                                    return e_failure;
                                }
                            }
                            else
                            {
                                printf("Failed to encode the secret file size\n");
                                return e_failure;
                            }
                        }
                        else
                        {
                            printf("Failed to encode the magic string\n");
                             return e_failure;
                        }
                    }
                }
                else
                {
                    printf("Failed to encode the magic string\n");
                    return e_failure;
                }
            }
            else
            {
                printf("Failed to copy the header\n");
                return e_failure;
            }
        }
        else
        {
            printf("Dont have enough RGB data to encode the secret message\n");
            return e_failure;
        }
    }
    else
    {
        printf("Failed to open th files\n");
        return e_failure;
    }
    return e_success;
}