#include <stdio.h>
#include <stdlib.h>
#include "png.h"

typedef struct {
    int width;
    int height;
    png_bytep* row_pointers;
} ImageData;

// Custom read function
void custom_read_function(png_structp png_ptr, png_bytep data, png_size_t length) {
    FILE* file = (FILE*)png_get_io_ptr(png_ptr);
    if (fread(data, 1, length, file) != length) {
        png_error(png_ptr, "Error reading file");
    }
}

// Clean up the allocated memory for image data
void cleanup(ImageData* img) {
    for (int y = 0; y < img->height; y++) {
        free(img->row_pointers[y]);
    }
    free(img->row_pointers);
}

// Read the PNG file into an ImageData struct
ImageData read_png_file(const char* filename) {
    FILE* fp = fopen(filename, "rb");
    if (!fp) {
        perror("File could not be opened for reading");
        exit(EXIT_FAILURE);
    }

    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr) {
        fclose(fp);
        exit(EXIT_FAILURE);
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        fclose(fp);
        exit(EXIT_FAILURE);
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        fclose(fp);
        exit(EXIT_FAILURE);
    }

    // Use the custom read function
    png_set_read_fn(png_ptr, fp, custom_read_function);

    // Read the PNG file info
    png_read_info(png_ptr, info_ptr);

    ImageData img;
    img.width = png_get_image_width(png_ptr, info_ptr);
    img.height = png_get_image_height(png_ptr, info_ptr);
    png_byte color_type = png_get_color_type(png_ptr, info_ptr);
    png_byte bit_depth = png_get_bit_depth(png_ptr, info_ptr);

    // Set up the input transformation
    if (bit_depth == 16)
        png_set_strip_16(png_ptr);

    if (color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_palette_to_rgb(png_ptr);

    // PNG_COLOR_TYPE_GRAY_ALPHA is always 8 or 16bit depth.
    if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
        png_set_expand_gray_1_2_4_to_8(png_ptr);

    if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
        png_set_tRNS_to_alpha(png_ptr);

    // These color_type don't have an alpha channel then fill it with 0xff.
    if (color_type == PNG_COLOR_TYPE_RGB ||
        color_type == PNG_COLOR_TYPE_GRAY ||
        color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_filler(png_ptr, 0xFF, PNG_FILLER_AFTER);

    if (color_type == PNG_COLOR_TYPE_GRAY ||
        color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
        png_set_gray_to_rgb(png_ptr);

    png_read_update_info(png_ptr, info_ptr);

    // Read the actual image data
    img.row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * img.height);
    for (int y = 0; y < img.height; y++) {
        img.row_pointers[y] = (png_byte*)malloc(png_get_rowbytes(png_ptr, info_ptr));
    }

    png_read_image(png_ptr, img.row_pointers);

    // Clean up
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    fclose(fp);

    return img;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <file.png>\n", argv[0]);
        return EXIT_FAILURE;
    }
    ImageData img = read_png_file(argv[1]);
    printf("image read hw=(%d, %d)\n", img.height, img.width);
    cleanup(&img);
    return 0;
}
