// cartographic.hpp
// Cartographic projections as transforms
//

#ifndef __transforms_transform_cartographic_hpp__
#define __transforms_transform_cartographic_hpp__

#include <string>
#include <cmath>
#include <type_traits>

#include "../cpu_op.hpp"
#include "../utility.hpp"

namespace transform {
	namespace cartographic {
		namespace ellipsoids {
			struct sphere {
				static constexpr const char *name = "sphere";

				struct params {
					static constexpr double major_axis = 6370997.0;
					static constexpr double minor_axis = 6370997.0;
					static constexpr double one_ecc2 = 1;
					static constexpr double ecc2 = 0;
					static constexpr double ecc = 0;
					static constexpr double
						en0 = 1, en1 = 0, en2 = 0, en3 = 0, en4 = 0;
				};
			};

			struct WGS84 {
				static constexpr const char *name = "WGS84";

				struct params {
					static constexpr double major_axis = 6378137;
					static constexpr double minor_axis = 6356752.31424;
					static constexpr double inverse_flattening = 298.25722349076665;
					static constexpr double one_ecc2 = 0.9933056200082402;
					static constexpr double ecc2 = 0.0066943799917598135;
					static constexpr double ecc = 0.08181919085251219;
					static constexpr double ec = 1.9955310875017547;
					static constexpr double 
						en0 = 0.9983242984313436263477115,
						en1 = 0.0050186784214849861768259,
						en2 = 0.0000210029809788678256417,
						en3 = 0.0000001093660339214014494,
						en4 = 0.0000000006178058818421671;
				};

			};
		}

		namespace projections {
			struct base_projection {
				std::string name;
				base_projection(const std::string& s) : name(s) { }
			};

			struct latlong : base_projection {
				latlong() : base_projection("latlong") { }
			};

			template<typename TEllipsoid, typename T>
			struct tmerc : base_projection {
				typedef typename std::pair<T, T> offset_t;
				typedef TEllipsoid ellipsoid_type;


				tmerc(const offset_t& off) : base_projection("tmerc"), offset(off),
					ml0(util::projection::mlfn<TEllipsoid>(offset.second)) {
				}

				offset_t offset;
				T ml0;
			};
		}
	}

	namespace transforms {
		template<
			typename TFrom,
			typename TTo
		>
		struct projection : cpu_op<projection<TFrom, TTo>> {
			TFrom from;
			TTo to;

			projection(const TFrom& from_, const TTo& to_): cpu_op<projection<TFrom, TTo>>(*this),
				from(from_), to(to_) {}
		};
	}
}

#endif // __transforms_transform_cartographic_hpp__
