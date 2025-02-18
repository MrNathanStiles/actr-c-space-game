#ifndef ACTRNTOS_H
#define ACTRNTOS_H
#include "actrwasm.h"
#include "actrlog.h"

void reverse(char *str, int len)
{
    int i = 0, j = len - 1;
    while (i < j)
    {
        char temp = str[i];
        str[i] = str[j];
        str[j] = temp;
        i++;
        j--;
    }
}

int _itos(long long value, char *string)
{
    int i = 0;
    int isNeg = 0;
    if (value == 0) {
        string[i++] = 48;
        return i;
    }
    if (value < 0)
    {
        isNeg = 1;
        value = -value;
    }

    while (value != 0)
    {
        long long rem = value % 10;
        value /= 10;
        string[i++] = rem + 48;
    }
    if (isNeg)
    {
        string[i++] = '-';
    }
    reverse(string, i);
    return i;
}
long long actr_pow(long long base, int exponent)
{
    long long result = base;
    for (int i = 1; i < exponent; i++)
    {
        result *= base;
    }
    return result;
}
char *itos(long long value)
{
    char buffer[256];
    int i = _itos(value, buffer);
    char *result = actr_malloc(i + 1);

    for (int j = 0; j < i; j++)
    {
        result[j] = buffer[j];
    }
    return result;
}
#define FTOS_CHAR_BUFF_SIZE 128

char * _float_to_char(double x, char *p) {
    // https://stackoverflow.com/questions/23191203/convert-float-to-string-without-sprintf
    char *s = p + FTOS_CHAR_BUFF_SIZE; // go to end of buffer
    unsigned int decimals;  // variable to store the decimals
    int units;  // variable to store the units (part to left of decimal place)
    if (x < 0) { // take care of negative numbers
        decimals = (long long)(x * -100) % 100; // make 1000 for 3 decimals etc.
        units = (long long)(-1 * x);
    } else { // positive numbers
        decimals = (int)(x * 100) % 100;
        units = (long long)x;
    }

    *--s = (decimals % 10) + '0';
    decimals /= 10; // repeat for as many decimal places as you need
    *--s = (decimals % 10) + '0';
    *--s = '.';

    do {
        *--s = (units % 10) + '0';
        units /= 10;
    } while (units > 0);
    if (x < 0) *--s = '-'; // unary minus sign for negative numbers
    return s;
}
char * float_to_char(double x)
{
    char buffer[FTOS_CHAR_BUFF_SIZE];
    for (int i = 0; i < FTOS_CHAR_BUFF_SIZE; i++) {
        buffer[i] = 0;
    }
    char * result = _float_to_char(x, &buffer[0]);
    int len = FTOS_CHAR_BUFF_SIZE - (result - &buffer[0]);
    
    return substr(result, 0, len);
}

char *ftos(double value, int precision)
{
    long long ipart = (long long)value;
    double fpart = value - ipart;

    char buffer[256];

    int i = _itos(ipart, buffer);

    if (precision != 0)
    {
        buffer[i++] = '.';
        fpart = fpart * actr_pow(10, precision);
        if (fpart < 0)
        {
            fpart = -fpart;
        }

        i += _itos((long long)fpart, buffer + i);
    }

    char *result = actr_malloc(i + 1);

    for (int j = 0; j < i; j++)
    {
        result[j] = buffer[j];
    }

    return result;
}

#endif
