#ifndef RESERVA_H
#define RESERVA_H

#include "Tipos.h"

#include <string>

// FA03 - Autor: Jefferson Sanchez.
// Reserva de una solicitud para un punto, una fecha y un turno. Una reserva en
// espera guarda su posicion en la lista; confirmada o cancelada guarda 0.
class Reserva {
private:
  std::string codigo;
  std::string solicitud;
  std::string punto;
  std::string fecha;
  std::string turno;
  int personas;
  EstadoReserva estado;
  int ordenEspera;

public:
  Reserva();
  Reserva(std::string codigo, std::string solicitud, std::string punto,
          std::string fecha, std::string turno, int personas,
          EstadoReserva estado, int ordenEspera);

  std::string getCodigo() const;
  std::string getSolicitud() const;
  std::string getPunto() const;
  std::string getFecha() const;
  std::string getTurno() const;
  int getPersonas() const;
  EstadoReserva getEstado() const;
  int getOrdenEspera() const;

  bool estaConfirmada() const;
  bool estaEnEspera() const;
  bool estaActiva() const;
  bool esDelTurno(const std::string &puntoBuscado,
                  const std::string &fechaBuscada,
                  const std::string &turnoBuscado) const;

  void confirmar();
  void ponerEnEspera(int orden);
  void cancelar();
};

// Formatos de FA03: fecha AAAA-MM-DD, turno HH:MM y enteros sin caracteres
// sobrantes.
bool fechaValida(const std::string &fecha);
bool turnoValido(const std::string &turno);
bool leerEntero(const std::string &texto, int &numero);
// Codigo de reserva formado por R y digitos, como R001.
bool codigoReservaValido(const std::string &codigo);

#endif
