# Datos compartidos

Esta carpeta es la única fuente de datos compartida por los módulos de C++, Haskell y Prolog. Los archivos usan codificación UTF-8, la coma como separador y el punto como separador decimal.

El lector sencillo de C++ separa los campos por comas; por ello, los campos no deben incluir comas internas ni comillas escapadas. Las categorías e intereses de FA01 se registran en archivos de relación, una por fila.

El módulo de C++ será el único que escriba o actualice estos archivos. Inicialmente, Haskell y Prolog los usarán en modo lectura. Cada módulo debe recibir la ruta de esta carpeta mediante un parámetro o una configuración; los CSV no deben duplicarse dentro de los módulos.

## `puntos.csv`

Registra los puntos que pueden ser origen o destino de un recorrido.

```csv
codigo,nombre,tipo,zona
```

- `codigo`: identificador único del punto.
- `nombre`: nombre descriptivo.
- `tipo`: `agencia`, `hotel`, `atractivo`, `mirador` o `espacio_publico`.
- `zona`: zona de Lima a la que pertenece el punto.
- `P000` representa la agencia y se utiliza únicamente como origen operativo.
- `espacio_publico` identifica parques, plazas y otros lugares de visita libre
  que no corresponden a un hotel, un atractivo con entrada o un mirador.
- `P000` (Agencia de Turismo) y `P001` (Hotel Miraflores) son puntos ficticios
  en Miraflores, a menos de 1,2 km del Parque Kennedy. Los otros doce puntos son
  lugares reales de Lima, y sus ubicaciones salen de OpenStreetMap.
- El Mirador de Barranco (`P004`) corresponde al Mirador Catalina Recavarren,
  junto al Puente de los Suspiros (`P013`), y el Parque del Amor (`P011`) figura
  en OpenStreetMap con su nombre oficial, Parque Alberto Andrade Carmona.

## `conexiones.csv`

Registra las conexiones entre dos puntos.

```csv
codigo,origen,destino,distancia_km,tiempo_min,estado
```

- `codigo`: identificador único de la conexión.
- `origen` y `destino`: códigos existentes en `puntos.csv`; deben ser diferentes.
- `distancia_km`: distancia mayor que cero, expresada en kilómetros.
- `tiempo_min`: tiempo mayor que cero, expresado en minutos.
- `estado`: `habilitada` o `cerrada`.

Cada conexión se almacena una sola vez. Haskell y Prolog deben interpretarla en ambos sentidos cuando esté `habilitada`.

Entre lugares reales, `distancia_km` es la ruta en auto más corta de los dos
sentidos según Open Source Routing Machine (OSRM) sobre OpenStreetMap, con
redondeo a 0,1 km y datos de la consulta del 15 de noviembre de 2026. Las
conexiones `C001`, `C002`, `C003` y `C006` tocan un punto ficticio y conservan
una distancia aproximada. El valor de `tiempo_min` corresponde a un traslado en
taxi o en bus turístico durante un día hábil y se obtiene como
`distancia_km` ÷ 15 km/h × 60, redondeado al minuto. Los 15 km/h redondean la
velocidad del centro de Lima de lunes a viernes entre las 7 y las 20 h, que es
de 14,9 km/h al calcularla con la serie semanal del TomTom Traffic Index 2025.
Esta velocidad queda entre los 13,9 km/h de la hora punta de la mañana y los
17,2 km/h del promedio anual que publica el mismo índice.

La guía peatonal de FA02 no usa `tiempo_min`, porque este valor corresponde a un
vehículo. FA02 calcula su traslado como `distancia_km` ÷ 3,6 km/h × 60,
redondeado al minuto, con la velocidad de caminata que Vega-Huerta et al.
(2026) usan para rutas peatonales en el Parque de las Leyendas, y la aplica
como aproximación a la distancia en auto de OSRM.

## `unidades.csv`

Registra las unidades disponibles para atender solicitudes.

```csv
codigo,tipo,capacidad,costo_km,estado
```

- `codigo`: identificador único de la unidad.
- `tipo`: `guia_peatonal`, `taxi_turistico` o `bus_turistico`.
- `capacidad`: cantidad máxima de turistas; debe ser mayor que cero.
- `costo_km`: costo por kilómetro; debe ser mayor que cero.
- `estado`: `disponible` u `ocupada`.

En los datos de prueba, cada unidad ocupada atiende una sola solicitud asignada,
y quedan disponibles una guía peatonal (`U001`), un bus turístico (`U003`) y dos
taxis turísticos (`U006` y `U009`).

## `solicitudes.csv`

Registra las solicitudes de recorrido. Sus códigos deben comenzar con `S`, por ejemplo `S001`.

```csv
codigo,origen,destino,cantidad_turistas,prioridad,estado,unidad_asignada
```

- `codigo`: identificador único de la solicitud.
- `origen` y `destino`: códigos existentes en `puntos.csv`; deben ser diferentes.
- `cantidad_turistas`: cantidad mayor que cero.
- `prioridad`: `alta`, `media` o `baja`.
- `estado`: `pendiente`, `asignada` o `entregada`.
- `unidad_asignada`: código de la unidad que atiende o atendió la solicitud.

Los datos de prueba incluyen solicitudes pendientes de las tres prioridades, que
son `S001` (alta), `S007` y `S011` (media) y `S004` (baja). El destino de `S011`,
el Puente de los Suspiros (`P013`), solo se alcanza por la conexión cerrada
`C022`, así que esta solicitud no tiene ruta habilitada desde su origen.

## Datos adicionales de FA02

### `entradas.csv`

Registra el costo de ingreso y los minutos de visita de cada punto turístico.
No incluye una fila para la agencia `P000`.

```csv
punto,precio_general,precio_estudiante,precio_nino,minutos_visita
```

- `punto`: código existente en `puntos.csv`.
- `precio_general`, `precio_estudiante` y `precio_nino`: tarifas no negativas.
- `minutos_visita`: duración estimada mayor que cero.
- Los puntos de tipo `atractivo` pueden tener una tarifa de entrada.
- Los hoteles, miradores y espacios públicos mantienen sus tarifas en cero,
  pero sus minutos sí forman parte del tiempo total del recorrido.

Las tarifas son las de entrada diurna para peruanos y residentes, consultadas el
15 de noviembre de 2026 en la web de cada lugar o de la entidad que lo
administra, y `precio_nino` corresponde a un niño de 5 a 7 años. El archivo
asigna a estudiantes y niños la tarifa reducida de S/ 7,50 de la Huaca
Pucllana, y la de S/ 1,00 del Museo Larco vale para niños de 3 a 8 años que van
con su familia. Los minutos de visita son estimaciones del grupo.

### `visitantes.csv`

Registra una fila por integrante de una solicitud aceptada en FA02.

```csv
solicitud,tipo_visitante
```

- `solicitud`: código existente en `solicitudes.csv`.
- `tipo_visitante`: `general`, `estudiante` o `nino`.
- La cantidad de filas de una solicitud debe coincidir con su cantidad de
  turistas.

## Datos adicionales de FA01

El recomendador explicable usa cinco archivos adicionales. C++ los mantiene y
Prolog los lee junto con los CSV base. Las categorías y los intereses se
guardan una por fila para que cada relación sea fácil de consultar.

### `caracteristicas_puntos.csv`

```csv
codigo,accesible_movilidad,apto_ninos,seguro_dia,seguro_noche,horario_inicio,horario_fin
```

- `codigo`: código existente en `puntos.csv`.
- `accesible_movilidad`, `apto_ninos`, `seguro_dia` y `seguro_noche`: valores `si` o `no`.
- `horario_inicio` y `horario_fin`: horas en formato `HH:MM`.

### `categorias_puntos.csv`

```csv
codigo,categoria
```

Cada fila asocia un punto existente con una categoría. Un mismo punto puede
aparecer varias veces si tiene más de una categoría.

### `accesibilidad_conexiones.csv`

```csv
codigo,accesible_movilidad
```

El código debe existir en `conexiones.csv`; la accesibilidad se evalúa solo
para conexiones habilitadas.

### `perfiles_turista.csv`

```csv
codigo,movilidad_reducida,viaja_con_ninos,hora_visita
```

- `codigo`: identificador único del perfil.
- `movilidad_reducida` y `viaja_con_ninos`: valores `si` o `no`.
- `hora_visita`: hora local de visita en formato `HH:MM`.

### `intereses_turista.csv`

```csv
codigo,interes
```

Cada fila relaciona un perfil existente con uno de sus intereses. Un perfil
puede aparecer en varias filas.

Una recomendación se descarta si incumple la accesibilidad requerida, la
seguridad del horario, la atención del lugar o la condición de visita con
niños. Las categorías coincidentes aumentan el puntaje y se explican en la
salida. Si el perfil requiere accesibilidad, el punto debe tener al menos una
ruta de conexiones habilitadas y marcadas como accesibles desde algún hotel.

Una solicitud `pendiente` no tiene unidad asignada. Para pasarla a `asignada`, la unidad debe estar `disponible` y tener capacidad suficiente; después de la asignación, la unidad pasa a `ocupada`. Al marcar la solicitud como `entregada`, la unidad vuelve a estar `disponible`, pero la solicitud conserva su código en `unidad_asignada` para los reportes históricos.

## Datos adicionales de FA03

Las reservas de cupos por turno con lista de espera usan dos archivos que solo
C++ lee y escribe. Cada cambio se guarda de inmediato con un archivo temporal,
de modo que una falla no deja el CSV a medio escribir.

### `cupos.csv`

```csv
punto,turno,capacidad_maxima
```

- `punto`: código existente en `puntos.csv` de tipo `atractivo` o `mirador`.
- `turno`: hora de inicio en formato `HH:MM`.
- `capacidad_maxima`: personas que caben en el turno; debe ser mayor que cero.
  Vale para todas las fechas.
- El par `punto` y `turno` no se repite.

### `reservas.csv`

```csv
codigo,solicitud,punto,fecha,turno,personas,estado,orden_espera
```

- `codigo`: identificador único que comienza con `R`, por ejemplo `R001`.
- `solicitud`: código existente en `solicitudes.csv`. El `punto` de la reserva
  es el destino de esa solicitud.
- `fecha`: día de la visita en formato `AAAA-MM-DD`.
- `turno`: turno que tiene cupo para ese punto en `cupos.csv`.
- `personas`: entre 1 y la `cantidad_turistas` de la solicitud, sin superar el
  cupo del turno.
- `estado`: `confirmada`, `en_espera` o `cancelada`.
- `orden_espera`: posición en la lista de espera de su punto, fecha y turno. Vale
  `0` si la reserva está confirmada o cancelada.

Una reserva nueva queda confirmada si cabe en los cupos libres de su turno y,
si no, entra al final de la lista de espera. Una solicitud tiene a lo sumo una
reserva activa por fecha y turno. Al cancelar una reserva confirmada o al subir
un cupo, el sistema recorre la lista de espera en orden de llegada y confirma
cada reserva que cabe en los cupos libres; la que no cabe conserva su lugar, y
una sola liberación puede confirmar varias reservas. Un cupo no puede bajar de
las personas ya confirmadas en ese turno ni del grupo más grande en espera. Al
cargar, la lista de espera se reconstruye desde `reservas.csv` ordenada por
`orden_espera`. Si una fila no cumple estas reglas, FA03 la informa y queda en
solo lectura hasta corregir el archivo, para no perderla al guardar. Las reservas no
cambian el estado de las solicitudes ni de las unidades.

## Reglas generales

- Los códigos no se repiten dentro de cada archivo.
- Las referencias a puntos y unidades deben corresponder a registros existentes.
- Los valores deben respetar los dominios y las restricciones indicadas en este documento.
- Las distancias, los tiempos y las tarifas de entrada son aproximaciones con las
  fuentes que indica este documento. Los costos por km, las capacidades, los
  cupos, los horarios y los minutos de visita son supuestos de simulación que el
  grupo puede ajustar.

## Fuentes de los datos

- Distancias, OSRM sobre OpenStreetMap, https://router.project-osrm.org/, con
  datos del mapa © colaboradores de OpenStreetMap y licencia Open Database
  License (ODbL).
- Velocidad del tráfico, TomTom Traffic Index 2025, página de Lima,
  https://www.tomtom.com/traffic-index/lima-traffic/.
- Velocidad a pie de FA02, Vega-Huerta et al. (2026), Revista Portuguesa de
  Estudos Regionais, 75, 117-137, https://doi.org/10.59072/rper.vi75.857.
- Huaca Pucllana, Ministerio de Cultura,
  https://museos.cultura.pe/museos/museo-de-sitio-pucllana.
- Museo Larco, tarifario del museo,
  https://www.museolarco.org/wp-content/uploads/2025/10/Tarifario-Museo-Larco.pdf.
- Museo de Arte de Lima, https://mali.pe/es/visitas/.
- Museo de Arte Contemporáneo de Lima,
  https://maclima.pe/wordpress/compra-tu-entrada/.
- Circuito Mágico del Agua, nota de Emilima,
  https://www.gob.pe/institucion/emilima/noticias/1088295-verano-2025-disfruta-y-refrescate-en-las-fuentes-del-circuito-magico-del-agua.

La consulta de todas las páginas es del 15 de noviembre de 2026.
