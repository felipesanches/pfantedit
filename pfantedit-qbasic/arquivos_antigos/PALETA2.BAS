bx = 1
a$ = " "
SCREEN 13
OUT &H3C8, 0
cx = 3 * 256
di = &H4F54
1
GET #1, di, a$
ax = ASC(a$)
ax = ax * bx
OUT &H3C9, (ax AND 256 * 255) / 256
di = di + 1
cx = cx - 1
IF cx > 0 THEN GOTO 1

