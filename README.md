# Simple Pebble TOTP

## Overview
This is presented purely for academic interest and should probably not be installed on a Pebble.

Note that a customized wscript is used - the WAF equivalent of Makefile - this is necessary in order to define _LITTLE_ENDIAN and link the appropriate google-authentication headers in to the user source directory.

## Building

Modify `src/config.h` to match your timezone (remember to adjust for DST) and your base32-encoded secret key.

## Usage

Up/down will change the GMT offset; note that due to current limitations of the Pebble SDK the watch does not know its own time zone and app settings do not persist. The current code in the new timezone will then be displayed.

Center will cause the current code to be displayed.

The second line is the code; the third line is the current GMT offset; the fourth line is the countdown to code expiry. When the code expires, the display is cleared as a security measure.