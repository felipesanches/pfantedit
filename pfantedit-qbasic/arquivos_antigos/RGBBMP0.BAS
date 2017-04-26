
r$ = " "
g$ = " "
b$ = " "
OPEN "c:\windows\desktop\tt01.bmp" FOR BINARY AS #1
OPEN "c:\windows\desktop\tt02.bmp" FOR BINARY AS #2
'OPEN "c:\windows\desktop\tt01r.bmp" FOR BINARY AS #2
'OPEN "c:\windows\desktop\tt01g.bmp" FOR BINARY AS #3
'OPEN "c:\windows\desktop\tt01b.bmp" FOR BINARY AS #4

z$ = CHR$(0)

FOR p = 1 TO 55
GET #1, p, r$
PUT #2, p, r$
''PUT #3, p, r$
''PUT #4, p, r$
NEXT p

FOR p = 55 TO 55 + 65535 * 3 STEP 3
IF r > 255 THEN r = 0: g = g + 16
IF g > 255 THEN g = 0: b = b + 16
IF b > 255 THEN r = 0: g = 0: b = 0
r$ = CHR$(r)
g$ = CHR$(g)
b$ = CHR$(b)
PUT #2, p, r$
PUT #2, p + 1, g$
PUT #2, p + 2, b$
r = r + 16
NEXT p
END



FOR p = 55 TO 820 STEP 3
GET #1, p, b$
PUT #2, p, b$
PUT #2, p + 1, z$
PUT #2, p + 2, z$

GET #1, p + 1, r$
PUT #3, p, z$
PUT #3, p + 1, g$
PUT #3, p + 2, z$

GET #1, p + 2, r$
PUT #4, p, z$
PUT #4, p + 1, z$
PUT #4, p + 2, r$

NEXT p

CLOSE

