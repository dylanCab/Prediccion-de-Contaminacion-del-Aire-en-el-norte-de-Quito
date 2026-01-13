#ifndef GESTION_AIRE_H
#define GESTION_AIRE_H

#include <stdio.h>

// --- CONSTANTES DEL SISTEMA ---
#define MAX_ZONAS 5
#define MAX_REGISTROS 100      // Límite de días en memoria
#define LIMITE_PM25_OMS 15.0   // Límite de alerta (OMS 2021)

// --- ESTRUCTURAS DE DATOS ---
typedef struct {
    char fecha[12];
    float pm25, co2, no2, so2;
    float temp, hum, viento;
} Registro;

typedef struct {
    char nombreZona[30];
    char nombreArchivo[35];
    Registro historial[MAX_REGISTROS]; // Arreglo anidado
    int totalRegistros;
    float promedioHistorico;
    float prediccion24h;
} Zona;

// --- PROTOTIPOS DE FUNCIONES ---
// Reciben punteros (*z) para optimizar memoria y velocidad
void cargarDatos(Zona *z);
void calcularPromedio(Zona *z);
void predecirNiveles(Zona *z);
void generarReporte(Zona zonas[], int n);

#endif