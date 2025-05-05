/*
  list.c is a simple generic dynamic array implementation.

  NOTE: Use of this library requires 'typeof' support.

  Basic usage:

      int main()
      {
          int* nums = list_new(int);

          list_append(nums, 10);
          list_append(nums, 20);
          for (size_t i = 0; i < list_get_length(nums); i++)
          {
              printf("%d", nums[i]);
          }

          free(nums);
      }

  Make sure to use the *_aggregate functions when appending and inserting to a list
  of structs or unions.

      typedef struct Vector2
      {
          int x;
          int y;
      } Vector2;

      int main()
      {
          Vector2* vecs = list_new(Vector2);

          Vector2 vec1 = { 0, 1 };
          list_append_aggregate(vecs, vec1);

          Vector2 vec2 = { 3, 6 };
          list_append_aggregate(vecs, vec2);

          free(nums);
      }

  You can also check the return value for success or failure.

      int main()
      {
          int* nums = list_new(int);

          ListResult result = list_append(nums, 20);
          if (result != LISTRESULT_SUCCESS)
          {
              // handle error
          }

          free(nums);
      }

 */

#ifndef LIST_H
#define LIST_H

#define List( T ) T

// allocated memory for a new list
#define list_new( T ) \
    _list_new( sizeof( T ) )

// appends an element to the end of the list (use this for primitive types and enums)
#define list_append( list_data, data ) \
    _list_append( ( void** )&( list_data ), &( typeof( *( list_data ) ) ){ ( data ) } )

// appends an element to the end of the list (use this for structs and unions)
#define list_append_aggregate( list_data, data ) \
    _list_append( ( void** )&( list_data ), &( data ) )

// appends an element at the specified index (use this for primitive types and enums)
#define list_insert( list_data, data, index ) \
    _list_insert( ( void** )&( list_data ), &( typeof( *( list_data ) ) ){ ( data ) }, ( index ) )

// appends an element at the specified index (use this for structs and unions)
#define list_insert_aggregate( list_data, data, index ) \
    _list_insert( ( void** )&( list_data ), &( data ), ( index ) )

// removes an element by index
#define list_remove( list_data, index ) \
    _list_remove( ( void** )&( list_data ), ( index ) )

// removes the last element in the list
#define list_remove_last( list_data ) \
    _list_remove( ( void** )&( list_data ), list_get_length( list_data ) - 1 )

// allocates enough memory for at least the specified capacity
#define list_reserve_minimum( list_data, desired_capacity ) \
    _list_reserve_minimum( ( void** )&( list_data ), ( desired_capacity ) )

// resizes array to match the length of the list
#define list_shrink_to_fit( list_data ) \
    _list_shrink_to_fit( ( void** )&( list_data ) )

// checks if length of list is 0
#define list_is_empty( list_data ) \
    ( list_get_length( list_data ) == 0 )

// used as return values for functions that can fail
typedef enum ListResult
{
    LISTRESULT_SUCCESS,
    LISTRESULT_ERROR_OUT_OF_BOUNDS,
    LISTRESULT_ERROR_ALLOCATION,
} ListResult;

// allocates memory for a new list (use the macro version)
void* _list_new( size_t size );

// deallocates memory for allocated list
void list_free( void* list_data );

// allocates enough memory for at least the specified capacity (use the macro version)
ListResult _list_reserve_minimum( void** list_data, size_t desired_capacity );

// resizes array to match the length of the list (use the macro version)
ListResult _list_shrink_to_fit( void** list_data );

// appends an element to the end of the list (use macro version)
ListResult _list_append( void** list_data, void* data );

// appends an element at the specified index (use macro version)
ListResult _list_insert( void** list_data, void* input_data, size_t index );

// removes an element by index (use the macro version)
ListResult _list_remove( void** list_data, size_t index );

// clears the list
void list_clear( void* list_data );

// returns the number of items in the list
size_t list_get_length( void* list_data );

// returns how many elements can currently fit on the list
size_t list_get_capacity( void* list_data );

#endif
