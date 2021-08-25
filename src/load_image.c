#include "load_image.h"

#include <GL/freeglut.h>
#include <stdio.h>
#include <stdlib.h>

void
load_tga(char* filename)
{
    char id, cmap, imgtype, bpp, c_garb, temp;
    char* image_data;
    short int s_garb, width, height;
    int nbytes, size, index;

    FILE* file = fopen(filename, "rb");
    if (file == NULL) {
        printf("error opening file %s\n", filename);
        exit(EXIT_FAILURE);
    }
    fread(&id, sizeof(id), 1, file);
    fread(&cmap, sizeof(cmap), 1, file);
    fread(&imgtype, sizeof(imgtype), 1, file);

    if (imgtype != 2 && imgtype != 3) {
        printf("Incompatible image type\n");
        exit(EXIT_FAILURE);
    }

    fread(&s_garb, sizeof(s_garb), 1, file);
    fread(&s_garb, sizeof(s_garb), 1, file);
    fread(&c_garb, sizeof(c_garb), 1, file);

    fread(&s_garb, sizeof(s_garb), 1, file);
    fread(&s_garb, sizeof(s_garb), 1, file);

    fread(&width, sizeof(width), 1, file);
    fread(&height, sizeof(height), 1, file);

    fread(&bpp, sizeof(bpp), 1, file);
    fread(&c_garb, sizeof(c_garb), 1, file);

    nbytes = bpp / 8;
    size = width * height * nbytes;
    printf("%d %d %d %d\n", width, height, size, nbytes);
    image_data = (char*)malloc(sizeof(char) * size);
    fread(image_data, sizeof(char), size, file);

    // swap R and B
    if (nbytes > 2) {
        for (int i = 0; i < width * height; i++) {
            index = i * nbytes;
            temp = image_data[index];
            image_data[index] = image_data[index + 2];
            image_data[index + 2] = temp;
        }
    }

    switch (nbytes) {
    case 1:
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            1,
            width,
            height,
            0,
            GL_LUMINANCE,
            GL_UNSIGNED_BYTE,
            image_data);
        break;
    case 3:
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            3,
            width,
            height,
            0,
            GL_RGB,
            GL_UNSIGNED_BYTE,
            image_data);
        break;
    case 4:
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            4,
            width,
            height,
            0,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            image_data);
        break;
    }
    free(image_data);
}

char*
load_bmp_to_array(char* filename, int* out_width, int* out_height)
{
    char* imageData;
    char header1[18];
    char header2[24];
    short int planes, bpp;
    int width, height;
    int nbytes, size, index, temp;

    FILE* file;
    file = fopen(filename, "rb");
    if (file == NULL) {
        printf("error opening file %s\n", filename);
        exit(EXIT_FAILURE);
    }

    fread(header1, sizeof(header1), 1, file);
    fread(&width, sizeof(int), 1, file);
    fread(&height, sizeof(int), 1, file);
    fread(&planes, sizeof(short int), 1, file);
    fread(&bpp, sizeof(short int), 1, file);
    fread(header2, sizeof(header2), 1, file);

    // printf("w %d h %d\n", width, height);

    nbytes = bpp / 8;               // No. of bytes per pixels
    size = width * height * nbytes; // Total number of bytes to be read
    imageData = malloc(size * sizeof(char));
    fread(imageData, size, 1, file);
    // Swap r and b values
    for (int i = 0; i < width * height; i++) {
        index = i * nbytes;
        temp = imageData[index];
        imageData[index] = imageData[index + 2];
        imageData[index + 2] = temp;
    }

    *out_width = width;
    *out_height = height;
    return imageData;
}

void
load_bmp(char* filename)
{
    int width, height;
    char* image_data = load_bmp_to_array(filename, &width, &height);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA8,
        width,
        height,
        0,
        GL_RGB,
        GL_UNSIGNED_BYTE,
        image_data);
    free(image_data);
}
