#include "../include/util.h"
#include "../include/bmp_handler.h"
#include "../include/transition_rotate.h"
#include <malloc.h>
#include <stdio.h>


bool rotate_bmp_image(const char* source_path, const char* destination_path){
    struct image img;
    if (!bmp_to_image(source_path, &img)){
        fprintf(stderr, "unable to rotate image because some error has occurred\n");
        if (img.data != NULL) free(img.data);
        return false;
    }
    struct image turned_image = rotate_left(&img);
    free(img.data);
    if (!image_to_bmp(&turned_image, destination_path)){
        fprintf(stderr, "unable to rotate image because some error has occurred\n");
        if (turned_image.data != NULL) free(turned_image.data);
        return false;
    }
    free(turned_image.data);
    fprintf(stdout, "image had been successfully turned\n");
    return true;
}
