#ifndef MY_PROJECT_BMP_HANDLER_H
#define MY_PROJECT_BMP_HANDLER_H

#include "image.h"
#include <stdbool.h>


bool bmp_to_image(const char* path, struct image* img);

bool image_to_bmp(const struct image* img, const char* path);

#endif
