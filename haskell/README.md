# Módulo Haskell

Implementación funcional del sistema turístico. Calcula vecinos, rutas, costos,
reportes y alternativas de recorrido a partir de los CSV compartidos.

## Compilar y ejecutar

Desde la carpeta `haskell/`:

```powershell
cabal build
cabal run turismo-haskell
```

Desde la raíz del repositorio:

```powershell
cabal run --project-dir=haskell turismo-haskell -- _datos
```

El menú se abre cuando `conexiones.csv`, `unidades.csv` y `solicitudes.csv` se
cargan correctamente.

## Funcionalidades

- **4.2.1 Lectura de archivos CSV:** convierte los datos compartidos en valores de Haskell.
- **4.2.2 Definición de tipos:** representa los elementos y estados del sistema.
- **4.2.3 Función de vecinos:** obtiene los puntos conectados mediante conexiones habilitadas.
- **4.2.4 Rutas posibles:** encuentra rutas habilitadas sin repetir puntos.
- **4.2.5 Costo de una ruta:** calcula la distancia y el tiempo de una ruta seleccionada.
- **4.2.6 Reporte de solicitudes:** resume solicitudes, turistas y carga por unidad.
- **4.2.7 Programa principal:** carga los datos y presenta el menú del módulo.
- **FA02:** genera hasta tres planes turísticos según visitantes, presupuesto y tiempo.
