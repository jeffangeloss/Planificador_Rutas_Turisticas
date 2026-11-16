# Módulo C++

Implementación orientada a objetos del sistema turístico. Administra los datos
compartidos, sus estados y el acceso a las funcionalidades de los tres módulos.

## Compilar y ejecutar

Desde la carpeta `cpp/`:

```powershell
cmake -S . -B build
cmake --build build --config Debug
.\build\Debug\turismo_cpp.exe
```

El programa busca los CSV en `../_datos`. Cabal y SWI-Prolog deben estar
disponibles en el `PATH` para abrir Haskell y Prolog desde el menú.

## Funcionalidades

- **4.1.1 Menú principal:** permite acceder a las funcionalidades del sistema.
- **4.1.2 Carga inicial:** carga puntos, conexiones, unidades y solicitudes desde los CSV.
- **4.1.3 Registro de información:** registra datos nuevos con sus validaciones.
- **4.1.4 Gestión de estados:** cambia conexiones y gestiona la asignación de solicitudes.
- **4.1.5 Generación de archivos CSV:** guarda los cambios realizados por el sistema.
- **4.1.6 Consultas básicas:** consulta y filtra la información registrada.
- **4.1.7 Reporte general:** muestra un resumen de solicitudes y unidades.
- **FA01:** registra los datos usados por el recomendador de atractivos de Prolog.
- **FA02:** cotiza planes turísticos calculados por Haskell y registra la alternativa elegida.
- **FA03:** gestiona cupos, reservas y lista de espera por turno.

## Ejecutar pruebas

```powershell
ctest --test-dir build -C Debug --output-on-failure
```

La prueba disponible verifica la gestión y persistencia de cupos y reservas de
FA03.
