
#ifndef ACTRWASM_H
#define ACTRWASM_H
#include "actralloc.h"
#define PI 3.14
#define TAU 6.28

// basic header required for integration with the ActR platform
// all actr_canvas2d prefixed method will follow html CanvasRenderingContext2d as closly as possible
// see docs at https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D

extern double actr_sin(double value);
extern double actr_cos(double value);
extern double actr_atan2(double y, double x);
extern double actr_sqrt(double x);

int actr_isnan(double a)
{
    if (a != a)
    {
        return 1;
    }
    return 0;
}


int actr_sign(double value)
{
    if (value < 0)
        return -1;
    if (value > 0)
        return 1;
    return 0;
}

extern void _actr_sanity_size(int intSize, int longSize, int floatSize, int doubleSize, int stateSize, void *state);
extern int actr_authenticated();
// end helpers
struct ActrPoint32 // size 8
{
    int x; // index 0, size 4
    int y; // index 4, size 4
};
struct ActrPoint64 // size 16
{
    long long x; // index 0, size 8
    long long y; // index 8, size 8
};
struct ActrPointF // size 8
{
    float x; // index 0, size 4
    float y; // index 4, size 4
};
struct ActrPointD // size 16
{
    double x; // index 0, size 8
    double y; // index 8, size 8
};

struct ActrSize32 // size 8
{
    int w; // index 0, size 4
    int h; // index 4, size 4
};

struct ActrSize64 // size 16
{
    long long w; // index 0, size 8
    long long h; // index 8, size 8
};

struct ActrSizeF // size 8
{
    float w; // index 0, size 4
    float h; // index 4, size 4
};

struct ActrSizeD // size 16
{
    double w; // index 0, size 8
    double h; // index 8, size 8
};

struct ActrState // size 28
{
    // canvasSize will be updated automatically
    struct ActrSize32 canvasSize; // index 0, size 8
    // pointer position will be updated automatically
    struct ActrPoint32 pointerPosition; // index 8, size 8
    // text size will be updated when actr_canvas2d_measureText is called
    struct ActrSize32 textSize; // index 16, size 8
    int debug;                  // index 20, size 4
};

struct ActrState *actrState;

struct ActrPointD actr_sub(struct ActrPointD *a, struct ActrPointD *b)
{
    struct ActrPointD result;
    result.x = a->x - b->x;
    result.y = a->y - b->y;
    return result;
}

struct ActrPointD actr_div(struct ActrPointD *a, double b)
{
    struct ActrPointD result;
    result.x = a->x / b;
    result.y = a->y / b;
    return result;
}

double actr_distance2(struct ActrPointD *a, struct ActrPointD *b)
{
    struct ActrPointD result = actr_sub(a, b);
    return result.x * result.x + result.y * result.y;
}

double actr_distance(struct ActrPointD *a, struct ActrPointD *b)
{
    return actr_sqrt(actr_distance2(a, b));
}

struct ActrPointD actr_normalize(struct ActrPointD * point) 
{

    return actr_div(point, actr_sqrt(point->x * point->x + point->y * point->y));

}

unsigned int actr_pack_bytes(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
    return (r << 24) | (g << 16) | (b << 8) | a;
}
void actr_unpack_bytes(unsigned int value, unsigned char *r, unsigned char *g, unsigned char *b, unsigned char *a)
{
    *r = (value >> 24) & 0xFF;
    *g = (value >> 16) & 0xFF;
    *b = (value >> 8) & 0xFF;
    *a = value & 0xFF;
}
/// @brief internal use
[[clang::export_name("_actr_sanity")]]
void _actr_sanity()
{
    actrState = actr_malloc(sizeof(struct ActrState));
    _actr_sanity_size(sizeof(int), sizeof(long long), sizeof(float), sizeof(double), sizeof(struct ActrState), actrState);
}

/// @brief get length of null terminated string
/// @param string
/// @return length of the string
unsigned long strlen(const char *string)
{
    int result = 0;
    while (*string++)
    {
        result++;
    }
    return result;
}

char *substr(char *text, int start, int length)
{
    if (length == 0)
    {
        length = strlen(text) - start;
    }
    char *newText = actr_malloc(length + 1);

    for (int i = 0; i < length; i++)
    {
        newText[i] = text[start + i];
    }
    return newText;
}
void actr_heap_string(char **target, char *text)
{
    if (*target != 0)
    {
        actr_free(*target);
    }
    int size = strlen(text);
    char *result = actr_malloc(size + 1);
    for (int i = 0; i < size; i++)
    {
        result[i] = text[i];
    }
    *target = result;
}
#endif
