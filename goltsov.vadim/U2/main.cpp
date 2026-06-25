#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include "hashtable.hpp"
#include "structPerson.hpp"

namespace goltsov
{
  namespace
  {
    std::istream& skipSpaces(std::istream& is)
    {
      while (is.peek() == ' ')
      {
        is.get();
      }
      return is;
    }

    bool isEOL(std::istream& is)
    {
      if (is.peek() == '\n' || is.peek() == EOF)
      {
        return true;
      }
      else
      {
        return false;
      }
    }

    void sortMeets(List< Meet >*& head)
    {
      if (!head || !head->next)
      {
        return;
      }

      std::vector< Meet > meets;
      List< Meet >* current = head;
      while (current)
      {
        meets.push_back(current->data.value);
        current = current->next;
      }

      std::sort(meets.begin(), meets.end(),
        [](const Meet& a, const Meet& b)
        {
          if (a.id_to != b.id_to)
          {
            return a.id_to < b.id_to;
          }
          return a.length < b.length;
        });

      deleteList(head);
      head = nullptr;

      for (const auto& m : meets)
      {
        List< Meet >* new_node = newListNode< Meet >(m.id_to, m, nullptr, nullptr);
        if (!head)
        {
          head = new_node;
        }
        else
        {
          List< Meet >* last = head;
          while (last->next)
          {
            last = last->next;
          }
          last->next = new_node;
          new_node->prev = last;
        }
      }
    }
  }

  std::pair< size_t, size_t > readPersons(HashTable< Person >& ht, std::istream& is, List< Person >** l)
  {
    size_t id;
    std::string info;
    List< Person >* start_l = *l;
    size_t good = 0, bad = 0;

    while (is)
    {
      if (is >> id)
      {
        skipSpaces(is);
        if (!isEOL(is))
        {
          is >> info;
          try
          {
            insertToHT(ht, id, {id, info});
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
          catch (...)
          {
            ++bad;
          }
        }
        else
        {
          try
          {
            if (!existsInHT(ht, id))
            {
              insertToHT(ht, id, {id, ""});
            }
          }
          catch (...)
          {
            ++bad;
          }
        }
      }
      else
      {
        if (!is.eof())
        {
          is.clear();
          char c;
          is >> c;
        }
        else
        {
          break;
        }
      }
    }
    *l = start_l;
    return {good, bad};
  }

  void readData(std::istream& ds, HashTable< Meet_arr >& ht_meet)
  {
    size_t id_from, id_to, length;
    while (ds >> id_from >> id_to >> length)
    {
      if (id_from == id_to)
      {
        continue;
      }

      try
      {
        Meet_arr& arr = getFromHT(ht_meet, id_from);
        List< Meet >* new_node = newListNode< Meet >(id_to, {id_to, length}, nullptr, nullptr);
        if (!arr.data)
        {
          arr.data = new_node;
        }
        else
        {
          List< Meet >* current = arr.data;
          while (current->next)
          {
            current = current->next;
          }
          current->next = new_node;
          new_node->prev = current;
        }
      }
      catch (const std::runtime_error&)
      {
        Meet_arr new_arr;
        new_arr.id = id_from;
        new_arr.data = newListNode< Meet >(id_to, {id_to, length}, nullptr, nullptr);
        insertToHT(ht_meet, id_from, new_arr);
      }
    }
  }

  void printMeets(std::ostream& os, List< Meet >* head)
  {
    if (!head)
    {
      return;
    }

    sortMeets(head);

    List< Meet >* current = head;
    bool first = true;
    while (current)
    {
      if (!first)
      {
        os << '\n';
      }
      os << current->data.value.id_to << ' ' << current->data.value.length;
      first = false;
      current = current->next;
    }
  }

  void processCommands(std::istream& is,
                       HashTable< Person >& ht_pers,
                       HashTable< Meet_arr >& ht_meet,
                       std::ostream& os,
                       std::ostream& err)
  {
    std::string cmd;

    while (is >> cmd)
    {
      try
      {
        if (cmd == "anons")
        {
          std::vector< size_t > anons;
          for (size_t i = 0; i < ht_pers.size; ++i)
          {
            List< Person >* node = ht_pers.data[i];
            while (node)
            {
              if (node->data.value.info.empty())
              {
                anons.push_back(node->data.key);
              }
              node = node->next;
            }
          }
          std::sort(anons.begin(), anons.end());
          for (size_t id : anons)
          {
            os << id << '\n';
          }
        }
        else if (cmd == "deanon")
        {
          size_t anon_id, id;
          if (!(is >> anon_id >> id))
          {
            os << "<INVALID COMMAND>\n";
            continue;
          }

          if (existsInHT(ht_pers, anon_id) || !existsInHT(ht_pers, id))
          {
            os << "<INVALID COMMAND>\n";
            continue;
          }

          try
          {
            std::string info = getFromHT(ht_pers, id).info;
            insertToHT(ht_pers, anon_id, {anon_id, info});

            Meet_arr& arr_from = getFromHT(ht_meet, anon_id);
            Meet_arr& arr_to = getFromHT(ht_meet, id);

            List< Meet >* current = arr_from.data;
            while (current)
            {
              if (current->data.value.id_to != anon_id)
              {
                List< Meet >* new_node = newListNode< Meet >(
                  current->data.value.id_to,
                  current->data.value,
                  nullptr,
                  nullptr
                );
                if (!arr_to.data)
                {
                  arr_to.data = new_node;
                }
                else
                {
                  List< Meet >* last = arr_to.data;
                  while (last->next)
                  {
                    last = last->next;
                  }
                  last->next = new_node;
                  new_node->prev = last;
                }
              }
              current = current->next;
            }

            deleteList(arr_from.data);
            arr_from.data = nullptr;
            deleteFromHT(ht_meet, anon_id);
          }
          catch (...)
          {
            os << "<INVALID COMMAND>\n";
          }
        }
        else if (cmd == "desc")
        {
          size_t id;
          if (!(is >> id))
          {
            os << "<INVALID COMMAND>\n";
            continue;
          }

          try
          {
            Person& p = getFromHT(ht_pers, id);
            if (p.info.empty())
            {
              os << "<ANON>\n";
            }
            else
            {
              os << p.info << '\n';
            }
          }
          catch (...)
          {
            os << "<INVALID COMMAND>\n";
          }
        }
        else if (cmd == "redesc")
        {
          size_t id;
          if (!(is >> id))
          {
            os << "<INVALID COMMAND>\n";
            continue;
          }

          char quote;
          is >> quote;
          if (quote != '"')
          {
            os << "<INVALID COMMAND>\n";
            continue;
          }

          std::string desc;
          std::getline(is, desc, '"');

          try
          {
            Person& p = getFromHT(ht_pers, id);
            p.info = desc;
          }
          catch (...)
          {
            insertToHT(ht_pers, id, {id, desc});
          }
        }
        else if (cmd == "meets")
        {
          size_t id;
          if (!(is >> id))
          {
            os << "<INVALID COMMAND>\n";
            continue;
          }

          try
          {
            Meet_arr& arr = getFromHT(ht_meet, id);
            if (!arr.data)
            {
              os << '\n';
            }
            else
            {
              printMeets(os, arr.data);
              os << '\n';
            }
          }
          catch (...)
          {
            os << "<INVALID COMMAND>\n";
          }
        }
        else if (cmd == "commons")
        {
          size_t id1, id2;
          if (!(is >> id1 >> id2))
          {
            os << "<INVALID COMMAND>\n";
            continue;
          }

          std::vector< size_t > common;
          try
          {
            Meet_arr& arr1 = getFromHT(ht_meet, id1);
            Meet_arr& arr2 = getFromHT(ht_meet, id2);

            List< Meet >* node1 = arr1.data;
            while (node1)
            {
              List< Meet >* node2 = arr2.data;
              while (node2)
              {
                if (node1->data.value.id_to == node2->data.value.id_to)
                {
                  common.push_back(node1->data.value.id_to);
                  break;
                }
                node2 = node2->next;
              }
              node1 = node1->next;
            }
          }
          catch (...)
          {
            os << "<INVALID COMMAND>\n";
            continue;
          }

          std::sort(common.begin(), common.end());
          auto last = std::unique(common.begin(), common.end());
          common.erase(last, common.end());

          for (size_t id : common)
          {
            os << id << '\n';
          }
        }
        else if (cmd == "less")
        {
          size_t time, id;
          if (!(is >> time >> id))
          {
            os << "<INVALID COMMAND>\n";
            continue;
          }

          try
          {
            Meet_arr& arr = getFromHT(ht_meet, id);
            List< Meet >* filtered = nullptr;
            List< Meet >* current = arr.data;
            while (current)
            {
              if (current->data.value.length < time)
              {
                List< Meet >* new_node = newListNode< Meet >(
                  current->data.value.id_to,
                  current->data.value,
                  nullptr,
                  nullptr
                );
                if (!filtered)
                {
                  filtered = new_node;
                }
                else
                {
                  List< Meet >* last = filtered;
                  while (last->next)
                  {
                    last = last->next;
                  }
                  last->next = new_node;
                  new_node->prev = last;
                }
              }
              current = current->next;
            }
            if (!filtered)
            {
              os << '\n';
            }
            else
            {
              printMeets(os, filtered);
              os << '\n';
            }
            deleteList(filtered);
          }
          catch (...)
          {
            os << "<INVALID COMMAND>\n";
          }
        }
        else if (cmd == "greater")
        {
          size_t time, id;
          if (!(is >> time >> id))
          {
            os << "<INVALID COMMAND>\n";
            continue;
          }

          try
          {
            Meet_arr& arr = getFromHT(ht_meet, id);
            List< Meet >* filtered = nullptr;
            List< Meet >* current = arr.data;
            while (current)
            {
              if (current->data.value.length > time)
              {
                List< Meet >* new_node = newListNode< Meet >(
                  current->data.value.id_to,
                  current->data.value,
                  nullptr,
                  nullptr
                );
                if (!filtered)
                {
                  filtered = new_node;
                }
                else
                {
                  List< Meet >* last = filtered;
                  while (last->next)
                  {
                    last = last->next;
                  }
                  last->next = new_node;
                  new_node->prev = last;
                }
              }
              current = current->next;
            }
            if (!filtered)
            {
              os << '\n';
            }
            else
            {
              printMeets(os, filtered);
              os << '\n';
            }
            deleteList(filtered);
          }
          catch (...)
          {
            os << "<INVALID COMMAND>\n";
          }
        }
        else if (cmd == "out-persons")
        {
          std::string filename;
          if (!(is >> filename))
          {
            os << "<INVALID COMMAND>\n";
            continue;
          }

          std::ofstream outFile(filename);
          if (!outFile.is_open())
          {
            err << "Cannot open output file\n";
            continue;
          }

          std::vector< Person > persons;
          for (size_t i = 0; i < ht_pers.size; ++i)
          {
            List< Person >* node = ht_pers.data[i];
            while (node)
            {
              if (!node->data.value.info.empty())
              {
                persons.push_back(node->data.value);
              }
              node = node->next;
            }
          }

          std::sort(persons.begin(), persons.end(),
            [](const Person& a, const Person& b)
            {
              return a.id < b.id;
            });

          for (const auto& p : persons)
          {
            outFile << p.id << ' ' << p.info << '\n';
          }
        }
        else
        {
          os << "<INVALID COMMAND>\n";
        }
      }
      catch (const std::exception&)
      {
        os << "<INVALID COMMAND>\n";
      }
    }
  }
}

int main(int argc, char** argv)
{
  using namespace goltsov;

  std::fstream inFile;
  std::fstream dataFile;
  std::string dataFilename;
  std::istream* is = &std::cin;
  std::istream* ds = &std::cin;
  std::ostream* os = &std::cout;

  if (argc > 3)
  {
    std::cerr << "Many args\n";
    return 0;
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
    else if (arg.substr(0, 5) == "data:" && dataFilename.empty())
    {
      dataFilename = arg.substr(5);
      dataFile.open(dataFilename);
      if (!dataFile.is_open())
      {
        std::cerr << "Cannot open data file\n";
        return 2;
      }
      ds = &dataFile;
    }
    else
    {
      std::cerr << "Invalid argument: " << arg << '\n';
      return 1;
    }
  }

  if (dataFilename.empty())
  {
    std::cerr << "Invalid arguments: no data file\n";
    return 1;
  }

  HashTable< Person > ht = newHT< Person >(100);
  List< Person >* l = nullptr;

  readPersons(ht, *is, &l);
  HashTable< Meet_arr > ht_meets = newHT< Meet_arr >(100);
  readData(*ds, ht_meets);

  processCommands(std::cin, ht, ht_meets, std::cout, std::cerr);

  deleteHashTable(ht);
  deleteHashTable(ht_meets);
  deleteList(l);

  return 0;
}
