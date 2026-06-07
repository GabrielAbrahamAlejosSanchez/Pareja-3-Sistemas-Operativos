#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>         // Necesario para register_chrdev y file_operations
#include <linux/uaccess.h>    // Necesario para la función copy_to_user

#define DEVICE_NAME "iochardevVilchez"
#define AUTHOR_NAME "Josue_Vilchez"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Josue Vilchez");
MODULE_DESCRIPTION("Módulo de registro de dispositivo de caracteres iochardev");

// Variables globales del módulo
static int major_number;
static char msg[100];
static short msg_size;

// Prototipo de la función de lectura
static ssize_t dev_read(struct file *filep, char __user *buffer, size_t len, loff_t *offset);

// Estructura que asocia las operaciones del dispositivo con nuestras funciones
static struct file_operations fops =
{
    .read = dev_read,
};

// Función de inicialización
int init_module(void)
{
    // 1. Registrar el dispositivo con un Major dinámico (0)
    major_number = register_chrdev(0, DEVICE_NAME, &fops);

    if (major_number < 0) {
        printk(KERN_ALERT "iochardevVilchez falló al registrar con el error %d\n", major_number);
        return major_number;
    }

    // 2. Preparar el mensaje que devolverá la operación read()
    msg_size = sprintf(msg, "Autor: Josue Vilchez | Major: %d\n", major_number);

    // 3. Imprimir los datos reales del subsistema asignado
    printk(KERN_INFO "iochardevVilchez: Subsistema de dispositivo de caracteres registrado correctamente.\n");
    printk(KERN_INFO "iochardevVilchez: Dispositivo asignado al Major dinámico: %d\n", major_number);
    
    return 0; // Carga exitosa
}

// Función de limpieza
void cleanup_module(void)
{
    // Liberar el dispositivo de caracteres
    unregister_chrdev(major_number, DEVICE_NAME);
    
    // Mensaje de descarga con el nombre del autor
    printk(KERN_INFO "Modulo iochardevVilchez descargado correctamente. Autor: Josue Vilchez.\n");
}

// Operación read() solicitada
static ssize_t dev_read(struct file *filep, char __user *buffer, size_t len, loff_t *offset)
{
    int bytes_read = 0;

    // Si ya llegamos al final del mensaje, enviamos 0 (EOF) para que cat termine
    if (*offset >= msg_size) {
        return 0;
    }

    // Ajustar la longitud si el usuario pide más bytes de los que quedan
    if (len > msg_size - *offset) {
        len = msg_size - *offset;
    }

    // copy_to_user mueve los datos del espacio del kernel al espacio del usuario de forma segura
    if (copy_to_user(buffer, msg + *offset, len)) {
        return -EFAULT; // Error de acceso a memoria
    }

    // Actualizar punteros de lectura
    *offset += len;
    bytes_read = len;

    return bytes_read;
}
