% fa01 recomendador explicable de atractivos por perfil. responsable: samantha rodriguez.
% recomienda atractivos segun el perfil y explica cada resultado.
:- module(recomendador, [
    cargar_recomendador/1,
    recomendar/2,
    ejecutar_recomendador/0
]).

:- use_module(library(csv)).
:- use_module('LecturaCSV', [punto/4, conexion/6]).
:- use_module('entradasMenu', [leer_codigo/2]).

% dynamic permite agregar hechos durante la ejecucion al leer los csv.
:- dynamic caracteristica_punto/7.
:- dynamic accesibilidad_conexion/2.
:- dynamic perfil_turista/4.
:- dynamic categoria_punto/2.
:- dynamic interes_perfil/2.

% limpia y carga los datos que necesita fa01.
cargar_recomendador(CarpetaDatos) :-
    validar_archivos(CarpetaDatos,
        ['caracteristicas_puntos.csv', 'accesibilidad_conexiones.csv',
         'perfiles_turista.csv', 'categorias_puntos.csv', 'intereses_turista.csv']),
    retractall(caracteristica_punto(_, _, _, _, _, _, _)),
    retractall(accesibilidad_conexion(_, _)),
    retractall(perfil_turista(_, _, _, _)),
    retractall(categoria_punto(_, _)),
    retractall(interes_perfil(_, _)),
    cargar_caracteristicas(CarpetaDatos),
    cargar_accesibilidad(CarpetaDatos),
    cargar_perfiles(CarpetaDatos),
    cargar_categorias(CarpetaDatos),
    cargar_intereses(CarpetaDatos).

% comprueba que existan los archivos csv requeridos.
validar_archivos(_, []).
validar_archivos(CarpetaDatos, [Nombre|Resto]) :-
    directory_file_path(CarpetaDatos, Nombre, Ruta),
    (   exists_file(Ruta)
    ->  validar_archivos(CarpetaDatos, Resto)
    ;   format('[ERROR] Falta el archivo ~w.~n', [Ruta]),
        fail
    ).

% lee las filas de un csv sin incluir su encabezado.
leer_csv(CarpetaDatos, Nombre, Filas) :-
    directory_file_path(CarpetaDatos, Nombre, Ruta),
    csv_read_file(Ruta, [_Encabezado|Filas],
        [functor(fila), strip(true), convert(true)]).

% carga las caracteristicas registradas para los puntos.
cargar_caracteristicas(CarpetaDatos) :-
    leer_csv(CarpetaDatos, 'caracteristicas_puntos.csv', Filas),
    guardar_caracteristicas(Filas).

% guarda cada fila y continua con las filas restantes.
guardar_caracteristicas([]).
guardar_caracteristicas([Fila|Resto]) :-
    guardar_caracteristica(Fila),
    guardar_caracteristicas(Resto).

% guarda una caracteristica como hecho de prolog.
% assertz agrega al final un hecho leido del csv.
guardar_caracteristica(fila(Codigo, Movilidad, Ninos, Dia, Noche, Inicio, Fin)) :-
    assertz(caracteristica_punto(Codigo, Movilidad, Ninos, Dia, Noche, Inicio, Fin)).

% carga la accesibilidad de las conexiones.
cargar_accesibilidad(CarpetaDatos) :-
    leer_csv(CarpetaDatos, 'accesibilidad_conexiones.csv', Filas),
    guardar_accesibilidades(Filas).

% guarda cada fila y continua con las filas restantes.
guardar_accesibilidades([]).
guardar_accesibilidades([Fila|Resto]) :-
    guardar_accesibilidad(Fila),
    guardar_accesibilidades(Resto).

% guarda la accesibilidad como hecho de prolog.
guardar_accesibilidad(fila(Codigo, Accesible)) :-
    assertz(accesibilidad_conexion(Codigo, Accesible)).

% carga los perfiles de turistas.
cargar_perfiles(CarpetaDatos) :-
    leer_csv(CarpetaDatos, 'perfiles_turista.csv', Filas),
    guardar_perfiles(Filas).

% guarda cada fila y continua con las filas restantes.
guardar_perfiles([]).
guardar_perfiles([Fila|Resto]) :-
    guardar_perfil(Fila),
    guardar_perfiles(Resto).

% guarda un perfil como hecho de prolog.
guardar_perfil(fila(Codigo, Movilidad, Ninos, Hora)) :-
    assertz(perfil_turista(Codigo, Movilidad, Ninos, Hora)).

% carga las categorias asociadas a los puntos.
cargar_categorias(CarpetaDatos) :-
    leer_csv(CarpetaDatos, 'categorias_puntos.csv', Filas),
    guardar_categorias(Filas).

% guarda cada fila y continua con las filas restantes.
guardar_categorias([]).
guardar_categorias([Fila|Resto]) :-
    guardar_categoria(Fila),
    guardar_categorias(Resto).

% guarda una categoria como hecho de prolog.
guardar_categoria(fila(Codigo, Categoria)) :-
    assertz(categoria_punto(Codigo, Categoria)).

% carga los intereses asociados a los perfiles.
cargar_intereses(CarpetaDatos) :-
    leer_csv(CarpetaDatos, 'intereses_turista.csv', Filas),
    guardar_intereses(Filas).

% guarda cada fila y continua con las filas restantes.
guardar_intereses([]).
guardar_intereses([Fila|Resto]) :-
    guardar_interes(Fila),
    guardar_intereses(Resto).

% guarda un interes como hecho de prolog.
guardar_interes(fila(Codigo, Interes)) :-
    assertz(interes_perfil(Codigo, Interes)).

% reune recomendaciones y descartes para un perfil.
recomendar(CodigoPerfil, Resultados) :-
    perfil_turista(CodigoPerfil, Movilidad, Ninos, Hora),
    findall(
        Resultado,
        ( punto(Codigo, Nombre, Tipo, _),
          member(Tipo, [atractivo, mirador, hotel]),
          resultado_punto(CodigoPerfil, Codigo, Nombre, Movilidad, Ninos, Hora, Resultado)
        ),
        Resultados
    ).

% obtiene los datos del punto y solicita su evaluacion.
resultado_punto(CodigoPerfil, Codigo, Nombre, Movilidad, Ninos, Hora, Resultado) :-
    caracteristica_punto(Codigo, Accesible, AptoNinos, SeguroDia, SeguroNoche, Inicio, Fin),
    evaluar_punto(CodigoPerfil, Codigo, Nombre, Accesible, AptoNinos,
        SeguroDia, SeguroNoche, Inicio, Fin, Movilidad, Ninos, Hora, Resultado).

% calcula motivos favorables y razones de descarte.
evaluar_punto(CodigoPerfil, Codigo, Nombre, Accesible, AptoNinos,
    SeguroDia, SeguroNoche, Inicio, Fin, Movilidad, Ninos, Hora, Resultado) :-
    findall(Motivo,
        motivo_favorable(CodigoPerfil, Codigo, Accesible, AptoNinos,
            SeguroDia, SeguroNoche, Inicio, Fin, Movilidad, Ninos, Hora, Motivo),
        Motivos),
    findall(Razon,
        motivo_descarte(Codigo, Accesible, AptoNinos, SeguroDia,
            SeguroNoche, Inicio, Fin, Movilidad, Ninos, Hora, Razon),
        Descartes),
    (   Descartes = []
    ->  length(Motivos, Puntaje),
        Resultado = recomendacion(Codigo, Nombre, Puntaje, Motivos)
    ;   Resultado = descarte(Codigo, Nombre, Descartes)
    ).

% reglas que explican por que un punto coincide con el perfil.
motivo_favorable(Perfil, Codigo, _, _, _, _, _, _, _, _, _,
    'coincide con un interes del perfil') :-
    interes_perfil(Perfil, Interes),
    categoria_punto(Codigo, Interes).
motivo_favorable(_, _, 'si', _, _, _, _, _, 'si', _, _,
    'el lugar es accesible para movilidad reducida').
motivo_favorable(_, _, _, 'si', _, _, _, _, _, 'si', _,
    'es adecuado para visitar con ninos').
motivo_favorable(_, _, _, _, _, _, Inicio, Fin, _, _, Hora,
    'el horario esta dentro del horario de atencion') :-
    hora_en_rango(Hora, Inicio, Fin).
motivo_favorable(_, _, _, _, 'si', _, _, _, _, _, Hora,
    'es seguro durante el dia') :-
    hora_del_dia(Hora, dia).
motivo_favorable(_, _, _, _, _, 'si', _, _, _, _, Hora,
    'es seguro durante la noche') :-
    hora_del_dia(Hora, noche).
motivo_favorable(CodigoPerfil, Codigo, _, _, _, _, _, _, 'si', _, _,
    'existe una ruta accesible desde un hotel') :-
    perfil_turista(CodigoPerfil, 'si', _, _),
    accesible_desde_hotel(Codigo), !.

% reglas que explican por que se descarta un punto.
motivo_descarte(_, 'no', _, _, _, _, _, 'si', _, _,
    'no cumple el requisito de movilidad reducida').
motivo_descarte(_, _, 'no', _, _, _, _, _, 'si', _,
    'no esta marcado como adecuado para ninos').
motivo_descarte(_, _, _, 'no', _, _, _, _, _, Hora,
    'no es seguro durante el horario indicado') :-
    hora_del_dia(Hora, dia).
motivo_descarte(_, _, _, _, 'no', _, _, _, _, Hora,
    'no es seguro durante el horario indicado') :-
    hora_del_dia(Hora, noche).
motivo_descarte(_, _, _, _, _, Inicio, Fin, _, _, Hora,
    'esta fuera del horario de atencion') :-
    \+ hora_en_rango(Hora, Inicio, Fin).
motivo_descarte(Codigo, _, _, _, _, _, _, 'si', _, _,
    'no existe una ruta accesible desde un hotel') :-
    \+ accesible_desde_hotel(Codigo).

% busca una ruta accesible desde algun hotel.
accesible_desde_hotel(Destino) :-
    punto(Origen, _, hotel, _),
    ruta_accesible(Origen, Destino, [Origen]).

% caso base y pasos recursivos de la ruta accesible.
ruta_accesible(Actual, Destino, _) :- Actual = Destino.
ruta_accesible(Actual, Destino, Visitados) :-
    conexion(Codigo, Actual, Vecino, _, _, habilitada),
    accesibilidad_conexion(Codigo, 'si'),
    \+ member(Vecino, Visitados),
    ruta_accesible(Vecino, Destino, [Vecino|Visitados]).
ruta_accesible(Actual, Destino, Visitados) :-
    conexion(Codigo, Vecino, Actual, _, _, habilitada),
    accesibilidad_conexion(Codigo, 'si'),
    \+ member(Vecino, Visitados),
    ruta_accesible(Vecino, Destino, [Vecino|Visitados]).

% comprueba si una hora esta dentro del horario permitido.
hora_en_rango(Hora, Inicio, Fin) :- Hora @>= Inicio, Hora @=< Fin.
% clasifica la hora como dia o noche.
hora_del_dia(Hora, dia) :- Hora @>= '06:00', Hora @< '18:00'.
hora_del_dia(Hora, noche) :- Hora @< '06:00' ; Hora @>= '18:00'.

% lista los perfiles que se pueden consultar.
mostrar_perfiles_disponibles :-
    writeln('Perfiles registrados (codigo | movilidad | ninos | hora | intereses):'),
    (   perfil_turista(_, _, _, _)
    ->  forall(
            perfil_turista(Codigo, Movilidad, Ninos, Hora),
            ( findall(Interes, interes_perfil(Codigo, Interes), Intereses),
              format(' - ~w | movilidad: ~w | viaja con ninos: ~w | hora: ~w | intereses: ~w~n',
                  [Codigo, Movilidad, Ninos, Hora, Intereses])
            )
        )
    ;   writeln('   No hay perfiles registrados.')
    ).

% pide el codigo de un perfil registrado.
leer_codigo_perfil(Codigo) :-
    (   perfil_turista(Ejemplo, _, _, _)
    ->  format(atom(Prompt), 'Codigo de perfil (ejemplo: ~w): ', [Ejemplo])
    ;   Prompt = 'Codigo de perfil (revisa la lista): '
    ),
    leer_codigo(Prompt, Codigo).

% guia la consulta del recomendador desde la terminal.
ejecutar_recomendador :-
    nl,
    writeln('=== Recomendador de atractivos FA01 ==='),
    mostrar_perfiles_disponibles,
    (   perfil_turista(_, _, _, _)
    ->  leer_codigo_perfil(CodigoPerfil),
        (   recomendar(CodigoPerfil, Resultados)
        ->  mostrar_resultados(CodigoPerfil, Resultados)
        ;   format('[ERROR] No se encontro el perfil ~w.~n', [CodigoPerfil])
        )
    ;   writeln('[INFO] Registra un perfil desde el menu FA01 de C++.')
    ).

% separa y presenta los recomendados y descartados.
mostrar_resultados(CodigoPerfil, Resultados) :-
    findall(
        recomendacion(Codigo, Nombre, Puntaje, Motivos),
        member(recomendacion(Codigo, Nombre, Puntaje, Motivos), Resultados),
        Recomendaciones
    ),
    findall(
        descarte(Codigo, Nombre, Razones),
        member(descarte(Codigo, Nombre, Razones), Resultados),
        Descartes
    ),
    length(Recomendaciones, CantidadRecomendados),
    length(Descartes, CantidadDescartados),
    nl,
    format('=== Resultados para el perfil ~w ===~n', [CodigoPerfil]),
    format('Lugares recomendados: ~d~n', [CantidadRecomendados]),
    format('Lugares que no cumplen el perfil: ~d~n', [CantidadDescartados]),
    mostrar_seccion_recomendaciones(Recomendaciones),
    mostrar_seccion_descartes(Descartes).

% muestra la lista de recomendaciones o informa si esta vacia.
mostrar_seccion_recomendaciones([]) :-
    nl,
    writeln('No hay lugares que cumplan el perfil.').
mostrar_seccion_recomendaciones(Resultados) :-
    Resultados \= [],
    nl,
    writeln('--- Lugares recomendados ---'),
    forall(member(Resultado, Resultados), mostrar_resultado(Resultado)).

% muestra los descartes o informa si no hay ninguno.
mostrar_seccion_descartes([]) :-
    nl,
    writeln('No se descartaron atractivos.').
mostrar_seccion_descartes(Resultados) :-
    Resultados \= [],
    nl,
    writeln('--- Lugares descartados y motivos ---'),
    forall(member(Resultado, Resultados), mostrar_resultado(Resultado)).

% presenta un lugar recomendado o descartado.
mostrar_resultado(recomendacion(Codigo, Nombre, Puntaje, Motivos)) :-
    format('[RECOMENDADO] ~w - ~w (coincidencias: ~d)~n',
        [Codigo, Nombre, Puntaje]),
    mostrar_motivos(Motivos, '+').
mostrar_resultado(descarte(Codigo, Nombre, Razones)) :-
    format('[DESCARTADO] ~w - ~w~n', [Codigo, Nombre]),
    mostrar_motivos(Razones, '-').

% imprime uno por uno los motivos del resultado.
mostrar_motivos([], _).
mostrar_motivos([Motivo|Resto], Simbolo) :-
    format('  ~w ~w~n', [Simbolo, Motivo]),
    mostrar_motivos(Resto, Simbolo).
