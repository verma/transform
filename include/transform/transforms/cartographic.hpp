// cartographic.hpp
// Cartographic projections as transforms
//

#ifndef __transforms_transform_cartographic_hpp__
#define __transforms_transform_cartographic_hpp__

#include <string>
#include <cmath>
#include <type_traits>

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

			struct latlong : public base_projection {
				latlong() : base_projection("latlong") { }
			};

			template<typename T>
			struct tmerc : public base_projection {
				typedef typename std::pair<T, T> offset_t;
				tmerc(const offset_t& off) : base_projection("tmerc"), offset(off) { }

				offset_t offset;
			};
		}
	}

	namespace transforms {
		template<
			typename TValue,
			typename TOutput,
			typename TFrom,
			typename TTo,
			typename TEllipsoid
		>
		void do_op(const TValue& x, const TValue& y,
				TOutput& ox, TOutput& oy) {
			static_assert(sizeof(TValue) == 0,
					"You need to specialize transform op for the projections you intend to use");
		}

		template<
			typename TFrom,
			typename TTo,
			typename TEllipsoid = cartographic::ellipsoids::sphere
		>
		struct projection {
			typedef TEllipsoid ellipsoid_type;

			TFrom from;
			TTo to;

			projection(const TFrom& from_, const TTo& to_):
				from(from_), to(to_) {}

			template<typename TValue, typename TOutput>
			void op(const TValue& x, const TValue& y,
					TOutput& ox, TOutput& oy) const {
				do_op<TValue, TOutput, TFrom, TTo, TEllipsoid>(x, y, ox, oy);
			}
		};

		// specialization do_op for projection for use by backends that do per point processing
		// e.g. cpu
		template<>
		void do_op<
			double,
			double,
			cartographic::projections::latlong,
			cartographic::projections::tmerc<double>,
			cartographic::ellipsoids::sphere
		>(const double& x, const double& y, double& ox, double&oy) {
			struct {
				double lam, phi;
			} lp;

			struct _P {
				double phi0;
			};

			struct _P __P = { 0.0 };
			struct _P *P = &__P;


			struct {
				double x, y;
			} xy;

			lp.lam = x * 0.017453292519943295769236907684886;
			lp.phi = y * 0.017453292519943295769236907684886;

			const double aks0 = 1.0;
			const double aks5 = 0.5 * aks0;

			const double scale = 6370997.0;
			const double EPS10 = 1e-10;

			double b, cosphi;
			if( lp.lam < -M_PI/2.0 || lp.lam > M_PI/2.0 )
			{
				xy.x = xy.y = std::numeric_limits<double>::infinity();
			}
			else {
				b = (cosphi = cos(lp.phi)) * sin(lp.lam);
				if (fabs(fabs(b) - 1.) <= EPS10) {
					xy.x = xy.y = std::numeric_limits<double>::infinity();
				}
				else {
					xy.x = aks5 * log((1. + b) / (1. - b));
					if ((b = fabs( xy.y = cosphi * cos(lp.lam) / sqrt(1. - b * b) )) >= 1.) {
						if ((b - 1.) > EPS10) {
							xy.x = xy.y = std::numeric_limits<double>::infinity();
						}
						else xy.y = 0.;
					} else
						xy.y = acos(xy.y);
					if (lp.phi < 0.) xy.y = -xy.y;
					xy.y = aks0 * (xy.y - P->phi0);

					xy.x = scale * xy.x;
					xy.y = scale * xy.y;
				}
			}

			ox = xy.x;
			oy = xy.y;
		}
	}
}

#endif // __transforms_transform_cartographic_hpp__
