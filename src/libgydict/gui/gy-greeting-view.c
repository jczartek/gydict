/* gy-greeting-view.c
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

#include "gy-greeting-view.h"
#include "gy-greeting-presenter.h"

struct _GyGreetingView
{
  GtkBin parent_instance;

  /* Priv fields */
  GyGreetingPresenter *presenter;

  /*UI*/
  GtkLabel *number_dictionaries;
};

G_DEFINE_TYPE (GyGreetingView, gy_greeting_view, GTK_TYPE_BIN)

static void
gy_greeting_view_dispose (GObject *object)
{
  GyGreetingView *self = (GyGreetingView *)object;

  if (self->presenter != NULL)
    g_clear_object (&self->presenter);

  G_OBJECT_CLASS (gy_greeting_view_parent_class)->finalize (object);
}

static void
gy_greeting_view_constructed (GObject *object)
{
  GyGreetingView *self = (GyGreetingView *)object;

  G_OBJECT_CLASS (gy_greeting_view_parent_class)->constructed (object);

  _gy_greeting_presenter_load_data (self->presenter);
}

static void
gy_greeting_view_class_init (GyGreetingViewClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  object_class->constructed = gy_greeting_view_constructed;
  object_class->dispose = gy_greeting_view_dispose;

  gtk_widget_class_set_template_from_resource (widget_class, "/org/gtk/gydict/gy-greeting-view.ui");
  gtk_widget_class_bind_template_child (widget_class, GyGreetingView, number_dictionaries);
}

static void
gy_greeting_view_init (GyGreetingView *self)
{
  gtk_widget_init_template ((GtkWidget *) self);

  self->presenter = gy_greeting_presenter_new (self);
}

void
_gy_greeting_view_set_number_of_dictionaries (GyGreetingView *self, guint number_dictionaries)
{
  g_return_if_fail (GY_IS_GREETING_VIEW (self));

  g_autofree gchar *message = g_strdup_printf ("Number of loaded plugin-dictionaries: %d", number_dictionaries);
  gtk_label_set_label (self->number_dictionaries, message);
}
