# Lecture 12-3 sockets and netgrids

This lecture will just be a fun overview of programming using network sockets.
There are two folders with sample code:

* `sockets`: contains an example of an "echo server" and "echo client". There
are two types of servers, one single-threaded and one multi-threaded.

* `netgrid`: this is a network game version of my `Turtle` and `Grid` solution
to Homeworks 09 and 10. It relies on code similar to the echo example just
above, but allows clients to connect as a turtle to a grid server, and then walk
around and do stuff.

I'll talk about both examples in lecture. But you may also want to
read the **NetGrid** section below so you can play along during lecture.

### Logistics

Here is a list of CS2 deadlines for myself and for you:

• Friday, May 1st, 3-4pm, last lecture!  
• Tuesday, May 6th, 3pm, midterm returned  
• Friday, May 8th, 3pm, feedback on Homework 9, 10  
• Monday, May 11th, 3pm, Homework 11 due  
• Monday, May 11th, 3-5pm, Review session  
• Tuesday, May 12th, 6-9pm, final exam official time    
• Tuesday, May 12th, 3pm, final exam available for download  
• Wednesday, May 13th, 3pm, final exam must be submitted  
• Thursday, May 14th, 3pm, Homework 11 and all late work due   

I have included PDFs of two sample final exams in this repo. Feel free
to use them to study for the exam.

### NetGrid

If you download this repo onto your computer, you should be able to
compile and run the `turtle` program under `netgrid`.  This will 
allow you to play a networked game that's being hosted by the Amazon Web
Services cloud and a "Elastic Cloud Instance," basically a machine
running Linux.

You'll want to:
  
    cd netgrid
    make turtle

And then run the turtle program with a line like

    ./turtle 3.21.148.148 8001

This will be running a copy of my `netgrid` program (also in that folder)
on a machine that listens for connections on that *IP address* (that four
number code) and at that *port number*. This will allow you to create
a turtle in an 8x18 grid and enter commands like

    forward
    left
    right
    build
    clear
    who
    text TURTLE-NAME MESSAGE
    quit

The grid starts with all prizes and no walls. You can collect prizes to
spend on building walls in unoccupied points in front of you. You can 
also spend money to clear a wall in front of you, and that places a
prize on that point in the grid. 

Feel free to join in on the demo.

**NOTE:** the IP address I've listed might change by the time of lecture,
but that is the current instance that is running at the moment.

You can also run a `netgrid` server on your own machine, as a server:

    make netgrid
    ./netgrid PORT-NUMBER

using a port number of 8000, 8001, etc.  Then you can run turtle clients
in a different console by typing

    ./turtle localhost PORT-NUMBER

with that same port number.
