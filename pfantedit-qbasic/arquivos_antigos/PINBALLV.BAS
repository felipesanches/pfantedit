a$ = " "
file$ = "c:\games\jogos\pinball\fantasie\table1.prg"
OPEN file$ FOR BINARY AS #1
SCREEN 13
CLS
parte = 0
0
parte = parte + 1
SELECT CASE parte
 CASE 1: p = &H53189
 CASE 2: p = &H5A4E7
 CASE 3: p = &H62835
 CASE 4: p = &H6B99F
 CASE 5: END
END SELECT
pr = p

1
IF prbl THEN x = 0: prbl = 0: GOTO 0

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
PSET (x, y), C
x = x + 1
IF x > 319 THEN
x = x - 320: y = y + 1
IF y = 144 THEN prbl = 1: x = 0
END IF

IF y > 199 THEN END
RETURN

