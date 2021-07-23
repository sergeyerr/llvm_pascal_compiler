program posneg;
uses crt;
var 
 no : integer;
begin
  Write('Enger a number:');
  readln(no);

  if (no >= 0) then
   writeln('kek')
  else
    if (no < 0) then
     writeln('jmek')
    else
      if (no <> 0) then
      writeln('well');

  readln;
 end.