.. _scenario_tutorial:

Scenario Tutorial
=================

This section describes how to get a map ready for scripting addition and how
to write simple Lua scripts. It goes trough all the concepts required to get
you started writing cool widelands scenarios and campaigns. 

Designing the Map
-----------------

Wideland's map files are plain directories containing various different files
that describe a map. Normally, widelands saves those files in one zip archive
file with the file extension ``.wmf``. To add scripting capabilities to maps,
we have to create new text files in the ``scripting/`` directory of the map,
therefore we want to have the map as a plain directory, so that we can 
easily add new scripting files. There are two ways to achieve this:

   1. Set the nozip option in the advanced options inside of Widelands.
      Widelands will now save maps (and savegames) as plain directories.
   2. Manually unpack the zip file. To do this, do the following:

      1. Rename the file: ``map.wmf``-> ``map.zip``
      2. Unpack this zipfile, a folder ``map.wmf`` will be created.

Now create a new directory called ``scripting/`` inside the map folder.

Hello World
-----------

The language widelands is using for scripting is called `Lua`_. It is a simple
scripting language that is reasonable fast and easy to embed in host
applications.  We will now learn how to start the map as a scenario and how to
add a simple Lua script to it.  For this create a new text file and write the
following inside:

.. code-block:: lua
   
   print "###############################"
   print "Hello World"
   print "###############################"

Save this file inside the maps directory as ``scripting/init.lua``.

.. note::

   When we talk about text files, we mean the very basic representation of
   characters that a computer can understand. That means that any fancy word
   processor (Word, OpenOffice and their like) will likely produce the wrong
   file format when saving. Make sure to use a plain text editor (like Notepad
   under Windows, nedit under Linux and TextEdit under Mac OS X). If you have
   programmed before, you likely already have found your favorite editor....

Now we try to start this scenario. We can either directly select the map when
starting a new single player game and mark the scenario box or we manually
tell widelands to open our map as a scenario directly from the console.
This is very convenient if you need to test drive the same map over and over
again: Open up a command line (Terminal under Mac OS X/Linux, Cmd under
Windows) and ``cd`` into the widelands directory.  Now start up widelands and
add the scenario switch like so:

.. code-block:: sh

   ./widelands --scenario=path/to/map/mapname.wmf

Widelands should start up and immediately load a map. Look at it's output on
the cmdline (Under Windows, you might have to look at ``stdout.txt``) and you
should see our text been printed::

   ###############################"
   Hello World"
   ###############################"

So what we learned is that widelands will run the script
``scripting/init.lua`` as soon as the map is finished loading. This script is
the entry point for all scripting inside of widelands. 

.. _`Lua`: http://www.lua.org/ 

A Lua Primer
------------

This section is intentionally cut short. There are excellent tutorials in
`Luas Wiki`_ and there is a complete book free online: `Programming in Lua`_.
You should definitively start there to learn Lua. 

This section only contains the parts of Lua that I found bewildering and it
also defines a few conventions that are used in this documentation. 

.. _`Luas Wiki`: http://lua-users.org/wiki/TutorialDirectory
.. _`Programming in Lua`: http://www.lua.org/pil/


Data Types
^^^^^^^^^^

Lua only has one fundamental data type called :class:`table`. A Table is what
Python calls a dictionary and other languages call a Hashmap. It basically
contains a set of Key-Value combinations. There are two ways to access the
values in the table, either by using the ``d[key]`` syntax or by using the
``d.key`` syntax.  For the later, ``key`` must be a string:

.. code-block:: lua

   d = { 
      value_a = 23,
      b  = 34,
      90 = "Hallo"
   }

   d.value_a -- is 23
   d['value_a'] -- the same
   d['b'] -- the same as d.b

   d[90]  -- is "Hallo"

   b.90 -- this is illegal

Tables that are indexed with integers starting from 1 are called
:class:`arrays` throughout the documentation. Lua also accepts them as
something special, for example it can determine their length via the #
operator and they can be specially created:

.. code-block:: lua

   a = { [1] = "Hi", [2] = "World }
   b = { "Hi", "World" }
   -- a and b have the same content

   print(#a) -- will print 2

Calling conventions
^^^^^^^^^^^^^^^^^^^

Calling a function is Lua is straight forward, the only thing that comes as a
surprise for most programmers is that Lua throws values away without notice.

.. code-block:: lua
   
   function f(a1, a2, a3) print("Hello World:", a1, a2, a3) end

   f() --- Prints 'Hello World: nil  nil  nil'
   f("a", "house", "blah") --- Prints 'Hello World: a  house  blah'

   f("a", "a", "a", "a", "a") --- Prints 'Hello World: a  a  a'

The same also goes for return values.

.. code-block:: lua

   function f() return 1, 2, 3 end

   a = f()  -- a == 1
   a,b = f() -- a == 1, b == 2
   a,b,c,d = f() -- a == 1, b == 2, c == 3, d == nil

Another thing that comes to a surprise for some developer is the syntactic
sugar that Lua adds to calls. The following rules apply: If a function is
given exactly one argument and this argument is either a :class:`string` or a
:class:`table`, the surrounding parenthesis can be left out. This makes for
something similar to optional arguments. The following two lines are equal

.. code-block:: lua

   some_function{a = "Hi", b = "no"}
   some_function({a = "Hi", b = "no"})

The first one though is often used for functions that take mostly optional
arguments. A second use case is for strings:

.. code-block:: lua

   print "hi"
   print("hi") -- same

We use this in widelands to our advantage to implement internationalization
via a global function called :func:`_` (an long standing gettext paradigm):

.. code-block:: lua

   print _ "Hello Word" -- Will print in German: "Hallo Welt"
   print( _("Hello World")) -- the same in more verbose writing


Coroutines
^^^^^^^^^^

The most important feature of Lua that widelands is using are coroutines. We
use them watered down and very simple, but their power is enormous. In
Widelands use case, a coroutine is simply a function that can interrupt it's
execution and give control back to widelands at any point in time. After
it is awoken again by widelands, it will resume at precisely the same point 
again. Let's dive into an example right away:

.. code-block:: lua

   use("aux", "coroutine")

   function print_a_word(word)
      while 1 do
         print(word)
         sleep(1000)
      end
   end

   run(print_a_word, "Hello World!")

If you put this code into our ``init.lua`` file from the earlier example, you
will see "Hello World!" begin printed every second on the console. Let's
digest this example. The first line imports the ``coroutine.lua`` script from
the auxiliary Lua library that comes bundled with widelands. We use two
functions from this in the rest of the code, namly :func:`sleep` and
:func:`run`. 

Then we define a simple function :func:`print_a_word` that takes one argument
and enters an infinite loop: it prints the argument, then sleeps for a second.
The :func:`sleep` function puts the coroutine to sleep and tells widelands to
wake the coroutine up again after 1000 ms have passed. The coroutine will then
continue its execution directly after the sleep call, that is it will enter
the loop's body again. 

All we need now is to get this function started and this is done via the
:func:`run` function: it takes as first argument a function and then any
number of arguments that will be passed on to the given function. The
:func:`run` will construct a coroutine and hand it over to widelands for
periodic execution.

These are all of the essential tools we need to write cool scenario scripts
for widelands.

.. note::

   Keep in mind that widelands won't do anything else while you're
   coroutine is running, so if you plan to do long running tasks consider
   adding some calls to :func:`sleep` here and there so that widelands can act
   and update the user interface.

Let's consider a final example on how coroutines can interact with each other.

.. code-block:: lua

   use("aux", "coroutine")

   function print_a()
      while 1 do
         print(a)
         sleep(1000)
      end
   end

   function change_a()
      while 1 do
         if a == "Hello" then
            a = "World"
         else
            a = "Hello"
         end
         sleep(1333)
      end
   end

   a = "Hello"
   run(print_a)
   run(change_a)

The first coroutine will print out the current value of a, the second changes
the value of the variable a asynchronously. So we see in this example that
coroutines share the same environment and can therefore use global variables
to communicate with each other.  

.. vim:ft=rst:spelllang=en:spell
