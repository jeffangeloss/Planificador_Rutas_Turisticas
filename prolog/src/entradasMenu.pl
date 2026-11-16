:- module(entrada, [
    leer_entrada/2,
    leer_codigo/2
]).

leer_entrada(Mensaje, Texto) :-
    write(Mensaje),
    flush_output,
    read_line_to_string(user_input, Entrada),
    (   Entrada == end_of_file
    ->  nl,
        halt
    ;   normalize_space(string(Texto), Entrada)
    ).

leer_codigo(Mensaje, Codigo) :-
    leer_entrada(Mensaje, Entrada),
    string_upper(Entrada, EntradaMayusculas),
    atom_string(Codigo, EntradaMayusculas).
