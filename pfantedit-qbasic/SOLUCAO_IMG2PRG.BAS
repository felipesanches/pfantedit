REM ESSE CODIGO FOI ESCRITO POR VOLTA DE 2003
REM ACHEI EM OUTUBRO DE 2016 EM UMA FOLHA SULFITE ESCRITA A CANETA

OPEN prg$ FOR BINARY AS #1
OPEN img$ FOR BINARY AS #2
a$ = " ": c$ = " "
pr = &H531D5
p = pr
0
GET #2, 1 + x + 320 * y, c$
CANT = ASC(c$)
n = 1
1
x = x + 1
IF x > 319 THEN x = 0: y = y + 1: pode = 1
IF pode AND (y = 144 OR y = 288 OR y = 432 OR y = 576) THEN GOSUB sub2
IF proxparte THEN proxparte = 0: GOTO 0
IF p - pr = 130 THEN
  a$ = CHR$(128)
  PUT #1, pr, a$
  pr = p
  p = p + 1
  GOTO 0
END IF
GET #2, 1 + x + 320 * y, c$
IF ASC(c$) = CANT THEN
  n = n + 1
  IF n = 127 THEN GOSUB sub1
  GOTO 1
ELSE
  GOSUB sub3
END IF
GOTO 1

sub1:
  a$ = CHR$(ABS(p - pr - 2))
  PUT #1, pr, a$
  a$ = CHR$(257 - n)
  PUT #1, p, a$
  a$ = CHR$(CANT)
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
      pr = &H5A457
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
  RETURN

sub3:
  IF n = 1 THEN
    IF pr = p THEN a$ = CHR$(0): PUT #1, p, a$: p = p + 1
    a$ = CHR$(CANT)
    PUT #1, p, a$
    p = p + 1
  ELSE
    GOSUB sub1
  END IF
  CANT = ASC(c$)
  RETURN

