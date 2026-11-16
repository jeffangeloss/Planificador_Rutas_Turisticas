-- 4.2.1 Lectura de archivos CSV. Responsable: Jefferson Sanchez.
-- Lee conexiones, unidades y solicitudes desde los CSV compartidos.
module LecturaCSV where

import Data.Char (isSpace)
import System.IO.Error (catchIOError)
import Text.Read (readMaybe)
import Tipos

-- Agrega un caracter al primer campo, o abre un campo nuevo si el caracter es una coma.
agregarCaracter :: Char -> [String] -> [String]
agregarCaracter ',' campos = "" : campos
agregarCaracter caracter (campo : campos) = (caracter : campo) : campos
-- Este caso no ocurre desde separarCampos, que empieza con [""], y se deja para cubrir la lista vacia.
agregarCaracter caracter [] = [[caracter]]

-- Separa una fila en sus campos recorriendola de derecha a izquierda con foldr.
-- Como avanza desde el final, cada caracter se pone al inicio y los campos quedan en orden.
separarCampos :: String -> [String]
separarCampos fila = foldr agregarCaracter [""] fila

-- Quita el retorno de carro que dejan los archivos guardados en Windows.
quitarRetorno :: String -> String
quitarRetorno texto = filter (\caracter -> caracter /= '\r') texto

-- Devuelve las filas de datos sin la cabecera ni las lineas en blanco.
leerFilas :: FilePath -> IO [String]
leerFilas ruta = do
  contenido <- readFile ruta
  let filas = map quitarRetorno (drop 1 (lines contenido))
  return (filter (\fila -> not (all isSpace fila)) filas)

parsearTipoPunto :: String -> Maybe TipoPunto
parsearTipoPunto "agencia" = Just Agencia
parsearTipoPunto "hotel" = Just Hotel
parsearTipoPunto "atractivo" = Just Atractivo
parsearTipoPunto "mirador" = Just Mirador
parsearTipoPunto "espacio_publico" = Just EspacioPublico
parsearTipoPunto _ = Nothing

parsearTipoUnidad :: String -> Maybe TipoUnidad
parsearTipoUnidad "guia_peatonal" = Just GuiaPeatonal
parsearTipoUnidad "taxi_turistico" = Just TaxiTuristico
parsearTipoUnidad "bus_turistico" = Just BusTuristico
parsearTipoUnidad _ = Nothing

parsearTipoVisitante :: String -> Maybe TipoVisitante
parsearTipoVisitante "general" = Just General
parsearTipoVisitante "estudiante" = Just Estudiante
parsearTipoVisitante "nino" = Just Nino
parsearTipoVisitante _ = Nothing

parsearEstadoConexion :: String -> Maybe EstadoConexion
parsearEstadoConexion "habilitada" = Just Habilitada
parsearEstadoConexion "cerrada" = Just Cerrada
parsearEstadoConexion _ = Nothing

parsearEstadoUnidad :: String -> Maybe EstadoUnidad
parsearEstadoUnidad "disponible" = Just Disponible
parsearEstadoUnidad "ocupada" = Just Ocupada
parsearEstadoUnidad _ = Nothing

parsearEstadoSolicitud :: String -> Maybe EstadoSolicitud
parsearEstadoSolicitud "pendiente" = Just Pendiente
parsearEstadoSolicitud "asignada" = Just Asignada
parsearEstadoSolicitud "entregada" = Just Entregada
parsearEstadoSolicitud _ = Nothing

parsearPrioridad :: String -> Maybe Prioridad
parsearPrioridad "baja" = Just Baja
parsearPrioridad "media" = Just Media
parsearPrioridad "alta" = Just Alta
parsearPrioridad _ = Nothing

parsearPunto :: String -> Maybe Punto
parsearPunto fila =
  case separarCampos fila of
    [codigo, nombre, tipoTexto, zona] -> do
      tipo <- parsearTipoPunto tipoTexto
      pure (Punto codigo nombre tipo zona)
    _ -> Nothing

-- parsearConexion, parsearUnidad y parsearSolicitud exigen el numero exacto de campos.
-- En el do de Maybe basta un Nothing (de readMaybe, de un parsear de texto o del if) para descartar la fila.
parsearConexion :: String -> Maybe Conexion
parsearConexion fila =
  case separarCampos fila of
    [codigo, origen, destino, distancia, tiempo, estadoTexto] -> do
      km <- readMaybe distancia
      minutos <- readMaybe tiempo
      estado <- parsearEstadoConexion estadoTexto
      if km >= 0 && minutos >= 0
        then Just (Conexion codigo origen destino km minutos estado)
        else Nothing
    _ -> Nothing

parsearUnidad :: String -> Maybe Unidad
parsearUnidad fila =
  case separarCampos fila of
    [codigo, tipoTexto, capacidad, costo, estadoTexto] -> do
      tipo <- parsearTipoUnidad tipoTexto
      cupo <- readMaybe capacidad
      costoKm <- readMaybe costo
      estado <- parsearEstadoUnidad estadoTexto
      if cupo > 0 && costoKm >= 0
        then Just (Unidad codigo tipo cupo costoKm estado)
        else Nothing
    _ -> Nothing

parsearSolicitud :: String -> Maybe Solicitud
parsearSolicitud fila =
  case separarCampos fila of
    [codigo, origen, destino, cantidad, prioridadTexto, estadoTexto, unidad] -> do
      turistas <- readMaybe cantidad
      prioridad <- parsearPrioridad prioridadTexto
      estado <- parsearEstadoSolicitud estadoTexto
      -- La columna unidad_asignada vacia significa que la solicitud no tiene unidad.
      let unidadAsignada = unidadAsignadaDe unidad
      if turistas > 0
        then Just (Solicitud codigo origen destino turistas prioridad estado unidadAsignada)
        else Nothing
    _ -> Nothing

-- Convierte la columna unidad_asignada: el texto vacio es Nothing y un codigo es Just codigo.
-- Ejemplos: unidadAsignadaDe "" = Nothing; unidadAsignadaDe "U01" = Just "U01".
unidadAsignadaDe :: String -> Maybe String
unidadAsignadaDe "" = Nothing
unidadAsignadaDe codigo = Just codigo

parsearVisitante :: String -> Maybe Visitante
parsearVisitante fila =
  case separarCampos fila of
    [solicitud, tipoTexto] -> do
      tipo <- parsearTipoVisitante tipoTexto
      pure (Visitante solicitud tipo)
    _ -> Nothing

parsearEntrada :: String -> Maybe Entrada
parsearEntrada fila =
  case separarCampos fila of
    [codigo, precioGeneral, precioEstudiante, precioNino, minutos] -> do
      general <- readMaybe precioGeneral
      estudiante <- readMaybe precioEstudiante
      nino <- readMaybe precioNino
      duracion <- readMaybe minutos
      if general >= 0 && estudiante >= 0 && nino >= 0 && duracion > 0
        then pure (Entrada codigo general estudiante nino duracion)
        else Nothing
    _ -> Nothing

-- Convierte una por una las filas con recursion: la lista vacia da Just [], y si una sola
-- fila es invalida el resultado es Nothing, sin seguir con las demas.
-- Ejemplos: parsearFilas parsearPrioridad ["alta", "baja"] = Just [Alta, Baja];
-- parsearFilas parsearPrioridad ["alta", "x"] = Nothing.
parsearFilas :: (String -> Maybe a) -> [String] -> Maybe [a]
parsearFilas _ [] = Just []
parsearFilas parsear (fila : filas) = do
  valor <- parsear fila
  resto <- parsearFilas parsear filas
  Just (valor : resto)

-- Convierte todas las filas con parsearFilas, que da Nothing si una sola fila es invalida.
-- length filas `seq` obliga a leer aqui el archivo completo, porque readFile es perezoso,
-- y asi catchIOError en leerCSV atrapa un error de lectura en vez de que aparezca despues.
leerYParsear :: (String -> Maybe a) -> FilePath -> IO (Maybe [a])
leerYParsear parsear ruta = do
  filas <- leerFilas ruta
  length filas `seq` return (parsearFilas parsear filas)

sinArchivo :: IOError -> IO (Maybe [a])
sinArchivo _ = return Nothing

-- Si el archivo no existe o no se puede leer, devuelve Nothing en vez de detener el programa.
leerCSV :: (String -> Maybe a) -> FilePath -> IO (Maybe [a])
leerCSV parsear ruta = catchIOError (leerYParsear parsear ruta) sinArchivo

leerPuntos :: FilePath -> IO (Maybe [Punto])
leerPuntos = leerCSV parsearPunto

leerConexiones :: FilePath -> IO (Maybe [Conexion])
leerConexiones ruta = leerCSV parsearConexion ruta

leerUnidades :: FilePath -> IO (Maybe [Unidad])
leerUnidades ruta = leerCSV parsearUnidad ruta

leerSolicitudes :: FilePath -> IO (Maybe [Solicitud])
leerSolicitudes ruta = leerCSV parsearSolicitud ruta

leerVisitantes :: FilePath -> IO (Maybe [Visitante])
leerVisitantes = leerCSV parsearVisitante

leerEntradas :: FilePath -> IO (Maybe [Entrada])
leerEntradas = leerCSV parsearEntrada
