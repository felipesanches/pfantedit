x = 1
y = 1

file$ = "table1.prg"

OPEN file$ FOR BINARY AS #1
SCREEN 13
CLS

p = &H6ABE0
r$ = " "
g$ = " "
b$ = " "
OUT &H3C8, 0
FOR C = 1 TO 256
  GET #1, p + 1, r$
  GET #1, p + 2, g$
  GET #1, p + 3, b$
  OUT &H3C9, ASC(r$) / 4
  OUT &H3C9, ASC(g$) / 4
  OUT &H3C9, ASC(b$) / 4
  REM PRINT ASC(r$); ASC(g$); ASC(b$)
  p = p + 3
  REM IF c = 10 THEN END
NEXT C
PRINT p
REM END



parte = 0
0
parte = parte + 1
SELECT CASE parte
CASE 1
y = 2
p = &H531D5
CASE 2
p = &H5A457
CASE 3
p = &H62835
CASE 4
p = &H6B99F
CASE 5
END
END SELECT
a$ = " "
GET #1, p, a$
pr = p + ASC(a$) + 2
p = p + 1
GOTO 2
1
IF prbl THEN x = 1: prbl = 0: GOTO 0
2
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
  REM PRINT C
  GOSUB dot
  p = p + 1
END IF

GOTO 1

drawline:
  GET #1, p, a$
  n = 255 - ASC(a$)
  GET #1, p + 1, a$
  C = ASC(a$)

  FOR I = 1 TO n
    GOSUB dot
  NEXT I
  pr = p + 2

  GET #1, pr, a$
  C = ASC(a$)
  RETURN

dot:
  PSET (x - 1, y - 1), C
  x = x + 1
  IF x > 320 THEN
    x = x - 320: y = y + 1
    IF y = 145 THEN prbl = 1: x = 1
  END IF

  IF y > 200 THEN END
  RETURN

















