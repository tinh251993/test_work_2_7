#ifndef CHROME_BROWSER_UI_VIEWS_CRM_UPDATE_REQUEST_VIEW_H_
#define CHROME_BROWSER_UI_VIEWS_CRM_UPDATE_REQUEST_VIEW_H_

#include <memory>
#include "ui/views/bubble/bubble_dialog_delegate_view.h"


class Browser;

class CRMC_UpdateRequestView
{

public:
    class  BrowserRemovalObserver;
    
    static void Show(Browser* browser, std::string version);
    static views::BubbleDialogDelegate* GetInstance();
    CRMC_UpdateRequestView();
    ~CRMC_UpdateRequestView();
    static views::BubbleDialogDelegate* ShowBubble(Browser* browser, std::string version);    
    static bool is_show;
};


#endif  // CHROME_BROWSER_UI_VIEWS_CRM_UPDATE_REQUEST_VIEW_H_