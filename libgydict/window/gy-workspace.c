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
#include "dictionaries/gy-dict.h"
#include "dictionaries/gy-dict-manager.h"
#include "entrylist/gy-tree-view.h"
#include "entryview/gy-text-view.h"

#define DEFAULT_POSITION 200

struct _GyWorkspace
{
  PnlDockOverlay      __parent__;
  PnlDockBin         *dockbin;
  GyTreeView         *treeview;
  GyTextView         *textview;
  GyDictManager      *manager;
  GSimpleActionGroup *actions;
};

enum
{
  PROP_0,
  PROP_MANAGER,
  PROP_LEFT_WIDGET,
  PROP_RIGHT_WIDGET,
  N_PROPS
};
static GParamSpec *properties[N_PROPS];

G_DEFINE_TYPE (GyWorkspace, gy_workspace, PNL_TYPE_DOCK_OVERLAY)

static void
gy_workspace_action_alter_dict (GSimpleAction *action,
                                GVariant      *parameter,
                                gpointer       data)
{
  GyWorkspace *self = (GyWorkspace *) data;
  GyDict      *dict = NULL;
  const gchar *str;

  str = g_variant_get_string (parameter, NULL);

  dict = gy_dict_manager_set_dict (self->manager, str,
                                   gtk_text_view_get_buffer (GTK_TEXT_VIEW (self->textview)));
  if (!dict) return;

  gtk_tree_view_set_model (GTK_TREE_VIEW (self->treeview),
                           gy_dict_get_tree_model (dict));

  g_action_change_state (G_ACTION (action), parameter);
}

static void
gy_workspace_set_property (GObject      *object,
                           guint         prop_id,
                           const GValue *value,
                           GParamSpec   *pspec)
{
  GyWorkspace *self = GY_WORKSPACE (object);

  switch (prop_id)
    {
    case PROP_MANAGER:
      self->manager = g_value_dup_object (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gy_workspace_get_property (GObject    *object,
                           guint       prop_id,
                           GValue     *value,
                           GParamSpec *pspec)
{
  GyWorkspace *self = GY_WORKSPACE (object);

  switch (prop_id)
    {
    case PROP_MANAGER:
      g_value_set_object (value, self->manager);
      break;
    case PROP_LEFT_WIDGET:
      g_value_take_object (value, self->treeview);
      break;
    case PROP_RIGHT_WIDGET:
      g_value_take_object (value, self->textview);
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gy_workspace_destroy (GtkWidget *widget)
{
  GyWorkspace *self = GY_WORKSPACE (widget);

  g_clear_object (&self->actions);
  g_clear_object (&self->manager);

  GTK_WIDGET_CLASS (gy_workspace_parent_class)->destroy (widget);
}

static void
gy_workspace_class_init (GyWorkspaceClass *klass)
{
  GObjectClass   *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  object_class->set_property = gy_workspace_set_property;
  object_class->get_property = gy_workspace_get_property;

  widget_class->destroy = gy_workspace_destroy;

  gtk_widget_class_set_template_from_resource (widget_class , "/org/gtk/gydict/gy-workspace.ui");
  gtk_widget_class_bind_template_child (widget_class, GyWorkspace, dockbin);
  gtk_widget_class_bind_template_child (widget_class, GyWorkspace, treeview);
  gtk_widget_class_bind_template_child (widget_class, GyWorkspace, textview);

  properties[PROP_MANAGER] =
    g_param_spec_object ("manager-dicts",
                         "manager-dicts",
                         "The manager of dictionaries.",
                         GY_TYPE_DICT_MANAGER,
                         G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);
  properties[PROP_LEFT_WIDGET] =
    g_param_spec_object ("left-widget",
                         "left-widget",
                         "The left widget of the workspace.",
                         GY_TYPE_TREE_VIEW,
                         G_PARAM_READABLE | G_PARAM_STATIC_STRINGS);
  properties[PROP_RIGHT_WIDGET] =
    g_param_spec_object ("right-widget",
                         "right-widget",
                         "The right widget of the workspace.",
                         GY_TYPE_TEXT_VIEW,
                         G_PARAM_READABLE | G_PARAM_STATIC_STRINGS);
  g_object_class_install_properties (object_class, N_PROPS, properties);
}

static void
gy_workspace_init (GyWorkspace *self)
{
  PnlDockRevealer *edge;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
  static const GActionEntry entries[] = {
      {"alter-dict", gy_workspace_action_alter_dict, "s", "''", NULL}
    };
#pragma GCC diagnostic pop

  gtk_widget_init_template (GTK_WIDGET (self));

  self->actions = g_simple_action_group_new ();
  g_action_map_add_action_entries (G_ACTION_MAP (self->actions),
                                   entries, G_N_ELEMENTS (entries), self);
  gtk_widget_insert_action_group (GTK_WIDGET (self), "workspace",
                                  G_ACTION_GROUP (self->actions));

  edge = PNL_DOCK_REVEALER (pnl_dock_bin_get_left_edge (PNL_DOCK_BIN (self->dockbin)));
  pnl_dock_revealer_set_position (edge, DEFAULT_POSITION);
  pnl_dock_revealer_set_reveal_child (edge, TRUE);

  g_object_set_data (G_OBJECT (self->textview), "treeview", self->treeview);
  g_object_set_data (G_OBJECT (self->treeview), "treeview", self->treeview);

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

  gtk_widget_insert_action_group (GTK_WIDGET (win), "workspace",
                                  G_ACTION_GROUP (self->actions));
}
