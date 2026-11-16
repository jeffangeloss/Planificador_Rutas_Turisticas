#include "Conexion.h"

// Crea una conexion con su origen, destino, distancia, tiempo y estado.
Conexion::Conexion(std::string codigo, std::string origen, std::string destino,
                   double distancia, int tiempo, EstadoConexion estado) {
  this->codigo = codigo;
  this->origen = origen;
  this->destino = destino;
  this->distancia = distancia;
  this->tiempo = tiempo;
  this->estado = estado;
}

// Devuelven el codigo, origen, destino, distancia, tiempo y estado de la conexion.
std::string Conexion::getCodigo() const { return codigo; }

std::string Conexion::getOrigen() const { return origen; }

std::string Conexion::getDestino() const { return destino; }

double Conexion::getDistancia() const { return distancia; }

int Conexion::getTiempo() const { return tiempo; }

EstadoConexion Conexion::getEstado() const { return estado; }

// Indica si la conexion esta habilitada.
bool Conexion::estaHabilitada() const {
  return estado == EstadoConexion::Habilitada;
}

// Marca la conexion como habilitada.
void Conexion::habilitar() { estado = EstadoConexion::Habilitada; }

// Marca la conexion como cerrada.
void Conexion::cerrar() { estado = EstadoConexion::Cerrada; }
