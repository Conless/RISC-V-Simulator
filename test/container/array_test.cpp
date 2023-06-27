#include <bits/stdc++.h>
#include "../../src/include/container/array.h"

using namespace std; // NOLINT

conless::array<int, 10> arr;

auto main() -> int {
  int n;
  for (auto data : arr) {
    cout << data.second << ' ';
  }
  cout << endl;
  cin >> n;
  for (int i = 1; i <= n; i++) {
    cin >> arr[i];
  }
  int m;
  cin >> m;
  for (int i = 1; i <= m; i++) {
    int pos;
    cin >> pos;
    arr.erase_move(pos);
  }
  for (auto data : arr) {
    cout << data.second << ' ';
  }
  cout << endl;
}