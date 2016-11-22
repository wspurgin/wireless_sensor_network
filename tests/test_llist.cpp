#include "../lib/LList.h"

// Test insert and append methods
LList<int> test_insertion() {
  LList<int> int_list;
  int_list.insert(2);
  int_list.insert(4);
  int_list.insert(6);
  int_list.append(0);
  int_list.append(-2);
  int_list.append(-4);
  int_list.append(-6);
  return int_list;
}

// Test that the nodes are connected both singly (going forward) and doubley
// (going backwards).
void test_linkage(const LList<int>& int_list) {
  for (auto i = int_list.begin(); i != int_list.end(); ++i)
    cout << *(i) << " ";
  cout << endl;

  // reverse order TODO need something better than reuse of `end`
  for (auto i = int_list.last(); i != int_list.end(); --i)
    cout << *(i) << " ";
  cout << endl;
}

int main(int argc, const char *argv[])
{
  LList<int> int_list = test_insertion();
  test_linkage(int_list);
  return 0;
}
