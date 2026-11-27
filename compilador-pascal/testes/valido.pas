program Calculadora;
{ Programa valido: usa todos os recursos da linguagem }
var
  a, b, resultado: integer;
  media: real;
  positivo: boolean;

function soma(x, y: integer): integer;
begin
  soma := x + y
end;

function fatorial(n: integer): integer;
var
  i, acumulado: integer;
begin
  acumulado := 1;
  i := 1;
  while i <= n do
  begin
    acumulado := acumulado * i;
    i := i + 1
  end;
  fatorial := acumulado
end;

procedure mostrar(valor: integer; mensagem: boolean);
var
  dobro: integer;

  (* procedimento aninhado: so existe dentro de mostrar *)
  procedure linha;
  begin
    writeln('----------')
  end;

begin
  linha;
  dobro := valor * 2;
  if mensagem then
    writeln('Valor: ', valor, ' Dobro: ', dobro)
  else
    writeln(valor);
  linha
end;

begin
  a := 10;
  b := 3;
  resultado := soma(a, b);
  media := (a + b) / 2;
  positivo := resultado > 0;
  mostrar(resultado, positivo);
  mostrar(fatorial(5), true);
  if (a mod b = 1) and not (a < b) then
    writeln('Media: ', media)
end.
