#include <stdio.h>
#include <stdint.h>

#define CLR__RESET  "\033[0m"
#define CLR_BLACK   "\033[0;30m"
#define CLR_RED     "\033[0;31m"
#define CLR_GREEN   "\033[0;32m"
#define CLR_YELLOW  "\033[0;33m"
#define CLR_BLUE    "\033[0;34m"
#define CLR_MAGENTA "\033[0;35m"
#define CLR_CYAN    "\033[0;36m"
#define CLR_WHITE   "\033[0;37m"

typedef enum TrailerTypeEnum
{
    TRAILER_UNKNOWN,
    TRAILER_JPEG,
    TRAILER_PNG
} trailer_t;

typedef enum PayloadTypeEnum
{
    PAYLOAD_UNKNOWN,
    PAYLOAD_ZIP,
    PAYLOAD_RAR
} payload_t;

void print_usage(char *prog_name)
{
    printf("Deduce zipjpeg structure\nby Dmitri S. Guskov @ OTUS-2024-03\n\n");
    printf("Usage:\n  %s <filename>\n\n", prog_name);
}

/// Returns
///   1 - payload found
///   0 - payload not found
///  -1 - processing error

int process_jpeg(FILE *fp, payload_t *payload, size_t *sz)
{
    rewind(fp);
    *payload = PAYLOAD_UNKNOWN;
    *sz = 0;
    return -1;
}

/// Returns
///   1 - payload found
///   0 - payload not found
///  -1 - processing error

int process_png(FILE *fp, payload_t *payload, size_t *sz)
{
    rewind(fp);
    *payload = PAYLOAD_UNKNOWN;
    *sz = 0;
    return -1;
}

int process_zip(FILE *fp)
{
    rewind(fp);
    return 0;
}

int process_rar(FILE *fp)
{
    rewind(fp);
    return 0;
}

size_t get_file_size(FILE *fp)
{
    long int last_position;
    size_t file_size;
    
    last_position = ftell(fp);
    fseek(fp, 0L, SEEK_END);
    file_size = (size_t)ftell(fp);
    fseek(fp, last_position, SEEK_SET);

    return file_size;
}

int determine_trailer_type(FILE *fp, trailer_t *result)
{
    unsigned char buf[8];
    *result = TRAILER_UNKNOWN;
    rewind(fp);

    if (!fread(&buf, sizeof(buf), 1, fp))
        return 0;

    if (buf[0] == 0xff && buf[1] == 0xd8 && buf[2] == 0xff)
    {
        *result = TRAILER_JPEG;
        return 1;
    }

    if (buf[0]==0x89 && buf[1]==0x50 && buf[2]==0x4e && buf[3]==0x47 &&
        buf[4]==0x0d && buf[5]==0x0a && buf[6]==0x1a && buf[7]==0x0a)
    {
        *result = TRAILER_PNG;
        return 1;
    }

    return 2;
}

int main(int argc, char **argv)
{
    FILE *fp;
    size_t file_size, payload_size;
    trailer_t trailer;
    payload_t payload;
    int trailer_process_result;
    const char *trailer_type[3] =
    {
        "of unknown type", 
        CLR_GREEN "probably JPEG file" CLR__RESET, 
        CLR_GREEN "probably PNG file" CLR__RESET
    };
    const char *payload_type[3] =
    {
        "of unknown type",
        CLR_GREEN "probably ZIP file" CLR__RESET,
        CLR_GREEN "probably RAR file" CLR__RESET
    };

    if (argc != 2)
    {
        print_usage(argv[0]);
        return 1;
    }

    fp = fopen(argv[1], "rb");
    if (!fp)
    {
        perror(argv[1]);
        return -1;
    }

    printf("Processing file [%s]\n", argv[1]);

    // Determining file size

    file_size = get_file_size(fp);
    printf("File size in bytes: %ld\n", file_size);

    if (file_size < 1)
        goto file_close;

    // Parsing file
    
    if (!determine_trailer_type(fp, &trailer))
        goto read_error;

    printf("Trailing data is %s.\n", trailer_type[trailer]);

    switch(trailer)
    {
        case TRAILER_JPEG:
            trailer_process_result = process_jpeg(fp, &payload, &payload_size);
            break;
        case TRAILER_PNG:
            trailer_process_result = process_png(fp, &payload, &payload_size);
            break;
        default:
            printf("There is nothing to process, terminating\n");
            goto file_close;
    }

    if (!trailer_process_result)
    {
        printf("No payload found at the end.\n");
        goto file_close;
    }
    else if (trailer_process_result == -1)
    {
        printf(CLR_RED "Trailing data was not properly processed.\n" CLR__RESET);
        goto file_close;
    }

    printf("Payload data (%ld bytes) is %s .\n", payload_size, payload_type[payload]);

    switch(payload)
    {
        case PAYLOAD_ZIP:
            process_zip(fp);
            break;
        case PAYLOAD_RAR:
            process_rar(fp);
            break;
        default:
            printf("There is nothing to process, terminating\n");
    }

    goto file_close;


read_error:
    perror("Could not read data from file");

file_close:
    printf("Closing file\n");
    fclose(fp);

    return 0;
}

