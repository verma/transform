// cpu_op.hpp
// provides base class for enable CPU ops (the functions the cpu type backends call)
//

#ifndef __transform_cpu_op_hpp__
#define __transform_cpu_op_hpp__

namespace transform {
	template<
		typename TTransform,
		typename TValue,
		typename TOutput
	>
	void do_op(const TValue& x, const TValue& y,
			TOutput& ox, TOutput& oy) {
		static_assert(sizeof(TValue) == 0,
				"You need to specialize transform op for the projections you intend to use");
	}

	template<typename TTransform>
	struct cpu_op {
		template<typename TValue, typename TOutput>
		void op(const TValue& x, const TValue& y,
				TOutput& ox, TOutput& oy) const {
			do_op<TTransform, TValue, TOutput>(x, y, ox, oy);
		}
	};
};



#endif // __transform_cpu_op_hpp__
