#include "Unidad.h"

// Crea una unidad con su tipo, capacidad, costo por km y estado.
Unidad::Unidad(std::string codigo, TipoUnidad tipo, int capacidad,
               double costoKm, EstadoUnidad estado) {
  this->codigo = codigo;
  this->tipo = tipo;
  this->capacidad = capacidad;
  this->costoKm = costoKm;
  this->estado = estado;
}

// Devuelven el codigo, tipo, capacidad, costo por km y estado de la unidad.
std::string Unidad::getCodigo() const { return codigo; }

TipoUnidad Unidad::getTipo() const { return tipo; }

int Unidad::getCapacidad() const { return capacidad; }

double Unidad::getCostoKm() const { return costoKm; }

EstadoUnidad Unidad::getEstado() const { return estado; }

// Indica si la unidad esta disponible.
bool Unidad::estaDisponible() const {
  return estado == EstadoUnidad::Disponible;
}

// Indica si la unidad admite esa cantidad de personas.
bool Unidad::tieneCapacidad(int cantidadPersonas) const {
  return capacidad >= cantidadPersonas;
}

// Marca la unidad como ocupada.
void Unidad::ocupar() { estado = EstadoUnidad::Ocupada; }

// Marca la unidad como disponible.
void Unidad::liberar() { estado = EstadoUnidad::Disponible; }
