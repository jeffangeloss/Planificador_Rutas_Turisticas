# Datos compartidos

Esta carpeta contiene los CSV compartidos por C++, Haskell y Prolog. Los
archivos usan codificación UTF-8, la coma como separador y el punto como
separador decimal.

## Datos base

### `puntos.csv`

Registra los puntos turísticos y operativos del sistema.

```csv
codigo,nombre,tipo,zona
```

### `conexiones.csv`

Registra las conexiones, distancias y tiempos entre puntos.

```csv
codigo,origen,destino,distancia_km,tiempo_min,estado
```

### `unidades.csv`

Registra los medios de transporte, su capacidad, costo y estado.

```csv
codigo,tipo,capacidad,costo_km,estado
```

### `solicitudes.csv`

Registra las solicitudes de recorrido y la unidad asignada.

```csv
codigo,origen,destino,cantidad_turistas,prioridad,estado,unidad_asignada
```

## Datos de FA01

### `caracteristicas_puntos.csv`

Registra accesibilidad, seguridad, horario y aptitud para niños de cada punto.

```csv
codigo,accesible_movilidad,apto_ninos,seguro_dia,seguro_noche,horario_inicio,horario_fin
```

### `accesibilidad_conexiones.csv`

Registra si cada conexión es accesible para personas con movilidad reducida.

```csv
codigo,accesible_movilidad
```

### `perfiles_turista.csv`

Registra las condiciones y el horario de visita de cada perfil turístico.

```csv
codigo,movilidad_reducida,viaja_con_ninos,hora_visita
```

### `categorias_puntos.csv`

Relaciona cada punto con una o más categorías turísticas.

```csv
codigo,categoria
```

### `intereses_turista.csv`

Relaciona cada perfil turístico con sus intereses.

```csv
codigo,interes
```

## Datos de FA02

### `entradas.csv`

Registra tarifas por tipo de visitante y minutos de visita de los puntos
considerados por FA02.

```csv
punto,precio_general,precio_estudiante,precio_nino,minutos_visita
```

### `visitantes.csv`

Registra el tipo de cada visitante asociado a una solicitud creada por FA02.

```csv
solicitud,tipo_visitante
```

## Datos de FA03

### `cupos.csv`

Registra la capacidad máxima de cada punto por turno.

```csv
punto,turno,capacidad_maxima
```

### `reservas.csv`

Registra las reservas, su estado y su posición en la lista de espera.

```csv
codigo,solicitud,punto,fecha,turno,personas,estado,orden_espera
```

## Fuentes de los datos

- Las ubicaciones y distancias se basan en
  [OpenStreetMap](https://www.openstreetmap.org/) y
  [OSRM](https://router.project-osrm.org/).
- La velocidad vehicular toma como referencia el
  [TomTom Traffic Index de Lima](https://www.tomtom.com/traffic-index/lima-traffic/).
- La velocidad peatonal toma como referencia el estudio de
  [Vega-Huerta et al.](https://doi.org/10.59072/rper.vi75.857).
- Las tarifas se consultaron en las páginas oficiales de
  [Huaca Pucllana](https://museos.cultura.pe/museos/museo-de-sitio-pucllana),
  [Museo Larco](https://www.museolarco.org/),
  [MALI](https://mali.pe/es/visitas/),
  [MAC Lima](https://maclima.pe/) y el
  [Circuito Mágico del Agua](https://www.gob.pe/institucion/emilima/tema/parques-y-centros-de-esparcimiento).
