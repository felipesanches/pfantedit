
file$ = "c:\table1.prg"

OPEN file$ FOR BINARY AS #1
SCREEN 13
CLS
a$ = " "

p = &H4F54
OUT &H3C8, 0

FOR c = 0 TO 300
GET #1, p + c, a$
OUT &H3C9, ASC(a$)
NEXT




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
IF y > 200 THEN END
RETURN

