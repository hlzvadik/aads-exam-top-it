#include <iostream>
#include <string>
#include <fstream>
#include <cctype>
#include "hashtable.hpp"

namespace
{
  std::istream& skipSpaces(std::istream& is)
  {
    while (std::isspace(is.peek()))
    {
      is.get();
    }
    return is;
  }
  bool isEmptyLine(std::istream& is)
  {
    int ch = is.peek();
    return (ch == '\n' || ch == EOF);
  }
}

namespace goltsov
{
  std::pair< size_t, size_t > readPersons(HashTable< Person >& ht, std::istream& is, List< Person >** l)
  {
    size_t id;
    std::string info;
    List< Person >* start_l = *l;
    size_t good = 0, bad = 0;
    while (is)
    {
      if (isEmptyLine(is))
      {
        is.get();
        continue;
      }
      if (is >> id)
      {
        skipSpaces(is);
        std::getline(is, info);
        size_t start = info.find_first_not_of(" \t");
        if (start != std::string::npos)
        {
          info = info.substr(start);
        }
        else
        {
          info.clear();
          ++bad;
          continue;
        }
        try
        {
          insertToHT< Person >(ht, id, {id, info});
          ++good;
          if (*l)
          {
            (*l)->next = newListNode< Person >(id, {id, info}, *l, nullptr);
            (*l) = (*l)->next;
          }
          else
          {
            *l = newListNode< Person >(id, {id, info}, *l, nullptr);
            start_l = *l;
          }
        }
        catch (const std::runtime_error&)
        {
          ++bad;
        }
      }
      else
      {
        if (!is.eof())
        {
          is.clear();
          std::string dummy;
          std::getline(is, dummy);
          ++bad;
        }
      }
    }
    *l = start_l;
    return {good, bad};
  }
  std::ostream& printRes(std::ostream& os, List< Person >* l)
  {
    if (l)
    {
      os << l->data.key << ' ' << l->data.value.info;
      l = l->next;
    }
    while (l)
    {
      os << '\n' << l->data.key << ' ' << l->data.value.info;
      l = l->next;
    }
    return os;
  }
}

int main(int argc, char **argv)
{
  std::fstream inFile;
  std::fstream outFile;
  std::string outFilename;
  std::istream* is = &std::cin;
  std::ostream* os = &std::cout;
  if (argc > 3)
  {
    std::cerr << "Many args\n";
    return 1;
  }
  for (int i = 1; i < argc; ++i)
  {
    std::string arg = argv[i];
    if (arg.substr(0, 3) == "in:" && is == &std::cin)
    {
      std::string filename = arg.substr(3);
      inFile.open(filename);
      if (!inFile.is_open())
      {
        std::cerr << "Cannot open input file\n";
        return 2;
      }
      is = &inFile;
    }
    else if (arg.substr(0, 4) == "out:" && outFilename.empty())
    {
      outFilename = arg.substr(4);
    }
    else
    {
      std::cerr << "Invalid argument: " << arg << '\n';
      return 1;
    }
  }
  goltsov::HashTable< goltsov::Person > ht = goltsov::newHT< goltsov::Person >(100);
  goltsov::List< goltsov::Person >* l = nullptr;
  std::pair< size_t, size_t > res = goltsov::readPersons(ht, *is, &l);
  if (!outFilename.empty())
  {
    outFile.open(outFilename, std::ios::trunc);
    if (!outFile.is_open())
    {
      std::cerr << "Cannot open output file\n";
      goltsov::deleteHashTable(ht);
      goltsov::deleteList(l);
      return 2;
    }
    os = &outFile;
  }
  goltsov::printRes(*os, l);
  *os << '\n';
  std::cerr << res.first << ' ' << res.second << '\n';
  goltsov::deleteHashTable(ht);
  goltsov::deleteList(l);
}
