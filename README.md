What is this?
=========

General purpose transformation library with focus on cartographic projections.

How to use?
===

Right now not many transforms exist, but eventually the library will grow on as needed basis.

The library is based on backends which can be switched around, not all backends support all transforms, and certain backends expect to have certain things defined inside transforms.

    #include "transform.hpp"
    
    int main() {
		using namespace transform;
		using namespace transform::backends;
		using namespace transform::transforms;

		// ...
        
        transformer<cpu> t;
        t.run(scale(10.0),
            x_in, y_in, x_out, y_out);
    }
    
Checkout out the `examples` directory for more fun stuff.

