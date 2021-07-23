program itatikmasu;
var a : integer;
begin
	repeat
		if (a < 2)
		then
			begin
			a := a + 1;
			a := a * 2;
			end
		else
			a := a / 3
	until a > 3;
end