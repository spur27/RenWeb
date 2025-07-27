#ifndef RENWEB_WEBVIEW_BINDS_H
#define RENWEB_WEBVIEW_BINDS_H
#include "window.hpp"
#include <nlohmann/json.hpp>
#include <boost/process.hpp>

using json = nlohmann::json;

namespace RenWeb {
    namespace Bindings {
        namespace Log {
            void bind_logTrace(RenWeb::Window*);
            void bind_logDebug(RenWeb::Window*);
            void bind_logInfo(RenWeb::Window*);
            void bind_logWarn(RenWeb::Window*);
            void bind_logError(RenWeb::Window*);
            void bind_logCritical(RenWeb::Window*);
        }
        namespace Filesystem {
            void bind_readFile(RenWeb::Window*);
            void bind_writeFile(RenWeb::Window*);
            void bind_exists(RenWeb::Window*);
            void bind_isDir(RenWeb::Window*);
            void bind_mkDir(RenWeb::Window*);
            void bind_rm(RenWeb::Window*);
            void bind_ls(RenWeb::Window*);
            void bind_rename(RenWeb::Window*);
            void bind_copy(RenWeb::Window*);
            void bind_chooseFiles(RenWeb::Window*);
        }
        namespace Webview {
            void bind_saveSettings(RenWeb::Window*);
            void bind_refreshSettings(RenWeb::Window*);
            void bind_reloadPage(RenWeb::Window*);
            void bind_setSettings(RenWeb::Window*);
            void bind_getSettings(RenWeb::Window*);
            void bind_resetSettingsToDefaults(RenWeb::Window*);
            void bind_updateSize(RenWeb::Window*);
            void bind_close(RenWeb::Window*);
            void bind_openWindow(RenWeb::Window*);
            void bind_minimize(RenWeb::Window*);
            void bind_maximize(RenWeb::Window*);
            void bind_fullscreen(RenWeb::Window*);
            void bind_hide(RenWeb::Window*);
            void bind_show(RenWeb::Window*);
        }
        namespace Util {
            void bind_getPID(RenWeb::Window*);
            void bind_getApplicationDirPath(RenWeb::Window*);
        }
        void bind_all(RenWeb::Window*);
    }
}
#endif