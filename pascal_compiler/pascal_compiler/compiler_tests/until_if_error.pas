program itatikmasu;
var a : real;
begin
	repeat
		if (a < 2)
		then
			begin
			a := a + 1;
			a := a * 2;
		else
			a := a / 3;
	until a > 3;
end