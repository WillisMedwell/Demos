#include "wath.h"

#define PRECISION_REQUIRED 0.00001f

inline float wsqrt(const float x) {
    /*
    // Newtons method -> Ref: https://math.mit.edu/~stevenj/18.335/newton-sqrt.pdf
    float i = x;
    if(x == 0) {
        return 0;
    }
    for(int j = 0; j < 20; j++) {
        i = 0.5 * (i + ( x / i) );  
    }
    return i;
    */

    // One of the few supported builtins!!!!
    return __builtin_sqrtf(x); 
}
inline float wisqrt(const float x) {
    // Famous quick inverse sqrt from Quake III Arena. id Software -> Ref: https://en.wikipedia.org/wiki/Fast_inverse_square_root
    long i;
	float x2, y;
	const float threehalfs = 1.5F;

	x2 = x * 0.5F;
	y  = x;
	i  = * ( long * ) &y;
	i  = 0x5f3759df - ( i >> 1 );
	y  = * ( float * ) &i;
	y  = y * ( threehalfs - ( x2 * y * y ) );  
	y  = y * ( threehalfs - ( x2 * y * y ) );  
    return y;   
}
inline float wtan(const float x) {
    // Power Series calculated from -> https://www.wolframalpha.com/widgets/view.jsp?id=f9476968629e1163bd4a3ba839d60925
    // For more information check out -> https://brilliant.org/wiki/taylor-series/
    double x2, x3, x5, x7, x9, x11, x13, x15, x17, x19, x21;
    x2  = (double)(x * x);
    x3  = x2  * x;
    x5  = x3  * x2;
    x7  = x5  * x2;
    x9  = x7  * x2;
    x11 = x9  * x2;
    x13 = x11 * x2;
    x15 = x13 * x2;
    x17 = x15 * x2;
    x19 = x17 * x2;
    x21 = x19 * x2;
    double a = 1.0;
    double b = 1.0/3.0;
    double c = 2.0/15.0;
    double d = 17.0/315.0;
    double e = 62.0/2835.0;
    double f = 1382.0/155925.0;
    double g = 21844.0/6081075.0;
    double h = 929569.0/638512875.0;
    double i = 6404582.0/10854718875.0;
    double j = 443861162.0/1856156927625.0;
    double l = 18888466084.0/194896477400625.0;
    return (float)((a*x) + (b*x3) + (c*x5) + (d*x7) + (e*x9) + (f*x11) + (g*x13) + (h*x15) + (i*x17) + (j*x19) + (l*x21));
}
inline float wmin(const float x, const float y) {
    return (x < y) ? x : y; 
}
inline float wmax(const float x, const float y) {
    return (x > y) ? x : y; 
}
inline float wabs(const float x) {
    return (x > 0) ? x : -x; 
}