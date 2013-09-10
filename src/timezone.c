#include "timezone.h"

//Abbreviated slightly to fit on screen and in memory.
//The value in '' marks is the value expected in gmt_offset.bin

char *tz_names[] = {
      "Eniwetok",
      "Midway Island",
      "Hawaii",
      "Alaska",
      "Pacific",
      "Mountain",
      "Central",
      "Eastern",
      "Atlantic",
      "Newfoundland",
      "Brazil",
      "Mid-Atlantic",
      "Azores",
      "GMT",
      "Brussels",
      "Kaliningrad",
      "Baghdad",
      "Tehran",
      "Abu Dhabi",
      "Kabul",
      "Ekaterinburg",
      "Bombay",
      "Kathmandu",
      "Almaty",
      "Bangkok",
      "Beijing",
      "Tokyo",
      "Adelaide",
      "E. Australia",
      "Magadan",
      "Auckland"
};

int tz_offsets[] = {
      -43200, //'0'
      -39600, //'1'
      -36000, //'2'
      -32400, //'3'
      -28800, //'4'
      -25200, //'5'
      -21600, //'6'
      -18000, //'7'
      -14400, //'8'
      -12600, //'9'
      -10800, //':'
      -7200,  //';'
      -3600,  //'<'
      0,      //'='
      3600,   //'>'
      7200,   //'?'
      10800,  //'@'
      12600,  //'A'
      14400,  //'B'
      16200,  //'C'
      18000,  //'D'
      19800,  //'E'
      20700,  //'F'
      21600,  //'G'
      25200,  //'H'
      28800,  //'I'
      32400,  //'J'
      34200,  //'K'
      36000,  //'L'
      39600,  //'M'
      43200   //'N'
};
