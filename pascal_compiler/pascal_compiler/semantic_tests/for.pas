program shrek; 
var
    en: (red,green,blue,white);
    c,a : char;
begin
    for en := red to blue do
        en := blue;
    for c := 'a' to 'z' do
        a := c;
end