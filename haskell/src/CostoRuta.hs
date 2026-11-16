-- 4.2.5 Costo de una ruta. Responsable: Melissa Ruíz.
module CostoRuta (costoRuta, ejecutarCostoRuta) where

import Rutas (rutasPosibles)
import Tipos (Conexion (..), EstadoConexion (Habilitada))

type ResultadoRuta = (Double, Int, Bool)

-- Comprueba si una conexión corresponde al tramo en cualquier sentido.
esMismoTramo :: String -> String -> String -> String -> Bool
esMismoTramo puntoA puntoB origen destino =
  (puntoA == origen && puntoB == destino)
    || (puntoA == destino && puntoB == origen)

-- Busca la distancia y el tiempo de un tramo habilitado.
buscarTramo :: String -> String -> [Conexion] -> ResultadoRuta
buscarTramo _ _ [] = (0, 0, False)
buscarTramo puntoA puntoB (Conexion _ origen destino distancia tiempo Habilitada : restantes)
  | esMismoTramo puntoA puntoB origen destino =
      (distancia, tiempo, True)
  | otherwise = buscarTramo puntoA puntoB restantes
buscarTramo puntoA puntoB (_ : restantes) = buscarTramo puntoA puntoB restantes

-- Une los resultados de dos partes de una ruta.
sumarCostos :: ResultadoRuta -> ResultadoRuta -> ResultadoRuta
sumarCostos (distancia, tiempo, tramoValido) (distanciaRestante, tiempoRestante, rutaValida) =
  ( distancia + distanciaRestante
  , tiempo + tiempoRestante
  , tramoValido && rutaValida
  )

-- Recorre los puntos consecutivos y acumula sus tramos.
sumarTramos :: [String] -> [Conexion] -> ResultadoRuta
sumarTramos [_] _ = (0, 0, True)
sumarTramos (puntoA : puntoB : restantes) conexiones =
  sumarCostos
    (buscarTramo puntoA puntoB conexiones)
    (sumarTramos (puntoB : restantes) conexiones)
sumarTramos _ _ = (0, 0, False)

-- Calcula distancia, tiempo y validez de una ruta completa.
costoRuta :: [String] -> [Conexion] -> ResultadoRuta
costoRuta puntos conexiones
  | length puntos < 2 = (0, 0, False)
  | otherwise = sumarTramos puntos conexiones

-- Muestra el resultado calculado para la ruta.
mostrarCosto :: ResultadoRuta -> IO ()
mostrarCosto (_, _, False) =
  putStrLn "[>] La ruta contiene una conexion inexistente o cerrada."
mostrarCosto (distancia, tiempo, True) = do
  putStrLn ("Distancia total: " ++ show distancia ++ " km")
  putStrLn ("Tiempo total: " ++ show tiempo ++ " minutos")

-- Enumera las rutas para que el usuario pueda elegir una.
mostrarOpcionesRuta :: [[String]] -> Int -> IO ()
mostrarOpcionesRuta [] _ = return ()
mostrarOpcionesRuta (ruta : restantes) numero = do
  putStrLn (show numero ++ ". " ++ show ruta)
  mostrarOpcionesRuta restantes (numero + 1)

-- Devuelve la ruta que corresponde al número ingresado.
seleccionarRuta :: String -> [[String]] -> Int -> [String]
seleccionarRuta _ [] _ = []
seleccionarRuta opcion (ruta : restantes) numero
  | opcion == show numero = ruta
  | otherwise = seleccionarRuta opcion restantes (numero + 1)

-- Calcula la ruta elegida o informa que el número no existe.
calcularRutaSeleccionada :: [String] -> [Conexion] -> IO ()
calcularRutaSeleccionada [] _ =
  putStrLn "[>] Numero de ruta no valido."
calcularRutaSeleccionada ruta conexiones = do
  putStrLn ("Ruta seleccionada: " ++ show ruta)
  mostrarCosto (costoRuta ruta conexiones)

-- Solicita origen, destino y la ruta que se desea calcular.
ejecutarCostoRuta :: [Conexion] -> IO ()
ejecutarCostoRuta conexiones = do
  putStrLn "Codigo de origen:"
  origen <- getLine
  putStrLn "Codigo de destino:"
  destino <- getLine

  let rutas = rutasPosibles origen destino conexiones

  if null rutas
    then putStrLn "[>] No existen rutas habilitadas entre los puntos ingresados."
    else do
      putStrLn "Rutas disponibles:"
      mostrarOpcionesRuta rutas 1
      putStrLn "Numero de ruta:"
      opcion <- getLine
      calcularRutaSeleccionada (seleccionarRuta opcion rutas 1) conexiones
