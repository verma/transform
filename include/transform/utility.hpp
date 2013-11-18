// utility.hpp
// Utility functions
//

#ifndef __transform_utility_hpp__
#define __transform_utility_hpp__

#include <chrono>
#include <cmath>

namespace transform {
	namespace util {
		template<typename Function>
		static inline long long timeit(Function& f) {
			typedef std::chrono::high_resolution_clock Clock;
			typedef std::chrono::milliseconds milliseconds;

			Clock::time_point t0 = Clock::now();
			f();
			Clock::time_point t1 = Clock::now();

			return std::chrono::duration_cast<milliseconds>(t1 - t0).count();
		}

		static inline std::chrono::high_resolution_clock::time_point timer_start() {
			return std::chrono::high_resolution_clock::now();
		}

		static inline long long timer_end(const std::chrono::high_resolution_clock::time_point& s) {
			return std::chrono::duration_cast<std::chrono::milliseconds>(
					std::chrono::high_resolution_clock::now() - s).count();
		}

		template<typename T>
		static inline T mod_pi(const T& v) {
			return std::abs(v) > M_PI ?  (v - (v > 0.0 ? 2*M_PI : -2*M_PI)) : v;
		}

		// some utility functions for project math
		namespace projection {
			template<typename TEllipsoid, typename T>
			static inline T mlfn(const T& phi, const T& sphi_in, const T& cphi_in) {
				T cphi = cphi_in * sphi_in;
				T sphi = sphi_in * sphi_in;

				typedef typename TEllipsoid::params p;

				return(p::en0 * phi - cphi * (p::en1 + sphi * (p::en2
								+ sphi * (p::en3 + sphi * p::en4))));
			}

			template<typename TEllipsoid, typename T>
			static inline T mlfn(const T& phi) {
				return mlfn<TEllipsoid>(phi, std::sin(phi), std::cos(phi));
			}

			template<typename TEllipsoid, typename T>
			static inline T inv_mlfn(const T& argphi) {
				typedef typename TEllipsoid::params p;

				T phi, sinPhi, cosPhi, t;
				T es = p::ecc2;
				T k = 1./(1.-es);

				phi = argphi;
				int iter = 20;
				do {
					sinPhi = std::sin(phi);
					cosPhi = std::cos(phi);

					t = 1. - es * sinPhi * sinPhi;
					t = (mlfn<TEllipsoid>(phi, sinPhi, cosPhi) - argphi) * (t * std::sqrt(t)) * k;
					phi -= t;
					if (fabs(t) < 1e-12)
						break;
				} while(--iter);

				return phi;
			}
		}
	}
}

#endif // __transform_utility_hpp__
