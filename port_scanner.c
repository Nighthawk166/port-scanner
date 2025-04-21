#include <stdio.h>

#include <stdlib.h>

#include <string.h>

#include <unistd.h>

#include <arpa/inet.h>

#include <pthread.h>

#include <gtk/gtk.h>

#include <netdb.h>



#define MAX_PORTS 65535



GtkWidget *text_view;

GtkWidget *entry_ip, *entry_start_port, *entry_end_port;



void append_text_to_buffer(const char *text) {

    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));

    GtkTextIter iter;

    gtk_text_buffer_get_end_iter(buffer, &iter);

    gtk_text_buffer_insert(buffer, &iter, text, -1);

    gtk_text_buffer_insert(buffer, &iter, "\n", -1);

}



const char *get_service_name(int port) {

    struct servent *service = getservbyport(htons(port), "tcp");

    return (service ? service->s_name : "Unknown");

}



void *scan_port(void *arg) {

    int port = *((int *)arg);

    struct sockaddr_in server;

    int sock = socket(AF_INET, SOCK_STREAM, 0);

    if (sock < 0) pthread_exit(NULL);



    server.sin_family = AF_INET;

    server.sin_port = htons(port);

    server.sin_addr.s_addr = inet_addr(gtk_entry_get_text(GTK_ENTRY(entry_ip)));



    char msg[127];

    snprintf(msg, sizeof(msg), "Scanning port %d...", port);

    g_idle_add((GSourceFunc)append_text_to_buffer, g_strdup(msg));



    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) == 0) {

        const char *service_name = get_service_name(port);

        snprintf(msg, sizeof(msg) - 1, "Port %d is OPEN (%s)", port, service_name);

        g_idle_add((GSourceFunc)append_text_to_buffer, g_strdup(msg));

    }

    close(sock);

    pthread_exit(NULL);

}



void start_scan(GtkWidget *widget, gpointer data) {

    int start_port = atoi(gtk_entry_get_text(GTK_ENTRY(entry_start_port)));

    int end_port = atoi(gtk_entry_get_text(GTK_ENTRY(entry_end_port)));



    if (start_port < 1 || end_port > MAX_PORTS || start_port > end_port) {

        append_text_to_buffer("Invalid port range!");

        return;

    }



    pthread_t threads[end_port - start_port + 1];

    int ports[end_port - start_port + 1];



    for (int i = start_port; i <= end_port; i++) {

        ports[i - start_port] = i;

        pthread_create(&threads[i - start_port], NULL, scan_port, &ports[i - start_port]);

    }



    for (int i = start_port; i <= end_port; i++) {

        pthread_join(threads[i - start_port], NULL);

    }



    append_text_to_buffer("Scan Completed!");

}



int main(int argc, char *argv[]) {

    gtk_init(&argc, &argv);



    GtkWidget *window, *grid, *button, *scrolled_window;

    

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

    gtk_window_set_title(GTK_WINDOW(window), "Port Scanner");

    gtk_window_set_default_size(GTK_WINDOW(window), 500, 400);

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);



    grid = gtk_grid_new();

    gtk_container_set_border_width(GTK_CONTAINER(grid), 10);

    gtk_grid_set_row_spacing(GTK_GRID(grid), 10);

    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);

    gtk_container_add(GTK_CONTAINER(window), grid);



    entry_ip = gtk_entry_new();

    gtk_entry_set_text(GTK_ENTRY(entry_ip), "127.0.0.1");

    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Target IP:"), 0, 0, 1, 1);

    gtk_grid_attach(GTK_GRID(grid), entry_ip, 1, 0, 2, 1);



    entry_start_port = gtk_entry_new();

    gtk_entry_set_text(GTK_ENTRY(entry_start_port), "1");

    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Start Port:"), 0, 1, 1, 1);

    gtk_grid_attach(GTK_GRID(grid), entry_start_port, 1, 1, 2, 1);



    entry_end_port = gtk_entry_new();

    gtk_entry_set_text(GTK_ENTRY(entry_end_port), "100");

    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("End Port:"), 0, 2, 1, 1);

    gtk_grid_attach(GTK_GRID(grid), entry_end_port, 1, 2, 2, 1);



    button = gtk_button_new_with_label("Start Scan");

    gtk_widget_set_size_request(button, 150, 40);

    g_signal_connect(button, "clicked", G_CALLBACK(start_scan), NULL);

    gtk_grid_attach(GTK_GRID(grid), button, 0, 3, 3, 1);



    scrolled_window = gtk_scrolled_window_new(NULL, NULL);

    gtk_widget_set_size_request(scrolled_window, 480, 200);

    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

    

    text_view = gtk_text_view_new();

    gtk_text_view_set_editable(GTK_TEXT_VIEW(text_view), FALSE);

    gtk_container_add(GTK_CONTAINER(scrolled_window), text_view);

    gtk_grid_attach(GTK_GRID(grid), scrolled_window, 0, 4, 3, 1);

    

    gtk_widget_show_all(window);

    gtk_main();



    return 0;

}