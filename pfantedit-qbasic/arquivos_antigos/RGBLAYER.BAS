zz$ = CHR$(0) + CHR$(0)
r$ = " "
g$ = " "
b$ = " "
OPEN "c:\windows\desktop\amostra.bmp" FOR BINARY AS #1
OPEN "c:\windows\desktop\resg.bmp" FOR BINARY AS #2
OPEN "c:\windows\desktop\resr.bmp" FOR BINARY AS #3
OPEN "c:\windows\desktop\resb.bmp" FOR BINARY AS #4


FOR p = 1 TO 55
GET #1, p, r$
PUT #2, p, r$
PUT #3, p, r$
PUT #4, p, r$
NEXT p
'GRB Green Red Blue
p = 56
0
GET #1, p, g$
GET #1, p + 1, r$
GET #1, p + 2, b$

PUT #2, p, g$
PUT #2, , zz$

PUT #3, p + 1, r$
PUT #3, , zz$

PUT #4, p + 2, b$
PUT #4, , zz$

p = p + 3
IF EOF(1) THEN CLOSE : END
GOTO 0
               

