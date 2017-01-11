/* gy-workspace.c
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

#include "gy-workspace.h"

#define DEFAULT_POSITION 200

struct _GyWorkspace
{
  PnlDockOverlay __parent__;
  PnlDockBin     *dockbin;
};

G_DEFINE_TYPE (GyWorkspace, gy_workspace, PNL_TYPE_DOCK_OVERLAY)

static void
gy_workspace_finalize (GObject *object)
{
  G_OBJECT_CLASS (gy_workspace_parent_class)->finalize (object);
}

static void
gy_workspace_class_init (GyWorkspaceClass *klass)
{
  GObjectClass   *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  object_class->finalize = gy_workspace_finalize;

  gtk_widget_class_set_template_from_resource (widget_class , "/org/gtk/gydict/gy-workspace.ui");
  gtk_widget_class_bind_template_child (widget_class, GyWorkspace, dockbin);
}

static void
gy_workspace_init (GyWorkspace *self)
{
  PnlDockRevealer *edge;

  gtk_widget_init_template (GTK_WIDGET (self));

  edge = PNL_DOCK_REVEALER (pnl_dock_bin_get_left_edge (PNL_DOCK_BIN (self->dockbin)));
  pnl_dock_revealer_set_position (edge, DEFAULT_POSITION);
  pnl_dock_revealer_set_reveal_child (edge, TRUE);
}


void
gy_workspace_attach_action (GyWorkspace *self,
                            GyWindow    *win)
{
  GActionGroup *group;

  g_return_if_fail (GY_IS_WORKSPACE (self));
  g_return_if_fail (GY_IS_WINDOW    (win));

  group = gtk_widget_get_action_group (GTK_WIDGET (self->dockbin), "dockbin");
  gtk_widget_insert_action_group (GTK_WIDGET (win), "dockbin", group);
}
