#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

struct datos_header
{
    int ancho_imagen;
    int alto_imagen;
    int padding;
    int stride;
}dh;

void convertir_imagen_gris_paralelo(FILE *archivo_entrada, FILE *archivo_salida,int ancho_imagen, int alto_imagen, int padding);
void convertir_imagen_gris_paralelo_pgm(FILE *archivo_entrada, FILE *archivo_salida,int ancho_imagen, int alto_imagen, int padding);
void obtener_datos_header_archivo_entrada (unsigned char header[] , FILE *archivo_entrada);
void mostrar_datos_header_archivo_entrada();

int main(int argc, char *argv[])
{
    if (argc < 2){
        printf("No se le paso el nombre del archivo\n");
        return 1;
    }

    unsigned char header[54];
    
    FILE *archivo_entrada = fopen(argv[1], "rb");

    if (!archivo_entrada)
    {
        printf("Error al abrir el archivo.\n");
        return 1;
    }
    char *nombre_completo;
    nombre_completo = argv[1];
    char delimitador[] = ".";
    char *nombre_archivo_modificado = strtok(nombre_completo, delimitador);
    FILE *archivo_salida;
    
    printf("El nombre del archivo es: %s\n", argv[1]); 
    
    if(argc > 2 && *argv[2] == 'b'){
        strcat(nombre_archivo_modificado, "_gris.bmp");
        archivo_salida = fopen(nombre_archivo_modificado, "wb");
        fread(header, sizeof(unsigned char), 54, archivo_entrada);
        fwrite(header, sizeof(unsigned char), 54, archivo_salida);

        //obtener y mostrar datos del header del archivo de entrada [imagen a color]
        obtener_datos_header_archivo_entrada(header,archivo_entrada);
        mostrar_datos_header_archivo_entrada(dh.ancho_imagen,dh.alto_imagen,dh.stride,dh.padding);

        //convertir imagen a escala de grises.
        convertir_imagen_gris_paralelo(archivo_entrada, archivo_salida,dh.ancho_imagen, dh.alto_imagen,dh.padding);
    }else{
    
            printf("%s\n", argv[2]);
            strcat(nombre_archivo_modificado, ".pgm"); 
            archivo_salida = fopen(nombre_archivo_modificado, "wb");
            fread(header, sizeof(unsigned char), 54, archivo_entrada);
            fprintf(archivo_salida, "P2\n");   
            fprintf(archivo_salida, "%d %d\n", *(int*)&header[18], abs(*(int*)&header[22])); 
            fprintf(archivo_salida, "255\n"); 
        
        //for(int i = 0; i < 101; i++){

        //obtener y mostrar datos del header del archivo de entrada [imagen a color]
            obtener_datos_header_archivo_entrada(header,archivo_entrada);
            mostrar_datos_header_archivo_entrada(dh.ancho_imagen,dh.alto_imagen,dh.stride,dh.padding);

        //convertir imagen a escala de grises.
            convertir_imagen_gris_paralelo_pgm(archivo_entrada, archivo_salida,dh.ancho_imagen, dh.alto_imagen,dh.padding);
        //}
    }    

    return 0;
}

void convertir_imagen_gris_paralelo(FILE *archivo_entrada, FILE *archivo_salida,int ancho_imagen, int alto_imagen, int padding){
    
    unsigned char pixel_imagen_entrada[3];
    int hilo=0;
    
    omp_set_num_threads(2);
    int max_threads = omp_get_max_threads();
    double duracion_ejecucion = omp_get_wtime();
     #pragma omp parallel  private(hilo,pixel_imagen_entrada)
    {
        hilo = omp_get_thread_num();
        int inicio = hilo * alto_imagen  / max_threads;
        int final = (hilo + 1) * alto_imagen / max_threads;
        
        for (int i = inicio; i < final; ++i){
            for (int j=0 ;j< ancho_imagen ; j++){
                fread(pixel_imagen_entrada, 3, 1, archivo_entrada);
                unsigned char gray = pixel_imagen_entrada[0] * 0.3 + pixel_imagen_entrada[1] * 0.58 + pixel_imagen_entrada[2]* 0.11;
                memset(pixel_imagen_entrada, gray, sizeof(pixel_imagen_entrada));
                fwrite(&pixel_imagen_entrada, 3, 1, archivo_salida);
        
            }
            fread(pixel_imagen_entrada, padding, 1, archivo_entrada);
            fwrite(pixel_imagen_entrada, padding, 1, archivo_salida);
         }
    }
    duracion_ejecucion= omp_get_wtime() - duracion_ejecucion;
    printf("Duracion: %f\n\n", duracion_ejecucion);
    fclose(archivo_salida);
    fclose(archivo_entrada);
}

void convertir_imagen_gris_paralelo_pgm(FILE *archivo_entrada, FILE *archivo_salida,int ancho_imagen, int alto_imagen, int padding){
    
    unsigned char pixel_imagen_entrada[3];
    int hilo=0, inicio, final;
    
    omp_set_num_threads(2);

    int max_threads = omp_get_max_threads();
    //printf("Hilos: %d\n", max_threads);
    double duracion_ejecucion = omp_get_wtime();
    #pragma omp parallel  private(hilo,pixel_imagen_entrada, inicio, final)
    {
        hilo = omp_get_thread_num();
        
        inicio = hilo * alto_imagen  / max_threads;
        final = (hilo + 1) * alto_imagen / max_threads;
        
        for (int i = inicio; i < final; ++i){
            for (int j=0 ;j< ancho_imagen ; j++){
                fread(pixel_imagen_entrada, 3, 1, archivo_entrada);
                unsigned char gray = pixel_imagen_entrada[0] * 0.3 + pixel_imagen_entrada[1] * 0.58 + pixel_imagen_entrada[2]* 0.11;
                memset(pixel_imagen_entrada, gray, sizeof(pixel_imagen_entrada));
                fprintf(archivo_salida, "%d ", gray);
        
            }
            fread(pixel_imagen_entrada, padding, 1, archivo_entrada);
            fprintf(archivo_salida, "\n");
         }
    }
    duracion_ejecucion = omp_get_wtime() - duracion_ejecucion;
    printf("Duracion: %f\n\n", duracion_ejecucion);
    fclose(archivo_salida);
    fclose(archivo_entrada);
}

void obtener_datos_header_archivo_entrada(unsigned char header[] , FILE *archivo_entrada){

    dh.ancho_imagen = *(int*)&header[18];
    dh.alto_imagen = abs(*(int*)&header[22]);
    dh.stride = (dh.ancho_imagen * 3 + 3) & ~3;
    dh.padding = dh.stride - dh.ancho_imagen * 3;

    fread(header, sizeof(unsigned char), 54, archivo_entrada);

}

void mostrar_datos_header_archivo_entrada(int ancho_imagen, int alto_imagen , int stride , int padding){

    printf("Ancho de la imagen: %d (%d)\n", ancho_imagen, ancho_imagen * 3);
    printf("Alto de la imagen: %d\n", alto_imagen);
    printf("stride: %d\n", stride);
    printf("padding: %d\n", padding);

}

