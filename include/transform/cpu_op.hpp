// cpu_op.hpp
// provides base class for enable CPU ops (the functions the cpu type backends call)
//

#ifndef __transform_cpu_op_hpp__
#define __transform_cpu_op_hpp__

namespace transform {
	template<
		typename TDerived,
		typename TValue,
		typename TOutput
	>
	void do_op(const TDerived& p, const TValue& x, const TValue& y,
			TOutput& ox, TOutput& oy) {
		static_assert(sizeof(TValue) == 0,
				"You need to specialize transform op for the projections you intend to use");
	}

	template<typename TDerived>
	struct cpu_op {
		// we accept a refernce of the derived class (although they are the same object),
		// to avoid code-smell with casting base types to derived types when we call do_op
		// below
		cpu_op(const TDerived& derived):
			derived_(derived) { }
		template<typename TValue, typename TOutput>
		void op(const TValue& x, const TValue& y,
				TOutput& ox, TOutput& oy) const {
			// assumption is that cpu_op is used as a base class and TTransform is the
			// derived class
			do_op<TDerived, TValue, TOutput>(derived_, x, y, ox, oy);
		}

		const TDerived& derived_;
	};
};



#endif // __transform_cpu_op_hpp__
