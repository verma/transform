// proj.hpp
// Proj4 backend
//

#ifndef HAVE_PROJ4
#error You cannot include this file unless you have proj4 enabled
#endif

#ifndef __transform_backends_proj_hpp__
#define __transform_backends_proj_hpp__

#include "../transforms/cartographic.hpp"

#include <cassert>
#include <stdexcept>
#include <sstream>
#include <cmath>

#include <proj_api.h>

namespace transform {
	namespace backends {
		struct proj {
			template<
				typename TProjection,
				typename ForwardIterableRange>
			static void run(const TProjection& p,
					const ForwardIterableRange& x, const ForwardIterableRange& y,
					ForwardIterableRange& out_x, ForwardIterableRange& out_y) {
				size_t sx = boost::size(x);

				// data sanity
				assert(sx == boost::size(y));
				assert(boost::size(y) == boost::size(out_x));
				assert(boost::size(out_x) == boost::size(out_y));

				typedef typename boost::range_iterator<ForwardIterableRange>::type iterator;
				typedef typename boost::range_const_iterator<ForwardIterableRange>::type const_iterator;

				// projcl does in-place transforms, so move all input values to output
				const_iterator bx = boost::begin(x),
							   by = boost::begin(y);

				iterator ox = boost::begin(out_x),
						 oy = boost::begin(out_y);

				std::copy(bx, boost::end(x), ox);
				std::copy(by, boost::end(y), oy);

				// TODO: do projection with proj
				do_transform(p, &(*ox), &(*oy), NULL, 1, sx);
			}

		
			private:
			// since projcl deals in doubles we use doubles here to make anything not convertible to
			// double* error out, raw pointers to doubles and std::vector<double>::iterator should pass through
			template<
				typename TFrom,
				typename TTo,
				typename TEllipsoid>
			static void do_transform(const transforms::projection<TFrom, TTo, TEllipsoid>& p,
					double *x, double *y, double *z, size_t stride, size_t point_count) {
				std::string from = projection_to_string(p.from, TEllipsoid());
				std::string to = projection_to_string(p.to, TEllipsoid());

				projPJ pj_in = pj_init_plus(from.c_str()),
					   pj_out = pj_init_plus(to.c_str());

				if (!pj_in || !pj_out)
					throw std::runtime_error("Failed to initialize transforms");

				// fasten your seatbelts
				int error =
					pj_transform(pj_in, pj_out,
							static_cast<long>(point_count),
							static_cast<int>(stride),
							x, y, NULL);

				pj_free(pj_in);
				pj_free(pj_out);

				if (error != 0)
					throw std::runtime_error("Transform failed");

				// all good otherwise
			}

			template<typename TEllipsoid>
			static std::string projection_to_string(const cartographic::projections::latlong& p, const TEllipsoid& ell) {
				std::stringstream sstr;
				sstr << "+proj=" << p.name
					<< " +ellps=" << ell.name;

				return sstr.str();
			}

			template<typename T, typename TEllipsoid>
			static std::string projection_to_string(const cartographic::projections::tmerc<T>& p, const TEllipsoid& ell) {
				std::stringstream sstr;
				sstr << "+proj=" << p.name
					<< " +ellps=" << ell.name
					<< " +lat_0=" << std::abs(p.offset.first) << (p.offset.first > 0.0 ? "n" : "s")
					<< " +lon_0=" << std::abs(p.offset.second) << (p.offset.second > 0.0 ? "e" : "w");

				return sstr.str();
			}
		};
	}
}
#endif // __transform_backends_proj_hpp__

