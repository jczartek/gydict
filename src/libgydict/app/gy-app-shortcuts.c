/* gy-app-shortcuts.c
 *
 * Copyright 2020 Jakub Czartek <kuba@linux.pl>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "config.h"
#include <glib/gi18n-lib.h>
#include "gy-app.h"
#include "gy-app-private.h"

#define I_(s) (g_intern_static_string(s))

void
_gy_app_init_shortcuts (GyApp *self)
{
  DzlShortcutManager *manager;
  DzlShortcutTheme *theme;

  manager = dzl_application_get_shortcut_manager (DZL_APPLICATION (self));
  theme = dzl_shortcut_manager_get_theme_by_name (manager, "internal");

  dzl_shortcut_manager_add_action (manager,
                                   I_("app.prefs"),
                                   N_("General shortcuts"),
                                   N_("Preferences"),
                                   N_("Show the preferences window"),
                                   NULL);
  dzl_shortcut_theme_set_accel_for_action (theme,
                                           "app.prefs",
                                           "<Primary>comma",
                                           DZL_SHORTCUT_PHASE_GLOBAL);
}
