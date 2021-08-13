#include "load_image.h"

#include <GL/freeglut.h>
#include <stdio.h>
#include <stdlib.h>

void
load_tga(char* filename)
{
    char id, cmap, imgtype, bpp, c_garb;
    char *image_data, temp;
    short int s_garb, wid, hgt;
    int nbytes, size, index;

    FILE* file = fopen(filename, "rb");
    if (file == NULL) {
        printf("error opening file %s\n", filename);
        exit(1);
    }
    fread(&id, sizeof(id), 1, file);
    fread(&cmap, sizeof(cmap), 1, file);
    fread(&imgtype, sizeof(imgtype), 1, file);

    if (imgtype != 2 && imgtype != 3) {
        printf("Incompatible image type\n");
        exit(1);
    }

    fread(&s_garb, sizeof(s_garb), 1, file);
    fread(&s_garb, sizeof(s_garb), 1, file);
    fread(&c_garb, sizeof(c_garb), 1, file);

    fread(&s_garb, sizeof(s_garb), 1, file);
    fread(&s_garb, sizeof(s_garb), 1, file);

    fread(&wid, sizeof(wid), 1, file);
    fread(&hgt, sizeof(hgt), 1, file);

    fread(&bpp, sizeof(bpp), 1, file);
    fread(&c_garb, sizeof(c_garb), 1, file);

    nbytes = bpp / 8;
    size = wid * hgt * nbytes;
    printf("%d %d %d\n", wid, hgt, size);
    image_data = (char*)malloc(sizeof(char) * size);
    fread(&image_data, sizeof(char), size, file);

    if (nbytes > 2) // swap R and B
    {
        for (int i = 0; i < wid * hgt; i++) {
            index = i * nbytes;
            temp = image_data[index];
            image_data[index] = image_data[index + 2];
            image_data[index + 2] = temp;
        }
    }

    switch (nbytes) {
    case 1:
        glTexImage2D(GL_TEXTURE_2D, 0, 1, wid, hgt, 0, GL_LUMINANCE,
            GL_UNSIGNED_BYTE, image_data);
        break;
    case 3:
        glTexImage2D(GL_TEXTURE_2D, 0, 3, wid, hgt, 0, GL_RGB, GL_UNSIGNED_BYTE,
            image_data);
        break;
    case 4:
        glTexImage2D(GL_TEXTURE_2D, 0, 4, wid, hgt, 0, GL_RGBA,
            GL_UNSIGNED_BYTE, image_data);
        break;
    }
    printf("here\n");
    free(image_data);
}

void
load_bmp(char* filename)
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
        exit(1);
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
    glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_RGB,
        GL_UNSIGNED_BYTE, imageData);
    free(imageData);
}
