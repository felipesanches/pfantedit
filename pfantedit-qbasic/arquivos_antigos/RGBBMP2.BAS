CLS
z$ = CHR$(0)
r$ = " "
g$ = " "
b$ = " "
OPEN "c:\windows\desktop\tt03.bmp" FOR BINARY AS #1

'sistema de cores RYG
'a = vermelho
'b = amarelo
'c = verde


FOR x = 0 TO 255 STEP 1
FOR y = 0 TO 255 STEP 1
a = INT(RND * 56) + 28
b = INT(RND * 56) + 128
c = INT(RND * 56) + 200
GOSUB plot
NEXT y
NEXT x

END

plot:
IF x > 0 AND x < 256 AND y > 0 AND y < 256 THEN
p = 55 + (y * 256 + x) * 3
y$ = CHR$(a)
g$ = CHR$(b)
r$ = CHR$(c)
PUT #1, p, y$
PUT #1, p + 1, g$
PUT #1, p + 2, r$
END IF

RETURN

