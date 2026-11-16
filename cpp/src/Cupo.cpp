#include "Cupo.h"

// FA03 Reservas de cupos por turno con lista de espera. Responsable: Jefferson Sanchez.

Cupo::Cupo(std::string punto, std::string turno, int capacidadMaxima) {
  this->punto = punto;
  this->turno = turno;
  this->capacidadMaxima = capacidadMaxima;
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
