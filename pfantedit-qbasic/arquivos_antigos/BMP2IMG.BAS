CLS


INPUT "BMP: ", bmp$
INPUT "IMG: ", img$

REM bmp$ = "c:\windows\desktop\pinball\z01"

IF RIGHT$(bmp$, 4) = ".bmp" THEN bmp$ = LEFT$(bmp$, LEN(bmp$) - 4)
IF RIGHT$(img$, 4) = ".img" THEN img$ = LEFT$(img$, LEN(img$) - 4)
IF bmp$ = "" THEN bmp$ = img$
IF img$ = "" THEN img$ = bmp$
IF bmp$ = "" THEN END

OPEN bmp$ + ".bmp" FOR BINARY AS #1
OPEN img$ + ".img" FOR BINARY AS #2

IF LOF(1) = 0 THEN END
header$ = SPACE$(14)
sizing$ = SPACE$(4)
GET #1, 1, header$
GET #1, 15, sizing$
Bmpinfosize = CVI(sizing$)
'bmpinfosize - Is The Size Of The Information Header For The Bitmap.
'              Different Bitmap Versions Have Variations In Filetypes.
'              40 Is A Standard Windows 3.1 Bitmap.
'              12 Is For Os/2 Bitmaps
'the Next Routine Reads In The Appropriate Headers And Colour Tables.
'nbits Is The Number Of Bits Per Pixel - I.e. Number Of Colours
'1 Bit = 2 Colours, 4 Bits = 16 Colours, 8 Bits = 256 Colours, Etc.
'the 24 Bit Mode Does Not Have A Palette, Its Colours Are Expressed As
'image Data
'design Of A Windows 3.1 Bitmap - Taken From Bmp.zip On The
'x2ftp.oulu.fi Ftp Site Under /pub/msdos/programming/formats
'specifications For A Windows 3.1 Bitmap. (.bmp)
'email Any Questions/responses To Me At Zabudsk@ecf.utoronto.ca
'or Post To Alt.lang.basic Or Comp.lang.basic.misc.
'       | # Of   |
'offset | Bytes  | Function (value)
'-------+--------+--- General Picture Information Starts Here---------
'  0    |   2    | (bm) - Tells Us That The Picture Is In Bmp Format
'  2    |   4    | Size Of The File (without Header?)
'  6    |   2    | (0) Reserved1 - Must Be Zero
'  8    |   2    | (0) Reserved2 - Must Be Zero
'  10   |   4    | Number Of Bytes Offset Of The Picture Data
'-------+--------+--- Information Header Starts Here -----------------
'  14   |   4    | (40/12) Size Of Information Header (win3.1/os2)
'  18   |   4    | Picture Width In Pixels
'  22   |   4    | Picture Height In Pixels
'  26   |   2    | (1) Number Of Planes, Must Be 1
'  28   |   2    | Number Of Bits Per Pixel (bpp), Must Be 1,4,8 Or 24
'  30   |   4    | (0) Compression - 0 Means No Compression, 1,2 Are Rles
'  34   |   4    | Image Size In Bytes
'  38   |   4    | Picture Width In Pels Per Metre
'  42   |   4    | Picture Height In Pels Per Metre
'  46   |   4    | (0) Number Of Colours Used In The Picture, 0 Means All
'  50   |   4    | (0) Number Of Important Colours, 0 Means All
'-------+--------+--- Palette Data Starts Here -----------------------
'  54   |   1    | (b) - Blue Intensity Component, Color 0 - Range 0 To 255
'  55   |   1    | (g) - Green Intensity Component, Color 0 - Range 0 To 255
'  56   |   1    | (r) - Red Intensity Component, Color 0 - Range 0 To 255
'  57   |   1    | (0) - Unused
'  58   |   1    | (b) - Blue Intensity Component, Color 0 - Range 0 To 255
'  ...  | ...    |
'  54   | 4*2^bpp| Total Range Of Palette
'-------+--------+--- Image Data Starts Here -------------------------
'54+    | Width* | Bitmap Data Starting At Lower Left Portion Of The
'(4*2^n)| Height*| Image Moving From Left Towards Right. Moving Up 1
'       | (8/bpp)| Pixel When At The Right Hand Side Of The Image, Starting
'       |        | From The Left Side Again, Until The Top Right Of The
'       |        | Image Is Reached
'note That This Format Is Slightly Different For A Os/2 Bitmap.
'the Header Is The Same Up To (but Not Including) Bit 30-
'the Palette Colour Values Follow At Bit 30, With The Form...
'1 Byte Blue Intensity
'1 Byte Green Intensity
'1 Byte Red Intensity
'for Each Colour Of The Picture.
'bitmapped Image Data Follows The Colour Tables
'special Note: When Storing 1 Bit (2 Colour) Pictures.
'8 Horizontal Pixels Are Packed Into 1 Byte. Each Bit Determines
'the Colour Of One Pixel (colour 0 Or Colour 1)
'4 Bit Pictures (16 Colours) Use 2 Nibbles (4 Bits) For Each Pixel
'thus There Are 2 Pixels For Each Byte Of Image Data.
'8 Bit Pictures Use 1 Byte Per Pixel. Each Byte Of Image Data
'represents One Of 256 Colours.
'24 Bit Pictures Express Colour Values By Using 3 Bytes And Each Has A
'value Between 0 And 255. The First Byte Is For Red, The Second Is For
'green And The Third Is For Blue. Thus (256)^3 Or 2^24 Of 16777216 Different
'colours.
IF Bmpinfosize = 12 THEN
   infoheader$ = SPACE$(12)
   GET #1, 15, infoheader$
   Nbits = CVI(MID$(infoheader$, 15, 4))
   IF Nbits = 1 THEN
      palet$ = SPACE$(6)
      GET #1, Bmpinfosize + 15, palet$
   ELSEIF Nbits = 4 THEN
      palet$ = SPACE$(48)
      GET #1, Bmpinfosize + 15, palet$
   ELSEIF Nbits = 8 THEN
      palet$ = SPACE$(768)
      GET #1, Bmpinfosize + 15, palet$
   END IF
ELSEIF Bmpinfosize = 40 THEN
   infoheader$ = SPACE$(40)
   GET #1, 15, infoheader$
   Nbits = CVI(MID$(infoheader$, 15, 4))
   IF Nbits = 1 THEN
      palet$ = SPACE$(8)
      GET #1, Bmpinfosize + 15, palet$
   ELSEIF Nbits = 4 THEN
      palet$ = SPACE$(64)
      GET #1, Bmpinfosize + 15, palet$
   ELSEIF Nbits = 8 THEN
      palet$ = SPACE$(1024)
      GET #1, Bmpinfosize + 15, palet$
   END IF
END IF
Ft$ = MID$(header$, 1, 2)
PRINT "type Of File (should Be Bm): "; Ft$
Filesize = CVL(MID$(header$, 3, 4))
PRINT "size Of File: "; Filesize
R1 = CVI(MID$(header$, 7, 2))
PRINT "reserved 1: "; R1
R2 = CVI(MID$(header$, 9, 2))
PRINT "reserved 2: "; R2
Offset = CVL(MID$(header$, 11, 4))
PRINT "number Of Bytes Offset From Beginning: "; Offset
PRINT
Headersize = CVL(MID$(infoheader$, 1, 4))
PRINT "size Of Header: "; Headersize
picwidth = CVL(MID$(infoheader$, 5, 4))
PRINT "width: "; picwidth
Picheight = CVL(MID$(infoheader$, 9, 4))
PRINT "height: "; Picheight
Nplanes = CVI(MID$(infoheader$, 13, 4))
PRINT "planes: "; Nplanes
PRINT "bits Per Plane: "; Nbits
PRINT
IF Headersize = 40 THEN
   PRINT "compression: ";
   Comptype = CVL(MID$(infoheader$, 17, 4))
   IF Comptype = 0 THEN PRINT "none"
   IF Comptype = 1 THEN PRINT "run Length - 8 Bits"
   IF Comptype = 2 THEN PRINT "run Length - 4 Bits"
   Imagesize = CVL(MID$(infoheader$, 21, 4))
   PRINT "image Size (bytes): "; Imagesize
   Xsize = CVL(MID$(infoheader$, 25, 4))
   PRINT "x Size (pixels Per Metre): "; Xsize
   Ysize = CVL(MID$(infoheader$, 29, 4))
   PRINT "y Size (pixels Per Metre): "; Ysize
   Colorsused = CVL(MID$(infoheader$, 33, 4))
   PRINT "number Of Colours Used: "; Colorsused
   Neededcolours = CVL(MID$(infoheader$, 37, 4))
   PRINT "number Of Important Colours: "; Neededcolours
END IF
PRINT
PRINT "press Any Key T"; O; Continue.; ""
WHILE INKEY$ = ""
WEND
IF Nbits = 1 THEN
   SCREEN 11
ELSEIF Nbits = 4 THEN
   SCREEN 12
ELSEIF Nbits = 8 OR Nbits = 24 THEN
   SCREEN 13
END IF
IF Bmpinfosize = 40 THEN Ngroups = 4
IF Bmpinfosize = 12 THEN Ngroups = 3
IF Nbits = 24 THEN
   IF Ngroups = 3 THEN
      FOR C = 0 TO 63
         d = C * 4
         palet$ = palet$ + CHR$(d) + CHR$(d) + CHR$(d)
         palet$ = palet$ + CHR$(d) + CHR$(d) + CHR$(d + 1)
         palet$ = palet$ + CHR$(d) + CHR$(d + 1) + CHR$(d)
         palet$ = palet$ + CHR$(d + 1) + CHR$(d) + CHR$(d)
      NEXT C
   ELSEIF Ngroups = 4 THEN
      FOR C = 0 TO 63
         d = C * 4
         palet$ = palet$ + CHR$(d) + CHR$(d) + CHR$(d) + CHR$(0)
         palet$ = palet$ + CHR$(d) + CHR$(d) + CHR$(d + 1) + CHR$(0)
         palet$ = palet$ + CHR$(d) + CHR$(d + 1) + CHR$(d) + CHR$(0)
         palet$ = palet$ + CHR$(d + 1) + CHR$(d) + CHR$(d) + CHR$(0)
      NEXT C
   END IF
END IF
FOR x = 1 TO LEN(palet$) STEP Ngroups
   Zb# = INT((ASC(MID$(palet$, x, 1))) / 4)
   Zg# = INT((ASC(MID$(palet$, x + 1, 1))) / 4)
   Zr# = INT((ASC(MID$(palet$, x + 2, 1))) / 4)
   Zc# = Zb# * 65536# + Zg# * 256# + Zr#
   Cres = ASC(MID$(palet$, x + 3, 1))
   PALETTE ((x - 1) / Ngroups), Zc#
NEXT x
IF Nbits = 24 THEN
   y = Picheight - 1
   x = 0
   dat$ = "   "
   WHILE y >= 0
      WHILE x < picwidth
         GET 1, , dat$
         p1 = INT((ASC(MID$(dat$, 1, 1)) + ASC(MID$(dat$, 1, 1)) + ASC(MID$(dat$, 1, 1))) / 3)
         cor$ = CHR$(p1)
         PSET (x, y), p1
         PUT #2, 1 + x + y * 320, cor$
         x = x + 1
      WEND
      y = y - 1
      x = 0
   WEND
ELSEIF Nbits = 8 THEN
   y = Picheight - 1
   x = 0
   dat$ = " "
   WHILE y >= 0
      WHILE x < picwidth
         GET 1, , dat$
         PSET (x, y), ASC(dat$)
         PUT #2, 1 + x + y * 320, dat$
         x = x + 1
      WEND
      y = y - 1
      x = 0
   WEND
ELSEIF Nbits = 4 THEN
   y = Picheight - 1
   x = 0
   dat$ = " "
   WHILE y >= 0
      WHILE x < picwidth
        GET 1, , dat$
        LOCATE 1, 1
        p1 = ASC(dat$) AND 15
        p2 = ASC(dat$) AND 240 / 16
        PSET (x, y), p1
        PSET (x + 1, y), p2
        cor$ = CHR$(p1) + CHR$(p2)
        PUT #2, 1 + x + y * 320, cor$
        x = x + 2
      WEND
      y = y - 1
      x = 0
   WEND
ELSEIF Nbits = 1 THEN
   y = Picheight - 1
   x = 0
   dat$ = " "
   WHILE y >= 0
      WHILE x < picwidth
        GET 1, , dat$
        p1 = ASC(dat$)
        FOR P = 0 TO 7
           cor = (p1 AND 2 ^ P) / 2 ^ P
           PSET (x + (7 - P), y), cor
           cor$ = CHR$(cor)
           PUT #2, 1 + x + y * 320, cor$
        NEXT P
        x = x + 8
      WEND
      y = y - 1
      x = 0
   WEND
END IF
CLOSE

