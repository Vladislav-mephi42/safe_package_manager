#ifndef MAP_H
#define MAP_H
#include <algorithm>
#include <cassert>
#include <functional>
#include <iterator>
#include <memory>

#include <span>
#include <type_traits>
#include <utility>

constexpr std::size_t MIN_CAPACITY = 10;
constexpr double CAPACITY_MULTIPLIER = 2;
constexpr double LOAD_FACTOR = 3;

template <typename K, typename V, typename Hash = std::hash<K>,
          typename KeyEqual = std::equal_to<K>,
          typename Alloc = std::allocator<std::pair<const K, V>>>
class Map;

template <typename K, typename V, typename KeyEqual = std::equal_to<K>,
          typename Alloc = std::allocator<std::pair<const K, V>>,
          bool is_const = false>
class MapIterator;

template <typename K, typename V> struct MapNode {
  std::pair<const K, V> value;
  MapNode *next = nullptr;

  template <typename... Args>
  explicit MapNode(Args &&...args)
      : value(std::forward<Args>(args)...), next(nullptr) {}

  ~MapNode() = default;
};

template <typename K, typename V, typename KeyEqual, typename Alloc>
struct ChainHead {
  typedef const K key_type;
  typedef V mapped_type;
  typedef Alloc allocator_type;
  typedef KeyEqual key_equal;
  typedef std::pair<const K, V> value_type;
  typedef std::pair<const K, V> &reference;
  typedef const std::pair<const K, V> &const_reference;
  typedef size_t size_type;
  typedef MapNode<K, V> node_t;
  typedef typename std::allocator_traits<Alloc>::template rebind_alloc<node_t>
      node_allocator_type;
  MapNode<K, V> *head = nullptr;
  MapNode<K, V> *tail = nullptr;
  size_t size = 0;

  ChainHead() = default;
  explicit ChainHead(MapNode<K, V> *head) : size(1), head(head), tail(head) {}
  ~ChainHead() = default;

  bool chain_contains(const K &key, key_equal key_comparator) const noexcept;
  void chain_insert(node_t *node);
  size_type chain_erase(const K &key, key_equal key_comparator,
                        node_allocator_type node_allocator);
  void deallocate(node_t *node, node_allocator_type node_allocator) noexcept {
    std::allocator_traits<node_allocator_type>::destroy(node_allocator, node);
    std::allocator_traits<node_allocator_type>::deallocate(node_allocator, node,
                                                           1);
  }
};

template <typename K, typename V, typename KeyEqual, typename Alloc,
          bool is_const>
class MapIterator {
public:
  typedef ptrdiff_t difference_type;
  typedef std::pair<const K, V> value_type;
  typedef std::conditional_t<is_const, const std::pair<const K, V>,
                             std::pair<const K, V>> *pointer;
  typedef std::conditional_t<is_const, const std::pair<const K, V>,
                             std::pair<const K, V>> &reference;
  typedef std::forward_iterator_tag iterator_category;

  MapIterator() noexcept;

  template <bool other_const>
  MapIterator(const MapIterator<K, V, KeyEqual, Alloc, other_const> &o) noexcept
    requires(is_const >= other_const);

  template <bool other_const>
  MapIterator &
  operator=(const MapIterator<K, V, KeyEqual, Alloc, other_const> &o) noexcept
    requires(is_const >= other_const);

  reference operator*() const noexcept;

  pointer operator->() const noexcept;

  template <bool other_const>
  bool operator==(
      const MapIterator<K, V, KeyEqual, Alloc, other_const> &o) const noexcept;

  MapIterator &operator++() noexcept;

  MapIterator operator++(int) noexcept;

private:
  typedef std::conditional_t<is_const, const MapNode<K, V>, MapNode<K, V>>
      *node_ptr_t;
  typedef std::conditional_t<is_const, const ChainHead<K, V, KeyEqual, Alloc>,
                             ChainHead<K, V, KeyEqual, Alloc>> *chain_ptr_t;

  chain_ptr_t chain_ptr = nullptr;
  node_ptr_t node_ptr = nullptr;

  chain_ptr_t end;

  MapIterator(chain_ptr_t chain_node, node_ptr_t node, chain_ptr_t end);

  explicit MapIterator(chain_ptr_t chain_node)
      : chain_ptr(chain_node), end(chain_node){

                               };

  template <typename K1, typename V1, typename Hash1, typename KeyEqual1,
            typename Alloc1>
  friend class Map;

  friend MapIterator<K, V, KeyEqual, Alloc, !is_const>;
};

template <typename K, typename V, typename Hash, typename KeyEqual,
          typename Alloc>
class Map {
public:
  typedef const K key_type;
  typedef V mapped_type;
  typedef Hash hasher;
  typedef KeyEqual key_equal;
  typedef Alloc allocator_type;
  typedef std::pair<const K, V> value_type;
  typedef std::pair<const K, V> &reference;
  typedef const std::pair<const K, V> &const_reference;
  typedef MapIterator<K, V, KeyEqual, Alloc, false> iterator;
  typedef MapIterator<K, V, KeyEqual, Alloc, true> const_iterator;
  typedef ptrdiff_t difference_type;
  typedef size_t size_type;
  typedef MapNode<K, V> node_t;
  typedef ChainHead<K, V, KeyEqual, Alloc> chain_t;
  typedef typename std::allocator_traits<Alloc>::template rebind_alloc<node_t>
      node_allocator_type;

  Map(const hasher &hash_function = hasher(),
      const key_equal &key_compare = key_equal(),
      const allocator_type &alloc =
          allocator_type()) noexcept(std::
                                         is_nothrow_default_constructible_v<
                                             Alloc>);

  Map(const Map &o)
    requires std::copy_constructible<K> && std::copy_constructible<V>;

  Map(const Map &o, Hash hasher, KeyEqual key_equal,
      const allocator_type &alloc)
    requires std::copy_constructible<K> && std::copy_constructible<V>;

  Map(Map &&o) noexcept;

  Map(Map &&o, Hash hasher, KeyEqual key_equal, const allocator_type &alloc);

  Map(size_type capacity, const hasher &hash_function = hasher(),
      const key_equal &key_compare = key_equal(),
      const allocator_type &alloc = allocator_type());

  template <std::input_iterator It>
  Map(It i, It j, const hasher &hash_function = hasher(),
      const key_equal &key_compare = key_equal(),
      const allocator_type &alloc = allocator_type())
    requires std::constructible_from<std::pair<const K, V>,
                                     std::iter_reference_t<It>>;

  Map(std::initializer_list<std::pair<const K, V>> il,
      const hasher &hash_function = hasher(),
      const key_equal &key_compare = key_equal(),
      const allocator_type &alloc = allocator_type())
    requires std::copy_constructible<K> && std::copy_constructible<V>;

  Map &operator=(const Map &o)
    requires std::copy_constructible<K> && std::copy_constructible<V>;

  Map &operator=(Map &&o) noexcept;

  ~Map();

  iterator begin() noexcept;

  iterator end() noexcept { return iterator(data + capacity); }

  const_iterator begin() const noexcept;

  const_iterator end() const noexcept {
    return const_iterator(data + capacity);
  }

  const_iterator cbegin() const noexcept;

  const_iterator cend() const noexcept {
    return const_iterator(data + capacity);
  }

  const_iterator chain_begin(chain_t *chain) const noexcept {
    return const_iterator(chain, chain->head, data + capacity);
  }

  const_iterator chain_end(chain_t *chain) const noexcept {
    return (++const_iterator(chain, chain->tail, data + capacity));
  };

  iterator chain_begin(chain_t *chain) noexcept {
    return iterator(chain, chain->head, data + capacity);
  }

  iterator chain_end(chain_t *chain) noexcept {
    return (++iterator(chain, chain->tail, data + capacity));
  };

  bool operator==(const Map &o) const;
  size_type size() const noexcept { return map_size; }
  size_type rows_size() const { return capacity; }
  bool empty() const noexcept { return (this->map_size == 0); }

  Map &operator=(std::initializer_list<std::pair<const K, V>> il)
    requires std::copy_constructible<K> && std::copy_constructible<V>;

  template <typename... Args>
  std::pair<iterator, bool> emplace(Args &&...args)
    requires std::constructible_from<std::pair<const K, V>, Args...>;

  std::pair<iterator, bool> insert(const std::pair<const K, V> &value)
    requires std::copy_constructible<K> && std::copy_constructible<V>;

  std::pair<iterator, bool> insert(std::pair<const K, V> &&value)
    requires std::move_constructible<K> && std::move_constructible<V>;

  void insert(std::initializer_list<std::pair<const K, V>> ilist); // make!!!

  template <std::input_iterator It>
  void insert(It start, It last)
    requires std::constructible_from<std::pair<const K, V>,
                                     std::iter_reference_t<It>>
  {
    for_each(start, last, [&](auto value) { this->insert(value); });
  }

  void clear() noexcept { erase(begin(), end()); };
  allocator_type get_allocator() const noexcept { return allocator; }
  node_allocator_type get_node_allocator() const noexcept {
    return node_allocator;
  }

  V &operator[](const K &key) {
    if (contains(key)) {
      iterator res = find(key);
      return res->second;
    }
    V value;
    auto res = emplace(std::move(key), std::move(value));
    return (res.first)->second;
  }

  V &operator[](K &&key) {
    if (contains(key)) {
      iterator res = find(key);
      return res->second;
    }
    V value;
    auto res = emplace(key, value);
    return (res.first)->second;
  }

  V &at(const K &key) {
    if (contains(key)) {
      iterator res = find(key);
      return res->second;
    }
    throw std::out_of_range("cannt find key");
  }

  const V &at(const K &key) const {
    if (contains(key)) {
      const_iterator res = find(key);
      return res->second;
    }
    throw std::out_of_range("cannt find key");
  }

  const_iterator erase(const_iterator pos);

  const_iterator erase(const_iterator first, const_iterator last);

  size_type erase(const K &key);

  iterator find(const K &key) noexcept;

  const_iterator find(const K &key) const noexcept;

  size_type count(const K &key) const noexcept {
    bool result = contains(key);
    return static_cast<size_type>(result);
  };

  bool contains(const K &key) const noexcept;

  size_type max_chain_count() const { return capacity; }

  size_type chain_size(size_type n) const { return data[n].size; }

private:
  typedef typename std::allocator_traits<Alloc>::template rebind_alloc<chain_t>
      data_allocator_type;

  hasher hash_function;
  key_equal key_comparator;
  allocator_type allocator;
  node_allocator_type node_allocator;
  data_allocator_type data_allocator;
  size_t capacity = 0;
  size_t map_size = 0;
  chain_t *data = nullptr;

  size_t get_load_factor() { return map_size / capacity; }

  template <typename... Args> node_t *allocate(Args &&...args);

  template <typename... Args> chain_t *data_allocate(size_t n);

  void deallocate(node_t *node) noexcept {
    std::allocator_traits<node_allocator_type>::destroy(node_allocator, node);
    std::allocator_traits<node_allocator_type>::deallocate(node_allocator, node,
                                                           1);
  }

  void chain_thread_deallocate(chain_t *chain);

  void data_deallocate(chain_t *data, size_t n) noexcept;

  void swap_state(Map &o) noexcept;

  const_iterator chain_find_it(const K &key, chain_t *chain) const noexcept;
  iterator chain_find_it(const K &key, chain_t *chain) noexcept;

  void rehash();
};

static_assert(std::forward_iterator<MapIterator<
                  std::string, std::string, std::equal_to<std::string>,
                  std::allocator<std::pair<std::string, std::string>>, false>>);
static_assert(std::forward_iterator<MapIterator<
                  std::string, std::string, std::equal_to<std::string>,
                  std::allocator<std::pair<std::string, std::string>>, true>>);

template <typename K, typename V, typename KeyEqual, typename Alloc,
          bool is_const>
MapIterator<K, V, KeyEqual, Alloc, is_const>::MapIterator() noexcept
    : chain_ptr(nullptr), node_ptr(nullptr) {}

template <typename K, typename V, typename KeyEqual, typename Alloc,
          bool is_const>
MapIterator<K, V, KeyEqual, Alloc, is_const>::MapIterator(
    chain_ptr_t chain_node, node_ptr_t node, chain_ptr_t end)
    : chain_ptr(chain_node), node_ptr(node), end(end) {}

template <typename K, typename V, typename KeyEqual, typename Alloc,
          bool is_const>
template <bool other_const>
MapIterator<K, V, KeyEqual, Alloc, is_const>::MapIterator(
    const MapIterator<K, V, KeyEqual, Alloc, other_const> &o) noexcept
  requires(is_const >= other_const)
    : chain_ptr(o.chain_ptr), node_ptr(o.node_ptr), end(o.end) {}

template <typename K, typename V, typename KeyEqual, typename Alloc,
          bool is_const>
template <bool other_const>
MapIterator<K, V, KeyEqual, Alloc, is_const> &
MapIterator<K, V, KeyEqual, Alloc, is_const>::operator=(
    const MapIterator<K, V, KeyEqual, Alloc, other_const> &o) noexcept
  requires(is_const >= other_const)
{
  chain_ptr = o.chain_ptr;
  node_ptr = o.node_ptr;
  end = o.end;
  return *this;
}

template <typename K, typename V, typename KeyEqual, typename Alloc,
          bool is_const>
typename MapIterator<K, V, KeyEqual, Alloc, is_const>::reference
MapIterator<K, V, KeyEqual, Alloc, is_const>::operator*() const noexcept {
  return node_ptr->value;
}

template <typename K, typename V, typename KeyEqual, typename Alloc,
          bool is_const>
typename MapIterator<K, V, KeyEqual, Alloc, is_const>::pointer
MapIterator<K, V, KeyEqual, Alloc, is_const>::operator->() const noexcept {
  return &node_ptr->value;
}

template <typename K, typename V, typename KeyEqual, typename Alloc,
          bool is_const>
template <bool other_const>
bool MapIterator<K, V, KeyEqual, Alloc, is_const>::operator==(
    const MapIterator<K, V, KeyEqual, Alloc, other_const> &o) const noexcept {
  return (node_ptr == o.node_ptr && chain_ptr == o.chain_ptr);
}

template <typename K, typename V, typename KeyEqual, typename Alloc,
          bool is_const>
MapIterator<K, V, KeyEqual, Alloc, is_const> &
MapIterator<K, V, KeyEqual, Alloc, is_const>::operator++() noexcept {

  if (node_ptr != nullptr && node_ptr->next != nullptr) {
    node_ptr = node_ptr->next;
  } else {
    chain_ptr++;
    while (chain_ptr->size == 0 && chain_ptr != end) {
      chain_ptr++;
    }
    node_ptr = chain_ptr->head;
  }
  return *this;
}

template <typename K, typename V, typename KeyEqual, typename Alloc,
          bool is_const>
MapIterator<K, V, KeyEqual, Alloc, is_const>
MapIterator<K, V, KeyEqual, Alloc, is_const>::operator++(int) noexcept {
  MapIterator res(*this);
  if (node_ptr != nullptr && node_ptr->next != nullptr) {
    node_ptr = node_ptr->next;
  } else {
    chain_ptr++;
    while (chain_ptr->size == 0 && chain_ptr != end) {
      chain_ptr++;
    }
    node_ptr = chain_ptr->head;
  }
  return res;
}

template <typename K, typename V, typename KeyEqual, typename Alloc>
bool ChainHead<K, V, KeyEqual, Alloc>::chain_contains(
    const K &key, key_equal key_comparator) const noexcept {
  if (size == 0) {
    return false;
  }
  node_t *ptr = head;
  if (key_comparator(head->value.first, key) ||
      key_comparator(tail->value.first, key)) {
    return true;
  }
  while (ptr->next != nullptr) {
    if (key_comparator(ptr->value.first, key)) {
      return true;
    }
    ptr = ptr->next;
  }
  return false;
}

template <typename K, typename V, typename KeyEqual, typename Alloc>
ChainHead<K, V, KeyEqual, Alloc>::size_type
ChainHead<K, V, KeyEqual, Alloc>::chain_erase(
    const K &key, key_equal key_comparator,
    ChainHead<K, V, KeyEqual, Alloc>::node_allocator_type node_allocator) {

  if (chain_contains(key, key_comparator) == true) {
    node_t *ptr = head;
    node_t *prev = head;
    if (ptr->value.first == key) {
      if (size == 1) {
        head = nullptr;
        tail = nullptr;
      } else {
        head = ptr->next;
      }

      deallocate(ptr, node_allocator);
      size--;
      return 1;
    }
    while (ptr->next != nullptr) {
      prev = ptr;
      ptr = ptr->next;
      if (ptr->value.first == key) {
        prev->next = ptr->next;
        if (tail == ptr) {
          tail = prev;
        }
        deallocate(ptr, node_allocator);
        size--;
        return 1;
      }
    }
  }
  return 0;
}

template <typename K, typename V, typename KeyEqual, typename Alloc>
void ChainHead<K, V, KeyEqual, Alloc>::chain_insert(node_t *node) {
  node->next = head;
  head = node;
  size++;
  if (size == 1) {
    tail = node;
  }
}

template <typename K, typename V, typename Hash, typename KeyEqual,
          typename Alloc>
Map<K, V, Hash, KeyEqual, Alloc>::Map(
    const hasher &hash_function, const key_equal &key_comparator,
    const allocator_type
        &alloc) noexcept(std::is_nothrow_default_constructible_v<Alloc>)
    : hash_function(hash_function), key_comparator(key_comparator),
      allocator(alloc), node_allocator(this->allocator),
      data_allocator(this->allocator), capacity(MIN_CAPACITY), map_size(0) {
  data = data_allocate(MIN_CAPACITY + 1);
}

template <typename K, typename V, typename Hash, typename KeyEqual,
          typename Alloc>
Map<K, V, Hash, KeyEqual, Alloc>::Map(
    std::initializer_list<std::pair<const K, V>> il,
    const hasher &hash_function, const key_equal &key_comparator,
    const allocator_type &alloc)
  requires std::copy_constructible<K> && std::copy_constructible<V>
    : hash_function(hash_function), key_comparator(key_comparator),
      allocator(alloc), node_allocator(this->allocator),
      data_allocator(this->allocator), capacity(MIN_CAPACITY), map_size(0) {
  data = data_allocate(MIN_CAPACITY + 1);

  for (const std::pair<const K, V> &el : il) {
    insert(el);
  }
}

template <typename K, typename V, typename Hash, typename KeyEqual,
          typename Alloc>
Map<K, V, Hash, KeyEqual, Alloc>::Map(size_type capacity,
                                      const hasher &hash_function,
                                      const key_equal &key_comparator,
                                      const allocator_type &alloc)
    : hash_function(hash_function), key_comparator(key_comparator),
      allocator(alloc), node_allocator(this->allocator),
      data_allocator(this->allocator), capacity(capacity), map_size(0) {
  if (capacity < 1) {
    throw std::invalid_argument(" n < 0");
  }
  data = data_allocate(capacity + 1);
}

template <typename K, typename V, typename Hash, typename KeyEqual,
          typename Alloc>
Map<K, V, Hash, KeyEqual, Alloc>::Map(const Map &o)
  requires std::copy_constructible<K> && std::copy_constructible<V>
    : hash_function(o.hash_function), key_comparator(o.key_comparator),
      allocator(o.allocator), node_allocator(o.allocator),
      data_allocator(o.allocator), capacity(o.capacity), map_size(o.size()) {
  data = data_allocate(capacity + 1);

  insert(o.begin(), o.end());
}

template <typename K, typename V, typename Hash, typename KeyEqual,
          typename Alloc>
Map<K, V, Hash, KeyEqual, Alloc>::Map(Map &&o) noexcept

    : hash_function(o.hash_function), key_comparator(o.key_comparator),
      allocator(o.allocator), node_allocator(o.allocator),
      data_allocator(o.allocator), capacity(o.capacity), map_size(o.size()) {
  swap_state(o);
}

template <typename K, typename V, typename Hash, typename KeyEqual,
          typename Alloc>
Map<K, V, Hash, KeyEqual, Alloc>::Map(Map &&o, Hash hasher, KeyEqual key_equal,
                                      const allocator_type &alloc)

    : hash_function(hasher), key_comparator(key_equal), allocator(alloc),
      node_allocator(o.allocator), data_allocator(o.allocator),
      capacity(o.capacity), map_size(o.size()) {
  swap_state(o);
}

template <typename K, typename V, typename Hash, typename KeyEqual,
          typename Alloc>
Map<K, V, Hash, KeyEqual, Alloc>::Map(const Map &o, Hash hasher,
                                      KeyEqual key_equal,
                                      const allocator_type &alloc)
  requires std::copy_constructible<K> && std::copy_constructible<V>
    : hash_function(hasher), key_comparator(key_equal), allocator(alloc),
      node_allocator(allocator), data_allocator(allocator),
      capacity(o.capacity), map_size(o.size()) {
  data = data_allocate(capacity + 1);

  insert(o.begin(), o.end());
}

template <typename K, typename V, typename Hash, typename KeyEqual,
          typename Alloc>
template <std::input_iterator It>
Map<K, V, Hash, KeyEqual, Alloc>::Map(It i, It j, const hasher &hash_function,
                                      const key_equal &key_comparator,
                                      const allocator_type &alloc)
  requires std::constructible_from<std::pair<const K, V>,
                                   std::iter_reference_t<It>>
    : hash_function(hash_function), key_comparator(key_comparator),
      allocator(alloc), node_allocator(this->allocator),
      data_allocator(this->allocator), capacity(MIN_CAPACITY), map_size(0) {
  data = data_allocate(MIN_CAPACITY + 1);

  insert(i, j);
}

template <typename K, typename V, typename Hash, typename KeyEqual,
          typename Alloc>
Map<K, V, Hash, KeyEqual, Alloc> &
Map<K, V, Hash, KeyEqual, Alloc>::operator=(const Map &o)
  requires std::copy_constructible<K> && std::copy_constructible<V>
{
  if (this != &o) {

    std::span<chain_t> view{data, static_cast<size_t>(capacity)};
    for (auto &elem : view) {
      chain_thread_deallocate(&elem);
    }
    data_deallocate(data, capacity + 1);
    capacity = o.capacity;
    map_size = 0;
    allocator = o.allocator;
    data_allocator = o.data_allocator;
    node_allocator = o.node_allocator;
    key_comparator = o.key_comparator;
    hash_function = o.hash_function;
    data = data_allocate(capacity + 1);

    insert(o.begin(), o.end());
  }
  return *this;
}

template <typename K, typename V, typename Hash, typename KeyEqual,
          typename Alloc>
Map<K, V, Hash, KeyEqual, Alloc> &
Map<K, V, Hash, KeyEqual, Alloc>::operator=(Map &&o) noexcept {
  if (*this != o) {

    std::span<chain_t> view{data, static_cast<size_t>(capacity)};
    for (auto &elem : view) {
      chain_thread_deallocate(&elem);
    }
    data_deallocate(data, capacity + 1);
    data = o.data;
    capacity = o.capacity;
    map_size = o.map_size;
    allocator = o.allocator;
    data_allocator = o.data_allocator;
    node_allocator = o.node_allocator;
    key_comparator = o.key_comparator;
    hash_function = o.hash_function;
    o.data = o.data_allocate(2);

    o.capacity = 1;
    o.map_size = 0;
  }
  return *this;
};

template <typename K, typename V, typename Hash, typename KeyEqual,
          typename Alloc>
Map<K, V, Hash, KeyEqual, Alloc>::~Map() {
  std::span<chain_t> view{data, static_cast<size_t>(capacity)};
  for (chain_t &elem : view) {
    chain_thread_deallocate(&elem);
  }
  data_deallocate(data, capacity + 1);
}

template <typename K, typename V, typename Hash, typename KeyEqual,
          typename Alloc>
Map<K, V, Hash, KeyEqual, Alloc>::iterator
Map<K, V, Hash, KeyEqual, Alloc>::begin() noexcept {
  std::span<chain_t> view{data, static_cast<size_t>(capacity)};
  for (chain_t &elem : view) {
    if (elem.size != 0) {
      return iterator(&elem, elem.head, data + capacity);
    }
  }
  return iterator(data + capacity);
}

template <typename K, typename V, typename Hash, typename KeyEqual,
          typename Alloc>
Map<K, V, Hash, KeyEqual, Alloc>::const_iterator
Map<K, V, Hash, KeyEqual, Alloc>::begin() const noexcept {
  std::span<chain_t> view{data, static_cast<size_t>(capacity)};
  for (chain_t &elem : view) {
    if (elem.size != 0) {
      return const_iterator(&elem, elem.head, data + capacity);
    }
  }
  return const_iterator(data + capacity);
}

template <typename K, typename V, typename Hash, typename KeyEqual,
          typename Alloc>
Map<K, V, Hash, KeyEqual, Alloc>::const_iterator
Map<K, V, Hash, KeyEqual, Alloc>::cbegin() const noexcept {
  std::span<chain_t> view{data, static_cast<size_t>(capacity)};
  for (chain_t &elem : view) {
    if (elem.size != 0) {
      return const_iterator(&elem, elem.head, data + capacity);
    }
  }
  return const_iterator(data + capacity);
}

template <typename K, typename V, typename Hash, typename KeyEqual,
          typename Alloc>
bool Map<K, V, Hash, KeyEqual, Alloc>::operator==(const Map &o) const {
  if (map_size != o.size() || capacity != o.capacity) {
    return false;
  }
  auto start = cbegin();
  auto end = cend();
  auto o_start = o.cbegin();

  while (start != end) {
    if ((*start).first != (*o_start).first ||
        (*start).second != (*o_start).second) {
      return false;
    }
    start++;
    o_start++;
  }
  return true;
}

template <typename K, typename V, typename Hash, typename KeyEqual,
          typename Alloc>
Map<K, V, Hash, KeyEqual, Alloc> &Map<K, V, Hash, KeyEqual, Alloc>::operator=(
    std::initializer_list<std::pair<const K, V>> il)
  requires std::copy_constructible<K> && std::copy_constructible<V>
{
  erase(begin(), end());
  for (const std::pair<const K, V> &el : il) {
    insert(el);
  }
  return *this;
}

template <typename K, typename V, typename Hash, typename KeyEqual,
          typename Alloc>
std::pair<typename Map<K, V, Hash, KeyEqual, Alloc>::iterator, bool>
Map<K, V, Hash, KeyEqual, Alloc>::insert(const std::pair<const K, V> &value)
  requires std::copy_constructible<K> && std::copy_constructible<V>
{

  if (contains(value.first) == false) {
    size_type load_factor = get_load_factor();
    if (load_factor > LOAD_FACTOR) {
      rehash();
    }
    std::size_t index = (hash_function(value.first)) % (this->capacity);
    node_t *new_node = allocate(value.first, value.second);
    data[index].chain_insert(new_node);
    map_size++;
    return {iterator(data + index, (data + index)->head, data + capacity),
            true};
  }

  return {end(), false};
}

template <typename K, typename V, typename Hash, typename KeyEqual,
          typename Alloc>
template <typename... Args>
std::pair<typename Map<K, V, Hash, KeyEqual, Alloc>::iterator, bool>
Map<K, V, Hash, KeyEqual, Alloc>::emplace(Args &&...args)
  requires std::constructible_from<std::pair<const K, V>, Args...>
{
  std::pair<const K, V> tmp(std::forward<Args>(args)...);
  if (contains(tmp.first) == false) {
    size_type load_factor = get_load_factor();
    if (load_factor > LOAD_FACTOR) {
      rehash();
    }
    node_t *new_node = allocate(std::move(tmp));
    std::size_t index =
        (hash_function(new_node->value.first)) % (this->capacity);
    data[index].chain_insert(new_node);
    map_size++;
    return {iterator(data + index, (data + index)->head, data + capacity),
            true};
  }

  return {end(), false};
}
template <typename K, typename V, typename Hash, typename KeyEqual,
          typename Alloc>
std::pair<typename Map<K, V, Hash, KeyEqual, Alloc>::iterator, bool>
Map<K, V, Hash, KeyEqual, Alloc>::insert(std::pair<const K, V> &&value)
  requires std::move_constructible<K> && std::move_constructible<V>
{
  size_type load_factor = get_load_factor();
  if (load_factor > LOAD_FACTOR) {
    rehash();
  }
  if (contains(value.first) == false) {
    size_type load_factor = get_load_factor();
    if (load_factor > LOAD_FACTOR) {
      rehash();
    }
    std::size_t index = (hash_function(value.first)) % (this->capacity);
    node_t *new_node = allocate(std::move(value));
    data[index].chain_insert(new_node);
    map_size++;
    return {iterator(data + index, (data + index)->head, data + capacity),
            true};
  }

  return {end(), false};
}

template <typename K, typename V, typename Hash, typename KeyEqual,
          typename Alloc>
Map<K, V, Hash, KeyEqual, Alloc>::const_iterator
Map<K, V, Hash, KeyEqual, Alloc>::erase(const_iterator pos) {
  if (pos == cend()) {
    return end();
  }

  const_iterator result = pos;
  result++;
  chain_t *chain = const_cast<chain_t *>(pos.chain_ptr);
  size_type tmp =
      chain->chain_erase((*pos).first, key_comparator, get_node_allocator());
  map_size = map_size - tmp;
  return result;
}

template <typename K, typename V, typename Hash, typename KeyEqual,
          typename Alloc>
Map<K, V, Hash, KeyEqual, Alloc>::const_iterator
Map<K, V, Hash, KeyEqual, Alloc>::erase(const_iterator first,
                                        const_iterator last) {
  const_iterator it = first;
  const_iterator prev = first;
  while (it != last) {
    const_iterator it = erase(prev);
    prev = it;
    if (it == last) {
      break;
    }
  }
  if (last != end()) {
    last++;
    return last;
  }

  return end();
}

template <typename K, typename V, typename Hash, typename KeyEqual,
          typename Alloc>
bool Map<K, V, Hash, KeyEqual, Alloc>::contains(const K &key) const noexcept {
  std::size_t index = (hash_function(key)) % (this->capacity);
  if (data[index].size != 0) {
    return data[index].chain_contains(key, key_comparator);
  }
  return false;
}

template <typename K, typename V, typename Hash, typename KeyEqual,
          typename Alloc>
Map<K, V, Hash, KeyEqual, Alloc>::size_type
Map<K, V, Hash, KeyEqual, Alloc>::erase(const K &key) {

  std::size_t index = (hash_function(key)) % (this->capacity);
  size_type res =
      data[index].chain_erase(key, key_comparator, get_node_allocator());
  map_size = map_size - res;
  return res;
}

template <typename K, typename V, typename Hash, typename KeyEqual,
          typename Alloc>
Map<K, V, Hash, KeyEqual, Alloc>::const_iterator
Map<K, V, Hash, KeyEqual, Alloc>::chain_find_it(const K &key,
                                                chain_t *chain) const noexcept {

  const_iterator it =
      std::find_if(chain_begin(chain), chain_end(chain),
                   [&key](value_type value) { return value.first == key; });

  return it;
}

template <typename K, typename V, typename Hash, typename KeyEqual,
          typename Alloc>
Map<K, V, Hash, KeyEqual, Alloc>::const_iterator
Map<K, V, Hash, KeyEqual, Alloc>::find(const K &key) const noexcept {
  if (size() == 0) {
    return cend();
  }
  std::size_t index = (hash_function(key)) % (this->capacity);
  if (data[index].size != 0) {

    auto res = chain_find_it(key, &data[index]);
    if (res != chain_end(&data[index])) {
      return res;
    }
  }
  return cend();
}

template <typename K, typename V, typename Hash, typename KeyEqual,
          typename Alloc>
Map<K, V, Hash, KeyEqual, Alloc>::iterator
Map<K, V, Hash, KeyEqual, Alloc>::chain_find_it(const K &key,
                                                chain_t *chain) noexcept {

  iterator it = std::find_if(chain_begin(chain), chain_end(chain),
                             [&key, this](value_type value) {
                               return key_comparator(value.first, key);
                             });
  return it;
}

template <typename K, typename V, typename Hash, typename KeyEqual,
          typename Alloc>
Map<K, V, Hash, KeyEqual, Alloc>::iterator
Map<K, V, Hash, KeyEqual, Alloc>::find(const K &key) noexcept {
  if (size() == 0) {
    return end();
  }
  std::size_t index = (hash_function(key)) % (this->capacity);
  if (data[index].size != 0) {
    auto res = chain_find_it(key, &data[index]);
    if (res != chain_end(&data[index])) {
      return res;
    }
  }
  return end();
}

template <typename K, typename V, typename Hash, typename KeyEqual,
          typename Alloc>
template <typename... Args>
Map<K, V, Hash, KeyEqual, Alloc>::node_t *
Map<K, V, Hash, KeyEqual, Alloc>::allocate(Args &&...args) {
  node_t *node =
      std::allocator_traits<node_allocator_type>::allocate(node_allocator, 1);
  try {
    std::allocator_traits<node_allocator_type>::construct(
        node_allocator, node, std::forward<Args>(args)...);
    return node;
  } catch (...) {
    std::allocator_traits<node_allocator_type>::deallocate(node_allocator, node,
                                                           1);
    throw;
  }
}

template <typename K, typename V, typename Hash, typename KeyEqual,
          typename Alloc>
template <typename... Args>
Map<K, V, Hash, KeyEqual, Alloc>::chain_t *
Map<K, V, Hash, KeyEqual, Alloc>::data_allocate(size_t n) {
  if (n == 0) {
    return nullptr;
  }
  chain_t *data =
      std::allocator_traits<data_allocator_type>::allocate(data_allocator, n);
  for (auto *ptr = data, *end = data + n; ptr != end; ++ptr) {
    try {
      std::allocator_traits<data_allocator_type>::construct(data_allocator,
                                                            ptr);
    } catch (...) {
      data_deallocate(data, ptr - data);
      throw;
    }
  }
  return data;
}

template <typename K, typename V, typename Hash, typename KeyEqual,
          typename Alloc>
void Map<K, V, Hash, KeyEqual, Alloc>::chain_thread_deallocate(chain_t *chain) {
  if (chain->head == nullptr) {
    return;
  }

  node_t *ptr = chain->head;
  node_t *prev = chain->head;
  while (ptr != nullptr && ptr->next != nullptr) {
    ptr = ptr->next;
    deallocate(prev);
    prev = ptr;
  }
  deallocate(prev);
}

template <typename K, typename V, typename Hash, typename KeyEqual,
          typename Alloc>
void Map<K, V, Hash, KeyEqual, Alloc>::data_deallocate(chain_t *data,
                                                       size_t n) noexcept {
  if (data == nullptr) {
    return;
  }
  std::span<chain_t> view{data, static_cast<size_t>(capacity)};
  for (chain_t &elem : view) {

    std::allocator_traits<data_allocator_type>::destroy(data_allocator, &elem);
  }
  std::allocator_traits<data_allocator_type>::deallocate(data_allocator, data,
                                                         n);
}

template <typename K, typename V, typename Hash, typename KeyEqual,
          typename Alloc>
void Map<K, V, Hash, KeyEqual, Alloc>::swap_state(Map &o) noexcept {
  data = o.data;
  allocator = o.allocator;
  data_allocator = o.data_allocator;
  node_allocator = o.node_allocator;
  hash_function = o.hash_function;
  key_comparator = o.key_comparator;
  o.data = o.data_allocate(2);

  o.capacity = 1;
  o.map_size = 0;
}

template <typename K, typename V, typename Hash, typename KeyEqual,
          typename Alloc>
void Map<K, V, Hash, KeyEqual, Alloc>::rehash() {
  map_size = 0;
  chain_t *old_data = data;
  auto old_start = begin();
  auto old_last = end();
  size_type old_capacity = capacity;
  capacity *= CAPACITY_MULTIPLIER;
  data = data_allocate(capacity + 1);

  while (old_start != old_last) {
    insert(std::move(old_start.node_ptr->value));
    old_start++;
  }
  std::span<chain_t> view{old_data, static_cast<size_t>(old_capacity)};
  for (chain_t &elem : view) {
    chain_thread_deallocate(&elem);
  }
  data_deallocate(old_data, old_capacity + 1);
}

#endif
