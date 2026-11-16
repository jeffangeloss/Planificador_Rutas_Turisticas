#include "Solicitud.h"

// Crea una solicitud con sus turistas, prioridad, estado y unidad asignada.
Solicitud::Solicitud(std::string codigo, std::string origen,
                     std::string destino, int cantidadTuristas,
                     Prioridad prioridad, EstadoSolicitud estado,
                     std::string unidadAsignada) {
  this->codigo = codigo;
  this->origen = origen;
  this->destino = destino;
  this->cantidadTuristas = cantidadTuristas;
  this->prioridad = prioridad;
  this->estado = estado;
  this->unidadAsignada = unidadAsignada;
}

// Devuelven el codigo, origen, destino, turistas, prioridad, estado y unidad de la solicitud.
std::string Solicitud::getCodigo() const { return codigo; }

std::string Solicitud::getOrigen() const { return origen; }

std::string Solicitud::getDestino() const { return destino; }

int Solicitud::getCantidadTuristas() const { return cantidadTuristas; }

Prioridad Solicitud::getPrioridad() const { return prioridad; }

EstadoSolicitud Solicitud::getEstado() const { return estado; }

std::string Solicitud::getUnidadAsignada() const { return unidadAsignada; }

// Indica si la solicitud esta pendiente.
bool Solicitud::estaPendiente() const {
  return estado == EstadoSolicitud::Pendiente;
}

// Asigna una unidad a la solicitud y la marca como asignada.
void Solicitud::asignarUnidad(std::string codigoUnidad) {
  unidadAsignada = codigoUnidad;
  estado = EstadoSolicitud::Asignada;
}

// Marca la solicitud como entregada.
void Solicitud::marcarEntregada() { estado = EstadoSolicitud::Entregada; }
