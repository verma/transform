// basic.hpp
// Basic no-good transforms
//

#ifndef __transform_transforms_basic_hpp__
#define __transform_transforms_basic_hpp__

#include <cmath>

namespace transform {
	namespace transforms {
		template<typename T>
		struct scale {
			T s;
			scale(const T& _s) : s(_s) { }

			void op(const T& x, const T& y, T& xo, T&yo) const {
				xo = t * x;
				yo = t * y;
			}
		};

		template<typename T>
		struct sincos {
			void op(const T& x, const T& y, T& xo, T&yo) const {
				xo = std::sin(x) * (1 + std::cos(y) * std::log(x * y * 0.001));
				yo = std::sin(y) * (1 + std::cos(x) * std::log(x * y * 0.001));
			}
		};
	}
};
#endif // __transform_transforms_basic_hpp__