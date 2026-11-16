% 4.3.5 Regla de atención de solicitudes. Responsable: Melissa Ruíz.
:- module(atencion_solicitudes, [ejecutar_atencion/0, puede_atender/2]).

:- use_module('LecturaCSV', [unidad/5, solicitud/7]).
:- use_module('entradasMenu', [leer_codigo/2]).

% Obtiene la capacidad de una unidad disponible.
unidad_disponible(CodigoUnidad, Capacidad) :-
    unidad(CodigoUnidad, _, Capacidad, _, disponible).

% Obtiene la cantidad de turistas de una solicitud pendiente.
solicitud_pendiente(CodigoSolicitud, CantidadTuristas) :-
    solicitud(CodigoSolicitud, _, _, CantidadTuristas, _, pendiente, _).

% Comprueba disponibilidad y capacidad suficiente para la solicitud.
puede_atender(CodigoUnidad, CodigoSolicitud) :-
    unidad_disponible(CodigoUnidad, Capacidad),
    solicitud_pendiente(CodigoSolicitud, CantidadTuristas),
    Capacidad >= CantidadTuristas.

% Solicita ambos códigos y presenta el resultado de la regla.
ejecutar_atencion :-
    leer_codigo('Codigo de unidad: ', CodigoUnidad),
    leer_codigo('Codigo de solicitud: ', CodigoSolicitud),
    (   puede_atender(CodigoUnidad, CodigoSolicitud)
    ->  format('[SI] La unidad ~w puede atender la solicitud ~w.~n',
            [CodigoUnidad, CodigoSolicitud])
    ;   format('[NO] La unidad ~w no puede atender la solicitud ~w.~n',
            [CodigoUnidad, CodigoSolicitud])
    ).
