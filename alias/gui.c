/**
 * @file
 * Shared code for the Alias and Query Dialogs
 *
 * @authors
 * Copyright (C) 2020 Richard Russon <rich@flatcap.org>
 *
 * @copyright
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 2 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @page alias_gui Shared code for the Alias and Query Dialogs
 *
 * Shared code for the Alias and Query Dialogs
 */

#include "config.h"
#include <stdio.h>
#include "mutt/lib.h"
#include "config/lib.h"
#include "core/lib.h"
#include "gui/lib.h"
#include "gui.h"
#include "lib.h"
#include "menu/lib.h"

/**
 * alias_config_observer - Notification that a Config Variable has changed - Implements ::observer_t
 */
int alias_config_observer(struct NotifyCallback *nc)
{
  if ((nc->event_type != NT_CONFIG) || !nc->global_data || !nc->event_data)
    return -1;

  if (nc->event_subtype == NT_CONFIG_INITIAL_SET)
    return 0;

  struct EventConfig *ev_c = nc->event_data;

  const short c_menu_context = cs_subset_number(NeoMutt->sub, "menu_context");
  const bool c_menu_move_off = cs_subset_bool(NeoMutt->sub, "menu_move_off");
  const bool c_menu_scroll = cs_subset_bool(NeoMutt->sub, "menu_scroll");

  struct Menu *menu = nc->global_data;
  struct MuttWindow *dlg = dialog_find(menu->win);

  char buf[256] = { 0 };
  snprintf(buf, sizeof(buf), "context = %d, move_off = %s, scroll = %s",
           c_menu_context, BoolValues[c_menu_move_off], BoolValues[c_menu_scroll]);
  struct MuttWindow *sbar = window_find_child(dlg, WT_STATUS_BAR);
  sbar_set_title(sbar, buf);

  if (!mutt_str_equal(ev_c->name, "sort_alias"))
    return 0;

  menu_queue_redraw(menu, MENU_REDRAW_FULL);
  mutt_debug(LL_DEBUG5, "config done, request WA_RECALC, MENU_REDRAW_FULL\n");

  return 0;
}

/**
 * alias_set_title - Create a title string for the Menu
 * @param sbar      Simple Bar Window
 * @param menu_name Menu name
 * @param limit     Limit being applied
 */
void alias_set_title(struct MuttWindow *sbar, char *menu_name, char *limit)
{
  if (!limit)
  {
    sbar_set_title(sbar, menu_name);
    return;
  }

  char buf[256] = { 0 };

  int len = snprintf(buf, sizeof(buf), "%s - ", menu_name);

  snprintf(buf + len, sizeof(buf) - len, _("Limit: %s"), limit);

  sbar_set_title(sbar, buf);
}

/**
 * alias_recalc - Recalculate the display of the Alias Window - Implements MuttWindow::recalc()
 */
int alias_recalc(struct MuttWindow *win)
{
  struct Menu *menu = win->wdata;
  struct AliasMenuData *mdata = menu->mdata;

  alias_array_sort(&mdata->ava, mdata->sub);

  win->actions |= WA_REPAINT;
  mutt_debug(LL_DEBUG5, "recalc done, request WA_REPAINT\n");
  return 0;
}
