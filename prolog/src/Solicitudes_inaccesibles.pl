% Grupo 4: ruta turística en Lima. Módulo lógico (sección 4.3).
% 4.3.6 Regla de solicitudes inaccesibles. Responsable: Samantha Rodriguez.
% Enunciado 4.3.6: identificar solicitudes cuyo origen no alcanza su destino.
% 4.3.6 Solicitudes cuyo origen no puede alcanzar su destino.
:- module(solicitudes_inaccesibles, [
    ejecutar_solicitudes_inaccesibles/0,
    solicitudes_inaccesibles/1
]).

:- use_module('LecturaCSV', [solicitud/7]).
:- use_module('Conectividad', [alcanzable/2]).

% Se consideran todos los estados para que el reporte también permita
% detectar inconsistencias históricas en los datos.
solicitudes_inaccesibles(Solicitudes) :-
    findall(
        solicitud(Codigo, Origen, Destino, Cantidad, Prioridad, Estado, Unidad),
        ( solicitud(Codigo, Origen, Destino, Cantidad, Prioridad, Estado, Unidad),
          \+ alcanzable(Origen, Destino)
        ),
        Solicitudes
    ).

ejecutar_solicitudes_inaccesibles :-
    solicitudes_inaccesibles(Solicitudes),
    findall(Codigo, solicitud(Codigo, _, _, _, _, _, _), TodasSolicitudes),
    length(TodasSolicitudes, Total),
    length(Solicitudes, SinRuta),
    ConRuta is Total - SinRuta,
    nl,
    writeln('=== Diagnostico de rutas de solicitudes ==='),
    format('Solicitudes revisadas (todos los estados): ~d~n', [Total]),
    format('Con ruta habilitada: ~d~n', [ConRuta]),
    format('Sin ruta habilitada: ~d~n', [SinRuta]),
    (   Total =:= 0
    ->  writeln('[INFO] No hay solicitudes para revisar.')
    ;   Solicitudes = []
    ->  writeln('[OK] Todas las solicitudes tienen una ruta habilitada.')
    ;   writeln('[AVISO] Revisar estas solicitudes:'),
        forall(
            member(solicitud(Codigo, Origen, Destino, Cantidad, Prioridad, Estado, Unidad), Solicitudes),
            format(' - ~w: ~w -> ~w; ~d turistas; prioridad ~w; estado ~w; unidad ~w~n',
                [Codigo, Origen, Destino, Cantidad, Prioridad, Estado, Unidad])
        )
    ).
