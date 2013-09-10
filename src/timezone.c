#include "timezone.h"

//Abbreviated slightly to fit on screen and in memory.
//The value in '' marks is the value expected in gmt_offset.bin
TimeZone TIMEZONES[TIMEZONE_COUNT] = {
      {-43200, "Eniwetok"}, //'0'
      {-39600, "Midway Island"}, //'1'
      {-36000, "Hawaii"}, //'2'
      {-32400, "Alaska"}, //'3'
      {-28800, "Pacific"}, //'4'
      {-25200, "Mountain"}, //'5'
      {-21600, "Central"}, //'6'
      {-18000, "Eastern"}, //'7'
      {-14400, "Atlantic"}, //'8'
      {-12600, "Newfoundland"}, //'9'
      {-10800, "Brazil"}, //':'
      {-7200,  "Mid-Atlantic"}, //';'
      {-3600,  "Azores"}, //'<'
      {0,      "GMT"}, //'='
      {3600,   "Brussels"}, //'>'
      {7200,   "Kaliningrad"}, //'?'
      {10800,  "Baghdad"}, //'@'
      {12600,  "Tehran"}, //'A'
      {14400,  "Abu Dhabi"}, //'B'
      {16200,  "Kabul"}, //'C'
      {18000,  "Ekaterinburg"}, //'D'
      {19800,  "Bombay"}, //'E'
      {20700,  "Kathmandu"}, //'F'
      {21600,  "Almaty"}, //'G'
      {25200,  "Bangkok"}, //'H'
      {28800,  "Beijing"}, //'I'
      {32400,  "Tokyo"}, //'J'
      {34200,  "Adelaide"}, //'K'
      {36000,  "E. Australia"}, //'L'
      {39600,  "Magadan"}, //'M'
      {43200,  "Auckland"} //'N'
};

