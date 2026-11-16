#include "Cupo.h"

// FA03 Reservas de cupos por turno con lista de espera. Responsable: Jefferson Sanchez.

// Crea un cupo con su punto, turno y capacidad maxima.
Cupo::Cupo(std::string _punto, std::string _turno, int _capacidadMaxima) {
  punto = _punto;
  turno = _turno;
  capacidadMaxima = _capacidadMaxima;
}

// Devuelven el punto, el turno y la capacidad maxima del cupo.
std::string Cupo::getPunto() const { return punto; }

std::string Cupo::getTurno() const { return turno; }

int Cupo::getCapacidadMaxima() const { return capacidadMaxima; }

// Indica si el cupo corresponde a ese punto y turno.
bool Cupo::esDe(const std::string &puntoBuscado,
                const std::string &turnoBuscado) const {
  return punto == puntoBuscado && turno == turnoBuscado;
}

// Cambia la capacidad maxima del cupo.
void Cupo::cambiarCapacidad(int nuevaCapacidad) {
  capacidadMaxima = nuevaCapacidad;
}
