#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <math.h>
#include "list.h"

#define LIST_INITIAL_CAPACITY 10
#define LIST_MINIMUM_CAPACITY 2
#define LIST_GROWTH_RATE 1.5

typedef struct List
{
    // header
    size_t length;
    size_t capacity;
    size_t element_size;

    // actual data
    char data[];
} List;

typedef union ListUnion
{
    void* _void;
    char* _char;
    List* _list;
} ListUnion;

void* _list_new( size_t type_size )
{
    List* list = malloc( sizeof( List ) + ( type_size * LIST_INITIAL_CAPACITY ) );
    if( list == NULL )
    {
        return NULL;
    }

    list->length = 0;
    list->capacity = LIST_INITIAL_CAPACITY;
    list->element_size = type_size;

    return list->data;
}

static inline List* internal_list_get_header( void* list_data )
{
    ListUnion list_union = { ._void = list_data };
    list_union._char -= offsetof( List, data );
    List* list = list_union._list;

    return list;
}

static inline ListResult internal_list_grow( List** list, float multiplier )
{
    size_t new_capacity = ( *list )->capacity * multiplier;
    List* tmp = realloc( *list, sizeof( List ) + ( ( *list )->element_size * new_capacity ) );

    if( tmp == NULL )
    {
        return LISTRESULT_ERROR_ALLOCATION;
    }

    *list = tmp;
    ( *list )->capacity = new_capacity;

    return LISTRESULT_SUCCESS;
}

void list_free( void* list_data )
{
    List* list = internal_list_get_header( list_data );
    free( list );
}

static inline ListResult internal_list_reserve_minimum( List** list, size_t desired_capacity )
{
    if( ( *list )->capacity >= desired_capacity )
    {
        return LISTRESULT_SUCCESS;
    }

    // TODO: There is a way to calculate times_to_grow without using a loop. Implement
    //       that.
    int times_to_grow = 1;
    while( ( *list )->capacity * LIST_GROWTH_RATE * times_to_grow < desired_capacity )
    {
        times_to_grow++;
    }

    ListResult grow_result = internal_list_grow( list, LIST_GROWTH_RATE * times_to_grow );
    if( grow_result != LISTRESULT_SUCCESS )
    {
        return grow_result;
    }

    return LISTRESULT_SUCCESS;
}

ListResult _list_reserve_minimum( void** list_data, size_t desired_capacity )
{
    List* list = internal_list_get_header( *list_data );

    ListResult reserve_result = internal_list_reserve_minimum( &list, desired_capacity );
    if( reserve_result != LISTRESULT_SUCCESS )
    {
        return reserve_result;
    }
    *list_data = list->data;

    return LISTRESULT_SUCCESS;
}

ListResult _list_shrink_to_fit( void** list_data )
{
    List* list = internal_list_get_header( *list_data );

    //     new_capacity = max( LIST_MINIMUM_CAPACITY, list->length );
    size_t new_capacity = ( LIST_MINIMUM_CAPACITY > list->length ) ? LIST_MINIMUM_CAPACITY : list->length;
    List* tmp = realloc( list, sizeof( List ) + ( list->element_size * new_capacity ) );

    if( tmp == NULL )
    {
        return LISTRESULT_ERROR_ALLOCATION;
    }

    list = tmp;
    list->capacity = new_capacity;
    *list_data = list->data;

    return LISTRESULT_SUCCESS;
}

ListResult _list_append( void** list_data, void* input_data )
{
    List* list = internal_list_get_header( *list_data );

    // increase capacity if not enough
    if( list->length + 1 > list->capacity )
    {
        ListResult grow_result = internal_list_grow( &list, LIST_GROWTH_RATE );
        if( grow_result != LISTRESULT_SUCCESS )
        {
            return grow_result;
        }

        *list_data = list->data;
    }

    // offset of last element + 1 or where we would place the data to be appended
    size_t bytes_offset = list->length * list->element_size;
    memcpy( list->data + bytes_offset,
            input_data,
            list->element_size );

    list->length++;

    return LISTRESULT_SUCCESS;
}

ListResult _list_insert( void** list_data, void* input_data, size_t index )
{
    List* list = internal_list_get_header( *list_data );
    if( index >= list->length ) return LISTRESULT_ERROR_OUT_OF_BOUNDS;

    // increase capacity if not enough
    if( list->length + 1 > list->capacity )
    {
        ListResult grow_result = internal_list_grow( &list, LIST_GROWTH_RATE );
        if( grow_result != LISTRESULT_SUCCESS )
        {
            return grow_result;
        }

        *list_data = list->data;
    }

    // get offset at index
    size_t byte_offset_at_index = list->element_size * index;

    // number of bytes from index to end of array
    size_t remaining_bytes = ( list->length * list->element_size ) - byte_offset_at_index;

    // shift everything from index to end of array to the right
    memmove( list->data + byte_offset_at_index + list->element_size,
             list->data + byte_offset_at_index,
             remaining_bytes );

    // insert input_data
    memcpy( list->data + byte_offset_at_index,
            input_data,
            list->element_size );

    list->length++;

    return LISTRESULT_SUCCESS;
}

ListResult _list_remove( void** list_data, size_t index )
{
    List* list = internal_list_get_header( *list_data );
    if( index >= list->length ) return LISTRESULT_ERROR_OUT_OF_BOUNDS;

    list->length--;

    // if the last element is the one getting removed
    if( index == list->length )
    {
        return LISTRESULT_SUCCESS;
    }

    // get offset at index
    size_t byte_offset_at_index = list->element_size * index;

    // number of bytes from index to end of array
    size_t remaining_bytes = ( list->length * list->element_size ) - byte_offset_at_index;

    // shift everything from index to end of array to the left
    memmove( list->data + byte_offset_at_index,
             list->data + byte_offset_at_index + list->element_size,
             remaining_bytes );

    return LISTRESULT_SUCCESS;
}

void list_clear( void* list_data )
{
    List* list = internal_list_get_header( list_data );
    list->length = 0;
}

size_t list_get_length( void* list_data )
{
    List* list = internal_list_get_header( list_data );

    return list->length;
}

size_t list_get_capacity( void* list_data )
{
    List* list = internal_list_get_header( list_data );

    return list->capacity;
}
