#ifndef SIMULATED_ANNEALING_HPP
#define SIMULATED_ANNEALING_HPP
#include <vector>

class simulated_annealing {
  const double start_temp;
  const double cutoff;

  double median_change;
  std::vector<int> inputs;

public:
  explicit simulated_annealing(const unsigned long long int& iterations,
                               const double& start_temp = 1.,
                               const double& cutoff_range = 0.9):
    start_temp(start_temp),
    cutoff(static_cast<double>(iterations) * cutoff_range),
    median_change(0.) {}

  void update_median_change(const int& diff);

  [[nodiscard]] double get_probability(const int& diff, const unsigned long long int& iteration);

  [[nodiscard]] double get_temp(const unsigned long long int& iteration) const;
};

#endif
