:- use_module('../src/LecturaCSV', [cargar_datos/2]).
:- use_module('../src/entradasMenu', [leer_entrada/2]).
:- use_module('../src/Conectividad', [
    ejecutar_conexion/0,
    ejecutar_alcanzabilidad/0,
    ejecutar_ruta_evitando/0
]).
:- use_module('../src/Atencion_solicitudes', [ejecutar_atencion/0]).
:- use_module('../src/Solicitudes_inaccesibles', [ejecutar_pedidos_inaccesibles/0]).
:- use_module('../src/Recomendador', [cargar_recomendador/1, ejecutar_recomendador/0]).

% lee la carpeta de datos indicada al iniciar prolog.
main :-
    current_prolog_flag(argv, Argumentos),
    (   Argumentos = [CarpetaDatos|_]
    ->  ejecutar_carga(CarpetaDatos)
    ;   writeln('[ERROR] Uso: swipl prolog/app/main.pl -- _datos'),
        halt(1)
    ).

% carga los csv y luego inicia el menu.
ejecutar_carga(CarpetaDatos) :-
    % format inserta los valores en el mensaje y ~n representa un salto de linea.
    format('Cargando archivos desde: ~w~n~n', [CarpetaDatos]),
    % catch permite mostrar los errores de lectura sin dejar el proceso a medias.
    % 4.3.1 lectura de csv y base de conocimiento. responsable: melissa ruiz.
    catch(
        ( cargar_datos(CarpetaDatos, Resumen),
          mostrar_resumen(Resumen),
          cargar_fa01(CarpetaDatos),
          menu_principal
        ),
        Error,
        manejar_error(Error)
    ).

% fa01 se carga aparte para que un csv suyo ausente o mal formado no cierre
% el modulo. en ese caso el menu abre igual y la opcion 6 solo avisa, porque
% los datos de fa01 pueden haber quedado a medias.
:- dynamic fa01_cargado/0.

% intenta cargar fa01 sin impedir que abra el menu general.
cargar_fa01(CarpetaDatos) :-
    retractall(fa01_cargado),
    catch(cargar_recomendador(CarpetaDatos), Error,
          ( message_to_string(Error, Detalle),
            format('[AVISO] Detalle: ~w~n', [Detalle]),
            fail )),
    !,
    assertz(fa01_cargado).
cargar_fa01(_) :-
    writeln('[AVISO] No se cargaron los datos de FA01; la opcion 6 no estara disponible.').

% muestra cuantos registros se cargaron de cada csv.
mostrar_resumen(resumen(Puntos, Conexiones, Unidades, Solicitudes)) :-
    format('[OK] puntos.csv: ~d registros~n', [Puntos]),
    format('[OK] conexiones.csv: ~d registros~n', [Conexiones]),
    format('[OK] unidades.csv: ~d registros~n', [Unidades]),
    format('[OK] solicitudes.csv: ~d registros~n', [Solicitudes]),
    Total is Puntos + Conexiones + Unidades + Solicitudes,
    format('~nCarga completada: 4 archivos, ~d registros en total.~n', [Total]).

% muestra las funcionalidades prolog y procesa una opcion por vez.
% lee una opcion y vuelve al menu despues de ejecutarla.
menu_principal :-
    mostrar_menu,
    leer_entrada('Opcion: ', Opcion),
    ejecutar_opcion(Opcion).

% presenta las funcionalidades disponibles en prolog.
mostrar_menu :-
    nl,
    writeln('=== Modulo Prolog ==='),
    writeln('1. [4.3.2] Regla de conexion directa'),
    writeln('2. [4.3.3] Regla de alcanzabilidad'),
    writeln('3. [4.3.4] Regla de ruta evitando un punto'),
    writeln('4. [4.3.5] Regla de atencion de solicitudes'),
    writeln('5. [4.3.6] Pedidos pendientes sin ruta'),
    writeln('6. [FA01] Obtener recomendaciones segun perfil'),
    writeln('0. Volver al menu de C++').

% 4.3.2 regla de conexion directa. responsable: jefferson sanchez.
ejecutar_opcion("1") :-
    ejecutar_conexion,
    menu_principal.
% 4.3.3 regla de alcanzabilidad. responsable: jefferson sanchez.
ejecutar_opcion("2") :-
    ejecutar_alcanzabilidad,
    menu_principal.
% 4.3.4 regla de ruta evitando un punto. responsable: samantha rodriguez.
% ejecuta la regla de ruta que evita un punto.
ejecutar_opcion("3") :-
    ejecutar_ruta_evitando,
    menu_principal.
% 4.3.5 regla de atencion de solicitudes. responsable: melissa ruiz.
ejecutar_opcion("4") :-
    ejecutar_atencion,
    menu_principal.
% 4.3.6 regla de solicitudes inaccesibles. responsable: samantha rodriguez.
% ejecuta la regla de pedidos pendientes sin ruta.
ejecutar_opcion("5") :-
    ejecutar_pedidos_inaccesibles,
    menu_principal.
% fa01 recomendador explicable de atractivos por perfil. responsable: samantha rodriguez.
% abre el recomendador fa01 si sus datos se cargaron.
ejecutar_opcion("6") :-
    (   fa01_cargado
    ->  ejecutar_recomendador
    ;   writeln('[AVISO] FA01 no esta disponible porque sus datos no se cargaron.')
    ),
    menu_principal.
% termina el menu de prolog y regresa al modulo de c++.
ejecutar_opcion("0") :-
    writeln('Regresando al menu de C++...').
ejecutar_opcion(_) :-
    writeln('[ERROR] Opcion no valida.'),
    menu_principal.

% muestra el detalle si falla la carga de archivos.
manejar_error(Error) :-
    message_to_string(Error, Detalle),
    format(
        user_error,
        '[ERROR] No se pudieron cargar los archivos CSV.~nDetalle: ~w~n',
        [Detalle]
    ),
    halt(1).

:- initialization(main, main).
