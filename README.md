SAGA Globus Adaptors                              
====================

The Globus adaptors implement the following functionality:

  * SAGA job API on top of Globus GRAM (2 and 5) 
  * SAGA file API on top of Globus GridFTP
  * SAGA replica API on top of Globus RLS (Replica Location Service).


Setup
-----

(Please refer to the INSTALL file for more details)

Assuming the required SAGA C++ Core libraries are installed and the environment 
variable $SAGA_LOCATION is set accordingly, the Condor adaptor can be installed
by typing:

    ./configure
    make
    make install
    
The optional ``--with-globus-location=`` and ``--with-globus-flavor=`` options can 
be used in conjunction with ./configure to point the installer to a sepcific 
Globus installation installation.

Known Problems
--------------
Problem: Your compilation fails with the following error message:

    liblinking   libsaga_adaptor_globus_module_loader_util.so
    ld: skipping incompatible libsaga_core.so when searching for -lsaga_core
    ld: skipping incompatible libsaga_core.a when searching for -lsaga_core
    ld: cannot find -lsaga_core

Solution: Your SAGA Core installation and your globus installation have been compiled
for different architecture (32-bit/64-bit) and it is not possible to link
32-bit and 64-bit libraries together. You either have to re-build Globus 
Toolkit or SAGA Core. In many cluster environments, different 'flavors' of 
Globus are installed - in that case you can try to pick a flavor with 
has the same architecture like your SAGA installation:
 
    ./configure --with-globus-flavor=gcc64dbgpthr (or gcc32dbgpthr)
