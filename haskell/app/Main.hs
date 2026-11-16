module Main (main) where

import CostoRuta (ejecutarCostoRuta)
import LecturaCSV (leerConexiones, leerSolicitudes, leerUnidades)
import Rutas (ejecutarRutas)
import Tipos (Conexion)
import Tipos (Solicitud)
import Tipos (Unidad)
import Vecinos (ejecutarVecinos)
import ReporteSolicitudes (ejecutarReporteSolicitudes)
import SeleccionAtractivos (ejecutarSeleccionAtractivos)
import System.Environment (getArgs)

-- muestra las opciones disponibles en haskell.
mostrarMenu :: IO ()
mostrarMenu = do
  putStrLn ""
  putStrLn "=== Modulo Haskell ==="
  putStrLn "1. [4.2.3] Funcion de vecinos"
  putStrLn "2. [4.2.4] Rutas posibles"
  putStrLn "3. [4.2.5] Costo de una ruta"
  putStrLn "4. [4.2.6] Reporte de solicitudes"
  putStrLn "0. Volver al menu de C++"
  putStrLn "Opcion:"

-- envia cada opcion a la funcion que la implementa.
ejecutarOpcion :: String -> [Conexion] -> [Unidad] -> [Solicitud] -> IO ()
-- 4.2.3 Función de vecinos. Responsable: Melissa Ruíz.
ejecutarOpcion "1" conexiones _ _ = ejecutarVecinos conexiones
-- 4.2.4 Rutas posibles. Responsable: Melissa Ruíz.
ejecutarOpcion "2" conexiones _ _ = ejecutarRutas conexiones
-- 4.2.5 Costo de una ruta. Responsable: Melissa Ruíz.
ejecutarOpcion "3" conexiones _ _ = ejecutarCostoRuta conexiones
-- 4.2.6 Reporte de solicitudes. Responsable: Samantha Rodriguez.
ejecutarOpcion "4" _ unidades solicitudes = ejecutarReporteSolicitudes unidades solicitudes
ejecutarOpcion "0" _ _ _ = putStrLn "Regresando al menu de C++..."
ejecutarOpcion _ _ _ _ = putStrLn "[ERROR] Opcion no valida."

-- repite el menu hasta que se elija volver a c++.
menuPrincipal :: [Conexion] -> [Unidad] -> [Solicitud] -> IO ()
menuPrincipal conexiones unidades solicitudes = do
  mostrarMenu
  opcion <- getLine
  ejecutarOpcion opcion conexiones unidades solicitudes

  if opcion == "0"
    then return ()
    else menuPrincipal conexiones unidades solicitudes

-- muestra si se pudo leer cada archivo csv.
informarLectura :: String -> Maybe [a] -> IO ()
informarLectura archivo Nothing = putStrLn ("[ERROR] No se pudo leer " ++ archivo ++ ".")
informarLectura archivo (Just _) = putStrLn ("[OK] " ++ archivo ++ " cargado correctamente.")

-- Informa el resultado de cada archivo y abre el menu solo si los tres se leyeron bien.
-- abre el menu solo cuando se cargaron los tres archivos.
iniciarPrograma :: Maybe [Conexion] -> Maybe [Unidad] -> Maybe [Solicitud] -> IO ()
iniciarPrograma resultadoConexiones resultadoUnidades resultadoSolicitudes = do
  informarLectura "conexiones.csv" resultadoConexiones
  informarLectura "unidades.csv" resultadoUnidades
  informarLectura "solicitudes.csv" resultadoSolicitudes
  case (resultadoConexiones, resultadoUnidades, resultadoSolicitudes) of
    (Just conexiones, Just unidades, Just solicitudes) -> menuPrincipal conexiones unidades solicitudes
    _ -> putStrLn "[ERROR] Corrige los archivos indicados y vuelve a abrir el modulo."

-- 4.2.1 Lectura de archivos CSV. Responsable: Jefferson Sanchez.
-- lee conexiones, unidades y solicitudes desde la carpeta indicada.
iniciarMenu :: FilePath -> IO ()
iniciarMenu carpetaDatos = do
  resultadoConexiones <- leerConexiones (carpetaDatos ++ "/conexiones.csv")
  resultadoUnidades <- leerUnidades (carpetaDatos ++ "/unidades.csv")
  resultadoSolicitudes <- leerSolicitudes (carpetaDatos ++ "/solicitudes.csv")
  iniciarPrograma resultadoConexiones resultadoUnidades resultadoSolicitudes

-- 4.2.7 Programa principal y resultado esperado. Responsable: Samantha Rodriguez.
-- elige el menu turistico o la integracion existente con fa02.
main :: IO ()
main = do
  argumentos <- getArgs
  case argumentos of
    -- FA02 Selección de atractivos dentro de un presupuesto. Responsable: Melissa Ruíz.
    carpeta : "--fa02" : generales : estudiantes : ninos : presupuesto : tiempo : _ ->
      ejecutarSeleccionAtractivos carpeta generales estudiantes ninos presupuesto tiempo
    carpeta : _ -> iniciarMenu carpeta
    [] -> iniciarMenu "../_datos"
