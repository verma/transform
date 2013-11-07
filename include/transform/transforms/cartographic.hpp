// cartographic.hpp
// Cartographic projections as transforms
//

#ifndef __transforms_transform_cartographic_hpp__
#define __transforms_transform_cartographic_hpp__

#include <string>
#include <cmath>

#include "../cpu_op.hpp"

namespace transform {
	namespace cartographic {
		namespace ellipsoids {
			struct base_ellipsiod {
				std::string name;
				base_ellipsiod(const std::string& n) : name(n) { }
			};

			struct sphere : public base_ellipsiod {
				sphere() : base_ellipsiod("sphere") { }
			};

			struct WGS84 : public base_ellipsiod {
				WGS84() : base_ellipsiod("WGS84") { }
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

			template<typename T>
			struct tmerc : base_projection {
				typedef typename std::pair<T, T> offset_t;
				tmerc(const offset_t& off) : base_projection("tmerc"), offset(off) { }

				offset_t offset;
			};
		}
	}

	namespace transforms {
		template<
			typename TFrom,
			typename TTo,
			typename TEllipsoid = cartographic::ellipsoids::sphere
		>
		struct projection : cpu_op<projection<TFrom, TTo, TEllipsoid> > {
			typedef TEllipsoid ellipsoid_type;

			TFrom from;
			TTo to;

			projection(const TFrom& from_, const TTo& to_):
				from(from_), to(to_) {}
		};
	}
}

#endif // __transforms_transform_cartographic_hpp__
