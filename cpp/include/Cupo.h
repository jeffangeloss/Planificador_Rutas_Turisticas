#ifndef CUPO_H
#define CUPO_H

#include <string>

// FA03 Reservas de cupos por turno con lista de espera. Responsable: Jefferson Sanchez.
// Representa la capacidad de un punto por turno.
class Cupo {
private:
  std::string punto;
  std::string turno;
  int capacidadMaxima;

public:
  Cupo(std::string _punto, std::string _turno, int _capacidadMaxima);

  std::string getPunto() const;
  std::string getTurno() const;
  int getCapacidadMaxima() const;
  bool esDe(const std::string &puntoBuscado,
            const std::string &turnoBuscado) const;
  void cambiarCapacidad(int nuevaCapacidad);
};

#endif
