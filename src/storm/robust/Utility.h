#pragma once

#include <cmath>

namespace storm {
    namespace robust {
        double beta(double alpha, double beta) {
            return tgamma(alpha)*tgamma(beta)/tgamma(alpha+beta);
        }

        double betaPDF(double alpha, double beta, double x) {
            return pow(x, alpha-1)*pow(1-x, beta-1)/beta(alpha, beta);
        }
    }
}
