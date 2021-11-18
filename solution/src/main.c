#include "../include/main.h"
#include "../include/util.h"
#include <stdio.h>


int main(int argc, char** argv) {

    if (argc != 3){
        printf("the number of arg is %d\n", argc);
        for (int i = 0; i < argc; i++){
            printf("the argument number %d is %s\n", i, argv[i]);
        }
        fprintf(stderr, "Wrong number of arguments.\n Expected usage: %s <source-image> <transformed-image>\n" , argv[0]);
        return 1;
    }

    if (!rotate_bmp_image(argv[1], argv [2])){
        fprintf(stderr, "%s in function %s on line %d\n", __FILE__, __FUNCTION__, __LINE__ );
        fprintf(stderr, "Failed to rotate the image\n");
        return 1;
    }

    return 0;
}
