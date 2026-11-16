#include "Cupo.h"

// FA03 Reservas de cupos por turno con lista de espera. Responsable: Jefferson Sanchez.

Cupo::Cupo(std::string _punto, std::string _turno, int _capacidadMaxima) {
  punto = _punto;
  turno = _turno;
  capacidadMaxima = _capacidadMaxima;
}

std::string Cupo::getPunto() const { return punto; }

std::string Cupo::getTurno() const { return turno; }

int Cupo::getCapacidadMaxima() const { return capacidadMaxima; }

bool Cupo::esDe(const std::string &puntoBuscado,
                const std::string &turnoBuscado) const {
  return punto == puntoBuscado && turno == turnoBuscado;
}

void Cupo::cambiarCapacidad(int nuevaCapacidad) {
  capacidadMaxima = nuevaCapacidad;
}
