Java Card Synchronization Framework
===================================

The main goal of this framework is to synchronize Java Card 2 applets through
the exchange of ciphered APDUs.

The current state of the framework is lacking in many ways and requires more
works to be fully functional. However it can serve as a proof of concept  for
the synchronization of applet data in a secure fashion in a pure Java Card 2
setting (i.e.: without tweaking the Java Card VM).

## Setting up the environment

The framework is built using Ant and requires a few environment variables to be
set up:  
- `JAVA_HOME`: a path to a Java Development Kit 1.5 directory. It is recommended to use Java 1.5
  when working with Java Card 2.2.2 
- `JC_HOME`: a path to a Java Card Development Kit 2.2.2 directory.
- `JC_ANT_TASK_HOME`: a path to the Java Card And tasks directory.
- `ANT_HOME`: a path to an Ant directory.
- `ANT_CONTRIB_HOME`: a path an Ant Contrib directory.

Furthermore `$JC_HOME/bin`, `$JAVA_HOME/bin` and `$ANT_HOME/bin` should be added
to your path. Be sure to give execution right to the binaries.

## Building

After setting up the environment, the building process is started by calling
`Ant`. The framework is built and tests are launched to test the framework on
applets manually and automatically tweaked.
