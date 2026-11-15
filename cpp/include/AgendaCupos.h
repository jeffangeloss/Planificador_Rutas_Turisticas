#ifndef AGENDA_CUPOS_H
#define AGENDA_CUPOS_H

#include "Cupo.h"
#include "Reserva.h"

#include <string>
#include <vector>

// FA03 - Autor: Jefferson Sanchez.
// Agenda de cupos con clave (punto, fecha, turno) y lista de espera.
// Politica de promocion, la de la Propuesta: cuando se liberan cupos, se
// recorre la espera en orden de llegada y se confirma cada reserva que cabe en
// los cupos libres; la que no cabe conserva su lugar. Una sola liberacion
// puede confirmar varias reservas. Se prefiere al orden de llegada estricto
// porque ese dejaria cupos sin usar cada vez que el primer grupo no cabe; el
// grupo grande no pierde su lugar y entra en cuanto se liberan cupos
// suficientes. La cola se reconstruye desde reservas.csv, ordenada por
// orden_espera.
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

  // Capacidad del turno, o -1 si el punto no tiene cupo en ese turno.
  int capacidad(const std::string &punto, const std::string &turno) const;
  int ocupados(const std::string &punto, const std::string &fecha,
               const std::string &turno) const;
  std::vector<Reserva> enEspera(const std::string &punto,
                                const std::string &fecha,
                                const std::string &turno) const;
  bool tieneReservaActiva(const std::string &solicitud,
                          const std::string &fecha,
                          const std::string &turno) const;
  std::string siguienteCodigo() const;

  // Confirma la reserva si cabe y, si no, la pone al final de la espera.
  // Devuelve false si el turno no tiene cupo o el grupo nunca cabria en el.
  bool reservar(const std::string &solicitud, const std::string &punto,
                const std::string &fecha, const std::string &turno,
                int personas, Reserva &creada);
  // Cancela una reserva activa y, si estaba confirmada, promueve la espera.
  bool cancelar(const std::string &codigo, std::vector<std::string> &promovidas);
  // Crea o cambia un cupo. No lo deja por debajo de lo ya confirmado en
  // alguna fecha ni del grupo mas grande en espera, y si sube promueve la
  // espera de cada fecha.
  bool fijarCupo(const std::string &punto, const std::string &turno,
                 int capacidadNueva, std::vector<std::string> &promovidas);
  // Renumera cada lista de espera y confirma las reservas que ya caben, para
  // que la agenda cargada desde los CSV cumpla la politica de promocion.
  std::vector<std::string> normalizar();
};

#endif
