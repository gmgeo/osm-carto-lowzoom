#include <algorithm>
#include <iostream>
#include <unordered_map>
#include <string>
#include <sstream>
#include <vector>

#include <osmium/io/pbf_input.hpp>
#include <osmium/io/pbf_output.hpp>
#include <osmium/io/output_iterator.hpp>
#include <osmium/handler.hpp>
#include <osmium/osm/types.hpp>

class PlaceHandler : public osmium::handler::Handler {

  std::unordered_map<long long int, osmium::item_type>& id_store;

public:

  PlaceHandler(std::unordered_map<long long int, osmium::item_type>& id_store_)
  : id_store(id_store_) {}

  void node(const osmium::Node& node) {
    std::string place(node.tags()["place"] ? node.tags()["place"] : "");

    if (!place.empty() && (place == std::string("city") || place == std::string("town") ||
        place == std::string("village") || place == std::string("hamlet"))) {
      id_store[node.id()] = osmium::item_type::node;
    }
  }
};

class HighwayHandler : public osmium::handler::Handler {

  std::unordered_map<long long int, osmium::item_type>& id_store;

public:

  HighwayHandler(std::unordered_map<long long int, osmium::item_type>& id_store_)
  : id_store(id_store_) {}

  void way(const osmium::Way& way) {

    std::string highway(way.tags()["highway"] ? way.tags()["highway"] : "");
    std::string railway(way.tags()["railway"] ? way.tags()["railway"] : "");

    if (highway == std::string("motorway") || highway == std::string("trunk") || railway == std::string("rail")) {
      id_store[way.id()] = osmium::item_type::way;
      for (const osmium::NodeRef& nr : way.nodes()) {
          id_store[nr.ref()] = osmium::item_type::node;
      }
    }
  }
};

class GlacierHandler : public osmium::handler::Handler {

  std::unordered_map<long long int, osmium::item_type>& id_store;

public:

  GlacierHandler(std::unordered_map<long long int, osmium::item_type>& id_store_)
  : id_store(id_store_) {}

  void way(const osmium::Way& way) {

    std::string natural(way.tags()["natural"] ? way.tags()["natural"] : "");

    if (natural == std::string("glacier") || natural == std::string("water")) {
      auto id = id_store.find(way.id());
      if (id != id_store.end()) {
        id_store[way.id()] = osmium::item_type::way;
        for (const osmium::NodeRef& nr : way.nodes()) {
            id_store[nr.ref()] = osmium::item_type::node;
        }
      }
    }
  }

  void relation(const osmium::Relation& rel) {
    std::string natural(rel.tags()["natural"] ? rel.tags()["natural"] : "");

    if (natural == std::string("glacier") || natural == std::string("water")) {
      id_store[rel.id()] = osmium::item_type::relation;

      const osmium::RelationMemberList& rml = rel.members();
      for (const osmium::RelationMember& rm : rml) {
        if (rm.type() == osmium::item_type::way) {
          id_store[rm.ref()] = rm.type();
        }
      }
    }
  }
};

class FerryHandler : public osmium::handler::Handler {

  std::unordered_map<long long int, osmium::item_type>& id_store;

public:

  FerryHandler(std::unordered_map<long long int, osmium::item_type>& id_store_)
  : id_store(id_store_) {}

  void way(const osmium::Way& way) {

    std::string natural(way.tags()["route"] ? way.tags()["route"] : "");

    if (natural == std::string("ferry")) {
      auto id = id_store.find(way.id());
      if (id != id_store.end()) {
        id_store[way.id()] = osmium::item_type::way;
        for (const osmium::NodeRef& nr : way.nodes()) {
            id_store[nr.ref()] = osmium::item_type::node;
        }
      }
    }
  }

  void relation(const osmium::Relation& rel) {
    std::string natural(rel.tags()["route"] ? rel.tags()["route"] : "");

    if (natural == std::string("ferry")) {
      id_store[rel.id()] = osmium::item_type::relation;

      const osmium::RelationMemberList& rml = rel.members();
      for (const osmium::RelationMember& rm : rml) {
        if (rm.type() == osmium::item_type::way) {
          id_store[rm.ref()] = rm.type();
        }
      }
    }
  }
};

class AdminHandler : public osmium::handler::Handler {

  std::unordered_map<long long int, osmium::item_type>& id_store;

public:

  AdminHandler(std::unordered_map<long long int, osmium::item_type>& id_store_)
  : id_store(id_store_) {}

  void way(const osmium::Way& way) {
    std::string boundary(way.tags()["boundary"] ? way.tags()["boundary"] : "");
    std::string type(way.tags()["type"] ? way.tags()["type"] : "");
    std::string admin_level(way.tags()["admin_level"] ? way.tags()["admin_level"] : "");

    if ((boundary == std::string("administrative") && type == std::string("boundary") &&
      (admin_level == std::string("2") || admin_level == std::string("3") || admin_level == std::string("4"))) ||
      boundary == std::string("nature_reserve") || boundary == std::string("national_park")) {

      auto id = id_store.find(way.id());
      if (id != id_store.end()) {
        id_store[way.id()] = osmium::item_type::way;
        for (const osmium::NodeRef& nr : way.nodes()) {
            id_store[nr.ref()] = osmium::item_type::node;
        }
      }
    }
  }

  void relation(const osmium::Relation& rel) {
    std::string boundary(rel.tags()["boundary"] ? rel.tags()["boundary"] : "");
    std::string type(rel.tags()["type"] ? rel.tags()["type"] : "");
    std::string admin_level(rel.tags()["admin_level"] ? rel.tags()["admin_level"] : "");

    // add boundary relations
    if ((boundary == std::string("administrative") && type == std::string("boundary") &&
      (admin_level == std::string("2") || admin_level == std::string("3") || admin_level == std::string("4"))) ||
      boundary == std::string("nature_reserve") || boundary == std::string("national_park")) {

      id_store[rel.id()] = osmium::item_type::relation;

      const osmium::RelationMemberList& rml = rel.members();
      for (const osmium::RelationMember& rm : rml) {
        if (rm.type() == osmium::item_type::way) {
          id_store[rm.ref()] = rm.type();
        }
      }
    }
  }
};

class RelationWayHandler : public osmium::handler::Handler {

  std::unordered_map<long long int, osmium::item_type>& id_store;

public:

  RelationWayHandler(std::unordered_map<long long int, osmium::item_type>& id_store_)
  : id_store(id_store_) {}

  void way(const osmium::Way& way) {

    // add nodes to stored ways from relations
    auto id = id_store.find(way.id());
    if (id != id_store.end() && id->second == osmium::item_type::way) {
      for (const osmium::NodeRef& nr : way.nodes()) {
        id_store[nr.ref()] = osmium::item_type::node;
      }
    }
  }
};


int main(int argc, char* argv[]) {

  if (argc != 3) {
    std::cerr << "Usage: " << argv[0] << " input.osm.pbf output.osm.pbf\n";
    exit(1);
  }

  osmium::io::File input_file(argv[1]);
  osmium::io::File output_file(argv[2]);

  std::unordered_map<long long int, osmium::item_type> id_store;

  std::cerr << "pass 1 - getting relations and (their) ways\n";

  osmium::io::Reader reader1(input_file, osmium::osm_entity_bits::relation); // we need only relations this time
  AdminHandler admin_handler(id_store);
  GlacierHandler glacier_handler(id_store);
  FerryHandler ferry_handler(id_store);

  osmium::apply(reader1, admin_handler, glacier_handler, ferry_handler);
  reader1.close();

  std::cerr << "pass 2 - getting place nodes and nodes of ways\n";

  osmium::io::Reader reader2(input_file, osmium::osm_entity_bits::way | osmium::osm_entity_bits::node);
  HighwayHandler highway_handler(id_store);
  PlaceHandler place_handler(id_store);
  RelationWayHandler relation_way_handler(id_store);

  osmium::apply(reader2, glacier_handler, admin_handler, highway_handler, place_handler, relation_way_handler);
  reader2.close();

  std::cerr << "pass 3 - writing objects to file\n";

  osmium::io::Reader reader3(input_file, osmium::osm_entity_bits::relation | osmium::osm_entity_bits::way | osmium::osm_entity_bits::node);
  osmium::io::Header header = reader3.header();
  osmium::io::Writer writer(output_file, header, osmium::io::overwrite::allow);

  auto input_begin = osmium::io::InputIterator<osmium::io::Reader, osmium::OSMObject> {reader3};
  auto input_end = osmium::io::InputIterator<osmium::io::Reader, osmium::OSMObject> {};
  auto output_iterator = osmium::io::OutputIterator<osmium::io::Writer> {writer};

  std::copy_if(input_begin, input_end, output_iterator, [&id_store](const osmium::OSMObject& obj) {
    auto id = id_store.find(obj.id());
    if (id != id_store.end()) {
      return true;
    }

    return false;
  });

  writer.flush();
  writer.close();
  reader3.close();

  std::cerr << "done.\n";
}
