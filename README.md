# max-test
Automated Test Harness for Max

This package provides a set of simple tools with which to instrument patchers to verify expected behavior. Patchers that are instrumented as "test patchers" assist in fast identification of bugs, of the expectations of the patcher's author, and fast verification once a bug is addressed. Furthermore, instrumented test patchers can be evaluated as part of an automated system on multiple architectures and platforms.

A video introduction is available at https://www.youtube.com/watch?v=l_3W4tZcgXI

## Construction of a Test

A test patcher **must** meet the following criteria:

* File name must end in .maxtest.maxpat (for patchers) or .maxtest.maxzip (for projects).
* The patcher must start itself. Typically this is done with a loadbang object.
* The test **must** terminate itself when complete. Failing to do so could hang the automated system from proceeding. A test is terminated by sending a bang to the test.terminate object.
* The test must contain 1 or more test.assert objects. These objects define the expected results.


## Examples

The 'patchers' folder in this package contains some example test patchers for reference. Here are the examples with some associated topics:

* **2087-bitxor~** : use test.sample~ instead of snapshot~ for more predictable behavior in a test setting. Use test.equals for comparing floats instead of ==. Use right-to-left ordering and the last part of the test sequence to terminate the test rather than relying on asynchonous methods such as delays or defers.
* **2249-dict-syntax-space** : simple patcher (no audio) for checking that args to dict are interpretted correctly.
* **2779-3313-allpass~** : comparison of a complex impulse response produced by a filter by recording it into a buffer~, then comparing that to a reference buffer~ that contains the correct impulse response. Requires audio to run for at least 1 second in order to get the impulse response.
* **2859-dict-singles** : string comparison where the strings are composed of multiple-lines. Uses the test.string.equals object because otherwise the test, authored on a Mac, will fail on Windows due to line-endings being different between the results and reference strings.
* **4505-dict-notification-js** : example of a test as a Max project.
* **4521-dict-contains** : trapping expected (or unexpected) errors to the Max window.
* **4863-buffer~-duration** : simply checking accumulated error at the end instead of a bunch of individual checks.


## Running the Tests

You can open a test patcher/project at any time and view it visually to inspect the results.

To run the tests in an automated fashion from with Max, see the 'oscar.maxhelp' patcher in the 'help' folder. To access the results of the tests, see the section below on test results.


## Fully Automated Testing with Ruby

You can communicate with Max remotely to fully automate the running of test patchers and log the results. An example for how to do this is provided as a Ruby script that can be expanded or retooled to suit your needs.

### Remote Communication

Communication with uses Open Sound Control communicated via UDP. Internal to Max this is implemented using the udpsend and udpreceive objects. By default the oscar extension does not have remote communication enabled.

To enable this remote communication you must set it by creating a configuration file in the `max-test/misc` folder called `testpackage-config.json` -- the contents of which should look like this:

      {
        "port-send" : 4792,
        "port-listen" : 4791
      }
	
The ports may be changed to suit your needs. Any changes in the ports used by oscar will also require that you change the Ruby script to use the same ports for communication.

### Running the Ruby Script

Having configured Max to enable remote communication, now start a Terminal/Console session and cd into the `max-test/ruby` folder. Now you can run the script by typing `ruby test.rb` to get some simple assistance regarding the arguments.

To actually start the tests you will need to provide the path to your Max application folder, e.g.
`ruby test.rb "/Applications/Max 6.1"`

The process may take a few minutes. First Max is launched. Then two-way communication is established. Now we must wait for Max's file database to complete so that all of the tests in the searchpath can be found. Finally, the tests will begin running. When the tests are done, the results will be summarized on the console.


## Automated Test Results

The results of automated tests are stored in a SQLite database. The database is located in the `max-test` folder and named according to the path to the Max application used to run the tests. You can use any SQLite client application to read the results. On the Mac you can also use the built-in `sqlite3` program in the Terminal.

The Ruby script provides a summary of the test results by reading the database.
