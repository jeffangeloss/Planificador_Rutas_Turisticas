// FA02 Selección de atractivos dentro de un presupuesto. Responsable: Melissa Ruíz.
#ifndef FA02_H
#define FA02_H

#include "ArchivosCSV.h"
#include "Solicitud.h"
#include "Tipos.h"
#include "Unidad.h"
#include "Visitante.h"

#include <string>
#include <vector>

struct AlternativaFA02 {
  std::vector<std::string> ruta;
  TipoUnidad tipoTransporte;
  int cantidadVisitas;
  double costoTotal;
  int tiempoTotal;
};

class CotizadorFA02 {
private:
  std::string carpetaDatos;
  ArchivosCSV archivosCSV;

  std::string leerTexto(const std::string &mensaje);
  bool leerDoublePositivo(const std::string &mensaje, double &numero);
  bool leerEnteroPositivo(const std::string &mensaje, int &numero);
  bool leerEnteroNoNegativo(const std::string &mensaje, int &numero);
  std::vector<std::string> separarPor(const std::string &texto,
                                      char separador);
  bool parsearAlternativa(const std::string &linea,
                          AlternativaFA02 &alternativa);
  std::string comandoHaskell(int generales, int estudiantes, int ninos,
                             double presupuesto, int tiempoMaximo);
  bool generarAlternativas(int generales, int estudiantes, int ninos,
                           double presupuesto, int tiempoMaximo,
                           std::vector<AlternativaFA02> &alternativas);
  std::string siguienteCodigoSolicitud(
      const std::vector<Solicitud> &solicitudes);
  int buscarUnidadDisponible(const std::vector<Unidad> &unidades,
                             TipoUnidad tipoTransporte,
                             int cantidadPersonas);
  void agregarVisitantes(std::vector<Visitante> &visitantes,
                         const std::string &codigoSolicitud,
                         TipoVisitante tipoVisitante, int cantidad);
  void registrarPlan(int generales, int estudiantes, int ninos,
                     const AlternativaFA02 &alternativa);

public:
  CotizadorFA02(const std::string &rutaDatos);
  void ejecutar();
};

void ejecutarFA02(const std::string &carpetaDatos);

#endif
