// cartographic_cpu.cpp
// CPU specializations for cartographic transforms
//

#include "transform/transforms/cartographic.hpp"
#include <stdio.h>

namespace transform {
	// specialization do_op for projection for use by backends that do per point processing
	// e.g. cpu
	using namespace transforms;
	using namespace cartographic::projections;
	using namespace cartographic::ellipsoids;

	template<>
	void do_op<
		projection<latlong, tmerc<sphere, double>>, double, double
	>( const projection<latlong, tmerc<sphere, double>>& p,
			const double& x, const double& y, double& ox, double&oy) {
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

				xy.x = p.to.offset.first +  scale * xy.x;
				xy.y = p.to.offset.second + scale * xy.y;
			}
		}

		ox = xy.x;
		oy = xy.y;
	}

	template<>
	void do_op<
		projection<latlong, tmerc<WGS84, double>>, double, double
	>(const projection<latlong, tmerc<WGS84, double>>& p,
			const double& lambda_, const double& phi_, double& ox, double&oy) {
#define FC1 1.
#define FC2 .5
#define FC3 .16666666666666666666
#define FC4 .08333333333333333333
#define FC5 .05
#define FC6 .03333333333333333333
#define FC7 .02380952380952380952
#define FC8 .01785714285714285714

#define TO_RADIAN 0.017453292519943295769236907684886

		typedef cartographic::ellipsoids::WGS84			wgs84;
		typedef cartographic::ellipsoids::WGS84::params wgs84params;

		constexpr double scale = wgs84params::major_axis;

		double lambda = lambda_ * TO_RADIAN;
		double phi = phi_ * TO_RADIAN;

		double sinPhi, cosPhi, t, al, als, n;
		double x, y;

		sinPhi = sin(phi);
		cosPhi = cos(phi);
		t = 0.0;
		if (std::abs(cosPhi) > 1.0e-10)
			t = sinPhi/cosPhi;


		t *= t;
		al = cosPhi * lambda;
		als = al * al;
		al /= sqrt(1. - wgs84params::ecc2 * sinPhi * sinPhi);
		n = wgs84params::ecc2 / wgs84params::one_ecc2 * cosPhi * cosPhi;

		x = al * (FC1 +
				FC3 * als * (1. - t + n +
					FC5 * als * (5. + t * (t - 18.) + n * (14. - 58. * t)
						+ FC7 * als * (61. + t * ( t * (179. - t) - 479. ) )
						)));
		double __r = util::projection::mlfn<wgs84>(phi, sinPhi, cosPhi);

		y = (util::projection::mlfn<wgs84>(phi, sinPhi, cosPhi) - p.to.ml0 +
				sinPhi * al * lambda * FC2 * ( 1. +
					FC4 * als * (5. - t + n * (9. + 4. * n) +
						FC6 * als * (61. + t * (t - 58.) + n * (270. - 330. * t)
							+ FC8 * als * (1385. + t * ( t * (543. - t) - 3111.) )
							))));

		ox = p.to.offset.first + scale * x;
		oy = p.to.offset.second + scale * y;
	}
}
