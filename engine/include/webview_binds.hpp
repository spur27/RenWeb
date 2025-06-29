#include "webview.hpp"
#include <nlohmann/json.hpp>
#include <boost/process.hpp>

using json = nlohmann::json;

namespace Vanitas {
    namespace Bindings {
        namespace Log {
            void bind_logTrace(Vanitas::Webview*);
            void bind_logDebug(Vanitas::Webview*);
            void bind_logInfo(Vanitas::Webview*);
            void bind_logWarn(Vanitas::Webview*);
            void bind_logError(Vanitas::Webview*);
            void bind_logCritical(Vanitas::Webview*);
        }
        namespace Filesystem {
            void bind_readFile(Vanitas::Webview*);
            // void bind_writeFile(Vanitas::Webview*);
            void bind_exists(Vanitas::Webview*);
            void bind_mkDir(Vanitas::Webview*);
            void bind_rmDir(Vanitas::Webview*);
            void bind_ls(Vanitas::Webview*);
            void bind_rename(Vanitas::Webview*);
            void bind_move(Vanitas::Webview*);
            void bind_copyFile(Vanitas::Webview*);
        }
        namespace Webview {
            void bind_saveSettings(Vanitas::Webview*);
            // void bind_setSettings(Vanitas::Webview*);
            void bind_readSettings(Vanitas::Webview*);
            void bind_setHTML(Vanitas::Webview*);
            void bind_setHTMLToDoc(Vanitas::Webview*);
            void bind_setSize(Vanitas::Webview*);
            void bind_setTitle(Vanitas::Webview*);
            void bind_terminate(Vanitas::Webview*);
            void bind_openWindow(Vanitas::Webview*);
        }
        namespace Util {
            void bind_getPID(Vanitas::Webview*);
            void bind_getApplicationDirPath(Vanitas::Webview*);
            void bind_getHTMLDocName(Vanitas::Webview*);
        }
    }
}