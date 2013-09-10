#ifndef TIMEZONE_H
#define TIMEZONE_H

typedef struct {
      int tz_offset;
      char tz_name[];
} TimeZone;

#define TIMEZONE_COUNT 31
#define MAX_TIMEZONE_NAME_LENGTH 13

extern TimeZone TIMEZONES[TIMEZONE_COUNT];

#endif