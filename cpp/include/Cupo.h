#ifndef CUPO_H
#define CUPO_H

#include <string>

// FA03 - Autor: Jefferson Sanchez.
// Capacidad maxima de un punto en un turno. Vale para todas las fechas.
class Cupo {
private:
  std::string punto;
  std::string turno;
  int capacidadMaxima;

public:
  Cupo(std::string punto, std::string turno, int capacidadMaxima);

  std::string getPunto() const;
  std::string getTurno() const;
  int getCapacidadMaxima() const;
  bool esDe(const std::string &puntoBuscado,
            const std::string &turnoBuscado) const;
  void cambiarCapacidad(int nuevaCapacidad);
};

#endif
