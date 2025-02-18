#ifndef ACTRTIME_H
#define ACTRTIME_H
 
/// @brief get system time
/// @return allocated string must be freed 
extern char * actr_time_string();
// epoch time in seconds
extern long long actr_time();
#endif
