// proj.hpp
// Proj4 backend
//

#ifndef HAVE_PROJ4
#error You cannot include this file unless you have proj4 enabled
#endif

#ifndef __transform_backends_proj_hpp__
#define __transform_backends_proj_hpp__

#include "../transforms/cartographic.hpp"
#include "../concurrency.hpp"

#include <cassert>
#include <stdexcept>
#include <sstream>
#include <cmath>

#include <boost/range.hpp>

#include <proj_api.h>

namespace transform {
	namespace backends {
		namespace detail {
			template<
				typename TProjection,
				typename T
			>
			void pre_process(const TProjection& p, T& x, T& y, size_t count) {
				// default implementation does nothing
				//
			}

			template<
				typename TProjection,
				typename T
			>
			void post_process(const TProjection& p, T& x, T& y, size_t count) {
				// default implementation does nothing
				//
			}
		}

		template<unsigned MaxConcurrency>
		struct multi_proj {
			typedef multi_proj<MaxConcurrency> this_type;

			template<
				typename TProjection,
				typename ForwardIterableInputRange,
				typename ForwardIterableOutputRange
			>
			void run(const TProjection& p,
				const ForwardIterableInputRange& x,
				const ForwardIterableInputRange& y,
				ForwardIterableOutputRange& out_x,
				ForwardIterableOutputRange& out_y) const {
				size_t sx = boost::size(x);

				// data sanity
				assert(sx == boost::size(y));
				assert(boost::size(y) == boost::size(out_x));
				assert(boost::size(out_x) == boost::size(out_y));

				std::string from = projection_to_string(p.from);
				std::string to = projection_to_string(p.to);

				auto compute = [&p, &from, &to](double *x, double *y, double *z,
						size_t stride, size_t point_count) {
					projCtx ctx = pj_ctx_alloc();

					projPJ pj_in = pj_init_plus_ctx(ctx, from.c_str()),
						   pj_out = pj_init_plus_ctx(ctx, to.c_str());

					assert(pj_in != NULL);
					assert(pj_out != NULL);

					detail::pre_process(p.from, x, y, point_count);


					// fasten your seatbelts
					pj_transform(pj_in, pj_out,
							static_cast<long>(point_count),
							static_cast<int>(stride),
							x, y, NULL);

					pj_free(pj_in);
					pj_free(pj_out);

					pj_ctx_free(ctx);
					detail::post_process(p.to, x, y, point_count);
				};

				typedef typename boost::range_iterator<ForwardIterableOutputRange>::type iterator;
				typedef typename boost::range_const_iterator<ForwardIterableInputRange>::type const_iterator;

				// projcl does in-place transforms, so move all input values to output
				const_iterator bx = boost::begin(x),
							   by = boost::begin(y);

				iterator ox = boost::begin(out_x),
						 oy = boost::begin(out_y);

				std::copy(bx, boost::end(x), ox);
				std::copy(by, boost::end(y), oy);

				utility::scheduler<MaxConcurrency> c;
				unsigned max_threads = c.concurrency();
				size_t per_batch = sx / max_threads;

				size_t offset = 0;
				for (unsigned i = 0 ; i < max_threads ; i ++) {
					double *x = &(*(ox + offset));
					double *y = &(*(oy + offset));
					double *z = NULL;

					c.queue(compute, x, y, z, 1, per_batch);
					offset += per_batch;
				}

				c.wait();
			}

			private:
			static std::string projection_to_string(const cartographic::projections::latlong& p) {
				std::stringstream sstr;
				sstr << "+proj=" << p.name;

				return sstr.str();
			}

			template<typename TEllipsoid, typename T>
			static std::string projection_to_string(const cartographic::projections::tmerc<TEllipsoid, T>& p) {
				std::stringstream sstr;

				sstr << "+proj=" << p.name
					<< " +ellps=" << TEllipsoid::name
					<< " +lat_0=" << std::abs(p.offset.first) << (p.offset.first > 0.0 ? "n" : "s")
					<< " +lon_0=" << std::abs(p.offset.second) << (p.offset.second > 0.0 ? "e" : "w");

				return sstr.str();
			}
		};

		typedef multi_proj<0> full_concurrency_proj;
		typedef multi_proj<1> proj;

		// specializations for pre and post process operations
		template<>
		void detail::pre_process<cartographic::projections::latlong, double*>
			(const cartographic::projections::latlong& p, double *&x, double *&y, size_t size);
		template<>
		void detail::post_process<cartographic::projections::latlong, double*>
			(const cartographic::projections::latlong& p, double *&x, double *&y, size_t size);
	}
}
#endif // __transform_backends_proj_hpp__

