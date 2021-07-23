program arrays;
type d2 = array[1..5, 1..10] of integer;
var a : d2;
    b : array[1..15, d2] of string;
    c : array[1..15] of d2;
begin
	a[1,1,1] := a[3,1] + 1;
	a['a',1] := 1;
	a[1,1] := 'a';
	a[1,1] := 1;
	c := a;
	c[1] := a;
end