#include <iostream>
#include <random>

size_t calc(double r, size_t tests, size_t seed) {
  std::default_random_engine eng(seed);
  std::uniform_real_distribution< double > dist(0, 2 * r);
  size_t res = 0;
  for (size_t i = 0; i < tests; ++i) {
    res += isInside(dist(eng), dist(eng), r) ? 1 : 0;
  }
  return res;
}

bool isInside(double x, double y, double r) {
  return ((r - x) * (r - x)) <= r && ((r - y) * (r - y)) <= r;
}

int main() {

}
