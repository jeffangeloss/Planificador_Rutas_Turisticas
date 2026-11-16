#ifndef AGENDA_CUPOS_H
#define AGENDA_CUPOS_H

#include "Cupo.h"
#include "Reserva.h"

#include <string>
#include <vector>

// FA03 Reservas de cupos por turno con lista de espera. Responsable: Jefferson Sanchez.
// Controla la capacidad y promoción de reservas en espera.
class AgendaCupos {
private:
  std::vector<Cupo> cupos;
  std::vector<Reserva> reservas;

  int indiceCupo(const std::string &punto, const std::string &turno) const;
  int indiceReserva(const std::string &codigo) const;
  // Posiciones dentro de reservas de la espera de un turno, en orden de llegada.
  std::vector<int> indicesEnEspera(const std::string &punto,
                                   const std::string &fecha,
                                   const std::string &turno) const;
  void promover(const std::string &punto, const std::string &fecha,
                const std::string &turno, std::vector<std::string> &promovidas);
  void renumerarEspera(const std::string &punto, const std::string &fecha,
                       const std::string &turno);

public:
  AgendaCupos(std::vector<Cupo> cupos, std::vector<Reserva> reservas);

  const std::vector<Cupo> &getCupos() const;
  const std::vector<Reserva> &getReservas() const;

  // Capacidad del turno, o -1 si el punto no tiene capacidad en ese turno.
  int capacidad(const std::string &punto, const std::string &turno) const;
  int ocupados(const std::string &punto, const std::string &fecha,
               const std::string &turno) const;
  std::vector<Reserva> enEspera(const std::string &punto,
                                const std::string &fecha,
                                const std::string &turno) const;
  bool tieneReservaActiva(const std::string &solicitud,
                          const std::string &fecha,
                          const std::string &turno) const;
  // Codigo que sigue al mayor de las reservas, o vacio si se desbordaria.
  std::string siguienteCodigo() const;

  // Confirma la reserva si cabe y, si no, la pone al final de la espera.
  // Devuelve false si el turno no tiene capacidad, si el grupo la supera o si
  // no queda un codigo nuevo.
  bool reservar(const std::string &solicitud, const std::string &punto,
                const std::string &fecha, const std::string &turno,
                int personas, Reserva &creada);
  // Cancela una reserva activa y, si estaba confirmada, promueve la espera.
  bool cancelar(const std::string &codigo, std::vector<std::string> &promovidas);
  // Crea o cambia la capacidad de un turno. No la deja por debajo de lo ya
  // confirmado en alguna fecha ni del grupo mas grande en espera, y si sube
  // promueve la espera de cada fecha.
  bool fijarCupo(const std::string &punto, const std::string &turno,
                 int capacidadNueva, std::vector<std::string> &promovidas);
  // Renumera cada lista de espera y confirma las reservas que ya caben, para
  // que la agenda cargada desde los CSV cumpla la politica de promocion.
  std::vector<std::string> normalizar();
};

#endif
