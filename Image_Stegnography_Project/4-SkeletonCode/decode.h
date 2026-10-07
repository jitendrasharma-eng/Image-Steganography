#ifndef DECODE_H
#define DECODE_H

#include <stdio.h>
#include "types.h"
#include "common.h"

#define MAGIC_STRING "#*"

typedef struct _DecodeInfo
{
    char *stego_image_fname;
    char *output_fname;

    FILE *fptr_stego_image;
    FILE *fptr_output;

    int secret_file_size;
    int extn_size;

    char extn[20];

} DecodeInfo;



/* Function declarations */

/* Read and validate Encode args from argv */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);


Status open_decode_files(DecodeInfo *decInfo);

Status decode_magic_string(DecodeInfo *decInfo);

char decode_byte_from_lsb(char *image_buffer);

int decode_size_from_lsb(DecodeInfo *decInfo);

Status decode_secret_file_extn_size(DecodeInfo *decInfo);

Status decode_secret_file_extn(DecodeInfo *decInfo);

Status decode_secret_file_size(DecodeInfo *decInfo);

Status decode_secret_file_data(DecodeInfo *decInfo);

Status do_decoding(DecodeInfo *decInfo);

#endif