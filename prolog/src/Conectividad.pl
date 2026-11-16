% 4.3.2 regla de conexion directa. responsable: jefferson sanchez.
:- module(conectividad, [
    ejecutar_conexion/0,
    ejecutar_alcanzabilidad/0,
    conectado/3,
    alcanzable/2,
    ruta_evitando/4,
    ejecutar_ruta_evitando/0
]).

:- use_module('LecturaCSV.pl', [conexion/6]).
:- use_module('entradasMenu', [leer_codigo/2]).

% revisa una conexion habilitada en el sentido indicado.
% sentido de ida: la conexion esta registrada como origen -> destino.
conectado(Origen, Destino, Distancia) :-
    conexion(_, Origen, Destino, Distancia, _, habilitada).

% sentido de vuelta: la conexion esta registrada como destino -> origen.
conectado(Origen, Destino, Distancia) :-
    conexion(_, Destino, Origen, Distancia, _, habilitada).

% 4.3.3 regla de alcanzabilidad. responsable: jefferson sanchez.
% inicia la busqueda de un camino entre dos puntos.
% la lista de visitados empieza con el origen para no volver a el ni
% entrar en ciclos.
alcanzable(Origen, Destino) :-
    buscar_camino(Origen, Destino, [Origen]).

% 4.3.4 regla de ruta evitando un punto. responsable: samantha rodriguez.
% busca una ruta habilitada sin pasar por el punto excluido.
ruta_evitando(Origen, Destino, PuntoEvitado, Ruta) :-
    Origen \= PuntoEvitado,
    Destino \= PuntoEvitado,
    buscar_ruta(Origen, Destino, PuntoEvitado, [Origen], RutaInvertida),
    reverse(RutaInvertida, Ruta).

% caso base y paso recursivo de la ruta que evita el punto indicado.
buscar_ruta(Actual, Destino, _, Visitados, [Destino|Visitados]) :-
    conectado(Actual, Destino, _),
    \+ member(Destino, Visitados).
buscar_ruta(Actual, Destino, PuntoEvitado, Visitados, Ruta) :-
    conectado(Actual, Vecino, _),
    Vecino \= PuntoEvitado,
    \+ member(Vecino, Visitados),
    buscar_ruta(Vecino, Destino, PuntoEvitado, [Vecino|Visitados], Ruta).

% caso base y paso recursivo para comprobar alcanzabilidad.
buscar_camino(Actual, Destino, Visitados) :-
    conectado(Actual, Destino, _),
    \+ member(Destino, Visitados).

% caso recursivo. avanza a un vecino y continua la busqueda.
buscar_camino(Actual, Destino, Visitados) :-
    conectado(Actual, Vecino, _),
    \+ member(Vecino, Visitados),
    buscar_camino(Vecino, Destino, [Vecino | Visitados]).

% member comprueba si el punto ya aparece en la lista de visitados.

% pide los puntos y muestra si existe una conexion directa.
ejecutar_conexion :-
    leer_codigo('Codigo de origen: ', Origen),
    leer_codigo('Codigo de destino: ', Destino),
    (   conectado(Origen, Destino, Distancia)
    ->  format('[SI] Existe conexion directa entre ~w y ~w (~w km).~n',
            [Origen, Destino, Distancia])
    ;   format('[NO] No existe conexion directa entre ~w y ~w.~n',
            [Origen, Destino])
    ).

% pide los puntos y muestra si existe un camino.
ejecutar_alcanzabilidad :-
    leer_codigo('Codigo de origen: ', Origen),
    leer_codigo('Codigo de destino: ', Destino),
    (   alcanzable(Origen, Destino)
    ->  format('[SI] ~w es alcanzable desde ~w.~n', [Destino, Origen])
    ;   format('[NO] ~w no es alcanzable desde ~w.~n', [Destino, Origen])
    ).

% solicita los codigos y muestra la ruta o informa que no existe.
ejecutar_ruta_evitando :-
    leer_codigo('Codigo de origen: ', Origen),
    leer_codigo('Codigo de destino: ', Destino),
    leer_codigo('Codigo del punto que se debe evitar: ', PuntoEvitado),
    (   ruta_evitando(Origen, Destino, PuntoEvitado, Ruta)
    ->  format('[SI] Ruta evitando ~w: ~w~n', [PuntoEvitado, Ruta])
    ;   format('[NO] No hay ruta entre ~w y ~w evitando ~w.~n',
            [Origen, Destino, PuntoEvitado])
    ).
