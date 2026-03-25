#define CATCH_CONFIG_MAIN

#include "map/map.h"
#include <catch2/catch_all.hpp>
#include <string>
#include <utility>

TEST_CASE("MAP - constructors") {
  SECTION("Default constructor") {
    Map<std::string, std::string> map;
    Map<std::hash<std::string>, std::equal_to<std::string>,
        std::allocator<std::pair<const std::string, std::string>>>
        map_2;
    REQUIRE(map.size() == 0);
    REQUIRE(map.empty());
    REQUIRE(map.rows_size() == MIN_CAPACITY);
  }

  SECTION("Init list constructor") {
    Map<std::string, std::string> map{{"vlad", "value"}};
    REQUIRE(map.size() == 1);
    REQUIRE(map.empty() == false);
    REQUIRE(map.rows_size() == MIN_CAPACITY);
  }
  SECTION("Constructor with capacity") {
    Map<std::string, std::string> map(5);
    REQUIRE(map.size() == 0);
    REQUIRE(map.empty());
    REQUIRE(map.rows_size() == 5);
    REQUIRE_THROWS((Map<std::string, std::string>(0)));
  }
  SECTION("Copy constructor") {
    Map<std::string, std::string> input_map;
    std::vector<std::pair<const std::string, std::string>> input =
        {
            {"name", "value"},        {"email", "value"},
            {"phone", "value"},       {"address", "value"},
            {"city", "value"},        {"country", "value"},
            {"postal_code", "value"}, {"birth_date", "value"},
            {"gender", "value"},      {"username", "value"},
            {"password", "value"},    {"role", "value"},
            {"department", "value"},  {"position", "value"},
            {"salary", "value"},      {"hire_date", "value"},
            {"manager", "value"},     {"experience_years", "value"},
            {"education", "value"},   {"skills", "value"},
            {"languages", "value"},   {"status", "value"},
            {"active", "value"},      {"verified", "value"},
            {"created_at", "value"},  {"updated_at", "value"},
            {"last_login", "value"},  {"timezone", "value"},
            {"currency", "value"},    {"notes", "value"},
            {"profile_pic", "value"}, {"preferences", "value"},
        }

    ;
    for (const std::pair<const std::string, std::string> &el : input) {
      input_map.insert(el);
    }
    Map new_map(input_map);
    for (const std::pair<const std::string, std::string> &el : input) {
      REQUIRE(new_map.contains(el.first) == true);
    }
    for (const std::pair<const std::string, std::string> &el : input) {
      REQUIRE(input_map.contains(el.first) == true);
    }
    input_map.erase(input_map.begin(), input_map.end());
    for (const std::pair<const std::string, std::string> &el : input) {
      REQUIRE(input_map.contains(el.first) == false);
    }
    for (const std::pair<const std::string, std::string> &el : input) {
      REQUIRE(new_map.contains(el.first) == true);
    }
  }

  SECTION("Move constructor") {
    Map<std::string, std::string> input_map;
    std::vector<std::pair<const std::string, std::string>> input =
        {
            {"name", "value"},        {"email", "value"},
            {"phone", "value"},       {"address", "value"},
            {"city", "value"},        {"country", "value"},
            {"postal_code", "value"}, {"birth_date", "value"},
            {"gender", "value"},      {"username", "value"},
            {"password", "value"},    {"role", "value"},
            {"department", "value"},  {"position", "value"},
            {"salary", "value"},      {"hire_date", "value"},
            {"manager", "value"},     {"experience_years", "value"},
            {"education", "value"},   {"skills", "value"},
            {"languages", "value"},   {"status", "value"},
            {"active", "value"},      {"verified", "value"},
            {"created_at", "value"},  {"updated_at", "value"},
            {"last_login", "value"},  {"timezone", "value"},
            {"currency", "value"},    {"notes", "value"},
            {"profile_pic", "value"}, {"preferences", "value"},
        }

    ;
    for (const std::pair<const std::string, std::string> &el : input) {
      input_map.insert(el);
    }
    Map new_map(std::move(input_map));
    for (const std::pair<const std::string, std::string> &el : input) {
      REQUIRE(new_map.contains(el.first) == true);
    }
    for (const std::pair<const std::string, std::string> &el : input) {
      REQUIRE(input_map.contains(el.first) == false);
    }
    input_map.insert(new_map.begin(), new_map.end());
    for (const std::pair<const std::string, std::string> &el : input) {
      REQUIRE(input_map.contains(el.first) == true);
    }
    for (const std::pair<const std::string, std::string> &el : input) {
      REQUIRE(new_map.contains(el.first) == true);
    }
  }
  SECTION("Range constructor") {
    Map<std::string, std::string> input_map;
    std::vector<std::pair<const std::string, std::string>> input =
        {
            {"name", "value"},        {"email", "value"},
            {"phone", "value"},       {"address", "value"},
            {"city", "value"},        {"country", "value"},
            {"postal_code", "value"}, {"birth_date", "value"},
            {"gender", "value"},      {"username", "value"},
            {"password", "value"},    {"role", "value"},
            {"department", "value"},  {"position", "value"},
            {"salary", "value"},      {"hire_date", "value"},
            {"manager", "value"},     {"experience_years", "value"},
            {"education", "value"},   {"skills", "value"},
            {"languages", "value"},   {"status", "value"},
            {"active", "value"},      {"verified", "value"},
            {"created_at", "value"},  {"updated_at", "value"},
            {"last_login", "value"},  {"timezone", "value"},
            {"currency", "value"},    {"notes", "value"},
            {"profile_pic", "value"}, {"preferences", "value"},
        }

    ;
    for (const std::pair<const std::string, std::string> &el : input) {
      input_map.insert(el);
    }
    auto start_it = input_map.begin();
    auto last_it = input_map.begin();
    for (int i = 0; i < 10; i++) {
      last_it++;
    }

    Map<std::string, std::string> new_map(start_it, last_it);
    int counter = 0;
    for (const std::pair<const std::string, std::string> &el : input) {
      if (new_map.contains(el.first)) {
        counter++;
      }
    }
    REQUIRE(counter == 10);

    for (const std::pair<const std::string, std::string> &el : input) {
      REQUIRE(input_map.contains(el.first) == true);
    }
  }
}
TEST_CASE("MAP operators") {
  SECTION("=") {
    Map<std::string, std::string> map_1;

    std::vector<std::pair<const std::string, std::string>> input =
        {
            {"name", "value"},        {"email", "value"},
            {"phone", "value"},       {"address", "value"},
            {"city", "value"},        {"country", "value"},
            {"postal_code", "value"}, {"birth_date", "value"},
            {"gender", "value"},      {"username", "value"},
            {"password", "value"},    {"role", "value"},
            {"department", "value"},  {"position", "value"},
            {"salary", "value"},      {"hire_date", "value"},
            {"manager", "value"},     {"experience_years", "value"},
            {"education", "value"},   {"skills", "value"},
            {"languages", "value"},   {"status", "value"},
            {"active", "value"},      {"verified", "value"},
            {"created_at", "value"},  {"updated_at", "value"},
            {"last_login", "value"},  {"timezone", "value"},
            {"currency", "value"},    {"notes", "value"},
            {"profile_pic", "value"}, {"preferences", "value"},
        }

    ;
    for (const std::pair<const std::string, std::string> &el : input) {
      map_1.insert(el);
    }

    auto start_it = map_1.begin();
    auto last_it = map_1.begin();
    for (int i = 0; i < 10; i++) {
      last_it++;
    }

    Map<std::string, std::string> map_2(start_it, last_it);
    REQUIRE(map_2.size() == 10);
    map_2 = map_1;
    REQUIRE(map_2.size() == 32);
    map_2 = std::move(map_1);
    REQUIRE(map_2.size() == 32);
    REQUIRE(map_1.size() == 0);
    map_1 = {{"vlad_1", "value"}, {"vlad_2", "value"}};
    REQUIRE(map_1.size() == 2);
  }
  SECTION("[], at") {
    Map<std::string, std::string> map;
    std::vector<std::pair<const std::string, std::string>> input =
        {
            {"name", "value"},         {"email", "value"},
            {"phone", "value"},        {"address", "value"},
            {"city", "value"},         {"country", "value"},
            {"postal_code", "value"},  {"birth_date", "value"},
            {"gender", "value"},       {"username", "value"},
            {"password", "value"},     {"role", "value"},
            {"department", "value_1"}, {"position", "value"},
            {"salary", "value"},       {"hire_date", "value"},
            {"manager", "value"},      {"experience_years", "value"},
            {"education", "value"},    {"skills", "value"},
            {"languages", "value"},    {"status", "value"},
            {"active", "value"},       {"verified", "value"},
            {"created_at", "value"},   {"updated_at", "value"},
            {"last_login", "value"},   {"timezone", "value"},
            {"currency", "value"},     {"notes", "value"},
            {"profile_pic", "value"},  {"preferences", "value"},
        }

    ;
    for (const std::pair<const std::string, std::string> &el : input) {
      map.insert(el);
    }
    REQUIRE(input[12].second == map[input[12].first]);
    REQUIRE(input[13].second != map[input[12].first]);
    REQUIRE("" == map["vlad"]);
    std::string vlad = "vlad";
    map.erase("vlad");
    REQUIRE(input[13].second != map[std::move(input[12].first)]);
    REQUIRE("" == map[std::move(vlad)]);
    REQUIRE_THROWS(map.at("sigma"));
    REQUIRE(map.at("active") == "value");
  }
}
TEST_CASE("MAP - insert/erase") {

  SECTION("Base insert") {
    Map<std::string, std::string> map;
    const std::string key = "key";
    std::string value = "value";
    std::pair<const std::string, std::string> pair = std::make_pair(key, value);
    map.insert(pair);
    REQUIRE(map.size() == 1);
    REQUIRE(!map.empty());
    REQUIRE(map.rows_size() == MIN_CAPACITY);
    std::string search_key = "key";
    REQUIRE(map.contains(search_key) == true);
  }
  SECTION("Base copy insert check") {
    Map<std::string, std::string> map;
    const std::string key = "key";
    std::string value = "value";
    std::pair<const std::string, std::string> pair = std::make_pair(key, value);
    map.insert(pair);
    REQUIRE(map.size() == 1);
    REQUIRE(!map.empty());
    REQUIRE(map.rows_size() == MIN_CAPACITY);
    std::string new_key = "key";
    std::string new_value = "value";
    REQUIRE(map.contains(new_key) == true);
    std::pair<const std::string, std::string> new_pair =
        std::make_pair(new_key, new_value);
    auto result = map.insert(new_pair);
    REQUIRE(result.second == false);
  }
  SECTION("Load copy insert check") {
    Map<std::string, std::string> map;
    std::vector<std::pair<const std::string, std::string>> input =
        {
            {"name", "value"},        {"email", "value"},
            {"phone", "value"},       {"address", "value"},
            {"city", "value"},        {"country", "value"},
            {"postal_code", "value"}, {"birth_date", "value"},
            {"gender", "value"},      {"username", "value"},
            {"password", "value"},    {"role", "value"},
            {"department", "value"},  {"position", "value"},
            {"salary", "value"},      {"hire_date", "value"},
            {"manager", "value"},     {"experience_years", "value"},
            {"education", "value"},   {"skills", "value"},
            {"languages", "value"},   {"status", "value"},
            {"active", "value"},      {"verified", "value"},
            {"created_at", "value"},  {"updated_at", "value"},
            {"last_login", "value"},  {"timezone", "value"},
            {"currency", "value"},    {"notes", "value"},
            {"profile_pic", "value"}, {"preferences", "value"},
        }

    ;
    for (const std::pair<const std::string, std::string> &el : input) {
      map.insert(el);
    }
    for (const std::pair<const std::string, std::string> &el : input) {
      REQUIRE(map.contains(el.first) == true);
    }
  }

  SECTION("Load copy emplace check") {
    Map<std::string, std::string> map;
    std::vector<std::pair<const std::string, std::string>> input =
        {
            {"name", "value"},        {"email", "value"},
            {"phone", "value"},       {"address", "value"},
            {"city", "value"},        {"country", "value"},
            {"postal_code", "value"}, {"birth_date", "value"},
            {"gender", "value"},      {"username", "value"},
            {"password", "value"},    {"role", "value"},
            {"department", "value"},  {"position", "value"},
            {"salary", "value"},      {"hire_date", "value"},
            {"manager", "value"},     {"experience_years", "value"},
            {"education", "value"},   {"skills", "value"},
            {"languages", "value"},   {"status", "value"},
            {"active", "value"},      {"verified", "value"},
            {"created_at", "value"},  {"updated_at", "value"},
            {"last_login", "value"},  {"timezone", "value"},
            {"currency", "value"},    {"notes", "value"},
            {"profile_pic", "value"}, {"preferences", "value"},
        }

    ;
    std::vector<std::string> names;
    std::vector<std::string> values;

    for (const std::pair<const std::string, std::string> &el : input) {
      map.emplace(el.first, el.second);
    }
    for (const std::pair<const std::string, std::string> &el : input) {
      REQUIRE(map.contains(el.first) == true);
    }
  }

  SECTION("Load move insert check") {
    Map<std::string, std::string> map;
    std::vector<std::pair<const std::string, std::string>> input =
        {
            {"name", "value"},        {"email", "value"},
            {"phone", "value"},       {"address", "value"},
            {"city", "value"},        {"country", "value"},
            {"postal_code", "value"}, {"birth_date", "value"},
            {"gender", "value"},      {"username", "value"},
            {"password", "value"},    {"role", "value"},
            {"department", "value"},  {"position", "value"},
            {"salary", "value"},      {"hire_date", "value"},
            {"manager", "value"},     {"experience_years", "value"},
            {"education", "value"},   {"skills", "value"},
            {"languages", "value"},   {"status", "value"},
            {"active", "value"},      {"verified", "value"},
            {"created_at", "value"},  {"updated_at", "value"},
            {"last_login", "value"},  {"timezone", "value"},
            {"currency", "value"},    {"notes", "value"},
            {"profile_pic", "value"}, {"preferences", "value"},
        }

    ;
    for (std::pair<const std::string, std::string> el : input) {
      map.insert(std::move(el));
    }
    for (std::pair<const std::string, std::string> &el : input) {
      REQUIRE(map.contains(el.first) == true);
    }
  }
  SECTION("Load range insert check") {
    Map<std::string, std::string> input_map;
    std::vector<std::pair<const std::string, std::string>> input =
        {
            {"name", "value"},        {"email", "value"},
            {"phone", "value"},       {"address", "value"},
            {"city", "value"},        {"country", "value"},
            {"postal_code", "value"}, {"birth_date", "value"},
            {"gender", "value"},      {"username", "value"},
            {"password", "value"},    {"role", "value"},
            {"department", "value"},  {"position", "value"},
            {"salary", "value"},      {"hire_date", "value"},
            {"manager", "value"},     {"experience_years", "value"},
            {"education", "value"},   {"skills", "value"},
            {"languages", "value"},   {"status", "value"},
            {"active", "value"},      {"verified", "value"},
            {"created_at", "value"},  {"updated_at", "value"},
            {"last_login", "value"},  {"timezone", "value"},
            {"currency", "value"},    {"notes", "value"},
            {"profile_pic", "value"}, {"preferences", "value"},
        }

    ;
    for (const std::pair<const std::string, std::string> &el : input) {
      input_map.insert(el);
    }
    Map<std::string, std::string> map;
    map.insert(input[1]);
    map.insert(input[4]);
    map.insert(input[31]);
    map.insert(input_map.begin(), input_map.end());
    REQUIRE(map.size() == 32);
  }

  SECTION("Base erase check") {
    Map<std::string, std::string> map;
    const std::string key = "key";
    std::string value = "value";
    std::pair<const std::string, std::string> pair = std::make_pair(key, value);
    map.insert(pair);
    REQUIRE(map.size() == 1);
    REQUIRE(!map.empty());
    REQUIRE(map.rows_size() == MIN_CAPACITY);
    std::string new_key = "key";
    std::string new_value = "value";
    REQUIRE(map.contains(new_key) == true);
    REQUIRE(map.erase(new_key) == 1);
  }
  SECTION("Load copy erase check") {
    Map<std::string, std::string> map;
    std::vector<std::pair<const std::string, std::string>> input =
        {
            {"name", "value"},        {"email", "value"},
            {"phone", "value"},       {"address", "value"},
            {"city", "value"},        {"country", "value"},
            {"postal_code", "value"}, {"birth_date", "value"},
            {"gender", "value"},      {"username", "value"},
            {"password", "value"},    {"role", "value"},
            {"department", "value"},  {"position", "value"},
            {"salary", "value"},      {"hire_date", "value"},
            {"manager", "value"},     {"experience_years", "value"},
            {"education", "value"},   {"skills", "value"},
            {"languages", "value"},   {"status", "value"},
            {"active", "value"},      {"verified", "value"},
            {"created_at", "value"},  {"updated_at", "value"},
            {"last_login", "value"},  {"timezone", "value"},
            {"currency", "value"},    {"notes", "value"},
            {"profile_pic", "value"}, {"preferences", "value"},
        }

    ;

    std::vector<std::pair<const std::string, std::string>> fake_keys = {
        {"fake_1", "value"}, {"fake_2", "value"}, {"fake_3", "value"},
        {"fake_4", "value"}, {"fake_5", "value"}, {"fake_6", "value"},
        {"fake_7", "value"}, {"fale_8", "value"}, {"fake_9", "value"}};
    for (const std::pair<const std::string, std::string> &el : input) {
      map.insert(el);
    }
    for (const std::pair<const std::string, std::string> &el : fake_keys) {
      REQUIRE(map.erase(el.first) == 0);
    }
    for (const std::pair<const std::string, std::string> &el : input) {
      REQUIRE(map.erase(el.first) == 1);
    }
    for (const std::pair<const std::string, std::string> &el : input) {
      REQUIRE(map.erase(el.first) == 0);
    }
  }
  SECTION("Load erase const iterator check") {
    Map<std::string, std::string> map;
    std::vector<std::pair<const std::string, std::string>> input =
        {
            {"name", "value"},        {"email", "value"},
            {"phone", "value"},       {"address", "value"},
            {"city", "value"},        {"country", "value"},
            {"postal_code", "value"}, {"birth_date", "value"},
            {"gender", "value"},      {"username", "value"},
            {"password", "value"},    {"role", "value"},
            {"department", "value"},  {"position", "value"},
            {"salary", "value"},      {"hire_date", "value"},
            {"manager", "value"},     {"experience_years", "value"},
            {"education", "value"},   {"skills", "value"},
            {"languages", "value"},   {"status", "value"},
            {"active", "value"},      {"verified", "value"},
            {"created_at", "value"},  {"updated_at", "value"},
            {"last_login", "value"},  {"timezone", "value"},
            {"currency", "value"},    {"notes", "value"},
            {"profile_pic", "value"}, {"preferences", "value"},
        }

    ;

    for (const std::pair<const std::string, std::string> &el : input) {
      map.insert(el);
    }
    auto it = map.cbegin();
    auto prev_it = map.cbegin();
    while (it != map.cend()) {
      std::string key = (*it).first;
      it++;
      map.erase(prev_it);
      REQUIRE(map.contains(key) == false);
      prev_it = it;
    }
  }

  SECTION("Load erase range const iterator check") {
    Map<std::string, std::string> map;
    std::vector<std::pair<const std::string, std::string>> input =
        {
            {"name", "value"},        {"email", "value"},
            {"phone", "value"},       {"address", "value"},
            {"city", "value"},        {"country", "value"},
            {"postal_code", "value"}, {"birth_date", "value"},
            {"gender", "value"},      {"username", "value"},
            {"password", "value"},    {"role", "value"},
            {"department", "value"},  {"position", "value"},
            {"salary", "value"},      {"hire_date", "value"},
            {"manager", "value"},     {"experience_years", "value"},
            {"education", "value"},   {"skills", "value"},
            {"languages", "value"},   {"status", "value"},
            {"active", "value"},      {"verified", "value"},
            {"created_at", "value"},  {"updated_at", "value"},
            {"last_login", "value"},  {"timezone", "value"},
            {"currency", "value"},    {"notes", "value"},
            {"profile_pic", "value"}, {"preferences", "value"},
        }

    ;

    for (const std::pair<const std::string, std::string> &el : input) {
      map.insert(el);
    }

    map.erase(map.begin(), map.end());
    REQUIRE(map.size() == 0);
  }
  SECTION("Find  iterator check") {
    Map<std::string, std::string> map;
    std::vector<std::pair<const std::string, std::string>> input =
        {
            {"name", "value"},        {"email", "value"},
            {"phone", "value"},       {"address", "value"},
            {"city", "value"},        {"country", "value"},
            {"postal_code", "value"}, {"birth_date", "value"},
            {"gender", "value"},      {"username", "value"},
            {"password", "value"},    {"role", "value"},
            {"department", "value"},  {"position", "value"},
            {"salary", "value"},      {"hire_date", "value"},
            {"manager", "value"},     {"experience_years", "value"},
            {"education", "value"},   {"skills", "value"},
            {"languages", "value"},   {"status", "value"},
            {"active", "value"},      {"verified", "value"},
            {"created_at", "value"},  {"updated_at", "value"},
            {"last_login", "value"},  {"timezone", "value"},
            {"currency", "value"},    {"notes", "value"},
            {"profile_pic", "value"}, {"preferences", "value"},
        }

    ;

    for (const std::pair<const std::string, std::string> &el : input) {
      map.insert(el);
    }

    for (const std::pair<const std::string, std::string> &el : input) {
      auto it = map.find(el.first);
      REQUIRE(it != map.end());
      REQUIRE((*it).first == el.first);
    }
  }
}

TEST_CASE("Iterators") {
  SECTION("FOR EACH WiTH A LOT OF ELEMENTS") {
    Map<std::string, std::string> map;
    std::vector<std::pair<const std::string, std::string>> input = {
        {"key_1", "value"},  {"key_2", "value"},  {"key_3", "value"},
        {"key_4", "value"},  {"key_5", "value"},  {"key_6", "value"},
        {"key_7", "value"},  {"key_8", "value"},  {"key_9", "value"},
        {"key_10", "value"}, {"key_11", "value"}, {"key_12", "value"},
        {"key_13", "value"}, {"key_14", "value"}, {"key_15", "value"},
        {"key_16", "value"}, {"key_17", "value"}, {"key_18", "value"},
        {"key_19", "value"}, {"key_20", "value"}, {"key_21", "value"},
        {"key_22", "value"}, {"key_23", "value"}, {"key_24", "value"},
        {"key_25", "value"}, {"key_26", "value"}, {"key_27", "value"},
        {"key_28", "value"}, {"key_29", "value"}, {"key_30", "value"},
        {"key_31", "value"}, {"key_32", "value"},
    };
    for (const std::pair<const std::string, std::string> &el : input) {
      map.insert(el);
    }
    int counter = 0;
    for (const std::pair<const std::string, std::string> &el : map) {
      counter++;
    }
    REQUIRE(counter == 32);
  }
  SECTION("FOR EACH WiTH FEW ELEMENTS") {
    Map<std::string, std::string> map;
    std::vector<std::pair<const std::string, std::string>> input = {
        {"key_6", "value"},  {"key_7", "value"},  {"key_12", "value"},
        {"key_30", "value"}, {"key_31", "value"}, {"key_32", "value"},
    };
    for (const std::pair<const std::string, std::string> &el : input) {
      map.insert(el);
    }
    int counter = 0;
    for (const std::pair<const std::string, std::string> &el : map) {
      counter++;
    }
    REQUIRE(counter == 6);
  }
  SECTION("FOR EACH WiTH ONE ELEMENT") {
    Map<std::string, std::string> map;
    std::vector<std::pair<const std::string, std::string>> input = {
        {"key_6", "value"}};
    for (const std::pair<const std::string, std::string> &el : input) {
      map.insert(el);
    }
    int counter = 0;
    for (const std::pair<const std::string, std::string> &el : map) {
      counter++;
    }
    REQUIRE(counter == 1);
  }
  SECTION("FOR EACH WiTH ZERO ELEMENTS") {
    Map<std::string, std::string> map;

    int counter = 0;
    for (const std::pair<const std::string, std::string> &el : map) {
      counter++;
    }
    REQUIRE(counter == 0);
  }
}
