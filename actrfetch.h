#ifndef ACTRGITHUB_H
#define ACTRGITHUB_H
#include "actrwasm.h"
#include "actrasync.h"
extern int _actr_fetch_text_length(char * url, int urlLength, int mapId, char * name, int nameLength);

extern int _actr_fetch_json_length(char * url, int urlLength, int jsonId);

extern int _actr_fetch_image_length(char *url, int urlLength, int jsonId);
    
// requires 
// fetch the url asynchronously, puts the result into the provided map
// calls actr_async_result with the returned handle and success indicator  
// returns the handle of the asynchronous operation
// returns < 1 on error
int actr_fetch_text(char * url, int mapId, char * name) {
    return _actr_fetch_text_length(url, strlen(url), mapId, name, strlen(name));
}

// fetch the url asynchronously, puts the result into the provided json container
// calls actr_async_result with the returned handle and success indicator  
// returns the handle of the asynchronous operation
int actr_fetch_json(char * url, int jsonId) {
    return _actr_fetch_json_length(url, strlen(url), jsonId);
}
int actr_fetch_image(char *url, int jsonId) {
    return _actr_fetch_image_length(url, strlen(url), jsonId);
}
#endif
