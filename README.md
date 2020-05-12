##  Source
* project clone from: http://upnprenderer.sourceforge.net/

##  Building
* make 
* cd src && ./compile.sh

##  Modify by Shanjin Yang
* src/include/cRenderer.h: #include <string.h>
* add src/compile.sh

## error occur
`../include/cService.h:28:23: fatal error: upnp/upnp.h: No such file or directory`
Please use `sudo apt-get install libupnp-dev` to install library.
