-- 4.2.2 Definición de tipos. Responsable: Jefferson Sanchez.
module Tipos where

data TipoPunto = Agencia | Hotel | Atractivo | Mirador | EspacioPublico deriving (Show, Eq)
data TipoUnidad = GuiaPeatonal | TaxiTuristico | BusTuristico deriving (Show, Eq)
data TipoVisitante = General | Estudiante | Nino deriving (Show, Eq)
data EstadoConexion = Habilitada | Cerrada deriving (Show, Eq)
data EstadoUnidad = Disponible | Ocupada deriving (Show, Eq)
data EstadoSolicitud = Pendiente | Asignada | Entregada deriving (Show, Eq)
data Prioridad = Baja | Media | Alta deriving (Show, Eq, Ord)

data Punto = Punto String String TipoPunto String deriving (Show, Eq)

-- Sinonimos de tipo para nombrar los campos de Conexion, Unidad y Solicitud.
-- Solo cambian el nombre: Codigo sigue siendo String, Kilometros sigue siendo Double, etc.
type Codigo = String
type Kilometros = Double
type Minutos = Int
type Capacidad = Int
type CostoKm = Double
type Cantidad = Int

-- Campos en el orden de conexiones.csv (codigo, origen, destino, distancia_km, tiempo_min, estado).
data Conexion = Conexion Codigo Codigo Codigo Kilometros Minutos EstadoConexion deriving (Show, Eq)
-- Campos en el orden de unidades.csv (codigo, tipo, capacidad, costo_km, estado).
data Unidad = Unidad Codigo TipoUnidad Capacidad CostoKm EstadoUnidad deriving (Show, Eq)
-- Campos en el orden de solicitudes.csv (codigo, origen, destino, cantidad_turistas, prioridad,
-- estado, unidad_asignada).
data Solicitud = Solicitud Codigo Codigo Codigo Cantidad Prioridad EstadoSolicitud (Maybe Codigo) deriving (Show, Eq)
data Visitante = Visitante String TipoVisitante deriving (Show, Eq)
data Entrada = Entrada String Double Double Double Int deriving (Show, Eq)
