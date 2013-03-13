FxRemap

Purpose:
This program is meant to be used in conjunction with the 3DFX Voodoo
graphics and the S3 968/868 chipset.  It's purpose is to correct an
addressing bug in the S3 968 chipset.


How to Use:
If you have the S3 chipset simply add a line at the beginning of your
autoexec.bat to call fxRemap.exe.  This should be all that you have to
do.


Advanced Options:
FxRemap.exe will take three possible parameters.

/dS3 : This remaps the board even if the S3 chipset is not present.
example: fxremap.exe /dS3

/x : Excludes a memory range to map the board into.
example: fxremp.exe /x 0xE0000000-0xF0000000

/f : Forces the program to remap the board at the given location
example: fxremp.exe /f 0xC0000000
