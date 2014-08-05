/*
 * notify.c
 *
 * Copyright (C) 2014 - Wiky L
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include "notify.h"

void yd_notify(const gchar * summary, const gchar * body)
{
    if (!notify_is_initted()) {
        notify_init("yd");
    }
    NotifyNotification *notification =
        notify_notification_new(summary, body, "");
    notify_notification_show(notification, NULL);
    g_object_unref(notification);
}
