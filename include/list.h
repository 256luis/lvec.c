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

// allocated memory for a new list
#define lvec_new( T ) \
    _lvec_new( sizeof( T ) )

// appends an element to the end of the list (use this for primitive types and enums)
#define lvec_append( lvec, data ) \
    _lvec_append( ( void** )&( lvec ), &( typeof( *( lvec ) ) ){ ( data ) } )

// appends an element to the end of the list (use this for structs and unions)
#define lvec_append_aggregate( lvec, data ) \
    _lvec_append( ( void** )&( lvec ), &( data ) )

// appends an element at the specified index (use this for primitive types and enums)
#define lvec_insert( lvec, data, index ) \
    _lvec_insert( ( void** )&( lvec ), &( typeof( *( lvec ) ) ){ ( data ) }, ( index ) )

// appends an element at the specified index (use this for structs and unions)
#define lvec_insert_aggregate( lvec, data, index ) \
    _lvec_insert( ( void** )&( lvec ), &( data ), ( index ) )

// removes an element by index
#define lvec_remove( lvec, index ) \
    _lvec_remove( ( void** )&( lvec ), ( index ) )

// removes the last element in the list
#define lvec_remove_last( lvec ) \
    _lvec_remove( ( void** )&( lvec ), list_get_length( lvec ) - 1 )

// allocates enough memory for at least the specified capacity
#define lvec_reserve_minimum( lvec, desired_capacity ) \
    _lvec_reserve_minimum( ( void** )&( lvec ), ( desired_capacity ) )

// resizes array to match the length of the list
#define lvec_shrink_to_fit( lvec ) \
    _lvec_shrink_to_fit( ( void** )&( lvec ) )

// checks if length of list is 0
#define lvec_is_empty( lvec ) \
    ( lvecget_length( lvec ) == 0 )

// used as return values for functions that can fail
typedef enum LVecResult
{
    LVECRESULT_SUCCESS,
    LVECRESULT_ERROR_OUT_OF_BOUNDS,
    LVECRESULT_ERROR_ALLOCATION,
} LVecResult;

// allocates memory for a new list (use the macro version)
void* _lvec_new( size_t size );

// deallocates memory for allocated list
void lvec_free( void* lvec );

// allocates enough memory for at least the specified capacity (use the macro version)
LVecResult _lvec_reserve_minimum( void** lvec, size_t desired_capacity );

// resizes array to match the length of the list (use the macro version)
LVecResult _lvec_shrink_to_fit( void** lvec );

// appends an element to the end of the list (use macro version)
LVecResult _lvec_append( void** lvec, void* data );

// appends an element at the specified index (use macro version)
LVecResult _lvec_insert( void** lvec, void* input_data, size_t index );

// removes an element by index (use the macro version)
LVecResult _lvec_remove( void** lvec, size_t index );

// clears the list
void lvec_clear( void* lvec );

// returns the number of items in the list
size_t lvec_get_length( void* lvec );

// returns how many elements can currently fit on the list
size_t lvec_get_capacity( void* lvec );

#endif
