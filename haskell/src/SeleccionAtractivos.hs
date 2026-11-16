-- FA02 Selección de atractivos dentro de un presupuesto. Responsable: Melissa Ruíz.
module SeleccionAtractivos (ejecutarSeleccionAtractivos) where

import CostoRuta (costoRuta)
import LecturaCSV (leerConexiones, leerEntradas, leerPuntos, leerUnidades)
import Text.Printf (printf)
import Text.Read (readMaybe)
import Tipos (Conexion, Entrada (..), EstadoUnidad (Disponible), Punto (..),
              TipoPunto (Atractivo), TipoUnidad (..), Unidad (..))
import Vecinos (vecinos)

data AlternativaFA02 = AlternativaFA02 [String] TipoUnidad Int Double Int
data ParametrosFA02 = ParametrosFA02 Int Int Int Double Int

cantidadPersonas :: ParametrosFA02 -> Int
cantidadPersonas (ParametrosFA02 generales estudiantes ninos _ _) = generales + estudiantes + ninos

buscarPunto :: String -> [Punto] -> Maybe Punto
buscarPunto _ [] = Nothing
buscarPunto codigo (punto@(Punto codigoActual _ _ _) : restantes)
  | codigo == codigoActual = Just punto
  | otherwise = buscarPunto codigo restantes

buscarEntrada :: String -> [Entrada] -> Maybe Entrada
buscarEntrada _ [] = Nothing
buscarEntrada codigo (entrada@(Entrada codigoActual _ _ _ _) : restantes)
  | codigo == codigoActual = Just entrada
  | otherwise = buscarEntrada codigo restantes

-- Cada ruta comienza en P000. Los puntos ya visitados no pueden repetirse.
generarRutas :: String -> [Conexion] -> [String] -> [[String]]
generarRutas punto conexiones ruta = ruta : extenderRutas (vecinos punto conexiones) conexiones ruta

extenderRutas :: [String] -> [Conexion] -> [String] -> [[String]]
extenderRutas [] _ _ = []
extenderRutas (siguiente : restantes) conexiones ruta
  | siguiente `elem` ruta = extenderRutas restantes conexiones ruta
  | otherwise =
      generarRutas siguiente conexiones (ruta ++ [siguiente]) ++ extenderRutas restantes conexiones ruta

quitarRutaSinVisitas :: [[String]] -> [[String]]
quitarRutaSinVisitas [] = []
quitarRutaSinVisitas (ruta : restantes)
  | length ruta > 1 = ruta : quitarRutaSinVisitas restantes
  | otherwise = quitarRutaSinVisitas restantes

rutasDesdeAgencia :: [Conexion] -> [[String]]
rutasDesdeAgencia conexiones = quitarRutaSinVisitas (generarRutas "P000" conexiones ["P000"])

visitasDeRuta :: [String] -> [String]
visitasDeRuta [] = []
visitasDeRuta (_ : visitas) = visitas

costoPorPerfiles :: ParametrosFA02 -> Entrada -> Double
costoPorPerfiles (ParametrosFA02 generales estudiantes ninos _ _)
                  (Entrada _ precioGeneral precioEstudiante precioNino _) =
  fromIntegral generales * precioGeneral + fromIntegral estudiantes * precioEstudiante + fromIntegral ninos * precioNino

-- Acumula el costo de entradas y el tiempo de visita de una ruta.
calcularVisitas :: [String] -> [Punto] -> [Entrada] -> ParametrosFA02 -> Maybe (Double, Int)
calcularVisitas [] _ _ _ = Just (0, 0)
calcularVisitas (codigo : restantes) puntos entradas parametros =
  case (buscarPunto codigo puntos, buscarEntrada codigo entradas) of
    (Just (Punto _ _ tipo _), Just entrada@(Entrada _ _ _ _ minutos)) ->
      case calcularVisitas restantes puntos entradas parametros of
        Nothing -> Nothing
        Just (costoRestante, tiempoRestante) ->
          if tipo == Atractivo
            then Just (costoPorPerfiles parametros entrada + costoRestante, minutos + tiempoRestante)
            else Just (costoRestante, minutos + tiempoRestante)
    _ -> Nothing

unidadCompatible :: Int -> Unidad -> Bool
unidadCompatible cantidad (Unidad _ _ capacidad _ Disponible) = capacidad >= cantidad
unidadCompatible _ _ = False

esMejorUnidad :: Unidad -> Unidad -> Bool
esMejorUnidad (Unidad codigoA tipoA _ costoA _) (Unidad codigoB tipoB _ costoB _) =
  tipoA == tipoB && (costoA < costoB || (costoA == costoB && codigoA < codigoB))

existeUnidadMejor :: Unidad -> [Unidad] -> Bool
existeUnidadMejor _ [] = False
existeUnidadMejor unidad (otra : restantes)
  | esMejorUnidad otra unidad = True
  | otherwise = existeUnidadMejor unidad restantes

unidadesCompatibles :: Int -> [Unidad] -> [Unidad]
unidadesCompatibles cantidad unidades = filter (unidadCompatible cantidad) unidades

seleccionarUnidadesBaratas :: [Unidad] -> [Unidad]
seleccionarUnidadesBaratas unidades =
  [unidad | unidad <- unidades, not (existeUnidadMejor unidad unidades)]

unidadesRecomendadas :: ParametrosFA02 -> [Unidad] -> [Unidad]
unidadesRecomendadas parametros unidades =
  seleccionarUnidadesBaratas (unidadesCompatibles (cantidadPersonas parametros) unidades)

-- Combina una ruta y una unidad cuando cumplen las restricciones ingresadas.
crearAlternativa :: [String] -> Unidad -> [Conexion] -> [Punto] -> [Entrada] -> ParametrosFA02 -> Maybe AlternativaFA02
crearAlternativa ruta (Unidad _ tipo _ costoKm _) conexiones puntos entradas parametros =
  case costoRuta ruta conexiones of
    (_, _, False) -> Nothing
    (distancia, tiempoRuta, True) ->
      case calcularVisitas (visitasDeRuta ruta) puntos entradas parametros of
        Nothing -> Nothing
        Just (costoEntradas, tiempoVisitas) ->
          validarAlternativa ruta tipo costoEntradas distancia costoKm tiempoRuta tiempoVisitas parametros

-- Un grupo a pie avanza a 3,6 km/h (Vega-Huerta et al., 2026); las demas unidades usan tiempo_min.
tiempoTraslado :: TipoUnidad -> Double -> Int -> Int
tiempoTraslado GuiaPeatonal distancia _ = round (distancia / 3.6 * 60)
tiempoTraslado _ _ tiempoRuta = tiempoRuta

validarAlternativa :: [String] -> TipoUnidad -> Double -> Double -> Double -> Int -> Int -> ParametrosFA02 -> Maybe AlternativaFA02
validarAlternativa ruta tipo costoEntradas distancia costoKm tiempoRuta tiempoVisitas (ParametrosFA02 _ _ _ presupuesto tiempoMaximo) =
  crearAlternativaValida ruta tipo (costoEntradas + distancia * costoKm)
    (tiempoTraslado tipo distancia tiempoRuta + tiempoVisitas) presupuesto tiempoMaximo

crearAlternativaValida :: [String] -> TipoUnidad -> Double -> Int -> Double -> Int -> Maybe AlternativaFA02
crearAlternativaValida ruta tipo costoTotal tiempoTotal presupuesto tiempoMaximo
  | costoTotal <= presupuesto && tiempoTotal <= tiempoMaximo =
      Just (AlternativaFA02 ruta tipo (length (visitasDeRuta ruta)) costoTotal tiempoTotal)
  | otherwise = Nothing

alternativasPorUnidades :: [Unidad] -> [String] -> [Conexion] -> [Punto] -> [Entrada] -> ParametrosFA02 -> [AlternativaFA02]
alternativasPorUnidades [] _ _ _ _ _ = []
alternativasPorUnidades (unidad : restantes) ruta conexiones puntos entradas parametros =
  case crearAlternativa ruta unidad conexiones puntos entradas parametros of
    Nothing -> alternativasPorUnidades restantes ruta conexiones puntos entradas parametros
    Just alternativa -> alternativa : alternativasPorUnidades restantes ruta conexiones puntos entradas parametros

alternativasPorRutas :: [[String]] -> [Unidad] -> [Conexion] -> [Punto] -> [Entrada] -> ParametrosFA02 -> [AlternativaFA02]
alternativasPorRutas [] _ _ _ _ _ = []
alternativasPorRutas (ruta : restantes) unidades conexiones puntos entradas parametros =
  alternativasPorUnidades unidades ruta conexiones puntos entradas parametros
    ++ alternativasPorRutas restantes unidades conexiones puntos entradas parametros

esMejorAlternativa :: AlternativaFA02 -> AlternativaFA02 -> Bool
esMejorAlternativa (AlternativaFA02 _ _ visitasA costoA tiempoA) (AlternativaFA02 _ _ visitasB costoB tiempoB)
  | visitasA /= visitasB = visitasA > visitasB
  | costoA /= costoB = costoA < costoB
  | otherwise = tiempoA < tiempoB

insertarAlternativa :: AlternativaFA02 -> [AlternativaFA02] -> [AlternativaFA02]
insertarAlternativa alternativa [] = [alternativa]
insertarAlternativa alternativa (actual : restantes)
  | esMejorAlternativa alternativa actual = alternativa : actual : restantes
  | otherwise = actual : insertarAlternativa alternativa restantes

ordenarAlternativas :: [AlternativaFA02] -> [AlternativaFA02]
ordenarAlternativas [] = []
ordenarAlternativas (alternativa : restantes) =
  insertarAlternativa alternativa (ordenarAlternativas restantes)

unirRuta :: String -> [String] -> String
unirRuta _ [] = ""
unirRuta _ [punto] = punto
unirRuta separador (punto : restantes) = punto ++ separador ++ unirRuta separador restantes

mostrarAlternativas :: [AlternativaFA02] -> IO ()
mostrarAlternativas [] = putStrLn "No se encontraron alternativas dentro del presupuesto y tiempo disponibles."
mostrarAlternativas alternativas = do
  putStrLn "=== Alternativas generadas por Haskell ==="
  putStrLn ""
  mostrarAlternativasDesde 1 alternativas

mostrarAlternativasDesde :: Int -> [AlternativaFA02] -> IO ()
mostrarAlternativasDesde _ [] = return ()
mostrarAlternativasDesde numero (AlternativaFA02 ruta tipo visitas costo tiempo : restantes) = do
  putStrLn (show numero ++ ". " ++ unirRuta " -> " ruta)
  putStrLn ("   Visitas: " ++ show visitas)
  putStrLn ("   Transporte recomendado: " ++ textoTipoUnidad tipo)
  printf "   Costo total: S/ %.2f\n" costo
  putStrLn ("   Tiempo total: " ++ show tiempo ++ " min")
  putStrLn ("FA02_DATA|" ++ show numero ++ "|" ++ unirRuta ">" ruta ++ "|" ++ textoTipoUnidad tipo ++ "|" ++ show visitas ++ "|" ++ printf "%.2f" costo ++ "|" ++ show tiempo)
  putStrLn ""
  mostrarAlternativasDesde (numero + 1) restantes

textoTipoUnidad :: TipoUnidad -> String
textoTipoUnidad GuiaPeatonal = "guia_peatonal"
textoTipoUnidad TaxiTuristico = "taxi_turistico"
textoTipoUnidad BusTuristico = "bus_turistico"

mostrarError :: String -> IO ()
mostrarError mensaje = putStrLn ("FA02_ERROR|" ++ mensaje)

crearAlternativas :: ParametrosFA02 -> [Punto] -> [Conexion] -> [Unidad] -> [Entrada] -> [AlternativaFA02]
crearAlternativas parametros puntos conexiones unidades entradas =
  alternativasPorRutas (rutasDesdeAgencia conexiones) (unidadesRecomendadas parametros unidades)
    conexiones puntos entradas parametros

mejoresAlternativas :: ParametrosFA02 -> [Punto] -> [Conexion] -> [Unidad] -> [Entrada] -> [AlternativaFA02]
mejoresAlternativas parametros puntos conexiones unidades entradas =
  take 3 (ordenarAlternativas (crearAlternativas parametros puntos conexiones unidades entradas))

generarAlternativas :: ParametrosFA02 -> [Punto] -> [Conexion] -> [Unidad] -> [Entrada] -> IO ()
generarAlternativas parametros puntos conexiones unidades entradas =
  case buscarPunto "P000" puntos of
    Nothing -> mostrarError "No existe la agencia P000 en puntos.csv."
    Just _ -> mostrarAlternativas (mejoresAlternativas parametros puntos conexiones unidades entradas)

procesarDatos :: ParametrosFA02 -> Maybe [Punto] -> Maybe [Conexion] -> Maybe [Unidad] -> Maybe [Entrada] -> IO ()
procesarDatos _ Nothing _ _ _ = mostrarError "No se pudo leer puntos.csv."
procesarDatos _ _ Nothing _ _ = mostrarError "No se pudo leer conexiones.csv."
procesarDatos _ _ _ Nothing _ = mostrarError "No se pudo leer unidades.csv."
procesarDatos _ _ _ _ Nothing = mostrarError "No se pudo leer entradas.csv."
procesarDatos parametros (Just puntos) (Just conexiones) (Just unidades) (Just entradas) =
  generarAlternativas parametros puntos conexiones unidades entradas

ejecutarSeleccionAtractivos :: FilePath -> String -> String -> String -> String -> String -> IO ()
ejecutarSeleccionAtractivos carpetaDatos generalesTexto estudiantesTexto ninosTexto presupuestoTexto tiempoTexto =
  case (readMaybe generalesTexto, readMaybe estudiantesTexto, readMaybe ninosTexto, readMaybe presupuestoTexto, readMaybe tiempoTexto) of
    (Just generales, Just estudiantes, Just ninos, Just presupuesto, Just tiempoMaximo)
      | generales >= 0 && estudiantes >= 0 && ninos >= 0 && generales + estudiantes + ninos > 0
          && presupuesto > 0 && tiempoMaximo > 0 -> do
          puntos <- leerPuntos (carpetaDatos ++ "/puntos.csv")
          conexiones <- leerConexiones (carpetaDatos ++ "/conexiones.csv")
          unidades <- leerUnidades (carpetaDatos ++ "/unidades.csv")
          entradas <- leerEntradas (carpetaDatos ++ "/entradas.csv")
          let parametros = ParametrosFA02 generales estudiantes ninos presupuesto tiempoMaximo
          procesarDatos parametros puntos conexiones unidades entradas
    _ -> mostrarError "Revisa las cantidades, el presupuesto y el tiempo ingresados."
