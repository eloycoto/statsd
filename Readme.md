Kamailio mod statsd
===================

This project is the statsd module for Kamailio. With this module you can send info to statsd/graphite.

Install
--------

Fetch kamailio code:

    git clone --depth 1 --no-single-branch git://git.sip-router.org/kamailio kamailio
    cd kamailio
    git checkout -b 4.2 origin/4.2

Add the module:

    git submodule add https://github.com/eloycoto/statsd modules/statsd
    git submodule update

Compile the module:

    make include_modules="statsd" cfg
    make

And now you can follow the kamailio instructions. http://www.kamailio.org/wiki/install/4.2.x/git

Module parameters
------------------

You can find the module parameters in the [Doxygen docs](doc/statsd.txt)

[I wrote this small post, with some info, how to use and ideas. ](http://acalustra.com/kamailio-statsd-better-statistics-in-your-voip-platform.html) enjoy!
