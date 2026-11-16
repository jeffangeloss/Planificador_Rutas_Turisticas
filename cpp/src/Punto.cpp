#include "Punto.h"

// Crea un punto con su codigo, nombre, tipo y zona.
Punto::Punto(std::string codigo, std::string nombre, TipoPunto tipo,
             std::string zona) {
  this->codigo = codigo;
  this->nombre = nombre;
  this->tipo = tipo;
  this->zona = zona;
}

// Devuelven el codigo, el nombre, el tipo y la zona del punto.
std::string Punto::getCodigo() const { return codigo; }

std::string Punto::getNombre() const { return nombre; }

TipoPunto Punto::getTipo() const { return tipo; }

std::string Punto::getZona() const { return zona; }

// Indica si el punto es de tipo atractivo.
bool Punto::esAtractivo() const { return tipo == TipoPunto::Atractivo; }
