#include <stdio.h>

///////////////////////////////////////  list.h  ////////////////////////////

#include <stddef.h>  // for size_t
#include <utility>   // for std::move

namespace leanstl
{

struct list_node_base
{
  list_node_base* next;
  list_node_base* prev;
};

template <typename T>
class list
{
 public:
  list()
  {
    head_.next = &head_;
    head_.prev = &head_;
  }

  ~list()
  {
    for (list_node_base* n = head_.next; n != &head_;)
    {
      list_node* tod = static_cast<list_node*>(n);
      n = n->next;
      delete tod;
    }
  }

  list(const list& rhs) = delete;
  void operator=(const list& rhs) = delete;

  class iterator
  {
   public:
    iterator(list_node_base* n = nullptr) : node_(n) {}
    T& operator*() { return *operator->(); }
    T* operator->() { return &static_cast<list_node*>(node_)->value_; }
    iterator& operator++() { node_ = node_->next; return *this; }
    iterator operator++(int) { iterator old = *this; operator++(); return old; }
    iterator& operator--() { node_ = node_->prev; return *this; }
    iterator operator--(int) { iterator old = *this; operator--(); return old; }
    bool operator==(iterator rhs) const { return node_ == rhs.node_; }
    bool operator!=(iterator rhs) const { return node_ != rhs.node_; }

   private:
    list_node_base* node_;
  };

  class const_iterator
  {
   public:
    const_iterator(const list_node_base* n = nullptr) : node_(n) {}
    const T& operator*() { return *operator->(); }
    const T* operator->() { return &static_cast<const list_node*>(node_)->value_; }
    const_iterator& operator++() { node_ = node_->next; return *this; }
    const_iterator operator++(int) { const_iterator old = *this; operator++(); return old; }
    const_iterator& operator--() { node_ = node_->prev; return *this; }
    const_iterator operator--(int) { const_iterator old = *this; operator--(); return old; }
    bool operator==(const_iterator rhs) const { return node_ == rhs.node_; }
    bool operator!=(const_iterator rhs) const { return node_ != rhs.node_; }

   private:
    const list_node_base* node_;
  };
  // TODO: test operator==()

  iterator begin() { return iterator(head_.next); }
  const_iterator begin() const { return const_iterator(head_.next); }
  const_iterator cbegin() const { return const_iterator(head_.next); }
  iterator end() { return iterator(&head_); }
  const_iterator end() const { return const_iterator(&head_); }
  const_iterator cend() { return const_iterator(&head_); }

  void push_front(T x)
  {
    list_node* n = new list_node(std::move(x));
    n->next = head_.next;
    n->next->prev = n;
    head_.next = n;
    n->prev = &head_;
    size_++;
  }

  T& front()
  {
    return static_cast<list_node*>(head_.next)->value_;
  }

 private:
  struct list_node : list_node_base
  {
    T value_;
    list_node(T&& x) : value_(std::move(x)) {}
  };

  list_node_base head_;
  size_t size_ = 0;
};

}  // namespace leanstl

///////////////////////////////////////  main()  ////////////////////////////

#include <memory>
#include <string>

int main()
{
  leanstl::list<int> li;
  li.push_front(43);
  li.push_front(82);

  for (auto x : li)
    printf("%d\n", x);

  const leanstl::list<int>& lic(li);
  for (auto x : lic)
    printf("%d\n", x);

  leanstl::list<std::string> ls;
  ls.push_front("hello");
  printf("front=%s\n", ls.front().c_str());

  leanstl::list<std::unique_ptr<double>> lu;
  lu.push_front(std::unique_ptr<double>(new double(5.0)));  // make_unique in C++14
  printf("front=%f\n", *lu.front());
}
