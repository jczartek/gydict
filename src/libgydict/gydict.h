/* gydict.h
 *
 * Copyright (C) 2016 Jakub Czartek <kuba@linux.pl>
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
 */

#ifndef __GYDICT_H__
#define __GYDICT_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define GYDICT_INSIDE

#include "app/gy-app.h"
#include "app/gy-app-addin.h"
#include "deflist/gy-def-list.h"
#include "dictionaries/gy-depl.h"
#include "dictionaries/gy-dict.h"
#include "dictionaries/gy-dict-manager.h"
#include "dictionaries/gy-dict-manager-addin.h"
#include "dictionaries/gy-english-pwn.h"
#include "dictionaries/gy-german-pwn.h"
#include "dictionaries/gy-parsable.h"
#include "dictionaries/gy-pwn-dict.h"
#include "dictionaries/gy-pwntabs.h"
#include "entryview/gy-text-buffer.h"
#include "entryview/gy-text-view.h"
#include "helpers/gy-utility-func.h"
#include "preferences/gy-prefs-view.h"
#include "preferences/gy-prefs-view-addin.h"
#include "preferences/gy-prefs-window.h"
#include "printing/gy-print.h"
#include "printing/gy-print-compositor.h"
#include "search/gy-search-bar.h"
#include "window/gy-header-bar.h"
#include "window/gy-window.h"
#include "window/gy-window-addin.h"

#undef GYDICT_INSIDE

G_END_DECLS

#endif /* __GYDICT_H__ */
