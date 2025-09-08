import numpy as np
import matplotlib.pyplot as plt

# Función que lee cada archivo, calcula media y desviación estándar, y guarda datos para poder graficar
def process_files(archivos):
    for nombre, distancia in archivos:
        try:
            # Carga los datos del archivo de texto directamente a un array de numpy
            datos = np.loadtxt(nombre)

            # Calculamos media y desviación estándar
            media = np.mean(datos)
            desviacion = np.std(datos)

            print(f"Resultados para el archivo '{nombre}' (Distancia real de {distancia} cm):")
            print(f"  Media: {media:.2f} cm")
            print(f"  Desviación Estándar: {desviacion:.2f} cm\n")

            # Guardamos los resultados para graficar
            distancias_reales.append(distancia)
            lista_medias.append(media)
            lista_desviaciones.append(desviacion)

        except FileNotFoundError:
            print(f"Error: No se encontró el archivo '{nombre}'. Asegúrate de que esté en la misma carpeta que el script.")
        except Exception as e:
            print(f"Ocurrió un error procesando el archivo '{nombre}': {e}")

# Función que genera la gráfica para mostrar los resultados
def plot_results(titulo):
    if not distancias_reales:
        print("No se procesaron datos, no se puede generar la gráfica.")
        return

    x = distancias_reales
    y = lista_medias
    error = lista_desviaciones

    plt.figure(figsize=(10, 6))

    # Gráfica de errorbar
    plt.errorbar(x, y, yerr=error, fmt='o', color='blue', ecolor='lightblue', elinewidth=3, capsize=5, label='Mediciones del Sonar')

    # Línea de referencia con la medida que debiera ser
    min_val = min(x) if x else 0
    max_val = max(x) if x else 100
    ideal_line = np.linspace(min_val - 5, max_val + 5, 100)
    plt.plot(ideal_line, ideal_line, 'r--', label='Medición Ideal (Real = Medida)')

    plt.title(titulo)
    plt.xlabel('Distancia Real (cm)')
    plt.ylabel('Distancia Medida (Media, cm)')
    plt.legend()
    plt.grid(True)
    plt.show()
    
if __name__ == '__main__':
    # Listas para almacenar los resultados
    distancias_reales = []
    lista_medias = []
    lista_desviaciones = []

    # Obtenemos los archivos y la distancia que mide para las mediciones con objeto sólido
    archivos = [
        ('mediciones/10cm.txt', 10),
        ('mediciones/30cm.txt', 30),
        ('mediciones/50cm.txt', 50),
        ('mediciones/100cm.txt', 100),
    ]

    process_files(archivos)
    plot_results("Mediciones de objeto solido")
    
    # Listas para almacenar los resultados
    distancias_reales = []
    lista_medias = []
    lista_desviaciones = []

    # Obtenemos los archivos y la distancia que mide para las mediciones con objeto sólido
    archivos_trapo = [
        ('mediciones/10cm_trapo.txt', 10),
        ('mediciones/30cm_trapo.txt', 30),
        ('mediciones/50cm_trapo.txt', 50),
        ('mediciones/100cm_trapo.txt', 100),
    ]

    process_files(archivos_trapo)
    plot_results("Mediciones de trapo")
