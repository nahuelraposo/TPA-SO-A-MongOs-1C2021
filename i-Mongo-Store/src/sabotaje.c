#include "sabotaje.h"

void procesar_nuevo_sabotaje(int signal) {
	if(hay_sabotajes()) {

		if((socket_discordiador = crear_conexion(ip_discordiador, puerto_discordiador)) == -1){
			log_error(logger, "No me pude conectar al DISCORDIADOR");
			return;
		}
		log_info(logger, "Pude conectar al DISCORDIADOR");
		posicion_sabotaje = list_remove(posiciones_sabotaje, proxima_posicion_sabotaje);

		log_info(logger, "Mandamos la posicion de sabotaje %d|%d", posicion_sabotaje->pos_x, posicion_sabotaje->pos_y);
		enviar_discordiador_sabotaje(posicion_sabotaje, socket_discordiador);

		t_tripulante* tripulante = recibir_tripulante_sabotaje(socket_discordiador);
		log_info(logger, "Nos llega el tripulante %d para que resuelva el sabotaje", tripulante->id);
		enviar_respuesta(resolver_sabotaje(tripulante), socket_discordiador);

		liberar_conexion(&socket_discordiador);
	}
	else {
		log_warning(logger, "Ya no hay nuevas posiciones de sabotajes");
	}
}

bool hay_sabotajes() {
	return proxima_posicion_sabotaje < list_size(posiciones_sabotaje);
}

void verificar_sabotajes() {
	signal(SIGUSR1, procesar_nuevo_sabotaje);
}

t_respuesta resolver_sabotaje(t_tripulante* tripulante){
	if(detectar_algun_sabotaje_en_superbloque()){
		log_info(logger, "El tripulante %i resolvio el sabotaje en el superbloque", tripulante->id);
		return OK;
	}
	else if(detectar_algun_sabotaje_en_files()){
		log_info(logger, "El tripulante %i resolvio el sabotaje en files", tripulante->id);
		return OK;
	}
	else{
		log_warning(logger, "El tripulante %i no detecto sabotajes", tripulante->id);
		return FAIL;
	}
}

bool detectar_algun_sabotaje_en_superbloque(){
	if(detectar_sabotaje_superbloque_blocks())
		return true;
	else if(detectar_sabotaje_superbloque_bitmap())
		return true;
	else
		return false;
}

bool detectar_sabotaje_superbloque_blocks(){
	//Cambian el valor del campo blocks del superbloque. Ej, si teniamos 10 bloques de 80 bytes, y nos cambian a 20.
	//sobreescribir “cantidad de bloques” del superbloque con la cantidad de bloques real en el disco (tamaño del blocks.ims)


	FILE * superbloque = abrirSuperbloque("rb+");
	bool hubo_sabotaje=false;
	uint32_t sizeUnBlock;
	uint32_t supuesto_blocks;
	fseek(superbloque,0,SEEK_SET);
	fread(&sizeUnBlock,sizeof(uint32_t),1,superbloque);
	fread(&supuesto_blocks,sizeof(uint32_t),1,superbloque);
	uint32_t blocks_reales = tamanio_real_blocks_ims() / sizeUnBlock;
	if(blocks_reales != supuesto_blocks ){
		resolver_sabotaje_superbloque_blocks(blocks_reales, superbloque);
		hubo_sabotaje = true;
	}
	fclose(superbloque);
	return hubo_sabotaje;
}

uint32_t tamanio_real_blocks_ims(){
	struct stat st;

	stat(path_blocks, &st);
	int size_blocks_ims = st.st_size;
//	log_info(logger, "VALOR :: %d", size_blocks_ims);
	return size_blocks_ims;
}

void resolver_sabotaje_superbloque_blocks(uint32_t blocks_reales, FILE * superbloque){

	fseek(superbloque,sizeof(uint32_t),SEEK_SET);
	fwrite(&blocks_reales,sizeof(uint32_t),1,superbloque);
	log_info(logger,"FSCK: Resolviendo sabotaje -> Se actualiza valor de blocks de superbloque a %d",blocks_reales);
}

bool detectar_sabotaje_superbloque_bitmap(){
	//Corregir el bitmap con lo que se halle en la metadata de los archivos.
		//Constatar contra el bitmap que esten todos los valores correctos.
	//	Si el bloque está cruzado en un archivo, tiene que estar marcado como usado, y si no , tiene que estar marcado como libre


	t_list* bloques_usados = obtener_bloques_usados();
	pthread_mutex_lock(&mutex_bitmap);
	t_bitarray* bitarray = leer_bitmap();

//	log_info(logger, "size bloques usados %i size bitarray %i", list_size(bloques_usados), bitarray_get_max_bit(bitarray));

	bool inconsistencia_bitmap(int bloque){
		return bitarray_test_bit(bitarray,bloque)==0;
	}

	bool chequear_falsos_unos(){
		for(int i=0;i<bitarray_get_max_bit(bitarray);i++){

			bool no_esta_presente(uint32_t bloque){
				return i != bloque;
			}

			if(bitarray_test_bit(bitarray,i)==1 && list_all_satisfy(bloques_usados,no_esta_presente)){
				return 1;
			}
		}
		return 0;
	}

	if(list_any_satisfy(bloques_usados,inconsistencia_bitmap) || chequear_falsos_unos()){
		resolver_sabotaje_superbloque_bitmap(bitarray,bloques_usados);
		pthread_mutex_unlock(&mutex_bitmap);
		return true;
	}
	else{
		log_info(logger, "FSCK: No hubo sabotajes en el bitmap de superbloques.ims");
		pthread_mutex_unlock(&mutex_bitmap);
		return false;
	}
}

void resolver_sabotaje_superbloque_bitmap(t_bitarray* bitarray, t_list* bloques_usados){

	//Limpiar bitmap
	for(int i=0;i<bitarray_get_max_bit(bitarray);i++){
		bitarray_clean_bit(bitarray,i);
	}

	void actualizar_bitmap(int bloque){
		bitarray_set_bit(bitarray,bloque);
	}

	list_iterate(bloques_usados,actualizar_bitmap);

	subir_bitmap(bitarray);
	log_info(logger,"FSCK: Se resolvio sabotaje de bitmap");
	list_destroy(bloques_usados);
}

bool detectar_algun_sabotaje_en_files(){

	if(archivo_recursos_existe("Oxigeno.ims")){
		if(chequear_sabotajes_en_recurso("Oxigeno.ims"))
			return 1;
	}
	if(archivo_recursos_existe("Comida.ims")){
		if(chequear_sabotajes_en_recurso("Comida.ims")){
			return 1;
		}
	}
	if(archivo_recursos_existe("Basura.ims")){
		if(chequear_sabotajes_en_recurso("Basura.ims")){
			return 1;
		}
	}
	return 0;
}

bool chequear_sabotajes_en_recurso(char* path_relativo){
	char* path_archivo = obtener_path_files(path_relativo);
	t_config* config_recurso = config_create(path_archivo);

	if(detectar_sabotaje_files_blocks(config_recurso)){
		log_info(logger,"FSCK: Se detecto sabotaje en el campo BLOCKS del recurso %s",path_relativo);
		return 1;
	}
	if(detectar_sabotaje_files_blockcount(config_recurso)){
		log_info(logger,"FSCK: Se detecto sabotaje en el campo BLOCK_COUNT del recurso %s",path_relativo);
		return 1;
	}
	if(detectar_sabotaje_files_size(config_recurso)){
		log_info(logger,"FSCK: Se detecto sabotaje en el campo SIZE del recurso %s",path_relativo);
		return 1;
	}
	log_info(logger,"FSCK: No se detectaron sabotajes en el recurso %s",path_relativo);
	config_destroy(config_recurso);
	return 0;
}

bool detectar_sabotaje_files_size(t_config* archivo_recurso){
	//TODO
//Asumir correcto lo encontrado en los bloques.
//Hay que recorrer todos los bloques del archivo y obtener y asumir como correcto el tamaño encontrado dentro de los bloques recorriendolos en orden.
//	Ej. OOOOOOOOOOOOOOOOOOOOOOOOO recorremos hasta encontrar el primer carácter que no es una O, llegué al size 400, ese será el size del archivo.
//	PONER UN CENTINELA PARA DECIR HASTA ACÁ LLEGUE
	uint32_t supuesto_size = config_get_int_value(archivo_recurso,"SIZE");
	char* caracteres = obtener_caracteres_de_file(archivo_recurso);
	uint32_t cantidad_real_caracteres = string_length(caracteres); // FALTA CALCULAR
	if(supuesto_size != cantidad_real_caracteres){
		resolver_sabotaje_files_size(archivo_recurso, cantidad_real_caracteres);
		return 1;
	}
	else
		return 0;
}

void resolver_sabotaje_files_size(t_config* archivo_recurso, uint32_t cantidad_real_caracteres){
	log_info(logger, "Se entra a resolver el sabotaje en el campo size del file %s", archivo_recurso->path);
	config_set_value(archivo_recurso,"SIZE",string_itoa(cantidad_real_caracteres));
	config_save(archivo_recurso);
	config_destroy(archivo_recurso);
}

bool detectar_sabotaje_files_blockcount(t_config* archivo_recurso){
	t_list* bloques = obtener_bloques_totales(archivo_recurso);
	uint32_t cantidad_bloques = list_size(bloques);

	uint32_t block_count = config_get_int_value(archivo_recurso,"BLOCK_COUNT");

	list_destroy(bloques);

	if(block_count != cantidad_bloques){
		resolver_sabotaje_files_blockcount(archivo_recurso, cantidad_bloques);
		return 1;
	}
	return 0;
}

void resolver_sabotaje_files_blockcount(t_config* archivo_recurso, uint32_t cantidad_bloques){
	//Actualizar el valor de Block_count en base a lo que está en la lista de Blocks.
	config_set_value(archivo_recurso,"BLOCK_COUNT", string_itoa(cantidad_bloques));
	config_save(archivo_recurso);
	config_destroy(archivo_recurso);
}

//char* leer_blocks_sabotaje(t_list* bloques, int size){
//	char* informacion = malloc(size+1);
//	uint32_t offset = 0;
//	int tamanio_total = size;
//	for(int i=0;i<list_size(bloques);i++){
//		uint32_t bloque = (uint32_t) list_get(bloques,i);
//		int tamanio_leer = minimo(tamanio_total, block_size);
//		memcpy(informacion + offset, contenido_blocks_aux + (bloque*block_size), tamanio_leer);
//		offset += tamanio_leer;
//		tamanio_total -= block_size;
//	}
//
//	return informacion;
//}

bool detectar_sabotaje_files_blocks(t_config* archivo_recurso){
	//TODO
	//Lista de blocks alterado.
	//Entra en juego el md5_archivo. Concatenamos los bloques, calculamos el md5 hasta el size, y lo comparamos con el md5 que tenemos guardado.
	//Si no coinciden restaurar archivo, escribiendo tantos caracteres de llenado hasta completar el size, en el orden de bloques que tenemos
	char* md5_guardado = config_get_string_value(archivo_recurso,"MD5_ARCHIVO");
	char* caracteres_de_llenado = leer_FS(archivo_recurso->path);
//	log_info(logger, "caracteres_de_llenado %s", caracteres_de_llenado);
	char* calcular_md5 = obtener_hashmd5_string(caracteres_de_llenado);
	free(caracteres_de_llenado);

	if(!son_iguales(md5_guardado,calcular_md5)){
		log_info(logger,"MD5 GUARDADO: %s -- MD5 RECALCULADO: %s",md5_guardado,calcular_md5);
		resolver_sabotaje_files_blocks(archivo_recurso);
		free(calcular_md5);
		return 1;
	}
	free(calcular_md5);
	return 0;
}

void resolver_sabotaje_files_blocks(t_config* archivo_recurso){
	log_info(logger, "Se entra a resolver el sabotaje en el campo blocks del file %s", archivo_recurso->path);
	//TODO
	//tomar como referencia el size del archivo y el caracter de llenado e ir llenando los bloques hasta completar el size del archivo,
	//en caso de que falten bloques, los mismos se deberán agregar al final del mismo.

	pthread_mutex_lock(&mutex_FS);
	char* caracter_llenado = config_get_string_value(archivo_recurso,"CARACTER_LLENADO");
	int size = config_get_int_value(archivo_recurso,"SIZE");
	t_list* bloques = (t_list*) obtener_bloques_totales(archivo_recurso);
	char* stream = string_repeat(caracter_llenado[0], size);
	int offset_bloque = 0;
	int offset = 0;
	for(int i=0; i<list_size(bloques); i++){
		offset_bloque = (uint32_t) list_get(bloques, i) * block_size;
		int tamanio_subida = minimo((size - offset),block_size);
		if(offset_bloque+tamanio_subida!= block_size && offset + tamanio_subida == size)
			tamanio_subida++;
		memcpy(contenido_blocks_aux + offset_bloque, stream + offset, tamanio_subida);
		offset += tamanio_subida;
	}
	char* hash = obtener_hashmd5_string(stream);
	config_set_value(config,"MD5_ARCHIVO",hash);
	config_save(archivo_recurso);
	free(stream);
	list_destroy(bloques);
	pthread_mutex_unlock(&mutex_FS);

	config_destroy(archivo_recurso);
}


FILE* abrirSuperbloque(char* modo){
	FILE * superbloque = fopen(path_superbloque,modo);
	if(superbloque){
		return superbloque;
	}
	log_error(logger, "Error al leer superbloque");
	exit(1);
}

t_list* listaArchivosDeBitacora() {

	DIR *d;
	t_list* listaArchivos = list_create();
	struct dirent *dir;
	d = opendir(path_bitacoras);
	if (d) {
		while ((dir = readdir(d)) != NULL) {
			if( !son_iguales( dir->d_name, "." ) &&
					!son_iguales( dir->d_name, ".." )){

				char* cadena = string_duplicate(dir->d_name);
//				log_info(logger, "Se detecto la bitacora %s", cadena);
				list_add(listaArchivos,cadena);

			}
		}
		closedir(d);
	}



	return listaArchivos;
}

t_list* obtener_bloques_usados(){
	bool ordenar_bloques(int bloque_1, int bloque_2){
		return bloque_1 < bloque_2;
	}
	t_list* lista_bloques = list_create();

//	log_info(logger, "Se van a obtener los bloques de los archivos de recursos y de las bitacoras");
	list_add_all(lista_bloques,obtener_bloques_recursos());
//	log_info(logger, "Se obtuvieron %i bloques de los archivos de recursos", list_size(lista_bloques));
	list_add_all(lista_bloques,obtener_bloques_bitacora());
//	log_info(logger, "Se obtuvieron %i bloques de las bitacoras y recursos", list_size(lista_bloques));
	list_sort(lista_bloques,ordenar_bloques);

	return lista_bloques;
}

t_list* obtener_bloques_recursos(){
	t_list* lista_bloques = list_create();

	void traer_bloques_recursos(char* archivo){
		char* path_recurso = obtener_path_files(archivo);
		log_info(logger, "ABRIR Y OBTENER BLOQUES -> Se desea entrar en %s", path_recurso);
		t_config* recurso = config_create(path_recurso);
		t_list* bloques_recurso = obtener_bloques_totales(recurso);
		list_add_all(lista_bloques, bloques_recurso);
		config_destroy(recurso);
		list_destroy(bloques_recurso);
	}
	if(archivo_recursos_existe("Oxigeno.ims")){
		traer_bloques_recursos("Oxigeno.ims");
	}
	if(archivo_recursos_existe("Comida.ims")){
		traer_bloques_recursos("Comida.ims");
	}
	if(archivo_recursos_existe("Basura.ims")){
		traer_bloques_recursos("Basura.ims");
	}

	return lista_bloques;
}

t_list* obtener_bloques_bitacora(){
//	log_info(logger, "Se entra a obtener_bloques_bitacora");

	t_list* lista_bloques = list_create();
	t_list* listaArchivos = listaArchivosDeBitacora();

	void traer_bloques_bitacoras(char* archivo){
		char* path_bitacora = obtener_path_bitacora(archivo);
		log_info(logger, "ABRIR Y OBTENER BLOQUES -> Se desea entrar en %s", path_bitacora);
		t_config* bitacora = config_create(path_bitacora);
		t_list* bloques_bitacora = obtener_bloques_totales(bitacora);
		list_add_all(lista_bloques, bloques_bitacora);
		list_destroy(bloques_bitacora);
		config_destroy(bitacora);
	}

	list_iterate(listaArchivos,traer_bloques_bitacoras);
//	log_info(logger, "Se itero la lista de bitacoras con tamaño %i", list_size(listaArchivos));
	list_destroy(listaArchivos);
	return lista_bloques;
}

char* obtener_path_bitacora(char* nombreBitacora){
	char* pathArchivoBitacora = string_new();
	string_append_with_format(&pathArchivoBitacora, "%s/%s", path_bitacoras, nombreBitacora);
	return pathArchivoBitacora;
}
