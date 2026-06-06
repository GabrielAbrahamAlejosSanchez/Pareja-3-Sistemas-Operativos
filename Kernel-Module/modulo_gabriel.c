#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>  //Crucial para copy_to_user

#define DEVICE_NAME "io_chardev_Alejos"
#define AUTHOR_NAME "Gabriel_Alejos"

MODULE_AUTHOR(AUTHOR_NAME);
MODULE_DESCRIPTION("MODULO DE DISPOSITIVO DE CARACTERES CON MAJOR DINAMICO");
MODULE_LICENSE("GPL");

static int major_number;
static char message[256];
static short message_size;

//Funcion de lectura del dispositivo

static ssize_t dev_read(struct file  *filep, char __user *buffer, size_t len, loff_t *offset) {
	int uncopied_bytes = 0;

//Si el offset ya alcanzo o supero el tamano del mensaje, enviamos EOF (0)

if(*offset >= message_size) {
return 0;
}

//Ajustamos la longitud para no leer fuera de los limites de nuestro string

if(len > message_size - *offset) {
	len = message_size - *offset;
}

//copy_to_user(destino, origen, cantidad) transfiere los datos al espacio de usuario
uncopied_bytes = copy_to_user(buffer, message + *offset, len);

if(uncopied_bytes == 0){
	*offset += len; //actualizamos el offset
	return len; //retornamos los bytes leidos
}else{
	return -EFAULT; //Error de acceso a la memoria

}

}


//Esctructura de operaciones del archivo

static struct file_operations fops = {
	.read = dev_read,
};

//Funcion de inicializacion

int init_module(void){

//Registrar el dispositivo con Major 0 para que el kernel asigne uno dinamico

major_number = register_chrdev(0, DEVICE_NAME, &fops);

if(major_number < 0){

	printk(KERN_ALERT "FALLO AL REGISTRAR EL DISPOSITIVO %s CON ERROR %d\n", DEVICE_NAME, major_number);
	return major_number;
}

//Preparamos el mensaje que read() devolvera mas adelante

message_size = sprintf(message, "Autor: %s | Major: %d\n|", AUTHOR_NAME, major_number);

//Imprimir datos reales del subsistema (major devuelto) 

printk(KERN_INFO "dispositivo '%s' registado. \n", DEVICE_NAME);
printk(KERN_INFO "major dinamico asignado por el subsistema: %d\n", major_number);

	return 0;
}


//Funcion de limpieza/descarga

void cleanup_module(void){
	unregister_chrdev(major_number, DEVICE_NAME);
	printk(KERN_INFO "Modulo descargado. Autor: %s\n", AUTHOR_NAME);
}


