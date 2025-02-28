#include "actrwasm.h"
#include "actrstring.h"

#ifndef ACTRJSON_H
#define ACTRJSON_H
        
    extern long long _actr_json_get_int_length(int jsonId, char * path, int pathLength);
    extern double _actr_json_get_float_length(int jsonId, char * path, int pathLength);
    extern char * _actr_json_get_string_length(int jsonId, char * path, int pathLength);

    extern void _actr_json_set_int_length(int jsonId, char * path, int pathLength, long long value);
    extern void _actr_json_set_float_length(int jsonId, char * path, int pathLength, double value);
    extern void _actr_json_set_string_length(int jsonId, char * path, int pathLength, char * value, int valueLength);
 
    extern int actr_json_store(int jsonId);
    extern int actr_json_load(int jsonId);
    extern int actr_json_delete(int jsonId);

    void actr_json_set_int(int jsonId, char * path, long long value) {
        _actr_json_set_int_length(jsonId, path, actr_strlen(path), value);
    }
    void actr_json_set_float(int jsonId, char * path, double value) {
        _actr_json_set_float_length(jsonId, path, actr_strlen(path), value);
    }
    void actr_json_set_string(int jsonId, char * path, char * value) {
        _actr_json_set_string_length(jsonId, path, actr_strlen(path), value, actr_strlen(value));
    }

    long long actr_json_get_int(int jsonId, char * path) {
        return _actr_json_get_int_length(jsonId, path, actr_strlen(path));
    }
    double actr_json_get_float(int jsonId, char * path) {
        return _actr_json_get_float_length(jsonId, path, actr_strlen(path));
    }
    char * actr_json_get_string(int jsonId, char * path) {
        return _actr_json_get_string_length(jsonId, path, actr_strlen(path));
    }

#endif
