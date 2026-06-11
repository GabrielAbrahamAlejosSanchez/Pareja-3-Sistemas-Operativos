#!/bin/bash

# 1. Definir el directorio de destino y el formato de la fecha
LOG_DIR="./logs_auditoria"
TIMESTAMP=$(date +"%Y%m%d_%H%M%S")

# 2. Crear la carpeta para guardar los reportes si no existe
mkdir -p "$LOG_DIR"

echo "Iniciando la recolección de datos de la Sección B..."

# 3. Ejecutar comandos y redirigir la salida directamente a sus archivos de log
cat /proc/interrupts > "$LOG_DIR/interrupts_${TIMESTAMP}.log" 2>&1
cat /proc/iomem      > "$LOG_DIR/iomem_${TIMESTAMP}.log"      2>&1
cat /proc/ioports    > "$LOG_DIR/ioports_${TIMESTAMP}.log"    2>&1

# 4. Mostrar confirmación en pantalla
echo "¡Proceso finalizado con éxito!"
echo "Archivos guardados en $LOG_DIR:"
ls -l "$LOG_DIR" | grep "$TIMESTAMP"