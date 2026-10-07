#include <stdio.h>
#include "encode.h"
#include "types.h"

#include "decode.h"

#include <string.h>

#include "colour.h"

int main(int argc, char *argv[])
{
    //check operation type to know it is encoding or decoding
    if(check_operation_type(argv) == e_encode)
    {
        EncodeInfo encInfo;
        printf("Selected Encoding\n");
        if(read_and_validate_encode_args(argv, &encInfo) == e_success)
        {
            printf("Read and validate encode arguments is successful\n");

            if(do_encoding(&encInfo) == e_success)
            {
                printf("Encoding completed.....\n");
            }
            else
            {
                printf("Failed to encode the data\n");
            }
        }
        else
        {
            printf("Failed to validate the input arguments\n");
        }
    }
    else if(check_operation_type(argv)== e_decode)
    {
        //printf("\n<------------------------------------------------------->\n");
        printf(CYAN "\n******************Selected Decoding************************\n" RESET);
        DecodeInfo decInfo;
        
        if(read_and_validate_decode_args(argv, &decInfo) == e_success)
        {
            printf(CYAN"\n<------------------------------------------------------->\n"RESET);
            printf(GREEN "✓ " RESET MAGENTA "Read and validate decode arguments is successful\n" RESET);

            if(do_decoding(&decInfo) == e_success)
            {
                printf(GREEN"Decoding completed.....\n"RESET);
            }
            else
            {
                printf(RED "Failed to decode the data\n" RESET);
            }
        }
        else
        {
            printf(RED "Failed to validate the input arguments\n" RESET);
        }
    }
    else
    {
        printf( MAGENTA "Invalid operation\n *****************usage******************\n");
        printf("Encoding: ./a.out -e beautiful.bmp secret.txt stego.bmp\n");
        printf("Decoding: ./a.out -d stego.bmp\n");
        printf("******************************************\n" RESET);
    }
    return 0;
}


