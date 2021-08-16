#include "filesystem.h"

void sincronizar_blocks(){
	while(1){
		sleep(tiempo_sincronizacion);
		memcpy(contenido_blocks,contenido_blocks_aux,block_size * blocks);
		if(msync(contenido_blocks,block_size * blocks,MS_SYNC)==-1)
			log_error(logger, "Error con la sincronizacion de blocks");
		else
			log_info(logger, "Se sincronizo correctamente blocks");
	}
}

void inicializar_paths_aux(){
	if (!directorio_existe(punto_montaje))
		mkdir(punto_montaje, 0700);

	path_superbloque = string_new();
	path_blocks = string_new();
	path_files = string_new();
	path_bitacoras = string_new();

	pthread_mutex_init(&mutex_bitmap, NULL);
	pthread_mutex_init(&mutex_hash, NULL);
	pthread_mutex_init(&mutex_Tareas, NULL);

	string_append_with_format(&path_superbloque, "%s/SuperBloque.ims", punto_montaje);
	string_append_with_format(&path_blocks, "%s/Blocks.ims", punto_montaje);
	string_append_with_format(&path_files, "%s/Files", punto_montaje);
	string_append_with_format(&path_bitacoras, "%s/Bitacoras", path_files);

	if (!directorio_existe(path_files)) {
		mkdir(path_files, 0700);
	}

	if (!directorio_existe(path_bitacoras)) {
		mkdir(path_bitacoras, 0700);
	}

}

void crear_superbloque() {
	FILE* superbloque = fopen(path_superbloque,"wb");
	t_bitarray* bitarray = crear_bitmap();

	fwrite(&block_size,sizeof(uint32_t),1,superbloque);
	fwrite(&blocks,sizeof(uint32_t),1,superbloque);
	fwrite(bitarray->bitarray, bitarray->size, 1,superbloque);

	free(bitarray);

	fclose(superbloque);
}

void obtener_superbloque(){
	FILE* superbloque = fopen(path_superbloque, "r");

	if(superbloque){
		log_info(logger, "MONGO-STORE :: SE LEE EL SUPERBLOQUE");
		leer_superbloque(superbloque);
		fclose(superbloque);
	}
	else{
		log_info(logger, "MONGO-STORE :: NO SE PUDO LEER EL SUPERBLOQUE");
		exit(0);
	}
}

void leer_superbloque(FILE* archivo){
	FILE* superbloque = fopen(path_superbloque,"rb");

	fread(&block_size,sizeof(uint32_t),1,superbloque);
	fread(&blocks,sizeof(uint32_t),1,superbloque);
	bitarray_string = malloc(blocks / 8);
	fread(bitarray_string,blocks/8,1,superbloque);	// TODO hara falta leer el bitarray? eso se lee en el momento por lo gral

//	t_bitarray* bitarray = bitarray_create(bitarray_string,blocks/8);

	fclose(superbloque);
}

char* dar_hash_md5(char* archivo){
	char* system_command = string_new();
	char* path_aux = string_duplicate("aux_hash.tmp");
	string_append_with_format(&system_command, "md5sum %s > %s", archivo,path_aux);
	system(system_command);
	free(system_command);

	int archivo_aux = open(path_aux, O_RDWR);
	if(archivo_aux == -1){
		log_error(logger, "no se abrio archivo");
	}
	struct stat file_st;
	fstat(archivo_aux, &file_st);

//	log_info(logger, "file_st.st_size %i", file_st.st_size);
	char* contenido = malloc(file_st.st_size+1);
	read(archivo_aux,contenido,file_st.st_size);
//	log_info(logger, "contenido %s", contenido);

	char** hash = string_split(contenido," ");

	eliminar_archivo(path_aux);

	return hash[0];
}

char* obtener_hashmd5_string(char* stream){
	pthread_mutex_lock(&mutex_hash);

	char* path_aux = string_duplicate("aux.tmp");

	FILE* archivo_aux = fopen(path_aux, "w+");
	if(!archivo_aux){
		log_error(logger, "no se abrio archivo");
	}
	if(stream)
		fwrite(stream, string_length(stream), 1, archivo_aux);
	fclose(archivo_aux);

	char* hash = dar_hash_md5(path_aux);

	eliminar_archivo(path_aux);
	pthread_mutex_unlock(&mutex_hash);

	return hash;
}

void obtener_blocks(){
	log_info(logger, "MONGO-STORE :: SE OBTENDRAN LOS %d BLOQUES DE %d TAMANIO EN EL ARCHIVO %s", blocks, block_size, path_blocks);
	int fd = crear_archivo(path_blocks);
	if(fd == -1) {
		log_error(logger, "MONGO-STORE :: NO SE ABRIO EL ARCHIVO BLOCKS CORRECTAMENTE");
	}
	else {
		log_info(logger, "MONGO-STORE :: SE ABRIO EL ARCHIVO BLOCKS CORRECTAMENTE");struct stat file_st;
		fstat(fd, &file_st);
		contenido_blocks = calloc(1,file_st.st_size);
//		log_info(logger, "%s\nHASH de %s:\n%s\n",contenido_blocks, path_blocks, dar_hash_md5(path_blocks));

		contenido_blocks = mmap(NULL,file_st.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
		contenido_blocks_aux = calloc(1,file_st.st_size);
		memcpy(contenido_blocks_aux,contenido_blocks,file_st.st_size);
//		log_info(logger, "%s",contenido_blocks);
		if(contenido_blocks == MAP_FAILED) {
			log_error(logger,"Error  mapping - file_st.st_size %i", file_st.st_size);
			exit(1);
		}

//		close(fd);
	}
}

void crear_blocks(){
	log_info(logger, "MONGO-STORE :: SE INICIALIZARAN LOS %d BLOQUES DE %d TAMANIO EN EL ARCHIVO %s", blocks, block_size, path_blocks);
	int fd = crear_archivo(path_blocks);
	if(fd == -1) {
		log_error(logger, "MONGO-STORE :: NO SE CREO EL ARCHIVO BLOCKS CORRECTAMENTE");
	}
	else {
		log_info(logger, "MONGO-STORE :: SE CREO EL ARCHIVO BLOCKS CORRECTAMENTE");
		contenido_blocks = calloc(1,block_size * blocks);
		char a = '\0';
		memset(contenido_blocks,a,block_size * blocks);
		write(fd,contenido_blocks,block_size * blocks);
//		log_info(logger, "%s\nHASH de %s:\n%s\n",contenido_blocks, path_blocks, dar_hash_md5(path_blocks));
		struct stat file_st;
		fstat(fd, &file_st);
		contenido_blocks = mmap(NULL,file_st.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
		if(contenido_blocks == MAP_FAILED) {
			log_error(logger,"Error  mapping - file_st.st_size %i", file_st.st_size);
			exit(1);
		}
		contenido_blocks_aux = calloc(1,block_size * blocks);
		memcpy(contenido_blocks_aux,contenido_blocks,block_size * blocks);
//		log_info(logger, "%s",contenido_blocks);

//		close(fd);
	}
}

char* path_bitacora_tripulante(uint32_t id_tripulante){
	char* pathArchivoBitacora = string_new();
	string_append_with_format(&pathArchivoBitacora, "%s/Tripulante%d.ims", path_bitacoras, id_tripulante);

	return pathArchivoBitacora;
}

t_bitarray* crear_bitmap() {
	bitarray_string = malloc(blocks / 8);

		for (int i = 0; i < blocks / 8; i++) {
			bitarray_string[i] = '0';
		}

		t_bitarray* bitarray = bitarray_create_with_mode(bitarray_string,blocks/8,LSB_FIRST);
		for (int i = 0; i < blocks; i++) {
			bitarray_clean_bit(bitarray, i);
		}

		return bitarray;

}

t_bitarray* leer_bitmap(){
	FILE* superbloque = fopen(path_superbloque,"rb");

	if(superbloque){
		fseek(superbloque,2*sizeof(uint32_t),SEEK_SET);

		fread(bitarray_string,blocks/8,1,superbloque);

		t_bitarray* bitarray = bitarray_create_with_mode(bitarray_string,blocks/8,LSB_FIRST);

		fclose(superbloque);

		return bitarray;
	}

	return NULL;
}

void subir_bitmap(t_bitarray* bitarray){
	FILE* superbloque = fopen(path_superbloque,"rb+");

	if(superbloque){
		fseek(superbloque,2*sizeof(uint32_t),SEEK_SET);
		fwrite(bitarray->bitarray,bitarray->size,1,superbloque);

		fclose(superbloque);
	}

}

bool directorio_existe(char* path) {
	struct stat st = { 0 }; // stat(2) precisa de un struct stat a modo de buffer para llenar info del archivo que nosotros no necesitamos.
	char* aux = string_duplicate(path);
	bool existe = (stat(aux, &st) != -1);
	free(aux);
	return existe;
}

bool archivo_existe(char* nombreArchivo){

    if (access(nombreArchivo, F_OK) != 0){
        log_warning(logger,"'%s' no existe\n", nombreArchivo);
        return false;
    }
    else {
        log_info(logger,"'%s' existe\n", nombreArchivo);
        return true;
    }

}

int eliminar_archivo(char* archivo){
	if(remove(archivo)!=0){
		return 1;
	}else
		return 0;
}

int crear_archivo(char* archivo){
	return open(archivo, O_RDWR | O_CREAT,S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
}

char* obtener_path_files(char* pathRelativo) {
	char* path = string_new();
	string_append_with_format(&path, "%s/%s", path_files,pathRelativo);

	return path;
}

bool archivo_recursos_existe(char* nombreArchivo){
	char* ruta_archivo_recursos = obtener_path_files(nombreArchivo);
	return archivo_existe(ruta_archivo_recursos);
}

int crear_archivo_recursos(char* nombreArchivo, char caracter_llenado){
	char* ruta_archivo_recursos = obtener_path_files(nombreArchivo);
	FILE* archivo = fopen(ruta_archivo_recursos, "w+");
	if(archivo){
		char* file_generico = string_duplicate("SIZE=0\nBLOCK_COUNT=0\nBLOCKS=[]\nCARACTER_LLENADO=");
		string_append_with_format(&file_generico,"%c\nMD5_ARCHIVO=", caracter_llenado);
		char* hash = obtener_hashmd5_string(NULL);
		string_append(&file_generico,hash);
		fwrite(file_generico,string_length(file_generico),1,archivo);

		fclose(archivo);
//		string_append(&file_generico,dar_hash_md5(ruta_archivo_recursos));
		return 1;
	}
	log_error(logger, "No se pudo crear el archivo %s", ruta_archivo_recursos);
	return 0;
}

int crear_archivo_bitacora(uint32_t id_tripulante){
	char* ruta_archivo_bitacora = path_bitacora_tripulante(id_tripulante);
	if(!archivo_existe(ruta_archivo_bitacora)){
		FILE* archivo = fopen(ruta_archivo_bitacora, "w+");
		if(archivo){
			char* file_generico = string_duplicate("SIZE=0\nBLOCKS=[]");
			fwrite(file_generico,string_length(file_generico),1,archivo);

			fclose(archivo);
			free(ruta_archivo_bitacora);
			return 1;
		}
		free(ruta_archivo_bitacora);
		return 0;
	}
	else{
		return 2;
	}
}

int eliminar_archivo_recursos(char* nombreArchivo){
	char* ruta_archivo_recursos = obtener_path_files(nombreArchivo);
	return eliminar_archivo(ruta_archivo_recursos);
}

void vaciar_archivo(char* archivo){
	char* ruta_completa_archivo = obtener_path_files(archivo);
	FILE * file = fopen(ruta_completa_archivo, "wb");
	if(file==NULL)
		log_warning(logger,"No se pudo abrir el archivo %s al querer vaciarlo",archivo);
	fclose(file);
}

int dar_bloque_libre(){
	pthread_mutex_lock(&mutex_bitmap);
	t_bitarray* bitarray = leer_bitmap();
	for(int i=0;i<blocks;i++){
		if(bitarray_test_bit(bitarray, i) == 0){
			log_info(logger,"El bloque %i esta vacio",i);
			bitarray_set_bit(bitarray,i);
			if(bitarray_test_bit(bitarray, i) != 0)
				log_info(logger,"El bloque %i se seteo correctamente con %i", i, bitarray_test_bit(bitarray, i));
			subir_bitmap(bitarray);
			bitarray_destroy(bitarray);
			pthread_mutex_unlock(&mutex_bitmap);
			return i;
		}
	}
	log_info(logger,"No hay bloques libres");
	bitarray_destroy(bitarray);
	pthread_mutex_unlock(&mutex_bitmap);
	return -1;
}

//
//t_list* subir_buffer_blocks(t_buffer* buffer, uint32_t ultimo_bloque){
//	uint32_t offset = 0;
//	t_list* bloques = list_create();
//	int bloque_libre;
//	while(offset < buffer->size){
//		if(ultimo_bloque != -1){
//			uint32_t espacio_libre = espacio_libre_bloque(ultimo_bloque);
//
//			if(espacio_libre > 0)
//				bloque_libre = ultimo_bloque;
//			else
//				bloque_libre = dar_bloque_libre();
//		}
//		else
//			bloque_libre = dar_bloque_libre(); //TODO hacer verificacion
//		list_add(bloques, bloque_libre);
//
//		uint32_t tamanio_subida = minimo((buffer->size-offset),block_size);
//
//		memcpy(contenido_blocks + (bloque_libre*block_size),buffer->stream,tamanio_subida);
//
//		offset += tamanio_subida;
//	}
//
//	return bloques;
//}

void* obtener_informacion_bloque(uint32_t bloque){
	void* informacion = malloc(block_size);
	memcpy(informacion, contenido_blocks_aux + (bloque*block_size),block_size);

	return informacion;
}

t_list* agregar_stream_blocks(char* stream_a_agregar, int ultimo_bloque, int tamanio_restante){
//	log_info(logger, "Se entra a agregar_stream_blocks para el stream %s", stream_a_agregar);
	char* stream = string_duplicate(stream_a_agregar);
	uint32_t offset = 0;
	t_list* bloques = list_create();
	int bloque_libre;
	int offset_bloque = 0;

	uint32_t cant_caracteres = string_length(stream);
	while(offset < cant_caracteres){
//		log_info(logger, "Se entro al while");
		uint32_t tamanio_subida;
		if(ultimo_bloque != -1){
//			log_info(logger, "Se entro al if ultimo_bloque != -1");
			bloque_libre = ultimo_bloque;
			ultimo_bloque = -1;
			if(tamanio_restante > 0){
//				log_info(logger, "Se entro al if tamanio_restante > 0");
				offset_bloque = block_size - tamanio_restante;
				tamanio_subida = minimo((cant_caracteres - offset),tamanio_restante);
			}
			else{
//				log_info(logger, "Se entro al else de tamanio_restante > 0");
				bloque_libre = dar_bloque_libre(); //TODO hacer verificacion
				list_add(bloques, bloque_libre);
				offset_bloque = 0;
				tamanio_subida = minimo((cant_caracteres - offset),block_size);
			}
		}
		else{
//			log_info(logger, "Se entro al else de ultimo_bloque != -1");
			bloque_libre = dar_bloque_libre(); //TODO hacer verificacion
			list_add(bloques, bloque_libre);
			offset_bloque = 0;
			tamanio_subida = minimo((cant_caracteres - offset),block_size);
		}

//		log_info(logger, "Se subira %s del tamanio %i al bloque %i en el offset %i", stream + offset, tamanio_subida, bloque_libre, offset_bloque);
		if(offset_bloque+tamanio_subida!= block_size && offset + tamanio_subida == cant_caracteres)
			tamanio_subida++;
		offset_bloque += (bloque_libre*block_size);
		memcpy(contenido_blocks_aux + offset_bloque, stream + offset, tamanio_subida);

		offset += tamanio_subida;
	}

	free(stream);

	return bloques;
}

char* obtener_stream_bitacora(char caracter, t_list* bloques, uint32_t size){
	//------------ORDEN------------
		//1. SIZE
		//2. BLOCKS   					 	} COMO ES LISTA IRA DENTRO DE UN FOR
	//-----------------------------
	uint32_t cant_bloques = (uint32_t) list_size(bloques);

	char* stream = string_duplicate("SIZE=");
	string_append_with_format(&stream, "%i\n", size);
	string_append_with_format(&stream, "BLOCKS=[");
	for(int i=0;i<cant_bloques;i++){
		string_append_with_format(&stream, "%i", list_get(bloques,i));
		if(i!=cant_bloques-1)
			string_append_with_format(&stream, ",");
	}
	string_append_with_format(&stream, "]\n");

	return stream;
}

char* obtener_stream_tarea(char caracter, t_list* bloques, uint32_t size){
	//------------ORDEN------------
		//1. SIZE
		//2. BLOCK_COUNT
		//3. BLOCKS   					 	} COMO ES LISTA IRA DENTRO DE UN FOR
		//4. CARACTER_LLENADO
		//5. MD5_ARCHIVO
	//-----------------------------
	uint32_t cant_bloques = (uint32_t) list_size(bloques);

	char* stream = string_duplicate("SIZE=");
	string_append_with_format(&stream, "%i\n", size);
	string_append_with_format(&stream, "BLOCK_COUNT=%i\n", cant_bloques);
	string_append_with_format(&stream, "BLOCKS=[");
	for(int i=0;i<cant_bloques;i++){
		string_append_with_format(&stream, "%i", list_get(bloques,i));
		if(i!=cant_bloques-1)
			string_append_with_format(&stream, ",");
	}
	string_append_with_format(&stream, "]\n");
	string_append_with_format(&stream, "CARACTER_LLENADO=%c\n", caracter);
	string_append_with_format(&stream, "MD5_ARCHIVO=%s\n", "");

	return stream;
}

t_list* obtener_bloques_totales(t_config* config){
	t_list* bloques = list_create();
	int size = config_get_int_value(config, "SIZE");
	uint32_t cant_bloques;
	div_t aux = div(size, block_size);
	if (aux.rem == 0)
		cant_bloques = aux.quot;
	else
		cant_bloques = aux.quot + 1;

	if(cant_bloques != 0){
		char** bloques_config = config_get_array_value(config, "BLOCKS");
		for(int i=0; i<cant_bloques;i++){
			list_add(bloques, atoi(bloques_config[i]));
		}
	}

	return bloques;
}

t_list* sacar_bloques_config(t_list* bloques, t_config* config){
	t_list* bloques_nuevos = obtener_bloques_totales(config);
//	log_info(logger,"Se borraran %i bloques de %i", list_size(bloques), list_size(bloques_nuevos));

	bool es_bloque(uint32_t bloque){
		bool esta_aca(uint32_t bloque_a_sacar){
			return bloque == bloque_a_sacar;
		}
		return list_any_satisfy(bloques, esta_aca);
	}
	for(int i=0; i<list_size(bloques); i++)
		list_remove_by_condition(bloques_nuevos, es_bloque);

//	log_info(logger,"Nos quedaron %i bloques nuevos", list_size(bloques_nuevos));
	return bloques_nuevos;
}

t_list* sumar_bloques_config(t_list* bloques, t_config* config){
	t_list* bloques_nuevos = obtener_bloques_totales(config);
	list_add_all(bloques_nuevos, bloques);

	return bloques_nuevos;
}

char* obtener_stream_blocks(t_list* bloques){
	uint32_t cant_bloques = (int) list_size(bloques);

	char* stream = string_duplicate("[");
	for(int i=0;i<cant_bloques;i++){
		string_append_with_format(&stream, "%i", list_get(bloques,i));
		if(i!=cant_bloques-1)
			string_append_with_format(&stream, ",");
	}
	string_append_with_format(&stream, "]");

	return stream;
}

void actualizar_archivo_bitacora(char caracter, t_list* bloques, t_config* config, uint32_t size){
	t_list* bloques_a_subir = sumar_bloques_config(bloques, config);
	uint32_t size_old = config_get_int_value(config, "SIZE");
	char* blocks_stream = obtener_stream_blocks(bloques_a_subir);
	config_set_value(config,"BLOCKS",blocks_stream);
	config_set_value(config,"SIZE", string_itoa(size+size_old));
	free(blocks_stream);

	config_save(config);
	list_destroy(bloques_a_subir);
}

void actualizar_archivo_file(char caracter, t_list* bloques, t_config* config, int size){
//	log_info(logger, "\n ~~size %i~~ \n", size);
	t_list* bloques_a_subir;
	int size_old = config_get_int_value(config, "SIZE");
	int size_total = size_old+size;
	if(size < 0){
		if(size_total < 0)
			size_total = 0;
		bloques_a_subir = sacar_bloques_config(bloques, config);
	}
	else
		bloques_a_subir = sumar_bloques_config(bloques, config);

//	log_info(logger, "\n ~~BLOCK_COUNT %i~~ \n", list_size(bloques_a_subir));

	config_set_value(config,"SIZE", string_itoa(size_total));
	char* stream_aux = obtener_stream_blocks(bloques_a_subir);
	config_set_value(config,"BLOCK_COUNT", string_itoa(list_size(bloques_a_subir)));
	config_set_value(config,"BLOCKS",stream_aux);
	free(stream_aux);
//	stream_aux = string_repeat(caracter,1);
//	config_set_value(config,"CARACTER_LLENADO", stream_aux);
//	free(stream_aux);
	stream_aux = obtener_hashmd5_string(string_repeat(caracter, size_total));
	config_set_value(config,"MD5_ARCHIVO",stream_aux);

	free(stream_aux);

	config_save(config);
	list_destroy(bloques_a_subir);
}

t_buffer* serializar_tarea(t_tarea_Mongo* tarea) {
	t_buffer* buffer = crear_buffer();
	uint32_t offset = 0;

	char* caracteres = string_repeat(obtener_caracter_llenado(tarea->tarea),tarea->parametro);

	buffer->size = string_length(caracteres)+1;
	buffer->stream = malloc(buffer->size);

	memcpy(buffer->stream + offset, caracteres, buffer->size);

	return buffer;
}

int ultimo_bloque_config(t_config* config){
//	sleep(10);
	int size = config_get_int_value(config, "SIZE");
	if(size){
		uint32_t cant_bloques;
		div_t aux = div(size, block_size);
		if (aux.rem == 0)
			cant_bloques = aux.quot;
		else
			cant_bloques = aux.quot + 1;

//		log_info(logger, "Hay %i bloques por %i/%i es %i y el resto es %i",cant_bloques, size, block_size, aux.quot, aux.rem);

		if(cant_bloques != 0){
			char** bloques_config = config_get_array_value(config, "BLOCKS");
//			for(int i=0;i<cant_bloques;i++)
//				log_info(logger, "Bloque %i es bloque %i",i ,atoi(bloques_config[i]));

			return atoi(bloques_config[cant_bloques-1]);
		}
		return -1;
	}
	return -1;
}

int tamanio_restante_config(t_config* config){
	int size = config_get_int_value(config, "SIZE");
	div_t aux = div(size, block_size);
	if(size == 0)
		return block_size;
	if(aux.rem == 0 && size > 0)
		return 0;
	else
		return block_size - aux.rem;
}

void subir_FS(char* a_subir, char* archivo, bool es_files){
	pthread_mutex_lock(&mutex_FS);
	t_config* config = config_create(archivo);
//	log_info(logger, "Se obtuvo el config");
	int ultimo_bloque = ultimo_bloque_config(config);
//	log_info(logger, "Se obtuvo el ultimo_bloque %i", ultimo_bloque);
	int tamanio_restante = tamanio_restante_config(config);
//	log_info(logger, "Se obtuvo el tamanio_restante %i", tamanio_restante);
	t_list* bloques = agregar_stream_blocks(a_subir, ultimo_bloque, tamanio_restante);
//	log_info(logger, "Se subio a blocks");

	if(es_files)
		actualizar_archivo_file(a_subir[0], bloques, config, string_length(a_subir));
	else
		actualizar_archivo_bitacora(a_subir[0], bloques, config, string_length(a_subir));

	pthread_mutex_unlock(&mutex_FS);
	list_destroy(bloques);
	config_destroy(config);
}

char* leer_blocks(t_list* bloques, int size){
	char* informacion = malloc(size);
	uint32_t offset = 0;
	int tamanio_total = size;
	for(int i=0;i<list_size(bloques);i++){
		uint32_t bloque = (uint32_t) list_get(bloques,i);
		int tamanio_leer = minimo(tamanio_total, block_size);
		memcpy(informacion + offset, contenido_blocks_aux + (bloque*block_size), tamanio_leer);
		offset += tamanio_leer;
		tamanio_total -= tamanio_leer;
	}


//	log_info(logger, "informacion %s - size %i", informacion, size);

	return informacion;
}

char* leer_FS(char* archivo){

	pthread_mutex_lock(&mutex_FS);

	t_config* config = config_create(archivo);
	t_list* bloques = obtener_bloques_totales(config);
//	char* informacion = leer_blocks(bloques, config_get_int_value(config,"SIZE"));
	char* informacion = obtener_caracteres2(bloques);

	pthread_mutex_unlock(&mutex_FS);

	list_destroy(bloques);
	config_destroy(config);

	return informacion;
}

char* leer_file(char* tipo){
	char* path_file;
	if(son_iguales(tipo,"GENERAR_OXIGENO") || son_iguales(tipo,"CONSUMIR_OXIGENO")){
		path_file = obtener_path_files("Oxigeno.ims");
	}
	else if(son_iguales(tipo,"GENERAR_COMIDA") || son_iguales(tipo,"CONSUMIR_COMIDA")){
		path_file = obtener_path_files("Comida.ims");
	}
	else if(son_iguales(tipo,"GENERAR_BASURA") || son_iguales(tipo,"DESCARTAR_BASURA")){
		path_file = obtener_path_files("Basura.ims");
	}

	char* informacion = leer_FS(path_file);
	free(path_file);

	return informacion;
}

char* leer_bitacora(uint32_t id_tripulante){
	char* path_file = path_bitacora_tripulante(id_tripulante);
	char* informacion = leer_FS(path_file);
	free(path_file);

	return informacion;
}

int obtener_bloques_a_sacar(uint32_t cantidad, int size, int cant_bloques){
	int cant_bloques_sacar=0;
	if(cantidad){
		if(cantidad >= size)
			cant_bloques_sacar = cant_bloques;
		else{
			div_t aux = div(size - cantidad, block_size);
			int cant_bloques_actuales;
			if (aux.rem == 0)
				cant_bloques_actuales = aux.quot;
			else
				cant_bloques_actuales = aux.quot + 1;
			cant_bloques_sacar = cant_bloques - cant_bloques_actuales;
		}
	}
	return cant_bloques_sacar;
}

void liberar_bloque(int bloque){
	pthread_mutex_lock(&mutex_bitmap);
	t_bitarray* bitarray = leer_bitmap();
	bitarray_set_bit(bitarray,bloque);
	if(bitarray_test_bit(bitarray, bloque) == 0)
		log_info(logger,"El bloque %i se libero correctamente con %i", bloque, bitarray_test_bit(bitarray, bloque));
	subir_bitmap(bitarray);
	bitarray_destroy(bitarray);
	pthread_mutex_unlock(&mutex_bitmap);
}

void reset_archivo_file(char caracter, t_config* config){
	config_set_value(config,"SIZE", "0");
	config_set_value(config,"BLOCK_COUNT", "0");
	config_set_value(config,"BLOCKS","[]");
	config_set_value(config,"CARACTER_LLENADO",&caracter);
//	stream_aux = dar_hash_md5(config->path);
//	config_set_value(config,"MD5_ARCHIVO",stream_aux);
//	TODO VER HASH

	config_save(config);
}
//
//void eliminar_caracteres_FS2(char caracter, uint32_t cantidad, char* archivo){
//	char* stream_total = leer_FS(archivo);
//	char* nuevo_stream = string_substring_until(stream_total, cantidad);
//	pthread_mutex_lock(&mutex_FS);
//	t_config* config = config_create(archivo);
//
//	char** bloques_config = config_get_array_value(config, "BLOCKS");
//	int cant_bloques = config_get_int_value(config, "BLOCK_COUNT");
//	for(int i=0; i<cant_bloques;i++){
//		liberar_bloque(atoi(bloques_config[i]));
//	}
//	reset_archivo_file(caracter, config);
//	config_destroy(config);
//	pthread_mutex_unlock(&mutex_FS);
//
//	subir_FS(nuevo_stream, archivo, true);
//
//	log_info(logger, "Se eliminaron %i caracteres %c de llenado", cantidad, caracter);
//}

void eliminar_caracteres_FS(char caracter, int cantidad, char* archivo){
	pthread_mutex_lock(&mutex_FS);
	t_config* config = config_create(archivo);

	char** bloques_config = config_get_array_value(config, "BLOCKS");
	int size = config_get_int_value(config, "SIZE");
	int cant_bloques = config_get_int_value(config, "BLOCK_COUNT");
	int cant_bloques_sacar = obtener_bloques_a_sacar(cantidad,size,cant_bloques);
//	log_info(logger,"cant_bloques_sacar %i",cant_bloques_sacar);
	t_list* bloques_a_sacar = list_create();
	for(int i=0; i<cant_bloques_sacar;i++){
		liberar_bloque(atoi(bloques_config[cant_bloques+i-1]));
		list_add(bloques_a_sacar, atoi(bloques_config[cant_bloques-i-1]));
	}
	int offset;
	int ultimo_bloque_nuevo;
	if(cant_bloques == cant_bloques_sacar && size <= cantidad){
		offset = 0;
		ultimo_bloque_nuevo = -1;
		log_warning(logger,"MONGO-STORE: Se quisieron eliminar mas caracteres de los existentes en %s",archivo);
	}
	else{
		div_t aux = div(size-cantidad, block_size);
		if(aux.rem == 0)
			offset = -1;
		else
			offset = aux.rem;
		ultimo_bloque_nuevo = atoi(bloques_config[cant_bloques - cant_bloques_sacar-1]);
	}
	if(ultimo_bloque_nuevo != -1 && offset != -1){
		memcpy(contenido_blocks_aux + offset + (ultimo_bloque_nuevo*block_size), "\0", 1);
	}
	int cantidad_negativa = cantidad*(-1);
	actualizar_archivo_file(caracter, bloques_a_sacar, config, cantidad_negativa);
	config_destroy(config);
	pthread_mutex_unlock(&mutex_FS);


	log_info(logger, "Se eliminaron %i caracteres %c de llenado", cantidad, caracter);
}

char* obtener_caracteres_de_file(t_config* file_recurso){
	t_list* bloques = list_create();
	char** bloques_config = config_get_array_value(file_recurso, "BLOCKS");
	int cantidad_bloques= config_get_int_value(file_recurso, "BLOCK_COUNT");
//	log_info(logger, "Bloques del archivo %s (%i):", file_recurso->path, cantidad_bloques);
	for(int i=0; bloques_config[i];i++){
		list_add(bloques, atoi(bloques_config[i]));
//		log_info(logger, "Bloques del archivo %s: %i", file_recurso->path, atoi(bloques_config[i]));
	}


	char* caracteres = obtener_caracteres2(bloques);
//	log_info(logger, "caracteres: %s", caracteres);

	list_destroy(bloques);
	string_iterate_lines(bloques_config, free);

	return caracteres;
}

char* obtener_caracteres(t_list* bloques, char caracter_llenado){
//	int tam = 0;
//	char* stream = string_duplicate("hola");
//	tam = string_length(stream);
//	void* aux = calloc(1,10);
//	memcpy(aux, stream, tam);
//	printf("stream %s tam %i\n", aux, tam);
//
//
////	string_append(&stream, "\0");
//	tam = string_length(stream)+1;
//	printf("stream %s tam %i\n", aux, tam);
//	char* caracteres_nuevos = string_new();
//	int bloque=0;
//	int offset_bloque = 0;
//	int offset = 0;
//
//	do{
//		memcpy(caracteres_nuevos+offset, aux + offset_bloque + offset, 1);
//		printf("caracteres_nuevos %s\n", caracteres_nuevos);
//		offset++;
//	}while(caracteres_nuevos[offset]);

	char* leido = string_new();
	char* caracter = calloc(2,sizeof(char));

	int offset_bloque = 0;
	int offset = 0;
	int bloques_leidos = 0;

	offset_bloque = (uint32_t) list_get(bloques, bloques_leidos) * block_size;
	memcpy(caracter, contenido_blocks_aux + offset_bloque + offset, 1);
	offset++;
	while(caracter[0] != '\0' || caracter[0] != caracter_llenado){
		string_append(&leido, caracter);
		memcpy(caracter, contenido_blocks_aux + offset_bloque + offset, 1);
		offset++;
		if(offset >= block_size){
			offset_bloque = (uint32_t) list_get(bloques, ++bloques_leidos) * block_size;
			offset = 0;
		}
	}
	free(caracter);

	return leido;

}

char* obtener_caracteres2(t_list* bloques){
//	char* test = string_duplicate("AAAAA");
//	char* test_2 = string_duplicate("AAAAA");
//	char* test_3 = string_duplicate("AAA");
//
//	char* stream_1 = string_new();
//	string_append(&stream_1, test);
//	string_append(&stream_1, test_2);
//	string_append(&stream_1, test_3);
//
//	printf("stream leido %s size %i\n",stream_1, string_length(stream_1));
//	void* aux = malloc(20);
//	int offset = 0;
//	memcpy(aux + offset,test,string_length(test));
//	offset += string_length(test);
//	memcpy(aux + offset,test_2,string_length(test_2));
//	offset += string_length(test_2);
//	memcpy(aux + offset,test_3,string_length(test_3)+1);
//	offset += string_length(test_3)+1;
//
//	char* aux_1 = calloc(1, 5);
//	char* aux_2 = calloc(1, 5);
//	char* aux_3 = calloc(1, 5);
//
//
//	int offset_2 = 0;
//	memcpy(aux_1,aux + offset_2,5);
//	offset_2 += 5;
//	memcpy(aux_2,aux + offset_2,5);
//	offset_2 += 5;
//	memcpy(aux_3,aux + offset_2,5);
//	offset_2 += 5;
//
//	char* stream = string_new();
//	string_append(&stream, aux_1);
//	string_append(&stream, aux_2);
//	string_append(&stream, aux_3);
//
//	printf("stream leido %s size %i\n",stream, string_length(stream));

	char* stream = string_new();

	for(int i=0; i<list_size(bloques); i++){
		char* aux = calloc(1, block_size+1);
		int ptr_bloque = (uint32_t) list_get(bloques, i) * block_size;
		memcpy(aux, contenido_blocks_aux + ptr_bloque, block_size);
//		log_info(logger, "bloque %i ptr %i aux %s", ptr_bloque/block_size, ptr_bloque, aux);
		string_append(&stream, aux);
		if(string_length(aux)<block_size){
			free(aux);
			break;
		}
		free(aux);

	}

//	log_info(logger, "stream %s", stream);

	return stream;

}



