#include "../include/bmp_handler.h"
#include <inttypes.h>
#include <malloc.h>
#include <stdbool.h>
#include <stdio.h>


struct bmp_header {
    uint16_t bfType;
    uint32_t  bfileSize;
    uint32_t bfReserved;
    uint32_t bOffBits;
    uint32_t biSize;
    uint32_t biWidth;
    uint32_t  biHeight;
    uint16_t  biPlanes;
    uint16_t biBitCount;
    uint32_t biCompression;
    uint32_t biSizeImage;
    uint32_t pixXPerMeter;
    uint32_t pixYPerMeter;
    uint32_t biClrUsed;
    uint32_t  biClrImportant;
}__attribute__((packed));


static bool open_file_on_read(const char* path, FILE** file){
    *file = fopen(path, "rb");
    if (*file == NULL){
        fprintf(stderr, "%s in function %s on line %d\n", __FILE__, __FUNCTION__, __LINE__ );
        fprintf(stderr, "unable to open file on read \"%s\"\n",path);
        return false;
    }
    return true;
}


static bool open_file_on_write(const char* path, FILE** file){
    *file = fopen(path, "w+b");
    if (*file == NULL){
        fprintf(stderr, "%s in function %s on line %d\n", __FILE__, __FUNCTION__, __LINE__ );
        fprintf(stderr, "unable to open file on write\"%s\"\n",path);
        return false;
    }
    return true;
}

static bool close_file(FILE* file){
    if (file == NULL){
        fprintf(stderr, "%s in function %s on line %d\n", __FILE__, __FUNCTION__, __LINE__ );
        fprintf(stderr, "unable to close file because file pointer is NULL\n");
     return false;
    }
    const int i = fclose(file);
    if (i == 0){
        return true;
    } else {
        fprintf(stderr, "%s in function %s on line %d\n", __FILE__, __FUNCTION__, __LINE__ );
        fprintf(stderr, "unable to close file because of some\n");
        return false;
    }

}


static bool read_bmp_header(FILE* file, struct bmp_header* bmp_header){
    uint64_t amount_of_read_objects = fread(bmp_header, sizeof( struct bmp_header ), 1, file );
    fseek(file, 0, SEEK_SET);
    if (amount_of_read_objects != 1) {
        fprintf(stderr, "%s in function %s on line %d\n", __FILE__, __FUNCTION__, __LINE__ );
        fprintf(stderr, "Failed to read bmp header.\n" );
    }
    return (amount_of_read_objects == 1);
}


static bool is_bmp_file(FILE* file){
    if (file == NULL){
        fprintf(stderr, "%s in function %s on line %d\n", __FILE__, __FUNCTION__, __LINE__ );
        fprintf(stderr, "Unable to determine the file type because file pointer is NULL\n");
        return false;
    }
    const uint16_t code_of_bmp_file = 0x4D42;
    uint16_t code_of_type;
    bool b = fread(&code_of_type, sizeof (uint16_t), 1, file);
    fseek(file, 0, SEEK_SET);
    if (!b){
        fprintf(stderr, "%s in function %s on line %d\n", __FILE__, __FUNCTION__, __LINE__ );
        fprintf(stderr, "Unable to determine the file type because failed to read the file\n");
        return false;
    }
    if (code_of_bmp_file != code_of_type){
        fprintf(stderr, "%s in function %s on line %d\n", __FILE__, __FUNCTION__, __LINE__ );
        fprintf(stderr, "Unsupported format with code %" PRIu16 ". The only one that is supported is 0x4D42 (the bmp code)\n", code_of_type);
        return false;
    }
    return true;
}


static bool is_bmp_have_24_color_depth(FILE* file){
    if (file == NULL) return false;
    struct bmp_header header = {0};
    if (!read_bmp_header(file, &header)){
        fprintf(stderr, "%s in function %s on line %d\n", __FILE__, __FUNCTION__, __LINE__ );
        fprintf(stderr, "Unable to determine color depth of the image because failed to read the header of the file");
        return false;
    } else if (header.biBitCount != 24){
        fprintf(stderr, "%s in function %s on line %d\n", __FILE__, __FUNCTION__, __LINE__ );
        fprintf(stderr, "Unsupported color depth %"PRIu16 ". The only one that supported is 24.\n", header.biBitCount);
        return false;
    }
    return true;
}

static uint8_t count_padding(uint32_t width){
    return  (4 - (width * 3) % 4) % 4; //using uint8_t cos max value of padding is 3
}


static struct bmp_header create_bmp_header(struct image img){
    struct bmp_header bmp_header = {0};
    bmp_header.bfType = 19778;
    bmp_header.bfReserved = 0;
    bmp_header.bOffBits = sizeof (struct bmp_header);
    bmp_header.biSize = 40;
    bmp_header.biWidth = img.width;
    bmp_header.biHeight = img.height;
    bmp_header.biPlanes = 1;
    bmp_header.biBitCount = 24;
    bmp_header.biCompression = 0;
    bmp_header.biSizeImage = img.height * (img.width * 3 + count_padding(img.width));
    bmp_header.bfileSize = bmp_header.bOffBits + bmp_header.biSizeImage;
    bmp_header.pixXPerMeter = 0;
    bmp_header.pixYPerMeter = 0;
    bmp_header.biClrUsed = 0;
    bmp_header.biClrImportant = 0;
    return bmp_header;
}


static bool read_rgb_array_with_padding(FILE* file, uint8_t** rgb_array_ptr){
    if (file == NULL || !is_bmp_file(file) || !is_bmp_have_24_color_depth(file)){
        fprintf(stderr, "%s in function %s on line %d\n", __FILE__, __FUNCTION__, __LINE__ );
        fprintf(stderr, "Unable to read rgb array because some error has occurred");
        return false;
    }
    struct bmp_header bmpHeader = {0};
    if (!read_bmp_header(file, &bmpHeader)){
        return false;
    }
    if (*rgb_array_ptr == NULL) {
        fprintf(stderr, "%s in function %s on line %d\n", __FILE__, __FUNCTION__, __LINE__ );
        fprintf(stderr, "Unable to read rgb array because failed to allocate %" PRIu32 "bytes of heap memory\n", bmpHeader.biSizeImage);
        return false;
    }
    const uint32_t offset_of_pixel_data = bmpHeader.bOffBits;
    const size_t size_of_pixel_data = bmpHeader.biSizeImage;
    fseek(file, (long) offset_of_pixel_data, SEEK_SET);
    fread(*rgb_array_ptr, size_of_pixel_data, 1, file);
    fseek(file, 0 , SEEK_SET);
    return true;
}


static void write_bmp_header_to_file(FILE* file, struct bmp_header* bmpHeaderPointer ){
    fwrite(bmpHeaderPointer, sizeof (struct bmp_header), 1, file);
    fseek(file, 0, SEEK_SET);
}


void write_bgr_array_to_file(FILE* file, uint8_t* array, size_t arraySize){
    fwrite(array, arraySize, 1, file);
    fseek(file, 0, SEEK_SET);
}


/**
 * bgr_array is like rgb_array but red and green value is actually swapped
 *
 * consider padding (skip garbage bytes)
 **/
static bool bgr_array_to_pixels (uint8_t* bgr_array, struct pixel** pixels, uint32_t width, uint32_t height){
    if (bgr_array == NULL || width == 0 || height == 0) {
        fprintf(stderr, "%s in function %s on line %d\n", __FILE__, __FUNCTION__, __LINE__ );
        fprintf(stderr, "Unable to translate bgr array to pixels because the bgr array is empty");
        return false;
    }
    const uint8_t padding = count_padding(width);
    const size_t amount_of_pixels = height * width;
    *pixels = malloc(sizeof(struct pixel)*amount_of_pixels);
    if (*pixels == NULL){
        fprintf(stderr, "%s in function %s on line %d\n", __FILE__, __FUNCTION__, __LINE__ );
       fprintf(stderr, "Unable to read rgb array because failed to allocate %zu bytes of heap memory\n", sizeof(struct pixel) * amount_of_pixels);
        return false;
    }
    size_t count = 0;
    for (uint32_t i = 0; i < height; i++){
        for (uint32_t j = 0; j < width; j++){
            uint8_t b = bgr_array[count];
            uint8_t g = bgr_array[count + 1];
            uint8_t r = bgr_array[count + 2];
            struct pixel p = { .r = r, .g = g, .b = b};
            uint32_t number_of_pixel = (i * width) + j;
            (*pixels)[number_of_pixel] = p;
            count = count + 3;
            if (j == (width - 1)){
                count = count + padding;
            }
        }
    }
    return true;
}


static bool image_to_bgr_array(struct image img, uint8_t** bgr_array){
    if (img.width == 0 || img.height == 0) return false;
    const uint32_t width = img.width;
    const uint32_t height = img.height;
    struct pixel* pixels = img.data;
    const uint8_t padding = count_padding(width);
    const size_t size = height * (width * 3 + padding);// size of array in bytes
    *bgr_array = malloc(size);
    if (*bgr_array == NULL){
        fprintf(stderr, "%s in function %s on line %d\n", __FILE__, __FUNCTION__, __LINE__ );
        fprintf(stderr, "Unable to read rgb array because failed to allocate %zu bytes of heap memory", sizeof(int8_t)*size);
        return false;
    }
    size_t count = 0;
    for (uint32_t i = 0; i < height; i++){
        for (uint32_t j = 0; j < width; j++){
            uint32_t number_of_pixel = (i * width) + j;
            uint8_t r = pixels[number_of_pixel].r;
            uint8_t g = pixels[number_of_pixel].g;
            uint8_t b = pixels[number_of_pixel].b;
            (*bgr_array)[count] = b;
            (*bgr_array)[count+1] = g;
            (*bgr_array)[count+2] = r;
            count = count + 3;
            if (j == (width - 1))
                for (uint8_t k = 0; k < padding; k++){
                    (*bgr_array)[count] = 0; //doing so due to pass msan test
                    count++;
                }
        }
    }
    return true;
}



bool bmp_to_image(const char* path, struct image* img){
    FILE* file = NULL;
    if (!open_file_on_read(path, &file)) return false;
    if (!is_bmp_file(file) || !is_bmp_have_24_color_depth(file)){
        fprintf(stderr, "%s in function %s on line %d\n", __FILE__, __FUNCTION__, __LINE__ );
        fprintf(stderr, "Unable to read rgb array because some error has occurred");
        close_file(file);
        return false;
    }
    struct bmp_header header = {0};
    if (!read_bmp_header(file, &header)) {
        close_file(file);
        return false;
    }
    const uint32_t height = header.biHeight;
    const uint32_t  width = header.biWidth;
    img->height = height;
    img->width = width;
    uint8_t* rgb_array = malloc(header.biSizeImage);
    if (rgb_array == NULL){
        fprintf(stderr, "%s in function %s on line %d\n", __FILE__, __FUNCTION__, __LINE__ );
        fprintf(stderr, "Unable to translate bmp to image because failed to allocate %zu bytes of heap memory", sizeof(uint8_t) * height * width * 3);
        close_file(file);
        return false;
    }
    if (!read_rgb_array_with_padding(file, &rgb_array)){
        if (rgb_array != NULL) free(rgb_array);
        close_file(file);
        return false;
    }
    struct pixel* pixels = NULL;
    if (!bgr_array_to_pixels(rgb_array, &pixels, width, height)){
        free(rgb_array);
        if (pixels != NULL) free(pixels);
        close_file(file);
        return false;
    }
    free(rgb_array);
    img->data = pixels;
    return true;
}


bool image_to_bmp(const struct image* img, const char* path){
    FILE* file = NULL;
    if (!open_file_on_write(path, &file)) return false;
    struct bmp_header bmp_header = create_bmp_header(*img);
    write_bmp_header_to_file(file, &bmp_header);
    fseek(file, (long)bmp_header.bOffBits, SEEK_SET);

    uint8_t* bgr_array = NULL;
    if (!image_to_bgr_array(*img, &bgr_array)) {
        if (bgr_array != NULL) free(bgr_array);
        fprintf(stderr, "%s in function %s on line %d\n", __FILE__, __FUNCTION__, __LINE__ );
        fprintf(stderr, "Unable to image to a file because some error has occurred");
        close_file(file);
        return false;
    }
    
    write_bgr_array_to_file(file, bgr_array, bmp_header.biSizeImage);
    free(bgr_array);
    close_file(file);
    return true;
}
