#include "simulated_annealing.hpp"

#include <cmath>

void simulated_annealing::update_median_change(const int& diff) {
  if (inputs.size() < 10) {
    inputs.push_back(diff);
  } else if (median_change == 0.) {
    double sum = 0.;
    for (int i = 0; i < 10; ++i) {
      sum += inputs[i];
    }
    median_change = sum / 10;
  }
}

double simulated_annealing::get_probability(const int& diff, const unsigned long long int& iteration) {
  update_median_change(diff);
  const double temperature = get_temp(iteration);

  if (median_change != 0.) {
    return exp(-(diff / median_change) / temperature);
  }

  return exp(-diff / temperature);
}

double simulated_annealing::get_temp(const unsigned long long int& iteration) const {
  const bool reached_cutoff = static_cast<double>(iteration) >= this->cutoff;

  return reached_cutoff
                    ? 0.
                    : -1. * start_temp  * static_cast<double>(iteration) / cutoff + start_temp;
}
