// cartographic_cpu.cpp
// CPU specializations for cartographic transforms
//

#include "transform/transforms/cartographic.hpp"
#include <limits>

namespace transform {
	// specialization do_op for projection for use by backends that do per point processing
	// e.g. cpu
	template<>
	void do_op<
		transforms::projection<
			cartographic::projections::latlong,
			cartographic::projections::tmerc<double>,
			cartographic::ellipsoids::sphere
		>,
		double,
		double
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
