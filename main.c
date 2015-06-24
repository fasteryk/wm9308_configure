#include <gtk/gtk.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>


GtkWidget *main_window, *entry1, *entry2, *entry3, *conf_button;
int conf_result;


void destroy(GtkWidget *window, gpointer data)
{
	gtk_main_quit();
}

void display_dialog( GtkMessageType type, const gchar *msg)
{
	GtkWidget *dialog;

	dialog = gtk_message_dialog_new(GTK_WINDOW(main_window),
									GTK_DIALOG_MODAL,
									type,
									GTK_BUTTONS_CLOSE,
									msg);

	gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
}

gboolean finish_up_conf(gpointer data)
{
	if (conf_result)
		display_dialog(GTK_MESSAGE_INFO, "配置成功。");
	else
		display_dialog(GTK_MESSAGE_ERROR, "配置失败！");

	gtk_widget_set_sensitive(conf_button, TRUE);
	return FALSE;
}

gpointer conf_thread(gpointer data)
{
	conf_result = system("./upload_file") ? 0 : 1;

	g_idle_add(finish_up_conf, NULL);
	return NULL;
}

void write_entry(FILE *f, char *name, char *content)
{
	fprintf(f, name);

	if (strlen(name) >= 8)
		fprintf(f, "\t");
	else
		fprintf(f, "\t\t");

	fprintf(f, content);
	fprintf(f, "\n");
}

int create_setup_file()
{
	FILE *fp;

	fp = fopen("setup.ini", "w+");
	if (fp == NULL) {
		display_dialog(GTK_MESSAGE_ERROR, "无法创建配置文件。");
		return -1;
	}

	write_entry(fp, "TERMINALID", gtk_entry_get_text(GTK_ENTRY(entry1)));
	write_entry(fp, "GPSDEV", "/dev/ttyAMA2");
	write_entry(fp, "BUSNAME", "HUA88888");
	write_entry(fp, "GPSSPEED", "9600");
	write_entry(fp, "IPADDR", "116.236.238.212");
	write_entry(fp, "PORT", gtk_entry_get_text(GTK_ENTRY(entry2)));
	write_entry(fp, "FTPSERVERIP", "116.236.238.212");
	write_entry(fp, "MEDIAIP", "116.236.238.212");
	write_entry(fp, "BUSNUMBER", "778");
	write_entry(fp, "LINESN", gtk_entry_get_text(GTK_ENTRY(entry3)));
	write_entry(fp, "PASSENGER", "40");
	write_entry(fp, "OILBOX", "60.1");
	write_entry(fp, "TIMES", "10");
	write_entry(fp, "LENGTH", "300");
	write_entry(fp, "SPEED", "60.3");
	write_entry(fp, "CCID", "89860123456789089860");
	write_entry(fp, "FIREOUT", "30");

	fclose(fp);
	return 0;
}

void conf_clicked(GtkButton *button, GtkWindow *window)
{
	if (create_setup_file() < 0)
		return;

	gtk_widget_set_sensitive(conf_button, FALSE);
	g_thread_new("conf thread", conf_thread, NULL);
}

int main(int argc, char *argv[])
{
	GtkBuilder *builder;

	gtk_init(&argc, &argv);

	builder = gtk_builder_new();
	gtk_builder_add_from_file(builder, "gui.glade", NULL);

	main_window = GTK_WIDGET(gtk_builder_get_object(builder, "main_window"));
	entry1 = GTK_WIDGET(gtk_builder_get_object(builder, "entry1"));
	entry2 = GTK_WIDGET(gtk_builder_get_object(builder, "entry2"));
	entry3 = GTK_WIDGET(gtk_builder_get_object(builder, "entry3"));
	conf_button = GTK_WIDGET(gtk_builder_get_object(builder, "conf_button"));

	gtk_builder_connect_signals(builder, NULL);

	gtk_widget_show_all(main_window);

	/* Hand control over to the main loop. */
	gtk_main();

	return 0;
}
