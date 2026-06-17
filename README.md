# Video playing program for Amiga with ffmpeg

This program uses `ffmpeg` for Amiga to implement video playing from C programs.

The program was developed on AmiKit environment. On later AmiKit version ffmpeg comes as default and is automatically in the path.
This version uses ffmpeg to produce a raw video file to RAM Disk. Keep that in mind with RAM memory settings.

The C compiler used was VBCC. To compile the program use `vc PlayVideo.c -o PlayVideo -fpu=68882` or `vc PlayVideoStream.c -o PlayVideoStream -lamiga -fpu=68882`

Look inside the code, for more details. Just in case, please notice these:

```
#define FRAMES_PER_SECOND 7.0
#define WIDTH 512
#define HEIGHT 512
```

and

`int frameSize = WIDTH * HEIGHT * 4;`

Mp4s need that * 4, but if the video format doesn't use all these A R G B, * 3 is the right value...

This program was made only in small video files (in terms on file size) in mind.

## Picture

Picture of the program running on AmiKit environment.

<img width="1920" height="1080" alt="PlayVideo" src="https://github.com/user-attachments/assets/5a94fa5b-c69f-4bf1-a1ed-215c165f3f57" />
