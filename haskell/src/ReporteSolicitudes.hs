-- 4.2.6 Reporte de solicitudes. Responsable: Samantha Rodriguez.
module ReporteSolicitudes (reporteSolicitudes, ejecutarReporteSolicitudes) where

import Tipos (EstadoSolicitud (..), Prioridad (..), Solicitud (..), Unidad (..))

-- construye el reporte con los datos de unidades y solicitudes.
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

-- comprueba si una solicitud tiene el estado indicado.
esEstado :: EstadoSolicitud -> Solicitud -> Bool
esEstado esperado (Solicitud _ _ _ _ _ estado _) = esperado == estado

-- suma la cantidad de turistas de las solicitudes.
sumarTuristas :: Solicitud -> Int -> Int
sumarTuristas (Solicitud _ _ _ cantidad _ _ _) acumulado =
  cantidad + acumulado

-- crea una linea de carga para cada unidad.
lineasCargaUnidades :: [Unidad] -> [Solicitud] -> [String]
lineasCargaUnidades unidades solicitudes =
  concatMap mostrarCarga unidades
  where
    mostrarCarga (Unidad codigo tipo _ _ _) =
      [ " - " ++ codigo ++ " (" ++ show tipo ++ "): "
          ++ show (cargaUnidad codigo solicitudes)
          ++ " turistas asignados"
      ]

-- suma turistas de solicitudes asignadas a una unidad.
cargaUnidad :: String -> [Solicitud] -> Int
cargaUnidad codigo solicitudes =
  foldr (sumarSolicitudAsignada codigo) 0 solicitudes

-- agrega turistas solo si la solicitud esta asignada a esta unidad.
sumarSolicitudAsignada :: String -> Solicitud -> Int -> Int
sumarSolicitudAsignada codigo
    (Solicitud _ _ _ cantidad _ estado unidad) acumulado =
  case unidad of
    Just codigoUnidad
      | estado == Asignada && codigoUnidad == codigo -> cantidad + acumulado
    _ -> acumulado

-- ordena visualmente los pedidos pendientes por prioridad.
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

-- comprueba si una solicitud tiene la prioridad indicada.
tienePrioridad :: Prioridad -> Solicitud -> Bool
tienePrioridad prioridad (Solicitud _ _ _ _ actual _ _) =
  prioridad == actual

-- convierte una solicitud en una linea legible.
mostrarSolicitud :: Solicitud -> String
mostrarSolicitud
    (Solicitud codigo origen destino cantidad prioridad estado unidad) =
  codigo ++ ": " ++ origen ++ " -> " ++ destino
    ++ " | turistas: " ++ show cantidad
    ++ " | prioridad: " ++ textoPrioridad prioridad
    ++ " | estado: " ++ textoEstado estado
    ++ " | unidad: " ++ mostrarUnidad unidad

-- convierte el tipo prioridad a texto para el reporte.
textoPrioridad :: Prioridad -> String
textoPrioridad Baja = "baja"
textoPrioridad Media = "media"
textoPrioridad Alta = "alta"

-- convierte el estado a texto para el reporte.
textoEstado :: EstadoSolicitud -> String
textoEstado Pendiente = "pendiente"
textoEstado Asignada = "asignada"
textoEstado Entregada = "entregada"

-- muestra el codigo o indica que no hay unidad asignada.
mostrarUnidad :: Maybe String -> String
mostrarUnidad Nothing = "sin asignar"
mostrarUnidad (Just codigo) = codigo

-- une las lineas del reporte con saltos de linea.
unirLineas :: [String] -> String
unirLineas [] = ""
unirLineas [linea] = linea
unirLineas (linea : resto) = linea ++ "\n" ++ unirLineas resto

-- imprime el texto generado por el reporte.
ejecutarReporteSolicitudes :: [Unidad] -> [Solicitud] -> IO ()
ejecutarReporteSolicitudes unidades solicitudes =
  putStrLn (reporteSolicitudes unidades solicitudes)
