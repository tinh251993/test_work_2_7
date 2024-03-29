#ifndef CHROME_BROWSER_CRM_PANELWEBVIEW_H_
#define CHROME_BROWSER_CRM_PANELWEBVIEW_H_


#include "base/memory/raw_ptr.h"
#include "components/prefs/pref_change_registrar.h"
#include "ui/base/metadata/metadata_header_macros.h"
#include "ui/views/accessible_pane_view.h"
#include "ui/views/controls/resize_area_delegate.h"
#include "ui/views/view_observer.h"
class BrowserView;


class PanelWebview : public views::AccessiblePaneView,
                  public views::ViewObserver,
                  public views::ResizeAreaDelegate {
 public:
  // Determines the side from which the side panel will appear.
  // LTR / RTL conversions are handled in
  // BrowserViewLayout::LayoutSidePanelView. As such, left will always be on the
  // left side of the browser regardless of LTR / RTL mode.
  enum HorizontalAlignment { kAlignLeft = 0, kAlignRight };

  METADATA_HEADER(PanelWebview);
  explicit PanelWebview(BrowserView* browser_view,
                     HorizontalAlignment horizontal_alignment =
                         HorizontalAlignment::kAlignRight);
  PanelWebview(const PanelWebview&) = delete;
  PanelWebview& operator=(const PanelWebview&) = delete;
  ~PanelWebview() override;

  void PanelWebviewWidth(int width);
  void SetHorizontalAlignment(HorizontalAlignment alignment);
  HorizontalAlignment GetHorizontalAlignment();
  bool IsRightAligned();
  gfx::Size GetMinimumSize() const override;

  // views::ResizeAreaDelegate:
  void OnResize(int resize_amount, bool done_resizing) override;

  // Log UMA data for the side panel resize feature. Will only log if the side
  // panel has been resized since metrics were last logged.
  void RecordMetricsIfResized();
  void SetPanelWidth(int width);
  bool IsPanelShow();
 private:
  void UpdateVisibility();

  // views::View:
  void ChildVisibilityChanged(View* child) override;

  // views::ViewObserver:
  void OnChildViewAdded(View* observed_view, View* child) override;
  void OnChildViewRemoved(View* observed_view, View* child) override;

  const raw_ptr<View> border_view_;
  const raw_ptr<BrowserView> browser_view_;
  const raw_ptr<View> resize_area_;

  // -1 if a side panel resize is not in progress, otherwise the width of the
  // side panel when the current resize was initiated.
  int starting_width_on_resize_ = -1;

  // Should be true if the side panel was resized since metrics were last
  // logged.
  bool did_resize_ = false;

  // Keeps track of the side the side panel will appear on (left or right).
  HorizontalAlignment horizontal_alignment_;

  // Observes and listens to side panel alignment changes.
  PrefChangeRegistrar pref_change_registrar_;
  bool is_side_panel_show = false;
};


#endif  // CHROME_BROWSER_CRM_PANELWEBVIEW_H_