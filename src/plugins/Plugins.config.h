/* ---------------------------------------------------------------------------------------------------------------------------------
 * User plugins for OnStepX
 *
 * For each entery below one must specify the class instance name and #include the class header file.
 *
 * Each plugin should have a directory that contains all of its files, which gets dropped into to the /src/plugins directory.
 * The plugin main class instance name should match the directory name.
 * The plugin main class must have a "void init();" method for OnStepX to call when it starts up.
 * 
 * ---------------------------------------------------------------------------------------------------------------------------------
*/

// =================================================================================================================================

#define PLUGIN1                   website //    OFF, Specify the class instance (same as plugin directory name) to enable.    Option
#include "website/Website.h"              //         Specify the header file to include the class.

// *** Comment out PLUGIN1 above AND uncomment the following two lines to enable the sample plugin ***
//#define PLUGIN1                    sample //    OFF, Specify the class instance (same as plugin directory name) to enable.    Option
//#include "sample/Sample.h"                //         Specify the header file to include the class.

#define PLUGIN2                       OFF //    OFF, Specify the class instance (same as plugin directory name) to enable.    Option
//#include "plugin2/Name.h"               //         Specify the header file to include the class.

#define PLUGIN3                       OFF //    OFF, Specify the class instance (same as plugin directory name) to enable.    Option
//#include "plugin3/Name.h"               //         Specify the header file to include the class.

#define PLUGIN4                       OFF //    OFF, Specify the class instance (same as plugin directory name) to enable.    Option
//#include "plugin4/Name.h"               //         Specify the header file to include the class.

#define PLUGIN5                       OFF //    OFF, Specify the class instance (same as plugin directory name) to enable.    Option
//#include "plugin5/Name.h"               //         Specify the header file to include the class.

#define PLUGIN6                       OFF //    OFF, Specify the class instance (same as plugin directory name) to enable.    Option
//#include "plugin6/Name.h"               //         Specify the header file to include the class.

#define PLUGIN7                       OFF //    OFF, Specify the class instance (same as plugin directory name) to enable.    Option
//#include "plugin7/Name.h"               //         Specify the header file to include the class.

#define PLUGIN8                       OFF //    OFF, Specify the class instance (same as plugin directory name) to enable.    Option
//#include "plugin8/Name.h"               //         Specify the header file to include the class.
