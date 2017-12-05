#include "cache.h"

void cacheAdd(char * buffer, int site_size, char* url) {


    pthread_rwlock_wrlock(&lock);
    while(cache_size + site_size + sizeof(cache_object) >= MAX_CACHE_SIZE ){
        cacheRemove();
    }

    cache_object * data = (cache_object *)malloc(sizeof(cache_object));

    data -> next = head;
    data -> data = buffer;
    data -> lru_time_track = global_time++;
    data -> size = site_size;
    data -> url = malloc(( strlen( url ) + 1 ) * sizeof( char ));
    strcpy( data -> url, url );
    head = data;
    cache_size +=  sizeof( cache_object ) + site_size + strlen( url ) + 1;


    pthread_rwlock_unlock( &lock );
    return;
}

void cacheRemove(){

	cache_object * previous ;  	/* Previous pointer */
	cache_object * current ;		/* Current Pointer */
	cache_object * aux ;	/* pointer to free */

    /* Percorre a lista cache */
	if( head != NULL) {
		for (current = head, previous = head, aux =head ; current -> next != NULL; current = current -> next) {
			if(( (current -> next) -> lru_time_track) < (aux -> lru_time_track)) {
				aux = current -> next;
				previous = current;
			}
		}

		/* free no objeto da lista*/
		if(aux == head) {
			head = head -> next;
		} else {
			previous->next = aux->next;
		}

        /*seta o valor atualizado do tamanho do cache */
		cache_size = cache_size - (aux -> size) - sizeof(cache_object) -
			strlen(aux -> url) - 1;

		free(aux->data);
		free(aux->url);
		free(aux);
	}
	return;
}

cache_object* cacheFind( char * url ){

	cache_object * data = NULL;

	if( head != NULL ){
		for ( data = head; data != NULL ; data = data -> next ){
			if (!strcmp( data -> url, url ))
				break;
		}
	} else
		return NULL;

	return data;
}
