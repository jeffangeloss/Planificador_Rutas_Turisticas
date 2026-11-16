// FA03 Reservas de cupos por turno con lista de espera. Responsable: Jefferson Sanchez.
// Prueba cupos, reservas, promociones y persistencia CSV.

#include "AgendaCupos.h"
#include "ArchivosCSV.h"
#include "FA03.h"

#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

namespace {

int fallas = 0;

void comprobar(bool condicion, const string &descripcion) {
  cout << (condicion ? "[OK] " : "[FALLA] ") << descripcion << "\n";
  if (!condicion) ++fallas;
}

Reserva confirmada(const string &codigo, const string &solicitud, int personas) {
  return Reserva(codigo, solicitud, "P004", "2026-11-15", "10:00", personas,
                 EstadoReserva::Confirmada, 0);
}

Reserva enEspera(const string &codigo, const string &solicitud, int personas,
                 int orden) {
  return Reserva(codigo, solicitud, "P004", "2026-11-15", "10:00", personas,
                 EstadoReserva::EnEspera, orden);
}

// Carpeta de trabajo de una prueba dentro del directorio actual (con ctest, la
// carpeta de compilacion) y no en el directorio temporal compartido del sistema.
filesystem::path carpetaDePrueba(const string &nombre) {
  return filesystem::current_path() / ("prueba_fa03_" + nombre);
}

const Reserva *buscar(const AgendaCupos &agenda, const string &codigo) {
  for (const Reserva &reserva : agenda.getReservas()) {
    if (reserva.getCodigo() == codigo) return &reserva;
  }
  return nullptr;
}

// Caso de la Propuesta: 25 cupos, 23 ocupados y un grupo de 6 primero en espera.
AgendaCupos agendaDeLaPropuesta() {
  vector<Cupo> cupos = {Cupo("P004", "10:00", 25)};
  vector<Reserva> reservas = {
      confirmada("R001", "S003", 17), confirmada("R002", "S006", 4),
      confirmada("R003", "S007", 2), enEspera("R004", "S004", 6, 1)};
  return AgendaCupos(cupos, reservas);
}

void probarReservaQueCabe() {
  AgendaCupos agenda({Cupo("P004", "10:00", 25)}, {confirmada("R001", "S003", 10)});
  Reserva creada;
  bool hecha = agenda.reservar("S001", "P004", "2026-11-15", "10:00", 5, creada);
  comprobar(hecha && creada.estaConfirmada() && creada.getOrdenEspera() == 0,
            "una reserva que cabe queda confirmada");
  comprobar(agenda.ocupados("P004", "2026-11-15", "10:00") == 15,
            "los ocupados suman la reserva confirmada");
  comprobar(creada.getCodigo() == "R002", "el codigo nuevo sigue al mayor");
}

void probarCasoDeLaPropuesta() {
  AgendaCupos agenda = agendaDeLaPropuesta();
  comprobar(agenda.ocupados("P004", "2026-11-15", "10:00") == 23,
            "el turno empieza con 23 de 25 cupos ocupados");
  Reserva creada;
  agenda.reservar("S001", "P004", "2026-11-15", "10:00", 4, creada);
  comprobar(creada.estaEnEspera() && creada.getOrdenEspera() == 2,
            "un grupo de 4 que no cabe queda segundo en espera");

  vector<string> promovidas;
  bool cancelada = agenda.cancelar("R003", promovidas);
  comprobar(cancelada, "se cancela la reserva confirmada de 2 personas");
  comprobar(promovidas.size() == 1 && promovidas[0] == creada.getCodigo(),
            "con 4 cupos libres se confirma solo el grupo de 4");
  const Reserva *grupoDeSeis = buscar(agenda, "R004");
  comprobar(grupoDeSeis != nullptr && grupoDeSeis->estaEnEspera()
                && grupoDeSeis->getOrdenEspera() == 1,
            "el grupo de 6 no cabe y conserva la posicion 1");
  comprobar(agenda.ocupados("P004", "2026-11-15", "10:00") == 25,
            "el turno queda con 25 de 25 cupos");
}

void probarPromocionEnCascada() {
  AgendaCupos agenda({Cupo("P004", "10:00", 10)},
                     {confirmada("R001", "S003", 10), enEspera("R002", "S006", 3, 1),
                      enEspera("R003", "S007", 4, 2), enEspera("R004", "S004", 5, 3)});
  vector<string> promovidas;
  agenda.cancelar("R001", promovidas);
  comprobar(promovidas.size() == 2 && promovidas[0] == "R002" && promovidas[1] == "R003",
            "una cancelacion confirma en cascada a los que caben, en orden");
  const Reserva *ultimo = buscar(agenda, "R004");
  comprobar(ultimo != nullptr && ultimo->estaEnEspera() && ultimo->getOrdenEspera() == 1,
            "el que no cabe pasa a la posicion 1 tras renumerar");
}

void probarCancelarEnEspera() {
  AgendaCupos agenda({Cupo("P004", "10:00", 5)},
                     {confirmada("R001", "S003", 5), enEspera("R002", "S006", 3, 1),
                      enEspera("R003", "S007", 2, 2), enEspera("R004", "S004", 4, 3)});
  vector<string> promovidas;
  agenda.cancelar("R003", promovidas);
  comprobar(promovidas.empty(), "cancelar una reserva en espera no libera cupos");
  const Reserva *r2 = buscar(agenda, "R002");
  const Reserva *r4 = buscar(agenda, "R004");
  comprobar(r2 != nullptr && r2->getOrdenEspera() == 1 && r4 != nullptr
                && r4->getOrdenEspera() == 2,
            "la lista de espera se renumera sin huecos");
  comprobar(!agenda.cancelar("R003", promovidas), "no se cancela dos veces");
  comprobar(!agenda.cancelar("R999", promovidas), "no se cancela un codigo inexistente");
}

void probarCambiosDeCupo() {
  AgendaCupos agenda({Cupo("P004", "10:00", 10)},
                     {confirmada("R001", "S003", 8), enEspera("R002", "S006", 4, 1)});
  vector<string> promovidas;
  comprobar(!agenda.fijarCupo("P004", "10:00", 7, promovidas),
            "no se baja el cupo por debajo de los confirmados");
  comprobar(agenda.capacidad("P004", "10:00") == 10, "el cupo rechazado no cambia");
  comprobar(agenda.fijarCupo("P004", "10:00", 12, promovidas) && promovidas.size() == 1
                && promovidas[0] == "R002",
            "al subir el cupo se confirma a quien ahora cabe");
  promovidas.clear();
  comprobar(agenda.fijarCupo("P003", "15:00", 20, promovidas)
                && agenda.capacidad("P003", "15:00") == 20,
            "se crea un cupo nuevo para otro punto y turno");
  comprobar(!agenda.fijarCupo("P003", "15:00", 0, promovidas),
            "un cupo debe ser mayor que cero");
}

void probarLimites() {
  AgendaCupos agenda = agendaDeLaPropuesta();
  Reserva creada;
  comprobar(!agenda.reservar("S001", "P004", "2026-11-15", "10:00", 26, creada),
            "un grupo mayor que el cupo del turno se rechaza");
  comprobar(!agenda.reservar("S001", "P004", "2026-11-15", "16:00", 2, creada),
            "no se reserva en un turno sin cupo definido");
  comprobar(agenda.reservar("S001", "P004", "2026-11-16", "10:00", 2, creada)
                && creada.estaConfirmada(),
            "otra fecha del mismo turno tiene su propio cupo");
  comprobar(agenda.ocupados("P004", "2026-11-15", "10:00") == 23,
            "la reserva de otra fecha no ocupa el 15 de noviembre");
  comprobar(agenda.tieneReservaActiva("S004", "2026-11-15", "10:00"),
            "una reserva en espera cuenta como activa");
  comprobar(!agenda.tieneReservaActiva("S004", "2026-11-16", "10:00"),
            "la solicitud no tiene reserva en otra fecha");
}

void probarOrdenDesdeElArchivo() {
  AgendaCupos agenda({Cupo("P004", "10:00", 4)},
                     {confirmada("R001", "S003", 4), enEspera("R003", "S007", 2, 2),
                      enEspera("R002", "S006", 3, 1)});
  vector<Reserva> cola = agenda.enEspera("P004", "2026-11-15", "10:00");
  comprobar(cola.size() == 2 && cola[0].getCodigo() == "R002"
                && cola[1].getCodigo() == "R003",
            "la cola se ordena por orden_espera aunque las filas vengan desordenadas");
}

void probarValidaciones() {
  comprobar(fechaValida("2026-11-15") && fechaValida("2028-02-29"),
            "acepta fechas reales, incluso el 29 de febrero bisiesto");
  comprobar(!fechaValida("2026-02-30") && !fechaValida("2027-02-29")
                && !fechaValida("15/11/2026") && !fechaValida("2026-13-01"),
            "rechaza fechas imposibles o con otro formato");
  comprobar(turnoValido("10:00") && turnoValido("23:59"), "acepta turnos HH:MM");
  comprobar(!turnoValido("24:00") && !turnoValido("9:00") && !turnoValido("10-00"),
            "rechaza turnos fuera de formato");
}

void probarArchivos() {
  const filesystem::path carpeta = carpetaDePrueba("archivos");
  filesystem::remove_all(carpeta);
  filesystem::create_directories(carpeta);
  ArchivosCSV archivos(carpeta.string());
  AgendaCupos agenda = agendaDeLaPropuesta();
  comprobar(archivos.guardarCupos(agenda.getCupos())
                && archivos.guardarReservas(agenda.getReservas()),
            "se guardan cupos.csv y reservas.csv");
  vector<Cupo> cupos = archivos.cargarCupos();
  vector<Reserva> reservas = archivos.cargarReservas();
  comprobar(cupos.size() == 1 && cupos[0].getCapacidadMaxima() == 25,
            "cupos.csv vuelve con su capacidad");
  comprobar(reservas.size() == 4 && reservas[3].estaEnEspera()
                && reservas[3].getOrdenEspera() == 1 && reservas[0].getPersonas() == 17,
            "reservas.csv vuelve con estados, personas y orden de espera");

  ofstream archivo((carpeta / "reservas.csv").string(), ios::app);
  archivo << "R009,S001,P004,2026-11-15,10:00,2,perdida,0\n"
          << "R010,S001,P004,2026-02-30,10:00,2,confirmada,0\n";
  archivo.close();
  comprobar(archivos.cargarReservas().size() == 4,
            "una fila con estado o fecha invalidos se descarta al cargar");
  filesystem::remove_all(carpeta);
}

void probarCupoNoBajaDeLaEspera() {
  AgendaCupos agenda({Cupo("P004", "10:00", 10)},
                     {confirmada("R001", "S003", 2), enEspera("R002", "S004", 9, 1)});
  vector<string> promovidas;
  comprobar(!agenda.fijarCupo("P004", "10:00", 8, promovidas),
            "no se baja el cupo por debajo del grupo mas grande en espera");
}

void probarNormalizar() {
  AgendaCupos agenda({Cupo("P004", "10:00", 10)},
                     {confirmada("R001", "S003", 4), enEspera("R002", "S006", 3, 5),
                      enEspera("R003", "S004", 8, 2)});
  vector<string> promovidas = agenda.normalizar();
  const Reserva *grande = buscar(agenda, "R003");
  comprobar(promovidas.size() == 1 && promovidas[0] == "R002",
            "al normalizar se confirma la reserva en espera que ya cabia");
  comprobar(grande != nullptr && grande->estaEnEspera() && grande->getOrdenEspera() == 1,
            "al normalizar la espera queda numerada desde 1");
}

void probarCodigosGrandes() {
  AgendaCupos conMil({Cupo("P004", "10:00", 10)}, {confirmada("R999", "S003", 1)});
  comprobar(conMil.siguienteCodigo() == "R1000", "despues de R999 sigue R1000");
  AgendaCupos alLimite({Cupo("P004", "10:00", 10)},
                       {confirmada("R2147483647", "S003", 1)});
  comprobar(alLimite.siguienteCodigo() == "R2147483648",
            "el codigo siguiente no se desborda en el limite de int");
}

void probarCargaDefensiva() {
  const filesystem::path carpeta = carpetaDePrueba("carga");
  filesystem::remove_all(carpeta);
  filesystem::create_directories(carpeta);
  ofstream cupos((carpeta / "cupos.csv").string(), ios::binary);
  cupos << "punto,turno,capacidad_maxima\r\nP004,10:00,9\r\nP004,10:00,30\r\n"
        << "P003,15:00,20\r\n\r\n";
  cupos.close();
  ofstream reservas((carpeta / "reservas.csv").string(), ios::binary);
  reservas << "codigo,solicitud,punto,fecha,turno,personas,estado,orden_espera\r\n"
           << "R001,S006,P004,2026-11-15,10:00,4,confirmada,0\r\n"
           << "R001,S007,P004,2026-11-15,10:00,2,en_espera,1\r\n\r\n";
  reservas.close();
  ArchivosCSV archivos(carpeta.string());
  comprobar(archivos.contarFilas("cupos.csv") == 3
                && archivos.contarFilas("reservas.csv") == 2,
            "las lineas en blanco con CRLF no cuentan como filas");
  comprobar(archivos.cargarCupos().size() == 2,
            "se descarta el cupo repetido para el mismo punto y turno");
  comprobar(archivos.cargarReservas().size() == 1,
            "se descarta la reserva con codigo repetido");
  filesystem::remove_all(carpeta);
}

// Puntos y solicitudes de las pruebas de carga, tomados de _datos.
vector<Punto> puntosDePrueba() {
  return {Punto("P001", "Hotel Miraflores", TipoPunto::Hotel, "Miraflores"),
          Punto("P004", "Mirador de Barranco", TipoPunto::Mirador, "Barranco")};
}

vector<Solicitud> solicitudesDePrueba() {
  return {Solicitud("S003", "P001", "P004", 20, Prioridad::Alta,
                    EstadoSolicitud::Entregada, "U003"),
          Solicitud("S006", "P000", "P004", 4, Prioridad::Media,
                    EstadoSolicitud::Asignada, "U004"),
          Solicitud("S007", "P000", "P004", 2, Prioridad::Media,
                    EstadoSolicitud::Asignada, "U004")};
}

Reserva reservaEnP004(const string &codigo, const string &solicitud,
                      const string &turno, int personas, EstadoReserva estado,
                      int orden) {
  return Reserva(codigo, solicitud, "P004", "2026-11-15", turno, personas,
                 estado, orden);
}

void probarCodigoDeReserva() {
  comprobar(codigoReservaValido("R001") && codigoReservaValido("R1000"),
            "acepta codigos de reserva con R y digitos");
  comprobar(!codigoReservaValido("X010") && !codigoReservaValido("R")
                && !codigoReservaValido("r001") && !codigoReservaValido("R00A")
                && !codigoReservaValido(""),
            "rechaza codigos de reserva sin R inicial o con otros caracteres");
}

void probarCargaRechazaCodigoSinR() {
  const filesystem::path carpeta = carpetaDePrueba("codigo");
  filesystem::remove_all(carpeta);
  filesystem::create_directories(carpeta);
  ofstream reservas((carpeta / "reservas.csv").string(), ios::binary);
  reservas << "codigo,solicitud,punto,fecha,turno,personas,estado,orden_espera\n"
           << "R001,S006,P004,2026-11-15,10:00,4,confirmada,0\n"
           << "X010,S007,P004,2026-11-15,10:00,2,confirmada,0\n";
  reservas.close();
  ArchivosCSV archivos(carpeta.string());
  vector<Reserva> leidas = archivos.cargarReservas();
  comprobar(leidas.size() == 1 && leidas[0].getCodigo() == "R001",
            "la carga descarta la reserva cuyo codigo no empieza con R");
  filesystem::remove_all(carpeta);
}

void probarCargaConservaCanceladaGrande() {
  // Se reserva un grupo de 12, se cancela y despues la capacidad baja a 5.
  CargaFA03 carga = armarCargaFA03(
      {Cupo("P004", "16:00", 5)},
      {reservaEnP004("R006", "S003", "16:00", 12, EstadoReserva::Cancelada, 0)},
      false, puntosDePrueba(), solicitudesDePrueba());
  comprobar(carga.reservas.size() == 1 && carga.errores.empty() && !carga.soloLectura,
            "una reserva cancelada no se compara con la capacidad actual del turno");
}

void probarCargaDescartaActivaGrande() {
  CargaFA03 carga = armarCargaFA03(
      {Cupo("P004", "16:00", 5)},
      {reservaEnP004("R006", "S003", "16:00", 12, EstadoReserva::Confirmada, 0)},
      false, puntosDePrueba(), solicitudesDePrueba());
  comprobar(carga.reservas.empty() && carga.errores.size() == 1 && carga.soloLectura,
            "una reserva confirmada mayor que la capacidad se descarta y deja solo lectura");
}

void probarCargaDescartaActivaRepetida() {
  CargaFA03 carga = armarCargaFA03(
      {Cupo("P004", "10:00", 9)},
      {reservaEnP004("R001", "S006", "10:00", 4, EstadoReserva::Confirmada, 0),
       reservaEnP004("R011", "S006", "10:00", 1, EstadoReserva::Confirmada, 0)},
      false, puntosDePrueba(), solicitudesDePrueba());
  comprobar(carga.reservas.size() == 1 && carga.reservas[0].getCodigo() == "R001"
                && carga.soloLectura,
            "se descarta la segunda reserva activa de una solicitud en la misma fecha y turno");
}

void probarCargaAceptaCanceladaYActiva() {
  CargaFA03 carga = armarCargaFA03(
      {Cupo("P004", "10:00", 9)},
      {reservaEnP004("R012", "S007", "10:00", 2, EstadoReserva::Cancelada, 0),
       reservaEnP004("R013", "S007", "10:00", 2, EstadoReserva::Confirmada, 0)},
      false, puntosDePrueba(), solicitudesDePrueba());
  comprobar(carga.reservas.size() == 2 && !carga.soloLectura,
            "una reserva cancelada no impide otra activa de la misma solicitud y turno");
}

void probarSoloLecturaNoNormaliza() {
  // R001 se descarta porque su solicitud no existe, y sin ella R003 cabria.
  CargaFA03 carga = armarCargaFA03(
      {Cupo("P004", "10:00", 9)},
      {reservaEnP004("R001", "S999", "10:00", 4, EstadoReserva::Confirmada, 0),
       reservaEnP004("R002", "S007", "10:00", 2, EstadoReserva::Confirmada, 0),
       reservaEnP004("R003", "S003", "10:00", 6, EstadoReserva::EnEspera, 1)},
      false, puntosDePrueba(), solicitudesDePrueba());
  const Reserva *r3 = buscar(carga.agenda, "R003");
  comprobar(carga.soloLectura && carga.promovidas.empty() && r3 != nullptr
                && r3->estaEnEspera(),
            "en solo lectura la carga no confirma reservas en memoria");
}

void probarCargaNormalizaSinDescartes() {
  CargaFA03 carga = armarCargaFA03(
      {Cupo("P004", "10:00", 9)},
      {reservaEnP004("R002", "S007", "10:00", 2, EstadoReserva::Confirmada, 0),
       reservaEnP004("R003", "S003", "10:00", 6, EstadoReserva::EnEspera, 1)},
      false, puntosDePrueba(), solicitudesDePrueba());
  const Reserva *r3 = buscar(carga.agenda, "R003");
  comprobar(!carga.soloLectura && carga.promovidas.size() == 1
                && carga.promovidas[0] == "R003" && r3 != nullptr && r3->estaConfirmada(),
            "sin descartes la carga confirma la reserva que ya cabe");
  comprobar(carga.reservas.size() == 2 && carga.reservas[1].estaEnEspera(),
            "la carga conserva las filas leidas para volver a ellas si no puede guardar");
}

void probarGuardadoConVueltaAtras() {
  vector<string> pasos;
  auto paso = [&pasos](const string &nombre, bool resultado) {
    return [&pasos, nombre, resultado]() {
      pasos.push_back(nombre);
      return resultado;
    };
  };
  comprobar(guardarCambioFA03(true, true, paso("cupos", true), paso("reservas", true),
                              paso("restaurar", true)) == GuardadoFA03::Guardado
                && pasos == vector<string>{"cupos", "reservas"},
            "un cambio que se guarda bien escribe cupos y despues reservas");
  pasos.clear();
  comprobar(guardarCambioFA03(true, true, paso("cupos", false), paso("reservas", true),
                              paso("restaurar", true)) == GuardadoFA03::Revertido
                && pasos == vector<string>{"cupos"},
            "si falla cupos.csv no se escribe reservas.csv ni se restaura nada");
  pasos.clear();
  comprobar(guardarCambioFA03(true, true, paso("cupos", true), paso("reservas", false),
                              paso("restaurar", true)) == GuardadoFA03::Revertido
                && pasos == vector<string>{"cupos", "reservas", "restaurar"},
            "si falla reservas.csv despues de cupos.csv se restaura cupos.csv");
  pasos.clear();
  comprobar(guardarCambioFA03(true, true, paso("cupos", true), paso("reservas", false),
                              paso("restaurar", false)) == GuardadoFA03::SinRestaurar,
            "si tampoco se puede restaurar cupos.csv el resultado lo dice");
  pasos.clear();
  comprobar(guardarCambioFA03(false, true, paso("cupos", true), paso("reservas", false),
                              paso("restaurar", true)) == GuardadoFA03::Revertido
                && pasos == vector<string>{"reservas"},
            "un cambio que solo toca reservas.csv no reescribe cupos.csv al fallar");
}

void probarCargaClaveIncluyeFechaYTurno() {
  CargaFA03 dosTurnos = armarCargaFA03(
      {Cupo("P004", "10:00", 9), Cupo("P004", "16:00", 9)},
      {reservaEnP004("R001", "S006", "10:00", 4, EstadoReserva::Confirmada, 0),
       reservaEnP004("R002", "S006", "16:00", 4, EstadoReserva::Confirmada, 0)},
      false, puntosDePrueba(), solicitudesDePrueba());
  comprobar(dosTurnos.reservas.size() == 2 && !dosTurnos.soloLectura,
            "una solicitud puede tener reservas activas en dos turnos distintos");
  CargaFA03 dosFechas = armarCargaFA03(
      {Cupo("P004", "10:00", 9)},
      {Reserva("R001", "S006", "P004", "2026-11-15", "10:00", 4,
               EstadoReserva::Confirmada, 0),
       Reserva("R002", "S006", "P004", "2026-11-16", "10:00", 4,
               EstadoReserva::Confirmada, 0)},
      false, puntosDePrueba(), solicitudesDePrueba());
  comprobar(dosFechas.reservas.size() == 2 && !dosFechas.soloLectura,
            "una solicitud puede tener reservas activas en dos fechas distintas");
}

void probarCargaEsperaCuentaComoActiva() {
  CargaFA03 carga = armarCargaFA03(
      {Cupo("P004", "10:00", 9)},
      {reservaEnP004("R001", "S006", "10:00", 4, EstadoReserva::Confirmada, 0),
       reservaEnP004("R011", "S006", "10:00", 1, EstadoReserva::EnEspera, 1)},
      false, puntosDePrueba(), solicitudesDePrueba());
  comprobar(carga.reservas.size() == 1 && carga.reservas[0].getCodigo() == "R001"
                && carga.soloLectura,
            "una reserva en espera tambien cuenta como reserva activa repetida");
}

void probarCargaDescartaEsperaGrande() {
  CargaFA03 carga = armarCargaFA03(
      {Cupo("P004", "16:00", 5)},
      {reservaEnP004("R006", "S003", "16:00", 12, EstadoReserva::EnEspera, 1)},
      false, puntosDePrueba(), solicitudesDePrueba());
  comprobar(carga.reservas.empty() && carga.errores.size() == 1 && carga.soloLectura,
            "una reserva en espera mayor que la capacidad se descarta y deja solo lectura");
}

void probarCargaOtrasCausasDeSoloLectura() {
  CargaFA03 malFormadas = armarCargaFA03(
      {Cupo("P004", "10:00", 9)},
      {reservaEnP004("R002", "S007", "10:00", 2, EstadoReserva::Confirmada, 0),
       reservaEnP004("R003", "S003", "10:00", 6, EstadoReserva::EnEspera, 1)},
      true, puntosDePrueba(), solicitudesDePrueba());
  comprobar(malFormadas.soloLectura && malFormadas.promovidas.empty(),
            "con filas mal formadas la carga queda en solo lectura y no confirma");
  CargaFA03 deHotel = armarCargaFA03({Cupo("P001", "10:00", 5)}, {}, false,
                                     puntosDePrueba(), solicitudesDePrueba());
  comprobar(deHotel.cupos.empty() && deHotel.errores.size() == 1 && deHotel.soloLectura,
            "se descarta la capacidad de un punto que no es atractivo ni mirador");
  CargaFA03 sobrevendido = armarCargaFA03(
      {Cupo("P004", "10:00", 5)},
      {reservaEnP004("R001", "S006", "10:00", 4, EstadoReserva::Confirmada, 0),
       reservaEnP004("R002", "S007", "10:00", 2, EstadoReserva::Confirmada, 0)},
      false, puntosDePrueba(), solicitudesDePrueba());
  comprobar(sobrevendido.reservas.size() == 2 && sobrevendido.errores.size() == 1
                && sobrevendido.soloLectura,
            "un turno con mas confirmados que su capacidad deja solo lectura sin descartar");
}

void probarCodigoSiguienteAlMaximo() {
  AgendaCupos alMaximo({Cupo("P004", "10:00", 9)},
                       {confirmada("R9223372036854775807", "S003", 1)});
  Reserva creada;
  comprobar(alMaximo.siguienteCodigo().empty()
                && !alMaximo.reservar("S001", "P004", "2026-11-15", "10:00", 2, creada)
                && alMaximo.getReservas().size() == 1,
            "no se crea una reserva cuando el codigo siguiente se desbordaria");
}

void probarAgendaSegunGuardado() {
  AgendaCupos anterior({Cupo("P004", "10:00", 9)},
                       {confirmada("R001", "S003", 6), enEspera("R002", "S006", 4, 1)});
  AgendaCupos nueva({Cupo("P004", "10:00", 12)},
                    {confirmada("R001", "S003", 6), confirmada("R002", "S006", 4)});
  AgendaCupos guardada = agendaSegunGuardadoFA03(GuardadoFA03::Guardado, nueva, anterior);
  AgendaCupos revertida = agendaSegunGuardadoFA03(GuardadoFA03::Revertido, nueva, anterior);
  AgendaCupos sinRestaurar =
      agendaSegunGuardadoFA03(GuardadoFA03::SinRestaurar, nueva, anterior);
  const Reserva *r2 = buscar(sinRestaurar, "R002");
  comprobar(guardada.capacidad("P004", "10:00") == 12
                && revertida.capacidad("P004", "10:00") == 9
                && buscar(revertida, "R002")->estaEnEspera(),
            "tras guardar o revertir la agenda queda como el cambio o como antes");
  comprobar(sinRestaurar.capacidad("P004", "10:00") == 12 && r2 != nullptr
                && r2->estaEnEspera(),
            "si no se restaura cupos.csv la agenda toma la capacidad nueva y las reservas de antes");
}

}  // namespace

int main() {
  probarReservaQueCabe();
  probarCasoDeLaPropuesta();
  probarPromocionEnCascada();
  probarCancelarEnEspera();
  probarCambiosDeCupo();
  probarLimites();
  probarOrdenDesdeElArchivo();
  probarValidaciones();
  probarArchivos();
  probarCupoNoBajaDeLaEspera();
  probarNormalizar();
  probarCodigosGrandes();
  probarCargaDefensiva();
  probarCodigoDeReserva();
  probarCargaRechazaCodigoSinR();
  probarCargaConservaCanceladaGrande();
  probarCargaDescartaActivaGrande();
  probarCargaDescartaActivaRepetida();
  probarCargaAceptaCanceladaYActiva();
  probarSoloLecturaNoNormaliza();
  probarCargaNormalizaSinDescartes();
  probarGuardadoConVueltaAtras();
  probarCargaClaveIncluyeFechaYTurno();
  probarCargaEsperaCuentaComoActiva();
  probarCargaDescartaEsperaGrande();
  probarCargaOtrasCausasDeSoloLectura();
  probarCodigoSiguienteAlMaximo();
  probarAgendaSegunGuardado();
  cout << (fallas == 0 ? "Todas las pruebas de FA03 pasan.\n"
                       : "Hay pruebas de FA03 que fallan.\n");
  return fallas == 0 ? 0 : 1;
}
