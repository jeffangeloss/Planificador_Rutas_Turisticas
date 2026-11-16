# Módulo C++

Implementación orientada a objetos del dominio turístico. Requiere CMake 3.16
o posterior y un compilador compatible con C++17.

## Compilar y ejecutar

```bash
"C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools\Common7\Tools\VsDevCmd.bat"
cmake -S . -B build
cmake --build build
build\turismo_cpp.exe
```

En Windows con un generador multiconfiguración, el ejecutable puede quedar en
`build/Debug/turismo_cpp.exe`.

Ejecuta el programa desde cpp/ o desde la raíz del repositorio para que
encuentre _datos/. En Consultas básicas, presiona Enter para listar todos los
puntos, conexiones, solicitudes o unidades. También puedes filtrar conexiones
por punto, solicitudes por estado y unidades con sus solicitudes asignadas.
El reporte general muestra la unidad con más pedidos asignados y todos los
empates. El menú también inicia Haskell o Prolog y permite registrar datos FA01. SWI-Prolog y Cabal deben estar disponibles
en `PATH` para abrir esos módulos desde el menú.

Para una recomendación FA01, registra primero el perfil y los datos de los
puntos y conexiones en la opción 7. Luego abre Prolog desde la opción 6 y
selecciona FA01 en su menú.

El lector CSV de C++ usa separación sencilla por comas. Los campos no deben
contener comas internas ni comillas escapadas. Las categorías e intereses se
guardan en filas separadas.

La opción 9 abre FA03, las reservas de cupos por turno con lista de espera.
Primero se define el cupo de un punto en un turno y luego se reserva una fecha y
un turno para una solicitud cuyo destino es ese punto. La reserva queda
confirmada si cabe o entra a la lista de espera. Al cancelar una reserva
confirmada, el sistema confirma en orden de llegada cada reserva en espera que
cabe, y la que no cabe conserva su lugar. Se prefiere esta política al orden de
llegada estricto porque ese dejaría cupos sin usar cada vez que el primer grupo
no cabe; el grupo grande no pierde su lugar y entra en cuanto se liberan cupos
suficientes.

Con los datos de ejemplo, el turno de las 10:00 del 2026-11-15 en `P004` tiene
6 de 9 cupos ocupados y `R003`, un grupo de 6, primero en espera. Reservar `S004`
para 4 personas en ese turno deja la reserva en espera en la posición 2. Si
después se cancela `R002`, de 2 personas, quedan 5 cupos libres: `R003` no cabe
y conserva la posición 1, y la reserva de `S004` pasa a confirmada.

## Ejecutar pruebas

```bash
ctest --test-dir build -C Debug --output-on-failure
```

`prueba_fa03` comprueba la agenda de cupos de FA03: la confirmación, la lista de
espera, la promoción en cascada y la lectura y escritura de `cupos.csv` y
`reservas.csv`.

Si se usa un generador de configuración única (por ejemplo, Ninja o Make), el
argumento `-C Debug` puede omitirse.

