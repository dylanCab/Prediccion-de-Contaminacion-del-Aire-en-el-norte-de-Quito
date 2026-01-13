#include "gestion_aire.h"
#include "validaciones.h"
#include <string.h>
#include <stdlib.h>

// --- Funciones de Lectura y Cálculo (Misma lógica eficiente anterior) ---

void cargarDatos(Zona *z) {
    FILE *f = fopen(z->nombreArchivo, "r");
    if (f == NULL) {
        printf("Error: No se pudo leer %s. Verifique existencia.\n", z->nombreArchivo);
        z->totalRegistros = 0;
        return;
    }

    char cabecera[250];
    fgets(cabecera, sizeof(cabecera), f); // Ignora la cabecera descriptiva

    int i = 0;
    // Buffers para validación de datos
    char s_pm[20], s_co2[20], s_no2[20], s_so2[20], s_temp[20], s_hum[20], s_viento[20];

    while (fscanf(f, "%s %s %s %s %s %s %s %s", 
           z->historial[i].fecha, s_pm, s_co2, s_no2, s_so2, s_temp, s_hum, s_viento) != EOF) {
        
        // Validación y conversión segura usando tu librería
        z->historial[i].pm25 = esFlotantePositivo(s_pm) ? atof(s_pm) : 0.0;
        z->historial[i].co2  = esFlotantePositivo(s_co2) ? atof(s_co2) : 0.0;
        z->historial[i].no2  = esFlotantePositivo(s_no2) ? atof(s_no2) : 0.0;
        z->historial[i].so2  = esFlotantePositivo(s_so2) ? atof(s_so2) : 0.0;
        z->historial[i].temp = esFlotantePositivo(s_temp) ? atof(s_temp) : 0.0;
        z->historial[i].hum  = esFlotantePositivo(s_hum) ? atof(s_hum) : 0.0;
        z->historial[i].viento = esFlotantePositivo(s_viento) ? atof(s_viento) : 0.0;

        i++;
        if (i >= MAX_REGISTROS) break;
    }
    z->totalRegistros = i;
    fclose(f);
}

void calcularPromedio(Zona *z) {
    float suma = 0;
    int validos = 0;
    for (int i = 0; i < z->totalRegistros; i++) {
        if (z->historial[i].pm25 > 0) {
            suma += z->historial[i].pm25;
            validos++;
        }
    }
    z->promedioHistorico = (validos > 0) ? suma / validos : 0;
}

void predecirNiveles(Zona *z) {
    int n = z->totalRegistros;
    if (n < 3) {
        z->prediccion24h = (n > 0) ? z->historial[n-1].pm25 : 0;
        return;
    }
    
    // Algoritmo Ponderado: Tendencia reciente tiene más peso
    float p = (z->historial[n-1].pm25 * 0.5) + 
              (z->historial[n-2].pm25 * 0.3) + 
              (z->historial[n-3].pm25 * 0.2);
    
    // Factores climáticos (Si hay poco viento o mucha humedad, la contaminación sube)
    if (z->historial[n-1].viento < 5.0) p *= 1.15; // Factor de estancamiento
    if (z->historial[n-1].hum > 80.0) p *= 1.05;   // Factor de acumulación
    
    z->prediccion24h = p;
}

// --- NUEVA FUNCIÓN AUXILIAR PARA RECOMENDACIONES DETALLADAS ---
void escribirRecomendaciones(FILE *rep, float nivel) {
    if (nivel > LIMITE_PM25_OMS) {
        // Alerta: Nivel Crítico
        fprintf(rep, "  [!] ALERTA PREVENTIVA ACTIVADA: Nivel excede norma OMS (%.1f ug/m3)\n", LIMITE_PM25_OMS);
        fprintf(rep, "  MEDIDAS DE MITIGACION INMEDIATA:\n");
        fprintf(rep, "   1. SOCIAL (Salud Publica): Uso obligatorio de mascarillas en grupos vulnerables (niños/tercera edad).\n");
        fprintf(rep, "      - Suspender actividades fisicas escolares al aire libre.\n");
        fprintf(rep, "   2. CULTURAL/URBANO: Fomentar teletrabajo para reducir movilidad.\n");
        fprintf(rep, "      - Evitar uso de vehiculo privado (compartir auto o usar transporte publico).\n");
        fprintf(rep, "   3. AMBIENTAL: Prohibicion estricta de quema de basura o maleza en la zona.\n");
        fprintf(rep, "      - Riego de calles principales para asentar particulas de polvo.\n");
        fprintf(rep, "   4. GLOBAL: Notificar a entes de control para evaluacion de huella de carbono industrial.\n");
    } else if (nivel > (LIMITE_PM25_OMS * 0.8)) {
        // Alerta Naranja: Precaución
        fprintf(rep, "  [!] PRECAUCION: Niveles cercanos al limite (Zona de Riesgo).\n");
        fprintf(rep, "  RECOMENDACIONES:\n");
        fprintf(rep, "   - SOCIAL: Personas asmaticas deben limitar exposicion prolongada.\n");
        fprintf(rep, "   - CULTURAL: Uso racional del automovil (Revision de gases).\n");
    } else {
        // Nivel Verde: Óptimo
        fprintf(rep, "  [OK] ESTADO FAVORABLE: Calidad del aire dentro de parametros saludables.\n");
        fprintf(rep, "  ACCIONES DE MANTENIMIENTO:\n");
        fprintf(rep, "   - Fomentar uso de bicicleta y caminata recreativa en la zona.\n");
        fprintf(rep, "   - Mantenimiento de espacios verdes urbanos.\n");
    }
}

// --- FUNCIÓN PRINCIPAL DE REPORTE ACTUALIZADA ---
void generarReporte(Zona zonas[], int n) {
    FILE *rep = fopen("reporte_final.txt", "w");
    if (rep == NULL) {
        printf("Error al crear reporte_final.txt\n");
        return;
    }

    fprintf(rep, "================================================================\n");
    fprintf(rep, "   SISTEMA INTEGRAL DE GESTION AMBIENTAL (SIGP-AIRE v2.0)   \n");
    fprintf(rep, "   Reporte de Monitoreo y Alertas Preventivas               \n");
    fprintf(rep, "================================================================\n\n");

    for (int i = 0; i < n; i++) {
        if (zonas[i].totalRegistros == 0) continue;

        Registro actual = zonas[i].historial[zonas[i].totalRegistros - 1];
        
        fprintf(rep, "ZONA MONITOREADA: %s\n", zonas[i].nombreZona);
        fprintf(rep, "----------------------------------------------------------------\n");
        fprintf(rep, "DATOS ACTUALES (%s):\n", actual.fecha);
        fprintf(rep, "  PM2.5: %.2f | CO2: %.1f | Temp: %.1f C | Viento: %.1f km/h\n\n", 
                actual.pm25, actual.co2, actual.temp, actual.viento);
        
        fprintf(rep, "ANALISIS PREDICTIVO (Proximas 24 horas):\n");
        fprintf(rep, "  Promedio Historico (30 dias): %.2f ug/m3\n", zonas[i].promedioHistorico);
        fprintf(rep, "  Nivel Predicho Ponderado:     %.2f ug/m3\n\n", zonas[i].prediccion24h);

        fprintf(rep, "ESTADO Y RECOMENDACIONES:\n");
        // Llamada a la nueva función de recomendaciones detalladas
        escribirRecomendaciones(rep, zonas[i].prediccion24h);
        
        fprintf(rep, "\n________________________________________________________________\n\n");
    }
    
    // Pie de página del reporte
    fprintf(rep, "Nota Tecnica: Limite OMS establecido en %.1f ug/m3 para PM2.5 (24h).\n", LIMITE_PM25_OMS);
    fprintf(rep, "Analisis generado automaticamente considerando factores meteorologicos.\n");
    
    fclose(rep);
}