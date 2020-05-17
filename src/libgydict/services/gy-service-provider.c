/* gy-service-provider.c
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

#include "gy-service-provider.h"

struct _GyServiceProvider
{
  GObject parent_instance;

  GSList *services;
};

G_DEFINE_TYPE (GyServiceProvider, gy_service_provider, G_TYPE_OBJECT)

GyServiceProvider *
gy_service_provider_new (void)
{
  return g_object_new (GY_TYPE_SERVICE_PROVIDER, NULL);
}

static void
gy_service_provider_finalize (GObject *object)
{
  GyServiceProvider *self = (GyServiceProvider *)object;

  if (self->services != NULL)
    {
      g_slist_free_full (self->services, g_object_unref);
      self->services = NULL;
    }

  G_OBJECT_CLASS (gy_service_provider_parent_class)->finalize (object);
}

static void
gy_service_provider_class_init (GyServiceProviderClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = gy_service_provider_finalize;
}

static void
gy_service_provider_init (GyServiceProvider *self)
{
  self->services = NULL;
}

static gpointer *
get_service_by_id (GyServiceProvider *self,
                   const gchar       *id_searched_service)
{
  for (GSList *iter = self->services; iter != NULL; iter = g_slist_next (iter))
    {
      if (g_strcmp0 (id_searched_service, gy_service_get_service_id (iter->data)) == 0)
        return iter->data;
    }
  return NULL;
}

static gboolean
service_exist (GyServiceProvider *self,
               GyService     *service)
{
  return get_service_by_id (self, gy_service_get_service_id (service)) != NULL;
}

void
gy_service_provider_register_service (GyServiceProvider *self,
                                      GyService     *service)
{
  g_return_if_fail (GY_IS_SERVICE_PROVIDER (self) &&
                    GY_IS_SERVICE (service));

  if (service_exist (self, service))
    {
      g_critical ("The service [%s] already exists in the service provider. The service can be added. Try to change service_id.",
                  gy_service_get_service_id (service));
      return;
    }

  self->services = g_slist_append (self->services, (gpointer) service);
}

void
gy_service_provider_unregister_service (GyServiceProvider *self,
                                        GyService     *service)
{
  g_return_if_fail (GY_IS_SERVICE_PROVIDER (self) &&
                    GY_IS_SERVICE (service));

  if (service_exist (self, service))
    {
      self->services = g_slist_remove (self->services, service);
    }
}

/**
 * gy_service_provider_get_service_by_id:
 * @self: #GyServiceProvider object
 * @service_id: id of service
 *
 * Returns: (transfer none) (nullable): a service
 **/
GyService *
gy_service_provider_get_service_by_id (GyServiceProvider *self,
                                       const gchar       *service_id)
{
  g_return_val_if_fail (GY_IS_SERVICE_PROVIDER (self) && service_id != NULL, NULL);

  return (GyService *) get_service_by_id (self, service_id);
}
