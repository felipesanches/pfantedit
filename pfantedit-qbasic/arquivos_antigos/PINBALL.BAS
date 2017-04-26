
file$ = "c:\table1.prg"

OPEN file$ FOR BINARY AS #1
SCREEN 13
CLS

p = 340437
a$ = " "
GET #1, p, a$
pr = p + ASC(a$) + 2
p = p + 1
x = 1
y = 1

1
REM IF p = &H59650 THEN p = &H5A4EE
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
   C = ASC(a$)
   GOSUB dot
   p = p + 1
END IF

GOTO 1


drawline:
GET #1, p, a$
n = 257 - ASC(a$)
GET #1, p + 1, a$
C = ASC(a$)

FOR I = 1 TO n
  GOSUB dot
NEXT I
pr = p + 2
p = p + 2
RETURN

dot:
PSET (x - 1, y - 1), C
x = x + 1
IF x > 320 THEN x = x - 320: y = y + 1
IF y > 200 THEN END
RETURN

