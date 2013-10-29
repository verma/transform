// basic.hpp
// Basic no-good transforms
//

#ifndef __transform_transforms_basic_hpp__
#define __transform_transforms_basic_hpp__

#include <cmath>

namespace transform {
	namespace transforms {
		template<typename T, typename TOut>
		struct basic_scale {
			T s;
			basic_scale(const T& _s) : s(_s) { }

			void op(const T& x, const T& y, TOut& xo, TOut&yo) const {
				xo = static_cast<TOut>(s * x);
				yo = static_cast<TOut>(s * y);
			}
		};

		template<typename T>
		struct scale : basic_scale<T, T> {
			scale(const T& s) : basic_scale<T, T>(s) { }
		};

		template<typename T, typename TOut>
		struct sine_cosine {
			void op(const T& x, const T& y, TOut& xo, TOut& yo) const {
				xo = std::sin(x) * (1 + std::cos(y) * std::log(x * y * 0.001));
				yo = std::sin(y) * (1 + std::cos(x) * std::log(x * y * 0.001));
			}
		};
	}
};
#endif // __transform_transforms_basic_hpp__
