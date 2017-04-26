
file$ = "c:\table1.prg"

OPEN file$ FOR BINARY AS #1
CLS
v = 0
bx = &H100
a$ = " "
SCREEN 13



p = 340437
GET #1, p, a$
pr = p + ASC(a$) + 2
p = p + 1
x = 1
y = 1

1
IF pr = p THEN
   GET #1, p, a$
   IF ASC(a$) > 128 THEN
     GOSUB drawline
   ELSE
     pr = p + ASC(a$) + 2
     p = p + 1
   END IF
ELSE

GET #1, p, a$
c = ASC(a$)
GOSUB dot
p = p + 1
END IF
GOTO 1


drawline:
GET #1, p, a$
n = 257 - ASC(a$)
GET #1, p + 1, a$
c = ASC(a$)

FOR I = 1 TO n
  GOSUB dot
NEXT I
pr = p + 2
p = p + 2
RETURN

dot:
PSET (x - 1, y - 1), c
x = x + 1
REM IF y > 143 THEN PRINT p: END
IF x > 320 THEN x = x - 320: y = y + 1
IF y > 200 THEN GOTO 44
RETURN

44

OUT &H3C8, 0

cx = &H300
di = &H4C54 + v
42
GET #1, di, a$
ax = ASC(a$)
ax = ax * bx
ax = ax AND &HFFFF
OUT &H3C9, (ax AND &HFF00) / &HFF
di = di + 1
cx = cx - 1
IF cx >= 0 THEN GOTO 42
FOR pp = 1 TO 10 ^ 6: NEXT pp
LOCATE 1, 1
PRINT v
v = v + 1
GOTO 44

