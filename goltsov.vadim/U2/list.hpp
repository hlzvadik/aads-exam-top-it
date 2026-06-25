#ifndef LIST_HPP
#define LIST_HPP
#include <iosfwd>

namespace goltsov
{
  namespace detail
  {
    template < class T >
    struct NodeHT
    {
      size_t key;
      T value;
    };
  }

  template< class T >
  struct List
  {
    detail::NodeHT< T > data;
    List* prev;
    List* next;
  };

  template< class T >
  void deleteList(List< T >* current)
  {
    if (current)
    {
      if (current->prev)
      {
        while (current->prev)
        {
          current = current->prev;
        }
      }
      while (current)
      {
        List< T >* for_delete = current;
        current = current->next;
        delete for_delete;
      }
    }
  }

  template< class T >
  List< T >* newListNode(size_t key, T value, List< T >* prev, List< T >* next)
  {
    return new List< T >{{key, value}, prev, next};
  }

  template< class T >
  struct HashTable
  {
    List< T >** data;
    size_t size;
  };
}

#endif
