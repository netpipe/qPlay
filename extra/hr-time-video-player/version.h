#ifndef VERSION_H
#define VERSION_H

// File Version is our "release" values like
// Beta 5.1 = 0.5.1
// and actual release will be 1.0.0 or something like that
#define VER_FILEVERSION             0,1,0,0
#define VER_FILEVERSION_STR         "0.1.0.0\0"

// These are the date and time stamp of the build.
// Such as April 19th 2012 at midnight would be 12.4.19.0
// where the last digit is in quarter hours.

// NOTE!!! In order for these changes to take affect, you have to modify myapp.rc
// otherwise this file won't be read.  (i.e. delete and re-add a space in myapp.rc
// and then save it again)  Even if you do a build clean and qmake, etc.

#define VER_PRODUCTVERSION          15,05,19,30
#define VER_PRODUCTVERSION_STR      "15.05.19.30"

// This needs to stay "EyeTechDS" or it will change the location of the INI File
#define VER_COMPANYNAME_STR         "Post Meridian"
#define VER_FILEDESCRIPTION_STR     "HR Time Video Player"
// This needs to stay "Captain Crunch" or it will change the location of the INI File
#define VER_INTERNALNAME_STR        "HR Time Video Player"
#define VER_LEGALCOPYRIGHT_STR      "Copyright © 2015 Post Meridian"
#define VER_LEGALTRADEMARKS1_STR    "All Rights Reserved"
#define VER_LEGALTRADEMARKS2_STR    VER_LEGALTRADEMARKS1_STR
#define VER_ORIGINALFILENAME_STR    "player.exe"
#define VER_PRODUCTNAME_STR         "HR Time Video Player"

#define VER_COMPANYDOMAIN_STR       "pmify.com"

#endif // VERSION_H
