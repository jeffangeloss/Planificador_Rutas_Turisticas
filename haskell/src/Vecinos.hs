-- 4.2.3 Función de vecinos. Responsable: Melissa Ruíz.
module Vecinos (vecinos, ejecutarVecinos) where

import Tipos (Conexion (..), EstadoConexion (Habilitada))

-- Obtiene los destinos de las conexiones que salen del punto.
vecinosDesdeOrigen :: String -> [Conexion] -> [String]
vecinosDesdeOrigen punto conexiones =
  [ destino
  | Conexion _ origen destino _ _ Habilitada <- conexiones
  , origen == punto
  ]

-- Obtiene los orígenes de las conexiones que llegan al punto.
vecinosDesdeDestino :: String -> [Conexion] -> [String]
vecinosDesdeDestino punto conexiones =
  [ origen
  | Conexion _ origen destino _ _ Habilitada <- conexiones
  , destino == punto
  ]

-- Considera ambos sentidos de cada conexión habilitada.
vecinos :: String -> [Conexion] -> [String]
vecinos punto conexiones =
  vecinosDesdeOrigen punto conexiones
    ++ vecinosDesdeDestino punto conexiones

-- Solicita un punto y muestra sus vecinos habilitados.
ejecutarVecinos :: [Conexion] -> IO ()
ejecutarVecinos conexiones = do
  putStrLn "Codigo del punto:"
  codigoPunto <- getLine

  let puntosVecinos = vecinos codigoPunto conexiones

  if null puntosVecinos
    then putStrLn "[>] El punto no tiene vecinos habilitados."
    else do
      putStrLn "Puntos vecinos:"
      print puntosVecinos
