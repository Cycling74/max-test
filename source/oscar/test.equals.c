//
// 	unit/integration test extension for max
//	tim place
//	cycling '74
//

#include "oscar.h"
#include "z_dsp.h"


/** like == but can compare floating-point numbers while tolerating the floating-point (im)precision.
 */
typedef struct _testequals {
	t_pxobject	x_ob;				///< header
	void		*x_outlet;			///< float/list for sampled values
	void		*x_inlet;			///< for setting the operand
	double		x_operand;			///< the number against which to test input
	long		x_tolerance;		///< number of floating-point representations around the specified operand to consider as "equal"
	long		x_single_precision;	///< operate on 32-bit floats rather than 64-bit doubles
} t_testequals;


// prototypes
void*	testequals_new(t_symbol *s, long argc, t_atom *argv);
void 	testequals_free(t_testequals *x);
void	testequals_assist(t_testequals *x, void *b, long m, long a, char *s);
void	testequals_float(t_testequals *x, double f);


// class variables
static t_class		*s_testequals_class = NULL;


/************************************************************************/

void testequals_classinit(void)
{
	t_class *c = class_new("test.equals", (method)testequals_new, (method)testequals_free, sizeof(t_testequals), (method)NULL, A_GIMME, 0);
	
	class_addmethod(c, (method)testequals_float,	"float",	A_FLOAT, 0);
	class_addmethod(c, (method)testequals_assist,	"assist",	A_CANT, 0);
	
	CLASS_ATTR_LONG(c, "tolerance", 0, t_testequals, x_tolerance);
	CLASS_ATTR_LONG(c, "single_precision", 0, t_testequals, x_single_precision);
	
	class_register(_sym_box, c);
	s_testequals_class = c;
}


/************************************************************************/


void* testequals_new(t_symbol *s, long argc, t_atom *argv)
{
	t_testequals	*x = (t_testequals*)object_alloc(s_testequals_class);
	long			attrstart = attr_args_offset((short)argc, argv);

	if (attrstart)
		x->x_operand = atom_getfloat(argv);
	
	x->x_outlet = outlet_new(x, NULL);
	x->x_inlet = proxy_new(x, 1, NULL);
	x->x_tolerance = 1;
#ifdef C74_X64
	x->x_single_precision = false;
#else
	x->x_single_precision = true;
#endif
	
	attr_args_process(x, (short)argc, argv);
	autocolorbox((t_object*)x);
	return x;
}


void testequals_free(t_testequals *x)
{
	object_free(x->x_inlet);
}


#pragma mark -
/************************************************************************/

void testequals_assist(t_testequals *x, void *b, long m, long a, char *s)
{
	strcpy(s, "log messages to the test result, or to the max window");
}


#ifdef OLD
static const double k_epsilon = 0.000001;

t_bool testequals_equivalent(double a, double b)
{
	t_bool	result;
	double	a_abs = fabs(a);
	double	b_abs = fabs(b);
	double	absolute_or_relative = (1.0f > a_abs ? 1.0f : a_abs);
	
	absolute_or_relative = (absolute_or_relative > b_abs ? absolute_or_relative : b_abs);
	if (fabs(a - b) <= k_epsilon * absolute_or_relative)
		result = true;
	else
		result = false;

	return result;
}
#else

#ifdef WIN_VERSION
#include <float.h>
#endif

t_bool testequals_equivalent(double a, double b, long tolerance, long single_precision)
{
	if (a == b) {
		return true;
	}
	else {
		t_bool	result;
		int		i;
		int		operand_is_negative = (b < 0.0);
		
		if (single_precision) {
			float	next = nextafter((float)b, FLT_MAX);
			float	prev = nextafter((float)b, -FLT_MAX);
			
			// if next == prev then bail on this method and just compare strings
			if (next == prev) {
				char a_str[16];
				char b_str[16];
				
				snprintf(a_str, 16, "%.6f", a);
				snprintf(b_str, 16, "%.6f", b);
				if (!strcmp(a_str, b_str))
					return true;
				else
					return false;
			}
			
			for (i=1; i<tolerance; i++) {
				next = nextafter(next, FLT_MAX);
				prev = nextafter(prev, -FLT_MAX);
			}
			
			if (operand_is_negative && a <= prev && a >= next)
				result = true;
			else if (a >= prev && a <= next) // operand is positive
				result = true;
			else
				result = false;
		}
		else { // double precision
			double	next = nextafter(b, DBL_MAX);
			double	prev = nextafter(b, -DBL_MAX);
			
			// if next == prev then bail on this method and just compare strings
			if (next == prev) {
				char a_str[32];
				char b_str[32];
				
				snprintf(a_str, 32, "%.15f", a);
				snprintf(b_str, 32, "%.15f", b);
				if (!strcmp(a_str, b_str))
					return true;
				else
					return false;
			}

			for (i=1; i<tolerance; i++) {
				next = nextafter(next, DBL_MAX);
				prev = nextafter(prev, -DBL_MAX);
			}

			if (operand_is_negative && a <= prev && a >= next)
				result = true;
			else if (a >= prev && a <= next) // operand is positive
				result = true;
			else
				result = false;
		}

		return result;
	}
}

#endif // OLD




void testequals_float(t_testequals *x, double f)
{
	if (proxy_getinlet((t_object*)x) == 1)
		x->x_operand = f;
	else
		outlet_int(x->x_outlet, testequals_equivalent(x->x_operand, f, x->x_tolerance, x->x_single_precision));
}

