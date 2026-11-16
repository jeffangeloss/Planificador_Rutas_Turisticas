-- Grupo 4: ruta turística en Lima. Módulo funcional (sección 4.2).
-- 4.2.6 Reporte de solicitudes. Responsable: Samantha Rodriguez.
module ReporteSolicitudes (reporteSolicitudes, ejecutarReporteSolicitudes) where

import Tipos (EstadoSolicitud (..), Prioridad (..), Solicitud (..), Unidad (..))

reporteSolicitudes :: [Unidad] -> [Solicitud] -> String
reporteSolicitudes unidades solicitudes =
  unirLineas
    ( encabezado
        ++ lineasCargaUnidades unidades solicitudes
        ++ lineasPendientes solicitudes
    )
  where
    pendientes = filter (esEstado Pendiente) solicitudes
    asignadas = filter (esEstado Asignada) solicitudes
    entregadas = filter (esEstado Entregada) solicitudes
    totalTuristas = foldr sumarTuristas 0 solicitudes
    encabezado =
      [ "=== Reporte de solicitudes ==="
      , "Total: " ++ show (length solicitudes)
      , "Pendientes: " ++ show (length pendientes)
      , "Asignadas: " ++ show (length asignadas)
      , "Entregadas: " ++ show (length entregadas)
      , "Turistas solicitantes: " ++ show totalTuristas
      , "Carga asignada por unidad:"
      ]

esEstado :: EstadoSolicitud -> Solicitud -> Bool
esEstado esperado (Solicitud _ _ _ _ _ estado _) = esperado == estado

sumarTuristas :: Solicitud -> Int -> Int
sumarTuristas (Solicitud _ _ _ cantidad _ _ _) acumulado =
  cantidad + acumulado

lineasCargaUnidades :: [Unidad] -> [Solicitud] -> [String]
lineasCargaUnidades unidades solicitudes =
  concatMap mostrarCarga unidades
  where
    mostrarCarga (Unidad codigo tipo _ _ _) =
      [ " - " ++ codigo ++ " (" ++ show tipo ++ "): "
          ++ show (cargaUnidad codigo solicitudes)
          ++ " turistas asignados"
      ]

cargaUnidad :: String -> [Solicitud] -> Int
cargaUnidad codigo solicitudes =
  foldr (sumarSolicitudAsignada codigo) 0 solicitudes

sumarSolicitudAsignada :: String -> Solicitud -> Int -> Int
sumarSolicitudAsignada codigo
    (Solicitud _ _ _ cantidad _ estado unidad) acumulado =
  case unidad of
    Just codigoUnidad
      | estado == Asignada && codigoUnidad == codigo -> cantidad + acumulado
    _ -> acumulado

lineasPendientes :: [Solicitud] -> [String]
lineasPendientes solicitudes =
  [ "Solicitudes pendientes por prioridad (alta, media, baja):" ]
    ++ concatMap mostrarPorPrioridad prioridades
  where
    solicitudesPendientes = filter (esEstado Pendiente) solicitudes
    prioridades = [Alta, Media, Baja]
    mostrarPorPrioridad prioridad =
      concatMap mostrarUna
        (filter (tienePrioridad prioridad) solicitudesPendientes)
    mostrarUna solicitud = [" - " ++ mostrarSolicitud solicitud]

tienePrioridad :: Prioridad -> Solicitud -> Bool
tienePrioridad prioridad (Solicitud _ _ _ _ actual _ _) =
  prioridad == actual

mostrarSolicitud :: Solicitud -> String
mostrarSolicitud
    (Solicitud codigo origen destino cantidad prioridad estado unidad) =
  codigo ++ ": " ++ origen ++ " -> " ++ destino
    ++ " | turistas: " ++ show cantidad
    ++ " | prioridad: " ++ textoPrioridad prioridad
    ++ " | estado: " ++ textoEstado estado
    ++ " | unidad: " ++ mostrarUnidad unidad

textoPrioridad :: Prioridad -> String
textoPrioridad Baja = "baja"
textoPrioridad Media = "media"
textoPrioridad Alta = "alta"

textoEstado :: EstadoSolicitud -> String
textoEstado Pendiente = "pendiente"
textoEstado Asignada = "asignada"
textoEstado Entregada = "entregada"

mostrarUnidad :: Maybe String -> String
mostrarUnidad Nothing = "sin asignar"
mostrarUnidad (Just codigo) = codigo

unirLineas :: [String] -> String
unirLineas [] = ""
unirLineas [linea] = linea
unirLineas (linea : resto) = linea ++ "\n" ++ unirLineas resto

ejecutarReporteSolicitudes :: [Unidad] -> [Solicitud] -> IO ()
ejecutarReporteSolicitudes unidades solicitudes =
  putStrLn (reporteSolicitudes unidades solicitudes)