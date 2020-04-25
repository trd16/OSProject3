# OSProject3
---------------------------------------------------------

## Division of Labor
*Scott Early*:       cd, mv, read, cp
  
*Steven Starnes*:    info, size, ls, write, rm

*Taylor Driver*:     creat, mkdir, open, close

## Contents of Tar
*project3.c* - our file containing main, all functions and all functions' definitions

*makefile*

*README.md*

*fat32.img*  -our imagefile

## Compilation Instructions
Simply type "make" and then use the executable "project3".

## Known Bugs\Unfinished Portions
We have correctly been able to retrieve information from the fat32 image file and have done all error checking for our functions. We however, unfortunately, were unable to complete much of the project functions.

*info, size, open, close*: should all be functioning correctly

*ls*: functions correctly with the home directory, but is unable to ls within other directories

*cp, mv, and read*: only do error checking

*cd*: Does error checking and attempts to change current directory variable


## Special Considerations
