/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * main.c
 * Copyright (C) 2014 Wiky L <wiiiky@yeah.net>
 * 
 * yuan is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * yuan is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include "yd.h"

int main(int argc, char *argv[])
{
    yd_init(argc, argv);

    GtkWindow *window = (GtkWindow *) gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size(window, 450, 360);
    gtk_window_set_position(window, GTK_WIN_POS_CENTER);
    g_signal_connect(G_OBJECT(window), "destroy",
                     G_CALLBACK(gtk_main_quit), NULL);

    WList *list = proc_net_tcp_open();
    WList *ptr = list;
    while (ptr) {
        ProcNetTcp *tcp = w_list_data(ptr);
        printf("%s\t%s\t%s\t%s\n", tcp->sl, tcp->local_address,
               tcp->rem_address, tcp->st);
        ptr = w_list_next(ptr);
    }

    gtk_widget_show_all(GTK_WIDGET(window));
    gtk_main();
    return (0);
}
