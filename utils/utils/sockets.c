#include "sockets.h"

int iniciar_servidor(t_log* logger, char* ip, char* puerto)
{
	int socket_servidor;

    struct addrinfo hints, *servinfo;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo(ip, puerto, &hints, &servinfo);

    if ((socket_servidor = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol)) == -1){
		log_warning(logger, "No se pudo crear el server correctamente. Levante el modulo nuevamente. Gracias, vuelvas prontos.");
		exit(-1);
	}

//    for (struct addrinfo *p = servinfo; p != NULL; p = p->ai_next)
//    {
//        socket_servidor = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
//        if (socket_servidor == -1)
//            continue;

    //Sin esto, el SO impide que se pueda reutilizar la direccion.
    int activado = 1;
    if(setsockopt(socket_servidor, SOL_SOCKET, SO_REUSEADDR, &activado, sizeof(activado))==-1){
    	log_warning(logger, "no se pudo crear el server correctamente. Levante el modulo nuevamente. Gracias, vuelvas prontos.");
    	exit(-1);
    }

    //Asociamos el socket con el puerto
	if (bind(socket_servidor, servinfo->ai_addr, servinfo->ai_addrlen) < 0) {
		log_warning(logger, "No se pudo crear el server correctamente. Levante el modulo nuevamente. Gracias, vuelvas prontos.");
		exit(-1);
	}

//        if (bind(socket_servidor, p->ai_addr, p->ai_addrlen) == -1) {
//            close(socket_servidor);
//            continue;
//        }
//        break;
//    }

	//Seteamos max conexiones posibles
    if(listen(socket_servidor, SOMAXCONN)){
    	log_warning(logger, "No se pudo crear el server correctamente. Levante el modulo nuevamente. Gracias, vuelvas prontos.");
    		exit(-1);
    }

    freeaddrinfo(servinfo);

    log_trace(logger, "Listo para escuchar a mi cliente");

    return socket_servidor;
}

int esperar_cliente(t_log* logger, int socket_servidor)
{
	struct sockaddr_in dir_cliente;
	socklen_t tam_direccion = sizeof(struct sockaddr_in);

	int socket_cliente;

	if((socket_cliente = accept(socket_servidor, (void*) &dir_cliente, &tam_direccion)) == -1){
		log_warning(logger, "No se pudo establecer la conexion con el cliente.");
	}

	log_info(logger, "Se conecto un cliente!");

	return socket_cliente;
}

// Esta de mas esto ??
/*
uint32_t conectar(char* ip, char* puerto){
	return crear_conexion(ip, puerto);
}*/

uint32_t crear_conexion(char* ip, char* puerto)
{
	struct addrinfo hints;
	struct addrinfo *server_info;
	uint32_t socket_cliente;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip, puerto, &hints, &server_info);

	socket_cliente = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);

	if(connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen) == -1){
		return -1;
	}

	freeaddrinfo(server_info);

	return socket_cliente;
}

void liberar_conexion(int* socket_cliente)
{
	close(*socket_cliente);
    *socket_cliente = -1;
}
