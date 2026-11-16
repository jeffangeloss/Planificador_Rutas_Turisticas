# Módulo Haskell

Implementación funcional del dominio turístico. Requiere GHC y Cabal.

## Compilar y ejecutar

```bash
cabal build
cabal run turismo-haskell
```

Desde la raíz del repositorio, se puede indicar la carpeta compartida:

```bash
cabal run --project-dir=haskell turismo-haskell -- _datos
```

Al iniciar, el menú carga `conexiones.csv`, `unidades.csv` y `solicitudes.csv`,
informa con `[OK]` o `[ERROR]` el resultado de cada archivo y solo se abre si
los tres se leyeron bien. La opción 4.2.6 muestra los totales por estado, la cantidad de turistas,
la carga asignada de cada unidad y los pedidos pendientes ordenados por
prioridad (alta, media y baja). La carga suma solo solicitudes asignadas a
esa unidad, incluidas las unidades con cero turistas. El reporte usa filter,
foldr y concatMap. La opción 4.2.7 no forma parte del menú.

## Ejecutar pruebas

```bash
cabal test
```

