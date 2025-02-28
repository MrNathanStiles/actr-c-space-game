#ifndef ACTRFORMAT_H
#define ACTRFORMAT_H
#include "actrntos.h"
#include "actrvector.h"
#include "actrstring.h"

struct ActrFormatState
{
    const char *format;
    struct ActrVector *values;
    struct ActrVector *allocated;
};
struct ActrFormatState *actr_format(char *format)
{
    struct ActrFormatState *afs = (struct ActrFormatState *)actr_malloc(sizeof(struct ActrFormatState));
    afs->values = actr_vector_init(4, 4);
    afs->allocated = actr_vector_init(4, 4);
    afs->format = format;
    return afs;
}
void actr_format_float(struct ActrFormatState *state, double value)
{
    actr_vector_add(state->allocated, (void *)state->values->count);
    // actr_vector_add(state->values, ftos(value, precision));
    actr_vector_add(state->values, float_to_char(value));
}

void actr_format_int(struct ActrFormatState *state, long long value)
{
    actr_vector_add(state->allocated, (void *)state->values->count);
    actr_vector_add(state->values, itos(value));
}

void actr_format_str(struct ActrFormatState *state, char *value)
{
    actr_vector_add(state->values, value);
}
void _actr_format_free(struct ActrFormatState *state)
{
    for (int i = 0; i < state->allocated->count; i++)
    {
        int index = (int)state->allocated->head[i];
        actr_free(state->values->head[index]);
    }
    actr_vector_free(state->allocated);
    actr_vector_free(state->values);
    actr_free(state);
}
char *actr_format_close(struct ActrFormatState *state)
{
    int length = actr_strlen(state->format);
    int lengths[state->values->count];

    for (int i = 0; i < state->values->count; i++)
    {
        lengths[i] = actr_strlen((const char *)state->values->head[i]);
        length += lengths[i];
    }

    char *result = (char *)actr_malloc(length + 1);
    int pos = 0;
    int flag = 0;
    int index = 0;
    while (*state->format)
    {
        if (flag)
        {
            flag = 0;
            if (*state->format == '%')
            {
                result[pos++] = '%';
            }
            else
            {
                char *value = (char *)state->values->head[index];
                for (int i = 0; i < lengths[index]; i++)
                {
                    result[pos++] = value[i];
                }
                index++;
            }
        }
        else
        {
            if (*state->format == '%')
            {
                flag = 1;
            }
            else
            {
                result[pos++] = *state->format;
            }
        }
        state->format++;
    }
    _actr_format_free(state);
    return result;
}

#endif
