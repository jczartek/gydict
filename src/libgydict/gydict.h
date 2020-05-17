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
#include "helpers/gy-utility-func.h"
#include "helpers/gy-text-attribute.h"
#include "helpers/gy-format-scheme.h"
#include "preferences/gy-prefs-view.h"
#include "preferences/gy-prefs-view-addin.h"
#include "preferences/gy-prefs-window.h"
#include "services/gy-dict-service.h"
#include "services/gy-dict-formatter.h"
#include "services/gy-service.h"
#include "services/gy-service-provider.h"
#include "gui/gy-header-bar.h"
#include "gui/gy-window.h"
#include "gui/gy-window-addin.h"
#include "gy-enum-types.h"

#undef GYDICT_INSIDE

G_END_DECLS

#endif /* __GYDICT_H__ */
