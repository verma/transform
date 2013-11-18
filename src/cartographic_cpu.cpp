// cartographic_cpu.cpp
// CPU specializations for cartographic transforms
//

#include "transform/transforms/cartographic.hpp"
#include <stdio.h>

#define FC1 1.
#define FC2 .5
#define FC3 .16666666666666666666
#define FC4 .08333333333333333333
#define FC5 .05
#define FC6 .03333333333333333333
#define FC7 .02380952380952380952
#define FC8 .01785714285714285714

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

		constexpr double TO_RADIAN = 0.017453292519943295769236907684886;

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

	template<>
	void do_op<projection<tmerc<sphere, double>, latlong>, double, double >(
			const projection<tmerc<sphere, double>, latlong>& p,
			const double& x_in, const double& y_in, double& ox, double&oy) {
		typedef cartographic::ellipsoids::sphere			sphere;
		typedef cartographic::ellipsoids::sphere::params	sphereparams;

		constexpr double scale = 1.0 / sphereparams::major_axis;


		double x = (x_in - p.from.offset.first) * scale;
		double y = (y_in - p.from.offset.second) * scale;

		double s = scale;

		double phi0 = 0.0; // TODO: get this value here somehow
		double lambda0 = 0.0; // TODO: get this value here somehow

		double h, g, phi, lambda;

		h = std::exp(x);
		g = 0.5 * (h - 1.0 / h);
		h = std::cos(phi0 + y);

		phi = std::asin(std::sqrt((1.0 - h * h) / (1.0 + g * g)));
		if (y < 0.0)
			phi = -phi;

		lambda = (g || h) ? std::atan2(g, h) : 0.0;
		if (std::abs(g) > 1e-10 || std::abs(h) > 1e-10)
			lambda = std::atan2(g, h);

		constexpr double TO_DEGREES = 180.0 / M_PI;

		ox = util::mod_pi(lambda + lambda0);

		ox = ox * TO_DEGREES;
		oy = phi * TO_DEGREES;
	}

	template<>
	void do_op<projection<tmerc<WGS84, double>, latlong>, double, double>(
			const projection<tmerc<WGS84, double>, latlong>& p,
			const double& x_in, const double& y_in, double& ox, double&oy) {
		typedef cartographic::ellipsoids::WGS84			wgs84;
		typedef cartographic::ellipsoids::WGS84::params	wgs84params;

		constexpr double scale = 1.0 / wgs84params::major_axis;

		double x = (x_in - p.from.offset.first) * scale;
		double y = (y_in - p.from.offset.second) * scale;


		double sinPhi, cosPhi, con, t, n, d, ds;

		double lambda, phi;
		double lambda0 = 0.0;

		phi = util::projection::inv_mlfn<wgs84>(p.from.ml0 + y);

		sinPhi = std::sin(phi);
		cosPhi = std::cos(phi);

		t = 0.0;
		if (std::abs(cosPhi) > 1e-10)
			t = sinPhi / cosPhi;

		n = wgs84params::ecc2 / wgs84params::one_ecc2 * cosPhi * cosPhi;
		con = 1. - wgs84params::ecc2 * sinPhi * sinPhi;
		d = x * std::sqrt(con);
		con *= t;
		t *= t;
		ds = d * d;

		phi -= (con * ds / wgs84params::one_ecc2) * FC2 * (1. - 
				ds * FC4 * (5. + t * (3. - 9. * n) + n * (1. - 4. * n) -
					ds * FC6 * (61. + t * (90. - 252. * n + 45. * t) + 46. * n
						- ds * FC8 * (1385. + t * (3633. + t * (4095. + 1574. * t)))
						)));
		lambda = d * (FC1 -
				ds * FC3 * (1. + 2. * t + n -
					ds * FC5 * (5. + t * (28. + 24. * t + 8. * n) + 6. * n
						- ds * FC7 * (61. + t * (662. + t * (1320. + 720. * t)))
						))) / cosPhi;

		constexpr double TO_DEGREES = 180.0 / M_PI;

		ox = util::mod_pi(lambda + lambda0) * TO_DEGREES;
		oy = phi * TO_DEGREES;
	}
}
