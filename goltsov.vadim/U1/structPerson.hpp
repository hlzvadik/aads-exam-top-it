#ifndef STRUCTPERSON_HPP
#define STRUCTPERSON_HPP
#include <iosfwd>
#include <string>
namespace goltsov
{
  struct Person
  {
    size_t id;
    std::string info;
  };
}
#endif
