The oscar extension for max (including unit test infrastructure for max)


Oscar is the first name of the man commonly known as The Wizard of Oz
	(we are pulling the strings of Max to automate its operation)

Oscar is also the name of the classic Grouch on Sesame Street
	(because testing is an activity that can make us grumpy)


______________________________________________

The oscar extension registers a handful of classes when Max launches.  
These class can then be used and invoked to run various kinds of unit tests on Max objects or Max patchers.

The Classes:

* test.master -- the ring-leader, can send messages to it ala ";test.master run borax bondo phasor~"
* test.runner -- the worker that actually wraps a set of tests on an object
* test.db     -- all testing activity is logged in a database, which is persistent such that results can be accessed even if Max crashes
* test.probe  -- utility object for instrumenting patchers that are loaded during tests
* test.unit   -- an object passed to a test that may be used to report results, etc.
* test.port   -- provide udp hooks for remote communication and automation


The Tests:

* Any class can have it's own custom tests.  A test is a method whose name begins with '###test###.'.  These are ideal for creating targeted/regression tests.  When an object is tested, any combination of the tests may be selected, or all, none.

* There is also a default set, which tries to spam any given object with all kinds of messages to try and make it crash or mis-behave under the stress.  This default set is inspired by the auval program Apple provides for AudioUnits.


Running Tests:

To run a test from within Max, you can send a message to the test.master object.  Here are some examples:

* ;test.master run zl			-- runs all tests on the zl external
* ;test.master run dictionary	-- runs all tests on the (nobox) dictionary object in the kernel
* ;test.master run :list		-- runs the 'list' test on all objects that have this test defined
* ;test.master run round :list	-- for a specific object (round), run a specific test ('list')
* ;test.master run				-- run all tests on all objects (may take a while)

Results are written to a database named "testresults.db3" which is saved in the 'testpackage' folder.  You can view this database in any SQLite database viewer.

Test can also be ordered-up remotely.  This allows you to run tests, for example, from the command-line.


Writing a Test:

1. The include:
	#include "ext_test.h"
	
2. Include the source file "ext_test.c" from c74support-private/max-includes

3. The message binding(s)
	addmess((method)acos_test,"###test###.values",A_CANT,0);

4. The method
	void acos_test(t_acos *x, t_test *t)
	{
		test_log(t, "Testing normal range [-1.0, 1.0]");	

		object_method(x, gensym("int"), 1);
		test_assert(t, "acos(1.0) == 0.0 -- int version", x->x_val == 0.0);
		object_method(x, gensym("float"), 1.0);
		test_assert(t, "acos(1.0) == 0.0 -- float version", x->x_val == 0.0);

		object_method(x, gensym("float"), -1.0);
		test_assert(t, "acos(-1.0) == 3.141593...", x->x_val > 3.141592 && x->x_val < 3.141594);
	
		test_log(t, "Testing NaN range");
		object_method(x, gensym("float"), -1.5);
		test_assert(t, "acos(-1.5) == NaN", x->x_val != x->x_val);
		object_method(x, gensym("float"), 200.0);
		test_assert(t, "acos(200.0) == NaN", x->x_val != x->x_val);

		test_log(t, "Back to normal range [-1.0, 1.0] to ensure sanity");	
		object_method(x, gensym("float"), 0.5);
		test_assert(t, "acos(0.5) == 1.047197", x->x_val > 1.047196 && x->x_val < 1.047198);
	}




______________________________________________
Old notes follow here...





THE PROCESS

Inspired by AudioUnits (auval):

1. load an object
2. send it messages and check its state

3. load the next object...


Note: the object loaded could be a patcher.  We could send it messages (including scripting), and we could export a png of the patcher for comparison to what it should look like.






IMPLEMENTATION


Writes results to a sqlite database:
- if max crashes, the results are persistent
- results can be searched or queried

table test {
	id
	name
	result (PASS|FAIL)
	notes
	type	-- might be a standard object test or some other unit test
}




Create a singleton instance and bind to the symbol 'testprobe' so that we can receive instructions, e.g.
	;test.probe run sometest someobject
	;test.probe run sometest
	
The second form would run the test on all objects.
Perhaps there would be a form that would pass arguments to the test?



Test can be defined in several ways (maybe?):

1. Internally to the testprobe extension (these are searched first)
2. Externally as a special kind of external (with appropriate metadata in the database)
3. Externally as a test 'script'
4. Externally as a patcher (similar to what we do now)

The last option is questionable.  We can already do that, and the way we already do it might be superior in that it is invoked by a separate process outside of the Max process.


Test scripts might look like this:

	create vst~ named myobj
	send myobj message plug foo
	validate error message in the max window
	read attr value to see if it changed or remained as it was

