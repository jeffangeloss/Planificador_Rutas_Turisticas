module Turismo
  ( Destino (..)
  , descripcion
  ) where

data Destino = Destino
  { nombre :: String
  , pais :: String
  }
  deriving (Eq, Show)

-- Devuelve el nombre y el pais del destino separados por una coma.
descripcion :: Destino -> String
descripcion destino = nombre destino ++ ", " ++ pais destino

