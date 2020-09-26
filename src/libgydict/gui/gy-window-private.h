/* gy-window-private.h
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

#pragma once

#include <gtk/gtk.h>
#include <dazzle.h>
#include <libpeas/peas.h>

#include "gy-window.h"
#include "gy-search-bar.h"
#include "gy-def-list.h"
#include "gy-text-view.h"
#include "gy-text-buffer.h"
#include "gy-greeting-view.h"
#include "services/gy-dict-service.h"
#include "services/gy-service-provider.h"

G_BEGIN_DECLS

struct _GyWindow
{
  DzlApplicationWindow  __parent__;

  DzlDockBin           *dockbin;
  GyDefList            *deflist;
  GtkTreeSelection     *selection;
  GyTextView           *textview;
  GyTextBuffer         *buffer;
  GtkSearchEntry       *search_entry;
  GtkHeaderBar         *header_bar;
  GySearchBar          *search_bar;
  PeasExtensionSet     *extens;
  GtkClipboard         *clipboard; /* Non free! */
  DzlMenuManager       *menu_manager;
  GtkWidget            *main_view_stack;

  const gchar       *service_id;
  GyServiceProvider *service_provider;
};


void _gy_window_plugins_init_extens (GyWindow *self);
void _gy_window_actions_init (GyWindow *self);
void _gy_window_settings_register (GtkWindow *window);

G_END_DECLS
