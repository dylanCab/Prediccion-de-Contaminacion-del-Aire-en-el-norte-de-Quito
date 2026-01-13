#include <stdio.h>
#include "gestion_aire.h"
#include "validaciones.h"

int main() {
    // Definición de las 5 zonas. 
    // IMPORTANTE: Los nombres de archivo deben ser exactos a los que subiste a GitHub.
    Zona zonas[MAX_ZONAS] = {
        {"Cotocollao", "Cotocollao.txt"},
        {"Carapungo", "Carapungo.txt"},
        {"Belisario", "Belisario.txt"},
        {"Condado", "Condado.txt"},
        {"San Antonio", "SanAntonio.txt"}
    };

    printf("\n--- SISTEMA INTEGRAL DE GESTION AMBIENTAL (SIGP-AIRE) ---\n");
    printf("Iniciando carga de modulos y validacion de datos...\n\n");

    // Proceso Modular
    for (int i = 0; i < MAX_ZONAS; i++) {
        printf(" [+] Procesando zona: %-15s", zonas[i].nombreZona);
        
        cargarDatos(&zonas[i]);        // Lee y valida (usa validaciones.h)
        
        if (zonas[i].totalRegistros > 0) {
            calcularPromedio(&zonas[i]);    // Calcula media 30 días
            predecirNiveles(&zonas[i]);     // Algoritmo ponderado + clima
            printf(" -> OK (%d registros procesados)\n", zonas[i].totalRegistros);
        } else {
            printf(" -> ERROR (No se encontraron datos validos)\n");
        }
    }

    // Generación del reporte final con recomendaciones
    printf("\nGenerando reporte tecnico con alertas y recomendaciones...\n");
    generarReporte(zonas, MAX_ZONAS);

    printf("PROCESO COMPLETADO EXITOSAMENTE.\n");
    printf("Revise el archivo generado: 'reporte_final.txt'\n");
    
    return 0;
}