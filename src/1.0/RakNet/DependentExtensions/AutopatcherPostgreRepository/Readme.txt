The autopatcher server and repository requires that you have PostgreSQL 8.1 or later installed and located at
C:\Program Files\PostgreSQL\8.1\include

If you have it installed but not located at that path then change the path in the Project properties page
C/C++ / General / Additional Include Directories
and under
Build Events / Post Build Events
copy "C:\Program Files\PostgreSQL\8.1\bin\*.dll" .\Debug
to whatever your path is.

If you don't have it installed you can either implement your own repository interface (See AutopatcherRepositoryInterface.h )
or you can use the DirectoryDeltaTransfer instead, which is a simpler version of an Autopatcher.

Requires _RAKNET_THREADSAFE defined for now.