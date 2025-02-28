#ifndef ACTRLOG_H
#define ACTRLOG_H
#include "actrwasm.h"
#include "actrstring.h"

extern void _actr_log_length(const char * pointer, int size);

/// @brief used for debugging, log messages will appear in the browser console
/// @param pointer 
void actr_log(const char * pointer) {
    _actr_log_length(pointer, actr_strlen(pointer));
}
#endif
