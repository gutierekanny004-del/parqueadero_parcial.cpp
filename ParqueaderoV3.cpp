#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <fstream>
#include <ctime>

using namespace std;

const int FILAS = 18;
const int COLS = 18;
const int MAX_CUPOS = 50;

const int PAGO_CARRO = 5000;
const int PAGO_MOTO  = 2500;
const int PAGO_BICI  = 1000;

const int TIPO_CARRO = 1;
const int TIPO_MOTO  = 2;
const int TIPO_BICI  = 3;

const string ARCHIVO = "Informe.txt";

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

void limpiarPantalla() { cin.ignore(1000, '\n'); }

string obtenerHora(time_t t) {
    struct tm *info = localtime(&t);
    char buf[30];
    sprintf(buf, "%02d:%02d:%02d  %02d/%02d/%04d",
        info->tm_hour, info->tm_min, info->tm_sec,
        info->tm_mday, info->tm_mon + 1, info->tm_year + 1900);
    return string(buf);
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
    cout << "\n+==========================================+\n";
    cout << "|       SISTEMA DE PARQUEADERO v2.0        |\n";
    cout << "+==========================================+\n";
    cout << "|  Fecha : " << setfill('0')
         << setw(2) << t->tm_mday << "/" << setw(2) << (t->tm_mon + 1) << "/" << (t->tm_year + 1900)
         << "                        |\n";
    cout << "|  Hora  : "
         << setw(2) << t->tm_hour << ":" << setw(2) << t->tm_min << ":" << setw(2) << t->tm_sec
         << "                           |\n";
    cout << "+==========================================+\n\n";
    cout << setfill(' ');
}

//Guardar
void guardarEstado(Parqueadero *p) {
    ofstream archivo(ARCHIVO);
    if (!archivo.is_open()) { cout << "\nNo se pudo guardar el estado.\n"; return; }

    archivo << fixed << setprecision(2) << p->ingresos_dia << "\n";
    for (int k = 0; k < p->cupos_totales; k++) {
        Espacio *e = &p->espacios[k];
        if (e->esta_ocupado == 1) {
            archivo << e->tipo_vehiculo << " " << e->placa << " "
                    << e->hora_entrada  << " " << e->fila << " " << e->col << "\n";
        }
    }
    archivo.close();
    cout << "\nEstado guardado en '" << ARCHIVO << "'.\n";
}

//Cargar archivo txt
void cargarEstado(Parqueadero *p, char (*m)[COLS]) {
    ifstream archivo(ARCHIVO);
    if (!archivo.is_open()) return;

    archivo >> p->ingresos_dia;

    int tipo, fila, col;
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
                m[fila][col] = 'O';
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
        cout << "\nSe restauraron " << restaurados << " vehiculo(s) del estado anterior.\n";
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
            char c = m[i][j];
    
            if (c == 'C' || c == 'M' || c == 'B') {
                Espacio *e = &p->espacios[idx];
                e->esta_ocupado = 0;
    
                // Reemplazo del operador ternario por bloques if/else
                if (c == 'C') {
                    e->tipo_vehiculo = TIPO_CARRO;
                } else if (c == 'M') {
                    e->tipo_vehiculo = TIPO_MOTO;
                } else {
                    e->tipo_vehiculo = TIPO_BICI;
                }
    
                e->placa = "";
                e->hora_entrada = 0;
                e->hora_entrada_texto = "";
                e->fila = i;
                e->col = j;
    
                idx++;
                p->cupos_totales++;
            }
        }
    }

    cargarEstado(p, m);
}

void mostrarMapa(char (*m)[COLS], Parqueadero *p) {
    cout << "\n--- MAPA DEL PARQUEADERO ---\n\n";
    cout << "   ";
    for (int j = 0; j < COLS; j++) cout << setw(2) << j << " ";
    cout << "\n";

    for (int i = 0; i < FILAS; i++) {
        cout << setw(2) << i << " ";
        for (int j = 0; j < COLS; j++) {
            char c = m[i][j];
            if (c == 'O') {
                string etiqueta = "???";
                for (int k = 0; k < p->cupos_totales; k++) {
                    Espacio *e = &p->espacios[k];
                    if (e->fila == i && e->col == j && e->esta_ocupado == 1) {
                        etiqueta = e->placa.substr(0, 3);
                        while ((int)etiqueta.size() < 3) etiqueta += ' ';
                        break;
                    }
                }
                cout << "[" << etiqueta << "]";
            } else {
                cout << " " << c << " ";
            }
        }
        cout << "\n";
    }
    cout << "\nOcupados: " << p->ocupados
         << "  Libres: " << (p->cupos_totales - p->ocupados)
         << "  Total: " << p->cupos_totales << "\n\n";
}

int elegirTipoVehiculo() {
    int op;
    cout << "\nTipo de vehiculo:\n";
    cout << "  1. Carro  ($" << PAGO_CARRO << "/hora)\n";
    cout << "  2. Moto   ($" << PAGO_MOTO  << "/hora)\n";
    cout << "  3. Bici   ($" << PAGO_BICI  << "/hora)\n";
    cout << "  0. Cancelar\n";
    cout << "Opcion: ";
    cin >> op; limpiarPantalla();
    if (op == 1) return TIPO_CARRO;
    if (op == 2) return TIPO_MOTO;
    if (op == 3) return TIPO_BICI;
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
    
    // Si el tiempo es menor a 10 minutos (10/60 de hora), se cobra el mínimo de 10 min
    if (horas < (10.0 / 60.0)) {
        horas = 10.0 / 60.0;
    }

    double tarifa;

    // Sustitución de los operadores ternarios por condicionales simples
    if (tipo == TIPO_CARRO) {
        tarifa = PAGO_CARRO;
    } else if (tipo == TIPO_MOTO) {
        tarifa = PAGO_MOTO;
    } else {
        // Por defecto se asume TIPO_BICI si no es carro ni moto
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
        cout << "\nNo hay espacios disponibles para ese tipo de vehiculo.\n";
        return 0;
    }

    string placa;
    cout << "\nIngrese la placa: ";
    cin >> placa; limpiarPantalla();

    for (int k = 0; k < p->cupos_totales; k++) {
        if (p->espacios[k].esta_ocupado == 1 && p->espacios[k].placa == placa) {
            cout << "\nERROR: La placa '" << placa << "' ya esta registrada adentro.\n";
            return 0;
        }
    }

    Espacio *e = &p->espacios[idx];
    e->esta_ocupado          = 1;
    e->placa            = placa;
    e->hora_entrada     = time(nullptr);
    e->hora_entrada_texto = obtenerHora(e->hora_entrada);
    m[fila][col] = 'O';

    p->ocupados++;
    if (tipo == TIPO_CARRO)     p->carros_actuales++;
    else if (tipo == TIPO_MOTO) p->motos_actuales++;
    else                        p->bicis_actuales++;

    cout << "\nIngreso registrado.\n";
    cout << "Vehiculo: " << nombreTipo(tipo) << " - " << placa << "\n";
    cout << "Espacio : fila " << fila << ", columna " << col << "\n";
    cout << "Entrada : " << e->hora_entrada_texto << "\n";

    guardarEstado(p);
    return 1;
}

void procesarSalida(Espacio *e, Parqueadero *p, char (*m)[COLS]) {
    time_t salida = time(nullptr);
    double total  = calcularPrecio(e->tipo_vehiculo, e->hora_entrada, salida);

    double seg = difftime(salida, e->hora_entrada);
    int h = (int)(seg / 3600);
    int mi = (int)((seg - h * 3600.0) / 60);
    int s = (int)seg % 60;

    cout << "\n--- RECIBO ---\n";
    cout << "Tipo   : " << nombreTipo(e->tipo_vehiculo) << "\n";
    cout << "Placa  : " << e->placa << "\n";
    cout << "Entrada: " << e->hora_entrada_texto << "\n";
    cout << "Salida : " << obtenerHora(salida) << "\n";
    cout << "Tiempo : " << h << "h " << mi << "m " << s << "s\n";
    cout << "TOTAL  : $" << fixed << setprecision(0) << total << " pesos\n";
    cout << "--------------\n";

    p->ingresos_dia += total;
    cout << "Ingresos del dia: $" << fixed << setprecision(0) << p->ingresos_dia << " pesos\n";

    if (e->tipo_vehiculo == TIPO_CARRO)     m[e->fila][e->col] = 'C';
    else if (e->tipo_vehiculo == TIPO_MOTO) m[e->fila][e->col] = 'M';
    else                                    m[e->fila][e->col] = 'B';

    p->ocupados--;
    if (e->tipo_vehiculo == TIPO_CARRO)     p->carros_actuales--;
    else if (e->tipo_vehiculo == TIPO_MOTO) p->motos_actuales--;
    else                                    p->bicis_actuales--;

    e->esta_ocupado = 0; e->placa = ""; e->hora_entrada = 0; e->hora_entrada_texto = "";
    guardarEstado(p);
}

int pagarSalida(Parqueadero *p, char (*m)[COLS]) {
    if (p->ocupados == 0) { cout << "\nEl parqueadero esta vacio.\n"; return 0; }

    int tipo = elegirTipoVehiculo();
    if (tipo == -1) return 0;

    int lista[MAX_CUPOS];
    int cantidad = 0;
    for (int k = 0; k < p->cupos_totales; k++) {
        if (p->espacios[k].esta_ocupado == 1 && p->espacios[k].tipo_vehiculo == tipo)
            lista[cantidad++] = k;
    }

    if (cantidad == 0) { cout << "\nNo hay vehiculos de ese tipo adentro.\n"; return 0; }

    cout << "\n--- VEHICULOS EN EL PARQUEADERO ---\n";
    cout << left << setw(5) << "No." << setw(10) << "PLACA"
         << setw(12) << "ESPACIO" << setw(22) << "ENTRADA" << "COBRO EST.\n";
    cout << string(55, '-') << "\n";

    for (int i = 0; i < cantidad; i++) {
        Espacio *e = &p->espacios[lista[i]];
        double est = calcularPrecio(e->tipo_vehiculo, e->hora_entrada, time(nullptr));
        cout << left << setw(5) << (i + 1) << setw(10) << e->placa
             << "[" << setw(2) << e->fila << "," << setw(2) << e->col << "]  "
             << setw(22) << e->hora_entrada_texto
             << "$" << fixed << setprecision(0) << est << "\n";
    }
    cout << "  0. Cancelar\n";

    int sel;
    cout << "\nSeleccione el numero: ";
    cin >> sel; limpiarPantalla();

    if (sel == 0) { cout << "\nCancelado.\n"; return 0; }
    if (sel < 1 || sel > cantidad) { cout << "\nNumero invalido.\n"; return 0; }

    Espacio *elegido = &p->espacios[lista[sel - 1]];

    cout << "\nConfirma la salida de " << elegido->placa << "? (S/N): ";
    char c; cin >> c; limpiarPantalla();
    if (c != 'S' && c != 's') { cout << "\nSalida cancelada.\n"; return 0; }

    procesarSalida(elegido, p, m);
    return 1;
}

void buscarPorPlaca(Parqueadero *p) {
    if (p->ocupados == 0) { cout << "\nNo hay vehiculos adentro.\n"; return; }

    string placa;
    cout << "\nIngrese la placa a buscar: ";
    cin >> placa; limpiarPantalla();

    for (int k = 0; k < p->cupos_totales; k++) {
        Espacio *e = &p->espacios[k];
        if (e->esta_ocupado == 1 && e->placa == placa) {
            double seg = segundosAdentro(e);
            int h = (int)(seg / 3600);
            int mi = (int)((seg - h * 3600.0) / 60);
            double est = calcularPrecio(e->tipo_vehiculo, e->hora_entrada, time(nullptr));

            cout << "\n--- VEHICULO ENCONTRADO ---\n";
            cout << "Placa     : " << e->placa << "\n";
            cout << "Tipo      : " << nombreTipo(e->tipo_vehiculo) << "\n";
            cout << "Espacio   : fila " << e->fila << ", col " << e->col << "\n";
            cout << "Entrada   : " << e->hora_entrada_texto << "\n";
            cout << "Tiempo    : " << h << "h " << mi << "m\n";
            cout << "Cobro est.: $" << fixed << setprecision(0) << est << " pesos\n";
            cout << "---------------------------\n\n";
            return;
        }
    }
    cout << "\nNo se encontro vehiculo con placa '" << placa << "'.\n";
}

void mostrarEstadisticas(Parqueadero *p) {
    cout << "\n--- ESTADISTICAS ---\n";
    cout << "Total espacios: " << p->cupos_totales << "\n";
    cout << "Ocupados      : " << p->ocupados << "\n";
    cout << "Disponibles   : " << (p->cupos_totales - p->ocupados) << "\n";
    cout << "Carros        : " << p->carros_actuales << "\n";
    cout << "Motos         : " << p->motos_actuales  << "\n";
    cout << "Bicis         : " << p->bicis_actuales  << "\n";
    cout << "Ingresos dia  : $" << fixed << setprecision(0) << p->ingresos_dia << " pesos\n";

    if (p->ocupados > 0) {
        cout << "\n" << left << setw(12) << "TIPO" << setw(10) << "PLACA"
             << setw(12) << "ESPACIO" << setw(22) << "HORA ENTRADA" << "TIEMPO\n";
        for (int k = 0; k < p->cupos_totales; k++) {
            Espacio *e = &p->espacios[k];
            if (e->esta_ocupado == 0) continue;
            double seg = segundosAdentro(e);
            int h = (int)(seg / 3600);
            int mi = (int)((seg - h * 3600.0) / 60);
            cout << left << setw(12) << nombreTipo(e->tipo_vehiculo)
                 << setw(10) << e->placa
                 << "[" << setw(2) << e->fila << "," << setw(2) << e->col << "]  "
                 << setw(22) << e->hora_entrada_texto
                 << h << "h " << mi << "m\n";
        }
    }
    cout << "--------------------\n\n";
}

void ejecutarMenu(Parqueadero *p, char (*m)[COLS]) {
    int op = 0;
    do {
        cout << "\n--- MENU ---\n";
        cout << "1. Ver mapa\n2. Registrar ingreso\n3. Registrar salida\n";
        cout << "4. Estadisticas\n5. Buscar por placa\n0. Salir\n";
        cout << "Opcion: ";
        cin >> op; limpiarPantalla();

        if      (op == 1) mostrarMapa(m, p);
        else if (op == 2) ingresarVehiculo(p, m);
        else if (op == 3) pagarSalida(p, m);
        else if (op == 4) mostrarEstadisticas(p);
        else if (op == 5) buscarPorPlaca(p);
        else if (op == 0) cout << "\nHasta luego.\n";
        else              cout << "\nOpcion invalida.\n";
    } while (op != 0);
}

int main() {
    mostrarBienvenida();
    Parqueadero parqueadero;
    inicializarParqueadero(&parqueadero, mapa);
    ejecutarMenu(&parqueadero, mapa);
    return 0;
}
