% 4.3.6 Regla de solicitudes inaccesibles. Responsable: Samantha Rodriguez.
% Devuelve solicitudes pendientes cuyo destino no es alcanzable.
:- module(solicitudes_inaccesibles, [
    ejecutar_pedidos_inaccesibles/0,
    pedidos_inaccesibles/1
]).

:- use_module('LecturaCSV', [solicitud/7]).
:- use_module('Conectividad', [alcanzable/2]).

pedidos_inaccesibles(Pedidos) :-
    findall(
        solicitud(Codigo, Origen, Destino, Cantidad, Prioridad, Estado, Unidad),
        ( solicitud(Codigo, Origen, Destino, Cantidad, Prioridad, Estado, Unidad),
          Estado == pendiente,
          \+ alcanzable(Origen, Destino)
        ),
        Pedidos
    ).

ejecutar_pedidos_inaccesibles :-
    pedidos_inaccesibles(Pedidos),
    (   Pedidos = []
    ->  writeln('[OK] No hay pedidos pendientes sin ruta habilitada.')
    ;   writeln('Pedidos pendientes sin ruta habilitada:'),
        forall(
            member(solicitud(Codigo, Origen, Destino, Cantidad, Prioridad, Estado, Unidad), Pedidos),
            format(' - ~w: ~w -> ~w; ~d turistas; prioridad ~w; estado ~w; unidad ~w~n',
                [Codigo, Origen, Destino, Cantidad, Prioridad, Estado, Unidad])
        )
    ).
