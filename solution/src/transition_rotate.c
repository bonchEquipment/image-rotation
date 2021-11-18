#include "../include/transition_rotate.h"
#include <malloc.h>
#include <stdint.h>

    

struct image rotate_left(const struct image* source_image) {
    const uint64_t source_height = source_image->height;
    const uint64_t source_width = source_image->width;
    struct image new_image = {.width = source_height, .height = source_width};
    struct pixel *new_array = malloc(sizeof(struct pixel) * (source_height * source_width));
    for (uint64_t i = 0; i < source_height; i++) {
        for (uint64_t j = 0; j < source_width; j++) {
            uint64_t index_of_new_place = (j + 1) * (source_height) - i - 1;
            uint64_t index_of_old = (i * source_width) + j;
            new_array[index_of_new_place] = source_image->data[index_of_old];
        }
    }
    new_image.data = new_array;
    return new_image;
}
