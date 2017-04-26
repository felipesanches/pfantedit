'*********
CLS
OPEN "c:\docume~1\a4941223\desktop\table1.prg" FOR BINARY AS #1
OPEN "c:\docume~1\a4941223\desktop\table1.img" FOR BINARY AS #2
a$ = " "
c$ = " "
pr = &H531D5
p = pr
0
GET #2, 1 + (319 - x) + 320 * y, c$
cant = ASC(c$)
n = 1
1
x = x + 1
IF x > 319 THEN x = 0: y = y + 1: pode = 1
IF pode AND (y = 144 - 1 OR y = 2 * 144 - 1 OR y = 3 * 144 - 1 OR y = 4 * 144 - 1) THEN GOSUB sub2
IF proxparte THEN proxparte = 0: GOTO 0

IF p - pr = 130 THEN
  a$ = CHR$(128)
  PUT #1, pr, a$
  pr = p
  p = p + 1
  GOTO 0
END IF

GET #2, 1 + (319 - x) + 320 * y, c$
IF ASC(c$) = cant THEN
 n = n + 1
 IF n = 127 THEN GOSUB sub1: x = x + 1: GOTO 0
 GOTO 1
ELSE
 GOSUB sub3
 IF n2 = 1 THEN n2 = 0: GOTO 0
END IF
GOTO 1

sub1:
  a$ = CHR$(ABS(p - pr - 2))
  PUT #1, pr, a$
  a$ = CHR$(257 - n)
  PUT #1, p, a$
  a$ = CHR$(cant)
  PUT #1, p + 1, a$
  p = p + 2
  n = 1
  pr = p
RETURN

sub2:
 GOSUB sub3
 parte = parte + 1
 SELECT CASE parte
   CASE 1
    pr = &H5A4E7
   CASE 2
    pr = &H62835
   CASE 3
    pr = &H6B99F
   CASE 4
    CLOSE : END
 END SELECT
 p = pr
 pode = 0
 proxparte = 1
 x = 0
RETURN



sub3:
 IF n = 1 THEN
  IF pr = p THEN a$ = CHR$(0): PUT #1, p, a$: p = p + 1
  a$ = CHR$(cant)
  PUT #1, p, a$
  p = p + 1
 ELSEIF n = 2 THEN
  IF pr = p THEN a$ = CHR$(0): PUT #1, p, a$: p = p + 1
  a$ = CHR$(cant)
  PUT #1, p, a$
  PUT #1, p + 1, a$
  p = p + 2
  n2 = 1
 ELSE
  GOSUB sub1
 END IF
 cant = ASC(c$)
RETURN

