#include <bits/stdc++.h>
#include "../../src/include/container/circular_queue.h"

using namespace std; // NOLINT

conless::circular_queue<int, 100> q;

auto main() -> int {
  int n;
  cin >> n;
  for (int i = 1; i <= n; i++) {
    int num;
    cin >> num;
    cout << q.push(num) << '\n';
  }
  cout << q.size() << '\n';
  for (auto data : q) {
    cout << data << ' ';
  }
  cout << endl;
}