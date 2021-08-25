#ifndef LOAD_TGA_H
#define LOAD_TGA_H

void
load_tga(char* filename);

char*
load_bmp_to_array(char* filename, int* out_width, int* out_height);

void
load_bmp(char* filename);

#endif
