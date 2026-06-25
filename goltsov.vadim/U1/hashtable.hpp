#ifndef HASHTABLE_HPP
#define HASHTABLE_HPP
#include <iosfwd>
#include <stdexcept>
#include <functional>
#include "structPerson.hpp"
#include "list.hpp"

namespace goltsov
{
  template< class T >
  HashTable< T > newHT(size_t size)
  {
    HashTable< T > ht;
    ht.data = new List< T >*[size];
    for (size_t i = 0; i < size; ++i)
    {
      ht.data[i] = nullptr;
    }
    ht.size = size;
    return ht;
  }
  template< class T >
  void deleteHashTable(HashTable< T >& ht)
  {
    if (ht.data)
    {
      for (size_t i = 0; i < ht.size; ++i)
      {
        deleteList(ht.data[i]);
      }
      delete[] ht.data;
      ht.data = nullptr;
    }
  }
  template< class T >
  T& getFromHT(HashTable< T >& ht, size_t id)
  {
    std::hash< size_t > hasher;
    size_t hash = hasher(id) % ht.size;
    List< T >* node = ht.data[hash];
    while (node)
    {
      if (node->data.key == id)
      {
        return node->data.value;
      }
      node = node->next;
    }
    throw std::runtime_error("No key");
  }
  template< class T >
  void insertToHT(HashTable< T >& ht, size_t id, T value)
  {
    std::hash< size_t > hasher;
    size_t hash = hasher(id) % ht.size;
    List< T >* node = ht.data[hash];
    while (node)
    {
      if (node->data.key == id)
      {
        throw std::runtime_error("Key in table");
      }
      node = node->next;
    }
    List< T >* new_node = newListNode< T >(id, value, nullptr, nullptr);
    new_node->next = ht.data[hash];
    if (ht.data[hash])
    {
      ht.data[hash]->prev = new_node;
    }
    ht.data[hash] = new_node;
  }
  template< class T >
  void deleteFromHT(HashTable< T >& ht, size_t id)
  {
    std::hash< size_t > hasher;
    size_t hash = hasher(id) % ht.size;
    List< T >* node = ht.data[hash];
    while (node)
    {
      if (node->data.key == id)
      {
        if (node->next)
        {
          node->next->prev = node->prev;
        }
        if (node->prev)
        {
          node->prev->next = node->next;
        }
        else
        {
          ht.data[hash] = node->next;
        }
        delete node;
        return;
      }
      node = node->next;
    }
    throw std::runtime_error("No key");
  }
}

#endif
