#ifndef RAYLIB_DEFS_H
#define RAYLIB_DEFS_H
// Texture, tex data stored in GPU memory (VRAM)
#ifndef RAYLIB_H
typedef struct Texture {
    unsigned int id;        // OpenGL texture id
    int width;              // Texture base width
    int height;             // Texture base height
    int mipmaps;            // Mipmap levels, 1 by default
    int format;             // Data format (PixelFormat type)
} Texture;
#endif

// Texture2D, same as Texture
typedef Texture Texture2D;

#endif
