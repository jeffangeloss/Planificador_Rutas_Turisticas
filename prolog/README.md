# Módulo Prolog

Implementación lógica del sistema turístico. Convierte los CSV compartidos en
hechos y permite consultar conectividad, atención y recomendaciones.

## Ejecutar el programa

Desde la raíz del repositorio:

```powershell
swipl prolog/app/main.pl -- _datos
```

El programa carga los datos y muestra el menú de funcionalidades de Prolog.
SWI-Prolog debe estar disponible en el `PATH`; su instalación se explica en
[`../INSTALACION.md`](../INSTALACION.md).

## Funcionalidades

- **4.3.1 Lectura de CSV:** crea la base de conocimiento a partir de los archivos compartidos.
- **4.3.2 Conexión directa:** comprueba si dos puntos tienen una conexión habilitada.
- **4.3.3 Alcanzabilidad:** determina si un destino puede alcanzarse desde un origen.
- **4.3.4 Ruta evitando un punto:** busca una ruta que no atraviese el punto indicado.
- **4.3.5 Atención de solicitudes:** comprueba si una unidad puede atender una solicitud.
- **4.3.6 Solicitudes inaccesibles:** lista solicitudes pendientes sin una ruta habilitada.
- **FA01:** recomienda atractivos para un perfil y explica el resultado.

## Realizar consultas independientes

Desde la raíz del repositorio, iniciar SWI-Prolog dentro de la carpeta del
módulo:

```powershell
cd prolog
swipl
```

En el prompt de Prolog, cargar los módulos y los CSV:

```prolog
[src/LecturaCSV, src/Conectividad, src/Atencion_solicitudes, src/Solicitudes_inaccesibles].
cargar_datos('../_datos').
```

Después se pueden realizar consultas como las siguientes:

```prolog
conectado('P001', 'P002', Distancia).
alcanzable('P001', 'P004').
ruta_evitando('P001', 'P004', 'P003', Ruta).
puede_atender('U001', 'S001').
pedidos_inaccesibles(Pedidos).
```

Para consultar FA01, cargar sus datos por separado:

```prolog
[src/Recomendador].
cargar_recomendador('../_datos').
recomendar('T001', Resultados).
```

Si una consulta tiene varios resultados, presionar `;` para mostrar el
siguiente. Para salir, ejecutar `halt.`.
