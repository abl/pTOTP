// Helper program to generate unix-like time from a PblTm struct.
//
// Copyright 2013 Hexxeh
// Author: Hexxeh
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "pebble_os.h"

unsigned int get_unix_time(int tm_tz)
{
  PblTm curr_time;
  get_time(&curr_time);
  unsigned int now = 0;
  now += (curr_time.tm_year-70)*31536000;
  now += ((curr_time.tm_year-69)/4)*86400;
  now -= ((curr_time.tm_year-1)/100)*86400;
  now += ((curr_time.tm_year+299)/400)*86400;
  now += curr_time.tm_yday*86400;
  now += (curr_time.tm_hour+(tm_tz))*3600;
  now += curr_time.tm_min*60;
  now += curr_time.tm_sec;
  return now;
}