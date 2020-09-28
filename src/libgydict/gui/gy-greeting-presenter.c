/* gy-greeting-presenter.c
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

#include <libpeas/peas.h>
#include "gy-greeting-presenter.h"
#include "../helpers/gy-plugins.h"

struct _GyGreetingPresenter
{
  GObject parent_instance;

  GyGreetingView *view;
  PeasEngine *engine;
};

G_DEFINE_TYPE (GyGreetingPresenter, gy_greeting_presenter, G_TYPE_OBJECT)

static void
gy_greeting_presenter_finalize (GObject *object)
{
  GyGreetingPresenter *self = (GyGreetingPresenter *)object;

  if (self->view == NULL) g_message ("adsfadsfasdfsadfasdfsadfasdfd");
  g_object_remove_weak_pointer ((GObject *)self->view, (gpointer *)&self->view);
  self->view = NULL;
  G_OBJECT_CLASS (gy_greeting_presenter_parent_class)->finalize (object);
}

static void
gy_greeting_presenter_class_init (GyGreetingPresenterClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = gy_greeting_presenter_finalize;
}

void
gy_greeting_presenter_load_plugin (PeasEngine          *engine,
                                   PeasPluginInfo      *info,
                                   GyGreetingPresenter *self)
{
  _gy_greeting_presenter_load_data (self);
}

void
gy_greeting_presenter_unload_plugin (PeasEngine          *engine,
                                     PeasPluginInfo      *info,
                                     GyGreetingPresenter *self)
{
  _gy_greeting_presenter_load_data (self);
}

static void
gy_greeting_presenter_init (GyGreetingPresenter *self)
{
  self->engine = peas_engine_get_default ();

  g_signal_connect_after (self->engine, "load-plugin",
                          G_CALLBACK (gy_greeting_presenter_load_plugin), self);
  g_signal_connect_after (self->engine, "unload-plugin",
                          G_CALLBACK (gy_greeting_presenter_unload_plugin), self);
}

GyGreetingPresenter *
gy_greeting_presenter_new (GyGreetingView *view)
{
  g_return_val_if_fail (GY_IS_GREETING_VIEW (view), NULL);

  GyGreetingPresenter *self = g_object_new (GY_TYPE_GREETING_PRESENTER, NULL);

  self->view = view;
  g_object_add_weak_pointer ((GObject *)self->view, (gpointer *)&self->view);

  return self;
}

void
_gy_greeting_presenter_load_data (GyGreetingPresenter *self)
{
  g_return_if_fail (GY_IS_GREETING_PRESENTER (self));

  guint number_dicts = gy_plugins_get_number_loaded_plugins ();
  _gy_greeting_view_set_number_of_dictionaries (self->view, number_dicts);
}
