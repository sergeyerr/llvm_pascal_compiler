program arrays;
type d2 = array[1..5, 1..10] of integer;
var a : d2;
    b : array[1..15] of string;
begin
	a[1,5] := a[3,1] + 1;
	b[2] := 'shreck';
end