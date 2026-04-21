#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <fstream>
#include <ctime>

using namespace std;

const int FILAS        = 18;
const int COLS         = 18;
const int MAX_CUPOS = 50;

const int PAGO_CARRO = 5000;
const int PAGO_MOTO  = 2500;
const int PAGO_BICI  = 1000;

const char CELDA_PARED   = 'W';
const char CELDA_VIA     = 'V';
const char CELDA_CARRO   = 'C';
const char CELDA_MOTO    = 'M';
const char CELDA_BICI    = 'B';
const char CELDA_OCUPADO = 'O';
const char CELDA_ENTRADA = 'E';
const char CELDA_SALIDA  = 'S';
const char CELDA_ZONA    = 'X';

const int TIPO_CARRO = 1;
const int TIPO_MOTO  = 2;
const int TIPO_BICI  = 3;

const string ARCHIVO = "Informe.txt";

const string RESET    = "\033[0m";
const string ROJO     = "\033[41;97m";
const string VERDE    = "\033[42;30m";
const string AZUL     = "\033[44;97m";
const string CYAN     = "\033[46;30m";
const string AMARILLO = "\033[43;30m";
const string GRIS     = "\033[100;37m";
const string MORADO   = "\033[45;97m";

struct Espacio {
    int    esta_ocupado;
    int    tipo_vehiculo;
    string placa;
    time_t hora_entrada;
    string hora_entrada_texto;
    int    fila;
    int    col;
};

struct Parqueadero {
    Espacio espacios[MAX_CUPOS];
    int    cupos_totales;
    int    ocupados;
    int    carros_actuales;
    int    motos_actuales;
    int    bicis_actuales;
    double ingresos_dia;
};

char mapa_base[FILAS][COLS] = {
    {'W','W','E','W','W','W','W','W','W','W','W','W','W','W','W','S','W','W'},
    {'W','X','V','X','X','X','X','X','X','X','X','X','X','X','X','V','X','W'},
    {'W','C','V','C','C','V','C','C','V','C','C','V','C','C','V','C','C','W'},
    {'W','C','V','C','C','V','C','C','V','C','C','V','C','C','V','C','C','W'},
    {'W','C','V','C','C','V','C','C','V','C','C','V','C','C','V','C','C','W'},
    {'W','C','V','C','C','V','C','C','V','C','C','V','C','C','V','C','C','W'},
    {'W','V','V','V','V','V','V','V','V','V','V','V','V','V','V','V','V','W'},
    {'W','M','V','M','M','V','M','M','V','M','M','V','M','M','V','M','M','W'},
    {'W','M','V','M','M','V','M','M','V','M','M','V','M','M','V','M','M','W'},
    {'W','M','V','M','M','V','M','M','V','M','M','V','M','M','V','M','M','W'},
    {'W','M','V','M','M','V','M','M','V','M','M','V','M','M','V','M','M','W'},
    {'W','V','V','V','V','V','V','V','V','V','V','V','V','V','V','V','V','W'},
    {'W','B','V','B','B','V','B','B','V','B','B','V','B','B','V','B','B','W'},
    {'W','B','V','B','B','V','B','B','V','B','B','V','B','B','V','B','B','W'},
    {'W','B','V','B','B','V','B','B','V','B','B','V','B','B','V','B','B','W'},
    {'W','B','V','B','B','V','B','B','V','B','B','V','B','B','V','B','B','W'},
    {'W','V','V','V','V','V','V','V','V','V','V','V','V','V','V','V','V','W'},
    {'W','W','W','W','W','W','W','W','W','W','W','W','W','W','W','W','W','W'},
};

char mapa[FILAS][COLS];

void   inicializarParqueadero         (Parqueadero *p, char (*m)[COLS]);
void   mostrarBienvenida       ();
void   mostrarMapa             (char (*m)[COLS], Parqueadero *p);
int    elegirTipoVehiculo      ();
int    ingresarVehiculo        (Parqueadero *p, char (*m)[COLS]);
void   procesarSalida (Espacio *e, Parqueadero *p, char (*m)[COLS]);
int    pagarSalida         (Parqueadero *p, char (*m)[COLS]);
double calcularPrecio           (int tipo, time_t entrada, time_t salida);
void   mostrarEstadisticas     (Parqueadero *p);
int    buscarEspacioLibre      (Parqueadero *p, int tipo, int *fila, int *col);
void   buscarPorPlaca          (Parqueadero *p);
void   guardarEstado           (Parqueadero *p);
void   cargarEstado            (Parqueadero *p, char (*m)[COLS]);
string obtenerHora           (time_t t);
string nombreTipo              (int tipo);
double segundosAdentro         (Espacio *e);
void   limpiarPantalla           ();
void   ejecutarMenu            (Parqueadero *p, char (*m)[COLS]);

void limpiarPantalla() {
    cin.clear();
    cin.ignore(1000, '\n');
}

string obtenerHora(time_t t) {
    struct tm *info = localtime(&t);
    ostringstream oss;
    oss << setfill('0')
        << setw(2) << info->tm_hour << ":"
        << setw(2) << info->tm_min  << ":"
        << setw(2) << info->tm_sec  << "  "
        << setw(2) << info->tm_mday << "/"
        << setw(2) << (info->tm_mon + 1) << "/"
        << (info->tm_year + 1900);
    return oss.str();
}

string nombreTipo(int tipo) {
    if (tipo == TIPO_CARRO) return "Carro";
    if (tipo == TIPO_MOTO)  return "Moto";
    return "Bicicleta";
}

double segundosAdentro(Espacio *e) {
    return difftime(time(nullptr), e->hora_entrada);
}

void mostrarBienvenida() {
    time_t ahora = time(nullptr);
    struct tm *t = localtime(&ahora);

    cout << "\n";
    cout << "  +==========================================+\n";
    cout << "  |       SISTEMA DE PARQUEADERO v2.0        |\n";
    cout << "  +==========================================+\n";
    cout << "  |  Fecha : " << setfill('0')
         << setw(2) << t->tm_mday   << "/"
         << setw(2) << (t->tm_mon + 1) << "/"
         << (t->tm_year + 1900)     << "                        |\n";
    cout << "  |  Hora  : "
         << setw(2) << t->tm_hour   << ":"
         << setw(2) << t->tm_min    << ":"
         << setw(2) << t->tm_sec    << "                           |\n";
    cout << "  +==========================================+\n\n";
    cout << setfill(' ');
}

void guardarEstado(Parqueadero *p) {
    ofstream archivo(ARCHIVO);
    if (!archivo.is_open()) {
        cout << "\n  Advertencia: no se pudo guardar el estado.\n";
        return;
    }

    archivo << fixed << setprecision(2) << p->ingresos_dia << "\n";

    for (int k = 0; k < p->cupos_totales; k++) {
        Espacio *e = &p->espacios[k];
        if (e->esta_ocupado == 1) {
            archivo << e->tipo_vehiculo << " "
                    << e->placa         << " "
                    << e->hora_entrada  << " "
                    << e->fila     << " "
                    << e->col      << "\n";
        }
    }

    archivo.close();
    cout << "\n  Estado guardado en '" << ARCHIVO << "'.\n";
}

void cargarEstado(Parqueadero *p, char (*m)[COLS]) {
    ifstream archivo(ARCHIVO);
    if (!archivo.is_open()) return;

    archivo >> p->ingresos_dia;

    int    tipo, fila, col;
    string placa;
    time_t hora;
    int restaurados = 0;

    while (archivo >> tipo >> placa >> hora >> fila >> col) {
        for (int k = 0; k < p->cupos_totales; k++) {
            Espacio *e = &p->espacios[k];
            if (e->fila == fila && e->col == col && e->esta_ocupado == 0) {
                e->esta_ocupado          = 1;
                e->tipo_vehiculo    = tipo;
                e->placa            = placa;
                e->hora_entrada     = hora;
                e->hora_entrada_texto = obtenerHora(hora);
                m[fila][col] = CELDA_OCUPADO;

                p->ocupados++;
                if (tipo == TIPO_CARRO)     p->carros_actuales++;
                else if (tipo == TIPO_MOTO) p->motos_actuales++;
                else                        p->bicis_actuales++;

                restaurados++;
                break;
            }
        }
    }

    archivo.close();
    if (restaurados > 0)
        cout << "\n  Se restauraron " << restaurados
             << " vehiculo(s) del estado anterior.\n";
}

void inicializarParqueadero(Parqueadero *p, char (*m)[COLS]) {
    p->cupos_totales = 0;
    p->ocupados       = 0;
    p->carros_actuales = 0;
    p->motos_actuales  = 0;
    p->bicis_actuales  = 0;
    p->ingresos_dia   = 0.0;

    for (int i = 0; i < FILAS; i++)
        for (int j = 0; j < COLS; j++)
            m[i][j] = mapa_base[i][j];

    int idx = 0;
    for (int i = 0; i < FILAS && idx < MAX_CUPOS; i++) {
        for (int j = 0; j < COLS && idx < MAX_CUPOS; j++) {
            char celda = m[i][j];
            if (celda == CELDA_CARRO || celda == CELDA_MOTO || celda == CELDA_BICI) {

                Espacio *e = &p->espacios[idx];
                e->esta_ocupado          = 0;
                e->tipo_vehiculo    = 0;
                e->placa            = "";
                e->hora_entrada     = 0;
                e->hora_entrada_texto = "";
                e->fila        = i;
                e->col         = j;

                if (celda == CELDA_CARRO) {
                    e->tipo_vehiculo = TIPO_CARRO;
                } else if (celda == CELDA_MOTO) {
                    e->tipo_vehiculo = TIPO_MOTO;
                } else {
                    e->tipo_vehiculo = TIPO_BICI;
                }

                idx++;
                p->cupos_totales++;
            }
        }
    }

    cargarEstado(p, m);
}

void mostrarMapa(char (*m)[COLS], Parqueadero *p) {
    cout << "\n";
    cout << "  +--------------------------------------------------+\n";
    cout << "  |         PARQUEADERO - MAPA EN TIEMPO REAL        |\n";
    cout << "  +--------------------------------------------------+\n\n";

    cout << "     ";
    for (int j = 0; j < COLS; j++) cout << setw(2) << j << " ";
    cout << "\n     ";
    for (int j = 0; j < COLS; j++) cout << "---";
    cout << "\n";

    for (int i = 0; i < FILAS; i++) {
        cout << " " << setw(2) << i << "| ";

        for (int j = 0; j < COLS; j++) {
            char celda = m[i][j];

            if      (celda == CELDA_PARED)   cout << GRIS     << " W " << RESET;
            else if (celda == CELDA_VIA)     cout << AMARILLO << " . " << RESET;
            else if (celda == CELDA_ENTRADA) cout << MORADO   << "ENT" << RESET;
            else if (celda == CELDA_SALIDA)  cout << MORADO   << "SAL" << RESET;
            else if (celda == CELDA_ZONA)    cout << GRIS     << "   " << RESET;
            else if (celda == CELDA_CARRO)   cout << VERDE    << " C " << RESET;
            else if (celda == CELDA_MOTO)    cout << AZUL     << " M " << RESET;
            else if (celda == CELDA_BICI)    cout << CYAN     << " B " << RESET;
            else if (celda == CELDA_OCUPADO) {
                string etiqueta = "???";
                for (int k = 0; k < p->cupos_totales; k++) {
                    Espacio *e = &p->espacios[k];
                    if (e->fila == i && e->col == j && e->esta_ocupado == 1) {
                        etiqueta = e->placa.substr(0, 3);
                        while ((int)etiqueta.size() < 3) etiqueta += ' ';
                        break;
                    }
                }
                cout << ROJO << etiqueta << RESET;
            }
        }
        cout << "\n";
    }

    cout << "\n  Leyenda:\n";
    cout << "    " << VERDE    << " C " << RESET << " Carro libre   ";
    cout          << AZUL      << " M " << RESET << " Moto libre   ";
    cout          << CYAN      << " B " << RESET << " Bici libre\n";
    cout << "    " << ROJO     << "ABC" << RESET << " Ocupado       ";
    cout          << AMARILLO  << " . " << RESET << " Via          ";
    cout          << GRIS      << " W " << RESET << " Pared\n";
    cout << "\n  Ocupados: " << p->ocupados
         << "  |  Libres: " << (p->cupos_totales - p->ocupados)
         << "  |  Total: "  << p->cupos_totales << "\n\n";
}

int elegirTipoVehiculo() {
    int opcion;
    cout << "\n  Tipo de vehiculo:\n";
    cout << "    1. Carro      (tarifa: $" << PAGO_CARRO << "/hora)\n";
    cout << "    2. Moto       (tarifa: $" << PAGO_MOTO  << "/hora)\n";
    cout << "    3. Bicicleta  (tarifa: $" << PAGO_BICI  << "/hora)\n";
    cout << "    0. Cancelar\n";
    cout << "  Opcion: ";
    cin  >> opcion;
    limpiarPantalla();

    if (opcion == 1) return TIPO_CARRO;
    if (opcion == 2) return TIPO_MOTO;
    if (opcion == 3) return TIPO_BICI;
    if (opcion == 0) return -1;
    cout << "\n  Opcion invalida.\n";
    return -1;
}

int buscarEspacioLibre(Parqueadero *p, int tipo, int *fila, int *col) {
    for (int k = 0; k < p->cupos_totales; k++) {
        Espacio *e = &p->espacios[k];
        if (e->esta_ocupado == 0 && e->tipo_vehiculo == tipo) {
            *fila = e->fila;
            *col  = e->col;
            return k;
        }
    }
    return -1;
}

double calcularPrecio(int tipo, time_t entrada, time_t salida) {
    double horas = difftime(salida, entrada) / 3600.0;
    if (horas < (10.0 / 60.0)) {
        horas = 10.0 / 60.0;
    }

    double tarifa;
    if (tipo == TIPO_CARRO) {
        tarifa = PAGO_CARRO;
    } else if (tipo == TIPO_MOTO) {
        tarifa = PAGO_MOTO;
    } else {
        tarifa = PAGO_BICI;
    }

    return horas * tarifa;
}

int ingresarVehiculo(Parqueadero *p, char (*m)[COLS]) {
    int tipo = elegirTipoVehiculo();
    if (tipo == -1) return 0;

    int fila, col;
    int idx = buscarEspacioLibre(p, tipo, &fila, &col);

    if (idx == -1) {
        string nombre;
        if (tipo == TIPO_CARRO) {
            nombre = "carros";
        } else if (tipo == TIPO_MOTO) {
            nombre = "motos";
        } else {
            nombre = "bicicletas";
        }
        cout << "\n  No hay espacios para " << nombre << " disponibles.\n";
        return 0;
    }

    string placa;
    cout << "\n  Ingrese la placa: ";
    cin  >> placa;
    limpiarPantalla();

    for (int k = 0; k < p->cupos_totales; k++) {
        if (p->espacios[k].esta_ocupado == 1 && p->espacios[k].placa == placa) {
            cout << "\n  ERROR: El vehiculo con placa '" << placa
                 << "' ya se encuentra adentro.\n";
            return 0;
        }
    }

    Espacio *e = &p->espacios[idx];
    e->esta_ocupado          = 1;
    e->placa            = placa;
    e->hora_entrada     = time(nullptr);
    e->hora_entrada_texto = obtenerHora(e->hora_entrada);

    m[fila][col] = CELDA_OCUPADO;

    p->ocupados++;
    if (tipo == TIPO_CARRO)     p->carros_actuales++;
    else if (tipo == TIPO_MOTO) p->motos_actuales++;
    else                        p->bicis_actuales++;

    cout << "\n  Ingreso registrado!\n";
    cout << "  Vehiculo  : " << nombreTipo(tipo) << " - " << placa << "\n";
    cout << "  Espacio   : fila " << fila << ", columna " << col << "\n";
    cout << "  Entrada   : " << e->hora_entrada_texto << "\n";

    guardarEstado(p);
    return 1;
}

void procesarSalida(Espacio *e, Parqueadero *p, char (*m)[COLS]) {
    time_t hora_salida = time(nullptr);
    double total = calcularPrecio(e->tipo_vehiculo, e->hora_entrada, hora_salida);

    double seg_total = difftime(hora_salida, e->hora_entrada);
    int h  = (int)(seg_total / 3600);
    int mi = (int)((seg_total - h * 3600.0) / 60);
    int s  = (int)seg_total % 60;

    struct tm *ts = localtime(&hora_salida);
    ostringstream sal;
    sal << setfill('0')
        << setw(2) << ts->tm_hour << ":"
        << setw(2) << ts->tm_min  << ":"
        << setw(2) << ts->tm_sec  << "  "
        << setw(2) << ts->tm_mday << "/"
        << setw(2) << (ts->tm_mon + 1) << "/"
        << (ts->tm_year + 1900);

    cout << "\n  ----------------------------------------\n";
    cout << "            RECIBO DE COBRO\n";
    cout << "  ----------------------------------------\n";
    cout << "  Tipo      : " << nombreTipo(e->tipo_vehiculo) << "\n";
    cout << "  Placa     : " << e->placa << "\n";
    cout << "  Entrada   : " << e->hora_entrada_texto << "\n";
    cout << "  Salida    : " << sal.str() << "\n";
    cout << "  Tiempo    : " << h << "h " << mi << "m " << s << "s\n";
    cout << "  ........................................\n";
    cout << "  TOTAL     : $" << fixed << setprecision(0) << total << " pesos\n";
    cout << "  ----------------------------------------\n\n";

    p->ingresos_dia += total;
    cout << "  Ingresos del dia hasta ahora: $"
         << fixed << setprecision(0) << p->ingresos_dia << " pesos\n";

    if (e->tipo_vehiculo == TIPO_CARRO) {
        m[e->fila][e->col] = CELDA_CARRO;
    } else if (e->tipo_vehiculo == TIPO_MOTO) {
        m[e->fila][e->col] = CELDA_MOTO;
    } else {
        m[e->fila][e->col] = CELDA_BICI;
    }

    p->ocupados--;
    if (e->tipo_vehiculo == TIPO_CARRO) {
        p->carros_actuales--;
    } else if (e->tipo_vehiculo == TIPO_MOTO) {
        p->motos_actuales--;
    } else {
        p->bicis_actuales--;
    }

    e->esta_ocupado          = 0;
    e->placa            = "";
    e->hora_entrada     = 0;
    e->hora_entrada_texto = "";

    guardarEstado(p);
}

int pagarSalida(Parqueadero *p, char (*m)[COLS]) {
    if (p->ocupados == 0) {
        cout << "\n  El parqueadero esta vacio.\n";
        return 0;
    }

    int tipo = elegirTipoVehiculo();
    if (tipo == -1) return 0;

    int lista[MAX_CUPOS];
    int cantidad = 0;

    for (int k = 0; k < p->cupos_totales; k++) {
        if (p->espacios[k].esta_ocupado == 1 && p->espacios[k].tipo_vehiculo == tipo) {
            lista[cantidad] = k;
            cantidad++;
        }
    }

    if (cantidad == 0) {
        string nombre;
        if (tipo == TIPO_CARRO) {
            nombre = "carros";
        } else if (tipo == TIPO_MOTO) {
            nombre = "motos";
        } else {
            nombre = "bicicletas";
        }
        cout << "\n  No hay " << nombre << " en el parqueadero ahora.\n";
        return 0;
    }

    string nombre_tipo;
    if (tipo == TIPO_CARRO) {
        nombre_tipo = "CARROS";
    } else if (tipo == TIPO_MOTO) {
        nombre_tipo = "MOTOS";
    } else {
        nombre_tipo = "BICICLETAS";
    }
    cout << "\n  " << string(50, '-') << "\n";
    cout << "   " << nombre_tipo << " EN EL PARQUEADERO\n";
    cout << "  " << string(50, '-') << "\n";
    cout << "  " << left
         << setw(5)  << "No."
         << setw(10) << "PLACA"
         << setw(12) << "ESPACIO"
         << setw(22) << "ENTRADA"
         << "COBRO EST.\n";
    cout << "  " << string(50, '-') << "\n";

    for (int i = 0; i < cantidad; i++) {
        Espacio *e = &p->espacios[lista[i]];
        double estimado = calcularPrecio(e->tipo_vehiculo, e->hora_entrada, time(nullptr));

        cout << "  " << left
             << setw(5)  << (i + 1)
             << setw(10) << e->placa
             << "[" << setw(2) << e->fila << "," << setw(2) << e->col << "]  "
             << setw(22) << e->hora_entrada_texto
             << "$" << fixed << setprecision(0) << estimado << "\n";
    }
    cout << "  " << string(50, '-') << "\n";
    cout << "    0. Cancelar\n";

    int seleccion;
    cout << "\n  Seleccione el numero del vehiculo: ";
    cin  >> seleccion;
    limpiarPantalla();

    if (seleccion == 0) {
        cout << "\n  Salida cancelada.\n";
        return 0;
    }

    if (seleccion < 1 || seleccion > cantidad) {
        cout << "\n  Numero invalido. Debe ser entre 1 y " << cantidad << ".\n";
        return 0;
    }

    Espacio *elegido = &p->espacios[lista[seleccion - 1]];

    cout << "\n  Confirma la salida del vehiculo " << elegido->placa << "? (S/N): ";
    char confirm;
    cin  >> confirm;
    limpiarPantalla();

    if (confirm != 'S' && confirm != 's') {
        cout << "\n  Salida cancelada.\n";
        return 0;
    }

    procesarSalida(elegido, p, m);
    return 1;
}

void buscarPorPlaca(Parqueadero *p) {
    if (p->ocupados == 0) {
        cout << "\n  No hay vehiculos en el parqueadero.\n";
        return;
    }

    string placa;
    cout << "\n  Ingrese la placa a buscar: ";
    cin  >> placa;
    limpiarPantalla();

    for (int k = 0; k < p->cupos_totales; k++) {
        Espacio *e = &p->espacios[k];
        if (e->esta_ocupado == 1 && e->placa == placa) {

            double seg = segundosAdentro(e);
            int h  = (int)(seg / 3600);
            int mi = (int)((seg - h * 3600.0) / 60);
            double estimado = calcularPrecio(e->tipo_vehiculo, e->hora_entrada, time(nullptr));

            cout << "\n  +--------------------------------------+\n";
            cout << "  |        VEHICULO ENCONTRADO           |\n";
            cout << "  +--------------------------------------+\n";
            cout << "  Placa      : " << e->placa << "\n";
            cout << "  Tipo       : " << nombreTipo(e->tipo_vehiculo) << "\n";
            cout << "  Espacio    : fila " << e->fila
                 << ", col " << e->col << "\n";
            cout << "  Entrada    : " << e->hora_entrada_texto << "\n";
            cout << "  Tiempo     : " << h << "h " << mi << "m\n";
            cout << "  Cobro est. : $"
                 << fixed << setprecision(0) << estimado << " pesos\n";
            cout << "  +--------------------------------------+\n\n";
            return;
        }
    }

    cout << "\n  No se encontro vehiculo con placa '" << placa << "'.\n";
}

void mostrarEstadisticas(Parqueadero *p) {
    cout << "\n  ========================================\n";
    cout << "       ESTADISTICAS DEL PARQUEADERO\n";
    cout << "  ========================================\n";
    cout << "  Total de espacios : " << p->cupos_totales << "\n";
    cout << "  Ocupados ahora    : " << p->ocupados << "\n";
    cout << "  Disponibles       : " << (p->cupos_totales - p->ocupados) << "\n";
    cout << "  ........................................\n";
    cout << "  Carros adentro    : " << p->carros_actuales << "\n";
    cout << "  Motos adentro     : " << p->motos_actuales  << "\n";
    cout << "  Bicis adentro     : " << p->bicis_actuales  << "\n";
    cout << "  ........................................\n";
    cout << "  Ingresos del dia  : $"
         << fixed << setprecision(0) << p->ingresos_dia << " pesos\n";
    cout << "  ========================================\n";

    if (p->ocupados == 0) {
        cout << "  (no hay vehiculos en este momento)\n";
    } else {
        cout << "\n  " << left
             << setw(12) << "TIPO"
             << setw(10) << "PLACA"
             << setw(12) << "ESPACIO"
             << setw(22) << "HORA ENTRADA"
             << "TIEMPO\n";
        cout << "  ........................................\n";

        for (int k = 0; k < p->cupos_totales; k++) {
            Espacio *e = &p->espacios[k];
            if (e->esta_ocupado == 0) continue;

            double seg = segundosAdentro(e);
            int h  = (int)(seg / 3600);
            int mi = (int)((seg - h * 3600.0) / 60);

            cout << "  " << left
                 << setw(12) << nombreTipo(e->tipo_vehiculo)
                 << setw(10) << e->placa
                 << "[" << setw(2) << e->fila << ","
                 << setw(2) << e->col << "]  "
                 << setw(22) << e->hora_entrada_texto
                 << h << "h " << mi << "m\n";
        }
    }
    cout << "  ========================================\n\n";
}

void ejecutarMenu(Parqueadero *p, char (*m)[COLS]) {
    int opcion = 0;
    do {
        cout << "\n  ====================================\n";
        cout << "       PARQUEADERO - MENU PRINCIPAL  \n";
        cout << "  ====================================\n";
        cout << "    1. Ver mapa del parqueadero\n";
        cout << "    2. Registrar ingreso de vehiculo\n";
        cout << "    3. Registrar salida y cobrar\n";
        cout << "    4. Ver estadisticas\n";
        cout << "    5. Buscar vehiculo por placa\n";
        cout << "    0. Salir del programa\n";
        cout << "  ====================================\n";
        cout << "  Opcion: ";
        cin  >> opcion;
        limpiarPantalla();

        if      (opcion == 1) mostrarMapa(m, p);
        else if (opcion == 2) ingresarVehiculo(p, m);
        else if (opcion == 3) pagarSalida(p, m);
        else if (opcion == 4) mostrarEstadisticas(p);
        else if (opcion == 5) buscarPorPlaca(p);
        else if (opcion == 0) cout << "\n  Hasta luego!\n\n";
        else                  cout << "\n  Opcion no valida, intente de nuevo.\n";

    } while (opcion != 0);
}

int main() {
    mostrarBienvenida();
    Parqueadero parqueadero;
    inicializarParqueadero(&parqueadero, mapa);
    ejecutarMenu(&parqueadero, mapa);
    return 0;
}
