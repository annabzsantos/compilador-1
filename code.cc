int x;
int y;

int calcula(int x, int y);

begin
    write "Informe o valor de X:";
    read x;

    write "\nInforme o valor de Y:";
    read y;

    write "\nChamando a funcao...\n";
    
    calcula(x, y);

    write "\nDe volta ao programa principal!";
end

int calcula(int x, int y)
begin
    x = x + y;
    write "Resultado de X + Y dentro da funcao: ";
    write x;
end