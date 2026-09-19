#include <iostream>
#include <random>
#include <vector>
#include <pthread.h>
#include <string.h>

struct calcArgs {
  double r;
  size_t tests;
  size_t seed;
};

double area(double r, size_t threads, size_t tests);
void* calcPthread(void* data);
size_t calc(double r, size_t tests, size_t seed);
bool isInside(double x, double y, double r);

int main(int argc, char** argv) {
  if (argc < 4) {
    std::cerr << "Not enough args\n";
    return 1;
  }
  double r = std::stod(argv[1]);
  size_t threads = std::stoull(argv[2]);
  size_t tests = std::stoull(argv[3]);

  if (r <= 0 || threads <= 0 || tests <= 0) {
    std::cerr << "All args must be greater than 0\n";
    return 1;
  }

  double result = 0;
  try {
    result = area(r, threads, tests);
  } catch (const std::exception& e) {
    std::cerr << e.what() << '\n';
    return 1;
  }
  std::cout << result << '\n';
}

double area(double r, size_t threads, size_t tests) {
  std::vector<pthread_t> pthreads = std::vector<pthread_t>(threads);
  std::vector<calcArgs> args = std::vector<calcArgs>(threads); 
  for (size_t i = 0; i < tests % threads; ++i) {
    pthread_attr_t attr = pthread_attr_t{};
    pthread_attr_init(&attr);
    args[i] = calcArgs{r, tests / threads + 1, i};
    int err = pthread_create(&pthreads[i], &attr, calcPthread, reinterpret_cast< void* >(&args[i]));
    pthread_attr_destroy(&attr);
    if (err) {
      throw std::runtime_error(strerror(err));
    }
  }
  for (size_t i = tests % threads; i < threads; ++i) {
    pthread_attr_t attr = pthread_attr_t{};
    pthread_attr_init(&attr);
    args[i] = calcArgs{r, tests / threads, i};
    int err = pthread_create(&pthreads[i], &attr, calcPthread, reinterpret_cast< void* >(&args[i]));
    pthread_attr_destroy(&attr);
    if (err) {
      throw std::runtime_error(strerror(err));
    }
  }
  size_t count_succes = 0;
  for (size_t i = 0; i < threads; ++i) {
    size_t res = 0;
    int err = pthread_join(pthreads[i], reinterpret_cast< void** >(&res));
    if (err) {
      throw std::runtime_error(strerror(err));
    }
    count_succes += res;
  }
  return 4.0 * r * r * count_succes / tests;
}

void* calcPthread(void* data) {
  calcArgs* calcArgsI = static_cast< calcArgs* >(data);
  return reinterpret_cast< void* >(calc(calcArgsI->r, calcArgsI->tests, calcArgsI->seed));
}

size_t calc(double r, size_t tests, size_t seed) {
  std::default_random_engine eng(seed);
  std::uniform_real_distribution< double > dist(0, 2 * r);
  size_t res = 0;
  for (size_t i = 0; i < tests; ++i) {
    res += (isInside(dist(eng), dist(eng), r) ? 1 : 0);
  }
  return res;
}

bool isInside(double x, double y, double r) {
  return ((x - r) * (x - r) + (y - r) * (y - r)) <= r * r;
}