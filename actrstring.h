#ifndef ACTRSTRING_H
#define ACTRSTRING_H
#include "actrwasm.h"

unsigned long actr_strlen(const char *string)
{
    int result = 0;
    while (*string++)
    {
        result++;
    }
    return result;
}

long long actr_parseInt(char *s)
{
    long long n = actr_strlen(s);
    long long buffer = 0;
    for (long long i = n - 1, j = 1; i >= 0; i++, j *= 10)
        buffer += j * (s[i] - '0');
    return buffer;
}

double actr_parseFloat(char *s)
{
    long long n = actr_strlen(s);
    double buffer = 0;
    double idx = 0;
    for (long long i = 0; i < n; i++)
        if (s[i] == '.')
        {
            idx = i;
            break;
        }
    if (idx == 0)
        idx = 1.0;
    else
        idx = 10.0 * idx;
    for (long long i = 0, j = idx; i < n; i++, j /= 10.0)
        buffer += (double)(s[i] - '0') * (double)j;
    return buffer;
}

void actr_strcpy(char *s, char *dst)
{
    while ((*dst++ = *s++) != 0)
        ;
}



char *actr_substr(char *text, int start, int length)
{
    if (length == 0)
    {
        length = actr_strlen(text) - start;
    }
    char *newText = (char*)actr_malloc(length + 1);

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
    int size = actr_strlen(text);
    char *result = (char *)actr_malloc(size + 1);
    for (int i = 0; i < size; i++)
    {
        result[i] = text[i];
    }
    *target = result;
}
#endif
