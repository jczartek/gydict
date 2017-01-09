/* gy-workspace.h
 *
 * Copyright (C) 2017 Jakub Czartek <kuba@linux.pl>
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
#ifndef __GY_WORKSPACE_H__
#define __GY_WORKSPACE_H__

#include <pnl.h>
#include "gy-window.h"

G_BEGIN_DECLS

#define GY_TYPE_WORKSPACE (gy_workspace_get_type())

G_DECLARE_FINAL_TYPE (GyWorkspace, gy_workspace, GY, WORKSPACE, PnlDockOverlay)

void gy_workspace_attach_action (GyWorkspace *self,
                                 GyWindow    *win);

G_END_DECLS

#endif /* __GY_WORKSPACE_H__ */

