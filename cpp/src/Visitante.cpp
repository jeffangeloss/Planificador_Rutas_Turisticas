#include "Visitante.h"

// Crea un visitante con su solicitud y tipo.
Visitante::Visitante(std::string solicitud, TipoVisitante tipo) {
  this->solicitud = solicitud;
  this->tipo = tipo;
}

// Devuelven la solicitud y el tipo del visitante.
std::string Visitante::getSolicitud() const { return solicitud; }

TipoVisitante Visitante::getTipo() const { return tipo; }
