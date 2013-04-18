# DISCLAIMER

I don't use this as my primary TOTP source and neither should you. I have not yet fully tested this with an actual Google account - while the implementation is a trivial modification of the Google Authenticator sources, it's possible something unique to the Pebble platform may cause issues.

This is meant to be used WITH another, reliable, mobile authenticator - no warranty express or implied.

# Simple Pebble TOTP

## Overview
This is presented purely for academic interest and should probably not be installed on a Pebble.

Note that a customized wscript is used - the WAF equivalent of Makefile - this is necessary in order to define _LITTLE_ENDIAN and link the appropriate google-authentication headers in to the user source directory.

## Building

Reconstruct the symlinks using `~/pebble-dev/pebble-sdk-release-001/tools/create_pebble_project.py --symlink-only ~/pebble-dev/pebble-sdk-release-001/sdk/ pTOTP`

Copy the modified `wscript.new` to `wscript`: `cp wscript.new wscript` - `create_pebble_project.py` will fail with an odd-looking error if any of the files already exist.

Modify `src/config.h` to match your timezone (remember to adjust for DST) and your base32-encoded secret key (the four groups of four letters from https://accounts.google.com/b/0/SmsAuthConfig - make sure to remove the spaces and capitalize the letters.)

## Usage

Up/down will change the GMT offset; note that due to current limitations of the Pebble SDK the watch does not know its own time zone and app settings do not persist. The current code in the new timezone will then be displayed.

Center will cause the current code to be displayed.

The second line is the code; the third line is the current GMT offset; the fourth line is the countdown to code expiry. When the code expires, the display is cleared as a security measure.