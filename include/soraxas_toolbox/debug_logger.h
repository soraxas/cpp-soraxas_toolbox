#ifdef USE_DEBUG_LOG
void LOG() { std::cout << std::endl; }

template <typename First, typename... Rest>
void LOG(First &&first, Rest &&...rest) {
  std::cout << std::forward<First>(first);
  LOG(std::forward<Rest>(rest)...);
}
#define DEBUG_LOG(...) LOG(__VA_ARGS__)
#else
#define DEBUG_LOG(...)
#endif
