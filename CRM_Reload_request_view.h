#ifndef CHROME_BROWSER_UI_VIEWS_CRM_RELOAD_REQUEST_VIEW_H_
#define CHROME_BROWSER_UI_VIEWS_CRM_RELOAD_REQUEST_VIEW_H_

#include <memory>
#include "ui/views/bubble/bubble_dialog_delegate_view.h"


class Browser;

class CRMC_ReloadRequestView
{

public:
    class  BrowserRemovalObserver;

    static void Show(Browser*);
    static views::BubbleDialogDelegate* GetInstance();
    CRMC_ReloadRequestView();
    ~CRMC_ReloadRequestView();
    static views::BubbleDialogDelegate* ShowBubble(Browser* browser);    
};


#endif  // CHROME_BROWSER_UI_VIEWS_CRM_RELOAD_REQUEST_VIEW_H_