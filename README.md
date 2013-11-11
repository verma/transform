What is this?
=========

General purpose transformation library with focus on cartographic projections.

How to use?
===

Right now not many transforms exist, but eventually the library will grow on as needed basis.

The library is based on backends which can be switched around, not all backends support all transforms, and certain backends expect to have certain things defined inside transforms.  Everything is validated statically at compile time.

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

We intend to develop a performant transform library.  Presently the benchmarks for WGS84 latlong->tmerc stand as:

         #     count(mil)      proj     mproj       cpu      mcpu       cCL       gCL
         1             10       482       141       355        79       195       310
         2             20       962       284       711       155       359       548
         3             30      1485       474      1092       226       557       811
         4             40      1999       578      1422       303       737      1036
         5             50      2411       734      1788       377       937      1289


         All times are in milliseconds.
         
		 mproj: Multi-threaded proj backend
		 cCL  : CPU device OpenCL
		 gCL  : GPU device OpenCL
    
For multi-threaded backends, the transforms run at full concurrency where the workload is divided among available hardware threads, as reported by `std::thread::hardware_concurrency()`


