# Video playing program for Amiga with ffmpeg

This program uses `ffmpeg` for Amiga to implement video playing in C programs.

The program was developed on AmiKit environment. On later AmiKit version ffmpeg comes as default and is automatically in the path.
This version uses ffpeg to produce a raw video file to RAM Disk. Keep that in mind with RAM memory settings.

The C compiler used was VBCC. To compile the program use `vc PlayVideo.c -o PlayVideo -fpu=68882`

Look inside the code, for more details.

This program was made only in small video files (in terms on file size) in mind.

## Picture

Picture of the program running on AmiKit environment.

