#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include "lvec.h"

#define LVEC_INITIAL_CAPACITY 10
#define LVEC_MINIMUM_CAPACITY 2
#define LVEC_GROWTH_RATE 1.5

typedef struct LVecInfo
{
    // header
    size_t length;
    size_t capacity;
    size_t element_size;

    // actual data
    char data[];
} LVecInfo;

typedef union LVecUnion
{
    void* _void;
    char* _char;
    LVecInfo* lvec_info;
} LVecUnion;

void* _lvec_new( size_t type_size )
{
    LVecInfo* lvec_info = malloc( sizeof( LVecInfo ) + ( type_size * LVEC_INITIAL_CAPACITY ) );
    if( lvec_info == NULL )
    {
        return NULL;
    }

    lvec_info->length = 0;
    lvec_info->capacity = LVEC_INITIAL_CAPACITY;
    lvec_info->element_size = type_size;

    return lvec_info->data;
}

static inline LVecInfo* internal_lvec_get_info( void* lvec )
{
    LVecUnion lvec_union = { ._void = lvec };
    lvec_union._char -= offsetof( LVecInfo, data );
    LVecInfo* lvec_info = lvec_union.lvec_info;

    return lvec_info;
}

static inline LVecResult internal_lvec_grow( LVecInfo** lvec_info, float multiplier )
{
    size_t new_capacity = ( *lvec_info )->capacity * multiplier;
    LVecInfo* tmp = realloc( *lvec_info, sizeof( LVecInfo ) + ( ( *lvec_info )->element_size * new_capacity ) );

    if( tmp == NULL )
    {
        return LVECRESULT_ERROR_ALLOCATION;
    }

    *lvec_info = tmp;
    ( *lvec_info )->capacity = new_capacity;

    return LVECRESULT_SUCCESS;
}

void lvec_free( void* lvec )
{
    LVecInfo* lvec_info = internal_lvec_get_info( lvec );
    free( lvec_info );
}

static inline LVecResult internal_lvec_reserve_minimum( LVecInfo** lvec_info, size_t desired_capacity )
{
    if( ( *lvec_info )->capacity >= desired_capacity )
    {
        return LVECRESULT_SUCCESS;
    }

    // TODO: There is a way to calculate times_to_grow without using a loop. Implement
    //       that.
    int times_to_grow = 1;
    while( ( *lvec_info )->capacity * LVEC_GROWTH_RATE * times_to_grow < desired_capacity )
    {
        times_to_grow++;
    }

    LVecResult grow_result = internal_lvec_grow( lvec_info, LVEC_GROWTH_RATE * times_to_grow );
    if( grow_result != LVECRESULT_SUCCESS )
    {
        return grow_result;
    }

    return LVECRESULT_SUCCESS;
}

LVecResult _lvec_reserve_minimum( void** lvec, size_t desired_capacity )
{
    LVecInfo* lvec_info = internal_lvec_get_info( *lvec );

    LVecResult reserve_result = internal_lvec_reserve_minimum( &lvec_info, desired_capacity );
    if( reserve_result != LVECRESULT_SUCCESS )
    {
        return reserve_result;
    }
    *lvec = lvec_info->data;

    return LVECRESULT_SUCCESS;
}

LVecResult _lvec_shrink_to_fit( void** lvec )
{
    LVecInfo* lvec_info = internal_lvec_get_info( *lvec );

    //     new_capacity = max( LIST_MINIMUM_CAPACITY, lvec_info->length );
    size_t new_capacity = ( LVEC_MINIMUM_CAPACITY > lvec_info->length ) ? LVEC_MINIMUM_CAPACITY : lvec_info->length;
    LVecInfo* tmp = realloc( lvec_info, sizeof( LVecInfo ) + ( lvec_info->element_size * new_capacity ) );

    if( tmp == NULL )
    {
        return LVECRESULT_ERROR_ALLOCATION;
    }

    lvec_info = tmp;
    lvec_info->capacity = new_capacity;
    *lvec = lvec_info->data;

    return LVECRESULT_SUCCESS;
}

LVecResult _lvec_append( void** lvec, void* input_data )
{
    LVecInfo* lvec_info = internal_lvec_get_info( *lvec );

    // increase capacity if not enough
    if( lvec_info->length + 1 > lvec_info->capacity )
    {
        LVecResult grow_result = internal_lvec_grow( &lvec_info, LVEC_GROWTH_RATE );
        if( grow_result != LVECRESULT_SUCCESS )
        {
            return grow_result;
        }

        *lvec = lvec_info->data;
    }

    // offset of last element + 1 or where we would place the data to be appended
    size_t bytes_offset = lvec_info->length * lvec_info->element_size;
    memcpy( lvec_info->data + bytes_offset,
            input_data,
            lvec_info->element_size );

    lvec_info->length++;

    return LVECRESULT_SUCCESS;
}

LVecResult _lvec_insert( void** lvec, void* input_data, size_t index )
{
    LVecInfo* lvec_info = internal_lvec_get_info( *lvec );
    if( index >= lvec_info->length ) return LVECRESULT_ERROR_OUT_OF_BOUNDS;

    // increase capacity if not enough
    if( lvec_info->length + 1 > lvec_info->capacity )
    {
        LVecResult grow_result = internal_lvec_grow( &lvec_info, LVEC_GROWTH_RATE );
        if( grow_result != LVECRESULT_SUCCESS )
        {
            return grow_result;
        }

        *lvec = lvec_info->data;
    }

    // get offset at index
    size_t byte_offset_at_index = lvec_info->element_size * index;

    // number of bytes from index to end of array
    size_t remaining_bytes = ( lvec_info->length * lvec_info->element_size ) - byte_offset_at_index;

    // shift everything from index to end of array to the right
    memmove( lvec_info->data + byte_offset_at_index + lvec_info->element_size,
             lvec_info->data + byte_offset_at_index,
             remaining_bytes );

    // insert input_data
    memcpy( lvec_info->data + byte_offset_at_index,
            input_data,
            lvec_info->element_size );

    lvec_info->length++;

    return LVECRESULT_SUCCESS;
}

LVecResult _lvec_remove( void** lvec, size_t index )
{
    LVecInfo* lvec_info = internal_lvec_get_info( *lvec );
    if( index >= lvec_info->length ) return LVECRESULT_ERROR_OUT_OF_BOUNDS;

    lvec_info->length--;

    // if the last element is the one getting removed
    if( index == lvec_info->length )
    {
        return LVECRESULT_SUCCESS;
    }

    // get offset at index
    size_t byte_offset_at_index = lvec_info->element_size * index;

    // number of bytes from index to end of array
    size_t remaining_bytes = ( lvec_info->length * lvec_info->element_size ) - byte_offset_at_index;

    // shift everything from index to end of array to the left
    memmove( lvec_info->data + byte_offset_at_index,
             lvec_info->data + byte_offset_at_index + lvec_info->element_size,
             remaining_bytes );

    return LVECRESULT_SUCCESS;
}

void lvec_clear( void* lvec )
{
    LVecInfo* lvec_info = internal_lvec_get_info( lvec );
    lvec_info->length = 0;
}

size_t lvec_get_length( void* lvec )
{
    LVecInfo* lvec_info = internal_lvec_get_info( lvec );

    return lvec_info->length;
}

size_t lvec_get_capacity( void* lvec )
{
    LVecInfo* lvec_info = internal_lvec_get_info( lvec );

    return lvec_info->capacity;
}
