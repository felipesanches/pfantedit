Cls
Input "filename To Load: ", Filename$
Open Filename$ For Binary As #1
Header$ = Space$(14)
Sizing$ = Space$(4)
Get #1, 1, Header$
Get #1, 15, Sizing$
Bmpinfosize = Cvi(sizing$)
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
If Bmpinfosize = 12 Then
   Infoheader$ = Space$(12)
   Get #1, 15, Infoheader$
   Nbits = Cvi(mid$(infoheader$, 15, 4))
   If Nbits = 1 Then
      Palet$ = Space$(6)
      Get #1, Bmpinfosize + 15, Palet$
   Elseif Nbits = 4 Then
      Palet$ = Space$(48)
      Get #1, Bmpinfosize + 15, Palet$
   Elseif Nbits = 8 Then
      Palet$ = Space$(768)
      Get #1, Bmpinfosize + 15, Palet$
   End If
Elseif Bmpinfosize = 40 Then
   Infoheader$ = Space$(40)
   Get #1, 15, Infoheader$
   Nbits = Cvi(mid$(infoheader$, 15, 4))
   If Nbits = 1 Then
      Palet$ = Space$(8)
      Get #1, Bmpinfosize + 15, Palet$
   Elseif Nbits = 4 Then
      Palet$ = Space$(64)
      Get #1, Bmpinfosize + 15, Palet$
   Elseif Nbits = 8 Then
      Palet$ = Space$(1024)
      Get #1, Bmpinfosize + 15, Palet$
   End If
End If
Ft$ = Mid$(header$, 1, 2)
Print "type Of File (should Be Bm): "; Ft$
Filesize = Cvl(mid$(header$, 3, 4))
Print "size Of File: "; Filesize
R1 = Cvi(mid$(header$, 7, 2))
Print "reserved 1: "; R1
R2 = Cvi(mid$(header$, 9, 2))
Print "reserved 2: "; R2
Offset = Cvl(mid$(header$, 11, 4))
Print "number Of Bytes Offset From Beginning: "; Offset
Print
Headersize = Cvl(mid$(infoheader$, 1, 4))
Print "size Of Header: "; Headersize
Picwidth = Cvl(mid$(infoheader$, 5, 4))
Print "width: "; Picwidth
Picheight = Cvl(mid$(infoheader$, 9, 4))
Print "height: "; Picheight
Nplanes = Cvi(mid$(infoheader$, 13, 4))
Print "planes: "; Nplanes
Print "bits Per Plane: "; Nbits
Print
If Headersize = 40 Then
   Print "compression: ";
   Comptype = Cvl(mid$(infoheader$, 17, 4))
   If Comptype = 0 Then Print "none"
   If Comptype = 1 Then Print "run Length - 8 Bits"
   If Comptype = 2 Then Print "run Length - 4 Bits"
   Imagesize = Cvl(mid$(infoheader$, 21, 4))
   Print "image Size (bytes): "; Imagesize
   Xsize = Cvl(mid$(infoheader$, 25, 4))
   Print "x Size (pixels Per Metre): "; Xsize
   Ysize = Cvl(mid$(infoheader$, 29, 4))
   Print "y Size (pixels Per Metre): "; Ysize
   Colorsused = Cvl(mid$(infoheader$, 33, 4))
   Print "number Of Colours Used: "; Colorsused
   Neededcolours = Cvl(mid$(infoheader$, 37, 4))
   Print "number Of Important Colours: "; Neededcolours
End If
Print
Print "press Any Key T"; O; Continue.; ""
While Inkey$ = ""
Wend
If Nbits = 1 Then
   Screen 11
Elseif Nbits = 4 Then
   Screen 12
Elseif Nbits = 8 Or Nbits = 24 Then
   Screen 13
End If
If Bmpinfosize = 40 Then Ngroups = 4
If Bmpinfosize = 12 Then Ngroups = 3
If Nbits = 24 Then
   If Ngroups = 3 Then
      For C = 0 To 63
         D = C * 4
         Palet$ = Palet$ + Chr$(d) + Chr$(d) + Chr$(d)
         Palet$ = Palet$ + Chr$(d) + Chr$(d) + Chr$(d + 1)
         Palet$ = Palet$ + Chr$(d) + Chr$(d + 1) + Chr$(d)
         Palet$ = Palet$ + Chr$(d + 1) + Chr$(d) + Chr$(d)
      Next C
   Elseif Ngroups = 4 Then
      For C = 0 To 63
         D = C * 4
         Palet$ = Palet$ + Chr$(d) + Chr$(d) + Chr$(d) + Chr$(0)
         Palet$ = Palet$ + Chr$(d) + Chr$(d) + Chr$(d + 1) + Chr$(0)
         Palet$ = Palet$ + Chr$(d) + Chr$(d + 1) + Chr$(d) + Chr$(0)
         Palet$ = Palet$ + Chr$(d + 1) + Chr$(d) + Chr$(d) + Chr$(0)
      Next C
   End If
End If
For X = 1 To Len(palet$) Step Ngroups
   Zb# = Int((asc(mid$(palet$, X, 1))) / 4)
   Zg# = Int((asc(mid$(palet$, X + 1, 1))) / 4)
   Zr# = Int((asc(mid$(palet$, X + 2, 1))) / 4)
   Zc# = Zb# * 65536# + Zg# * 256# + Zr#
   Cres = Asc(mid$(palet$, X + 3, 1))
   Palette ((x - 1) / Ngroups), Zc#
Next X
If Nbits = 24 Then
   Y = Picheight - 1
   X = 0
   Dat$ = "   "
   While Y >= 0
      While X < Picwidth
         Get 1, , Dat$
         P1 = Int((asc(mid$(dat$, 1, 1)) + Asc(mid$(dat$, 1, 1)) + Asc(mid$(dat$, 1, 1))) / 3)
         Pset (x, Y), P1
         X = X + 1
      Wend
      Y = Y - 1
      X = 0
   Wend
Elseif Nbits = 8 Then
   Y = Picheight - 1
   X = 0
   Dat$ = " "
   While Y >= 0
      While X < Picwidth
         Get 1, , Dat$
         Pset (x, Y), Asc(dat$)
         X = X + 1
      Wend
      Y = Y - 1
      X = 0
   Wend
Elseif Nbits = 4 Then
   Y = Picheight - 1
   X = 0
   Dat$ = " "
   While Y >= 0
      While X < Picwidth
        Get 1, , Dat$
        Locate 1, 1
        P1 = Asc(dat$) And 15
        P2 = Asc(dat$) And 240 / 16
        Pset (x, Y), P1
        Pset (x + 1, Y), P2
    X = X + 2
      Wend
      Y = Y - 1
      X = 0
   Wend
Elseif Nbits = 1 Then
   Y = Picheight - 1
   X = 0
   Dat$ = " "
   While Y >= 0
      While X < Picwidth
        Get 1, , Dat$
        P1 = Asc(dat$)
        For P = 0 To 7
           Pset (x + (7 - P), Y), (p1 And 2 ^ P) / 2 ^ P
        Next P
        X = X + 8
      Wend
      Y = Y - 1
      X = 0
   Wend
End If
Close
