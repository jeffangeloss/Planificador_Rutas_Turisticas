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

ejecutarOpcion :: String -> [Conexion] -> [Solicitud] -> IO ()
ejecutarOpcion "1" conexiones _ = ejecutarVecinos conexiones
ejecutarOpcion "2" conexiones _ = ejecutarRutas conexiones
ejecutarOpcion "3" conexiones _ = ejecutarCostoRuta conexiones
-- 4.2.6 Reporte de solicitudes. Responsable: Samantha Rodriguez.
ejecutarOpcion "4" _ solicitudes = ejecutarReporteSolicitudes solicitudes
ejecutarOpcion "0" _ _ = putStrLn "Regresando al menu de C++..."
ejecutarOpcion _ _ _ = putStrLn "[ERROR] Opcion no valida."

menuPrincipal :: [Conexion] -> [Solicitud] -> IO ()
menuPrincipal conexiones solicitudes = do
  mostrarMenu
  opcion <- getLine
  ejecutarOpcion opcion conexiones solicitudes

  if opcion == "0"
    then return ()
    else menuPrincipal conexiones solicitudes

informarLectura :: String -> Maybe [a] -> IO ()
informarLectura archivo Nothing = putStrLn ("[ERROR] No se pudo leer " ++ archivo ++ ".")
informarLectura archivo (Just _) = putStrLn ("[OK] " ++ archivo ++ " cargado correctamente.")

-- Informa el resultado de cada archivo y abre el menu solo si los tres se leyeron bien.
iniciarPrograma :: Maybe [Conexion] -> Maybe [Unidad] -> Maybe [Solicitud] -> IO ()
iniciarPrograma resultadoConexiones resultadoUnidades resultadoSolicitudes = do
  informarLectura "conexiones.csv" resultadoConexiones
  informarLectura "unidades.csv" resultadoUnidades
  informarLectura "solicitudes.csv" resultadoSolicitudes
  case (resultadoConexiones, resultadoUnidades, resultadoSolicitudes) of
    (Just conexiones, Just _, Just solicitudes) -> menuPrincipal conexiones solicitudes
    _ -> putStrLn "[ERROR] Corrige los archivos indicados y vuelve a abrir el modulo."

-- 4.2.1 Lectura de conexiones.csv, unidades.csv y solicitudes.csv. Responsable: Jefferson Sanchez.
iniciarMenu :: FilePath -> IO ()
iniciarMenu carpetaDatos = do
  resultadoConexiones <- leerConexiones (carpetaDatos ++ "/conexiones.csv")
  resultadoUnidades <- leerUnidades (carpetaDatos ++ "/unidades.csv")
  resultadoSolicitudes <- leerSolicitudes (carpetaDatos ++ "/solicitudes.csv")
  iniciarPrograma resultadoConexiones resultadoUnidades resultadoSolicitudes

-- 4.2.7 Resultado esperado: integración del programa. Responsable: Samantha Rodriguez.
main :: IO ()
main = do
  argumentos <- getArgs
  case argumentos of
    carpeta : "--fa02" : generales : estudiantes : ninos : presupuesto : tiempo : _ ->
      ejecutarSeleccionAtractivos carpeta generales estudiantes ninos presupuesto tiempo
    carpeta : _ -> iniciarMenu carpeta
    [] -> iniciarMenu "../_datos"
