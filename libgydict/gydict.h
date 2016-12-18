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
#include "resources/gy-resources.h"
#include "window/gy-header-bar.h"
#include "window/gy-lex-search-box.h"

#undef GYDICT_INSIDE

G_END_DECLS

#endif /* __GYDICT_H__ */
