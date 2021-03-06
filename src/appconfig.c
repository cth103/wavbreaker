/* wavbreaker - A tool to split a wave file up into multiple waves.
 * Copyright (C) 2002-2004 Timothy Robinson
 * Copyright (C) 2007 Thomas Perl
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
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <config.h>

#include "appconfig.h"
#include "sample.h"
#include "popupmessage.h"

#include "gettext.h"

#include "aoaudio.h"

#include <gtk/gtk.h>
#include <glib.h>

#include <stdlib.h>
#include <unistd.h>
#include <string.h>

static GtkWidget *window;

static gboolean loading_ui = FALSE;

static int config_file_version = 2;

/* Function pointers to the currently selected audio driver. */
static AudioFunctionPointers audio_function_pointers;

/* Output directory for wave files. */
static int use_outputdir = 0;
static GtkWidget *use_outputdir_toggle = NULL;
static char *outputdir = NULL;
static GtkWidget *outputdir_entry = NULL;
static GtkWidget *browse_button = NULL;

/* Etree filename suffix */
/* Filename suffix (not extension) for wave files. */
static int use_etree_filename_suffix = 0;

static char *etree_filename_suffix = NULL;
static GtkWidget *etree_filename_suffix_label = NULL;
static GtkWidget *etree_filename_suffix_entry = NULL;

/* Radio buttons */
static GtkWidget *radio1 = NULL;
static GtkWidget *radio2 = NULL;

/* Prepend File Number for wave files. */
static int prepend_file_number = 0;
static GtkWidget *prepend_file_number_toggle = NULL;

/* CD Length disc cutoff. */
static char *etree_cd_length = NULL;
static GtkWidget *etree_cd_length_label = NULL;
static GtkWidget *etree_cd_length_entry = NULL;

/* Config Filename */
static char *config_filename = NULL;

/* Window and pane sizes. */
static int main_window_xpos = -1;
static int main_window_ypos = -1;
static int main_window_width = -1;
static int main_window_height = -1;
static int vpane1_position = -1;
static int vpane2_position = -1;

/* Percentage for silence detection */
static int silence_percentage = 2;
static GtkWidget *silence_spin_button = NULL;

/* Ask user if the user really wants to quit wavbreaker. */
static int ask_really_quit = 1;

/* Show toolbar in main window */
static int show_toolbar = 1;

/* Draw moodbar in main window */
static int show_moodbar = 1;

/* function prototypes */
static int appconfig_read_file();
static void default_all_strings();
static void open_select_outputdir();

int appconfig_get_config_file_version()
{
    return config_file_version;
}

void appconfig_set_config_file_version(int x)
{
    config_file_version = x;
}

AudioFunctionPointers *get_audio_function_pointers()
{
    return &audio_function_pointers;
}

void set_audio_close_device(void (*f))
{
    audio_function_pointers.audio_close_device = f;
}

void set_audio_open_device(void (*f))
{
    audio_function_pointers.audio_open_device = f;
}

void set_audio_write(void (*f))
{
    audio_function_pointers.audio_write = f;
}

void set_audio_function_pointers()
{
    set_audio_close_device(ao_audio_close_device);
    set_audio_open_device(ao_audio_open_device);
    set_audio_write(ao_audio_write);
}

int appconfig_get_main_window_xpos()
{
    return main_window_xpos;
}

void appconfig_set_main_window_xpos(int x)
{
    main_window_xpos = x;
}

int appconfig_get_main_window_ypos()
{
    return main_window_ypos;
}

void appconfig_set_main_window_ypos(int x)
{
    main_window_ypos = x;
}

int appconfig_get_main_window_width()
{
    return main_window_width;
}

void appconfig_set_main_window_width(int x)
{
    main_window_width = x;
}

int appconfig_get_main_window_height()
{
    return main_window_height;
}

void appconfig_set_main_window_height(int x)
{
    main_window_height = x;
}

int appconfig_get_vpane1_position()
{
    return vpane1_position;
}

void appconfig_set_vpane1_position(int x)
{
    vpane1_position = x;
}

int appconfig_get_vpane2_position()
{
    return vpane2_position;
}

void appconfig_set_vpane2_position(int x)
{
    vpane2_position = x;
}

int appconfig_get_silence_percentage()
{
    return silence_percentage;
}

void appconfig_set_silence_percentage(int x)
{
    silence_percentage = x;
}

int appconfig_get_ask_really_quit()
{
    return ask_really_quit;
}

void appconfig_set_ask_really_quit(int x)
{
    ask_really_quit = x;
}

int appconfig_get_show_toolbar() {
    return show_toolbar;
}

void appconfig_set_show_toolbar(int x)
{
    show_toolbar = x;
}

int appconfig_get_show_moodbar() {
    return show_moodbar;
}

void appconfig_set_show_moodbar(int x)
{
    show_moodbar = x;
}

int appconfig_get_use_outputdir()
{
    return use_outputdir;
}

void appconfig_set_use_outputdir(int x)
{
    use_outputdir = x;
}

char *appconfig_get_outputdir()
{
    return outputdir;
}

void appconfig_set_outputdir(const char *val)
{
    if (outputdir != NULL) {
        g_free(outputdir);
    }
    outputdir = g_strdup(val);
}

int appconfig_get_use_etree_filename_suffix()
{
    return use_etree_filename_suffix;
}

void appconfig_set_use_etree_filename_suffix(int x)
{
    use_etree_filename_suffix = x;
}

char *appconfig_get_etree_filename_suffix()
{
    return etree_filename_suffix;
}

void appconfig_set_etree_filename_suffix(const char *val)
{
    if (etree_filename_suffix != NULL) {
        g_free(etree_filename_suffix);
    }
    etree_filename_suffix = g_strdup(val);
}

int appconfig_get_prepend_file_number()
{
    return prepend_file_number;
}

void appconfig_set_prepend_file_number(int x)
{
    prepend_file_number = x;
}

char *appconfig_get_etree_cd_length()
{
    return etree_cd_length;
}

void appconfig_set_etree_cd_length(const char *val)
{
    if (etree_cd_length != NULL) {
        g_free(etree_cd_length);
    }
    etree_cd_length = g_strdup(val);
}

char *get_config_filename()
{
    return config_filename;
}

void set_config_filename(const char *val)
{
    if (config_filename != NULL) {
        g_free(config_filename);
    }
    config_filename = g_strdup(val);
}

static void use_outputdir_toggled(GtkWidget *widget, gpointer user_data)
{
    if (loading_ui) {
        return;
    }

    if (appconfig_get_use_outputdir()) {
        // disable the output dir widget
        gtk_widget_set_sensitive(outputdir_entry, FALSE);
        gtk_widget_set_sensitive(browse_button, FALSE);
        appconfig_set_use_outputdir(0);
    } else {
        // enable the output dir widget
        gtk_widget_set_sensitive(outputdir_entry, TRUE);
        gtk_widget_set_sensitive(browse_button, TRUE);
        appconfig_set_use_outputdir(1);
    }
}

static void use_etree_filename_suffix_toggled(GtkWidget *widget, gpointer user_data)
{
    if (appconfig_get_use_etree_filename_suffix()) {
        appconfig_set_use_etree_filename_suffix(0);
    } else {
        appconfig_set_use_etree_filename_suffix(1);
    }
}

static void radio_buttons_toggled(GtkWidget *widget, gpointer user_data)
{
    if (loading_ui) {
        return;
    }

    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radio1)) == TRUE) {
        gtk_widget_set_sensitive( prepend_file_number_toggle, TRUE);
        gtk_widget_set_sensitive( etree_filename_suffix_entry, TRUE);
        gtk_widget_set_sensitive( etree_filename_suffix_label, TRUE);
        gtk_widget_set_sensitive( etree_cd_length_entry, FALSE);
        gtk_widget_set_sensitive( etree_cd_length_label, FALSE);
    } else {
        gtk_widget_set_sensitive( prepend_file_number_toggle, FALSE);
        gtk_widget_set_sensitive( etree_filename_suffix_entry, FALSE);
        gtk_widget_set_sensitive( etree_filename_suffix_label, FALSE);
        gtk_widget_set_sensitive( etree_cd_length_entry, TRUE);
        gtk_widget_set_sensitive( etree_cd_length_label, TRUE);
    }
}

static void prepend_file_number_toggled(GtkWidget *widget, gpointer user_data)
{
    if (loading_ui) {
        return;
    }

    if (appconfig_get_prepend_file_number()) {
        appconfig_set_prepend_file_number(0);
    } else {
        appconfig_set_prepend_file_number(1);
    }
}

static void appconfig_hide(GtkWidget *main_window)
{
    gtk_widget_destroy(main_window);
}

static void browse_button_clicked(GtkWidget *widget, gpointer user_data)
{
    open_select_outputdir();
}

static void open_select_outputdir() {
    GtkWidget *dialog;

    dialog = gtk_file_chooser_dialog_new(_("Select Output Directory"),
        GTK_WINDOW(window), GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
        GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_OPEN,
        GTK_RESPONSE_ACCEPT, NULL);
    gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(dialog),
        gtk_entry_get_text(GTK_ENTRY(outputdir_entry)));

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char *filename;

        filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        gtk_entry_set_text(GTK_ENTRY(outputdir_entry), filename);
        g_free(filename);
    }

    gtk_widget_destroy(dialog);
}

static void ok_button_clicked(GtkWidget *widget, gpointer user_data)
{
    appconfig_set_outputdir(gtk_entry_get_text(GTK_ENTRY(outputdir_entry)));
    appconfig_set_etree_filename_suffix(gtk_entry_get_text(GTK_ENTRY(etree_filename_suffix_entry)));
    appconfig_set_etree_cd_length(gtk_entry_get_text(GTK_ENTRY(etree_cd_length_entry)));
    appconfig_set_silence_percentage( gtk_spin_button_get_value_as_int( GTK_SPIN_BUTTON(silence_spin_button)));
    set_audio_function_pointers();

    track_break_rename( FALSE);
    appconfig_hide(GTK_WIDGET(user_data));
    appconfig_write_file();
}

void appconfig_show(GtkWidget *main_window)
{
    GtkWidget *vbox;
    GtkWidget *table;
    GtkWidget *hbbox;
    GtkWidget *ok_button;
    GtkWidget *label;

    GtkWidget *notebook;

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_widget_realize(window);
    gtk_window_set_modal(GTK_WINDOW(window), TRUE);
    gtk_window_set_transient_for(GTK_WINDOW(window), GTK_WINDOW(main_window));
    gtk_window_set_type_hint(GTK_WINDOW(window), GDK_WINDOW_TYPE_HINT_DIALOG);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER_ON_PARENT);
    gdk_window_set_functions(gtk_widget_get_window(window), GDK_FUNC_MOVE);
    gtk_window_set_title( GTK_WINDOW(window), _("wavbreaker Preferences"));

    /* create the vbox for the first tab */
    vbox = gtk_vbox_new(FALSE, 10);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 10);
    gtk_container_add( GTK_CONTAINER(window), vbox);

    notebook = gtk_notebook_new();
    gtk_container_add( GTK_CONTAINER(vbox), notebook);

    /* Selectable Output Directory */
    table = gtk_table_new(2, 3, FALSE);
    gtk_container_set_border_width(GTK_CONTAINER(table), 10);
    gtk_table_set_row_spacings( GTK_TABLE(table), 5);
    gtk_notebook_append_page( GTK_NOTEBOOK(notebook), table, gtk_label_new( _("General")));

    use_outputdir_toggle = gtk_check_button_new_with_label(_("Save output files in folder:"));
    gtk_table_attach(GTK_TABLE(table), use_outputdir_toggle,
        0, 2, 0, 1, GTK_FILL, 0, 5, 0);
    g_signal_connect(GTK_OBJECT(use_outputdir_toggle), "toggled",
        G_CALLBACK(use_outputdir_toggled), NULL);

    outputdir_entry = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(outputdir_entry), outputdir);
    gtk_entry_set_width_chars(GTK_ENTRY(outputdir_entry), 40);
    gtk_table_attach(GTK_TABLE(table), outputdir_entry,
        0, 1, 1, 2, GTK_EXPAND | GTK_FILL, 0, 5, 0);

    browse_button = gtk_button_new_with_label(_("Browse"));
    gtk_table_attach(GTK_TABLE(table), browse_button,
        1, 2, 1, 2, GTK_FILL, 0, 5, 0);
    g_signal_connect(G_OBJECT(browse_button), "clicked",
            (GCallback)browse_button_clicked, window);

    silence_spin_button = (GtkWidget*)gtk_spin_button_new_with_range( 1.0, 100.0, 1.0);
    gtk_spin_button_set_digits( GTK_SPIN_BUTTON(silence_spin_button), 0);
    gtk_spin_button_set_value( GTK_SPIN_BUTTON(silence_spin_button), appconfig_get_silence_percentage());
    
    label = gtk_label_new( _("Maximum volume considered silence (in percent):"));
    gtk_misc_set_alignment( GTK_MISC(label), 0.0, 0.5);

    gtk_table_attach( GTK_TABLE(table), label,
        0, 1, 2, 3, GTK_FILL, 0, 5, 0);
    gtk_table_attach( GTK_TABLE(table), silence_spin_button,
        1, 2, 2, 3, GTK_EXPAND, 0, 5, 0);

    /* Etree Filename Suffix */

    table = gtk_table_new(4, 10, FALSE);
    gtk_container_set_border_width(GTK_CONTAINER(table), 10);
    gtk_notebook_append_page( GTK_NOTEBOOK(notebook), table, gtk_label_new( _("File Naming")));

    radio1 = gtk_radio_button_new_with_label(NULL, _("Standard (##)"));
    gtk_table_attach(GTK_TABLE(table), radio1, 0, 3, 0, 1, GTK_FILL, 0, 5, 2);
    g_signal_connect(GTK_OBJECT(radio1), "toggled",
        G_CALLBACK(radio_buttons_toggled), NULL);

    label = gtk_label_new("   ");
    gtk_misc_set_alignment(GTK_MISC(label), 0, 0.5);
    gtk_table_attach(GTK_TABLE(table), label,
            0, 1, 2, 3, GTK_FILL, 0, 5, 2);

    prepend_file_number_toggle = gtk_check_button_new_with_label(_("Prepend number before filename"));
    gtk_table_attach(GTK_TABLE(table), prepend_file_number_toggle,
            1, 3, 2, 3, GTK_FILL, 0, 5, 0);
    g_signal_connect(GTK_OBJECT(prepend_file_number_toggle), "toggled",
        G_CALLBACK(prepend_file_number_toggled), NULL);

    etree_filename_suffix_label = gtk_label_new(_("Separator:"));
    gtk_misc_set_alignment(GTK_MISC(etree_filename_suffix_label), 0, 0.5);
    gtk_table_attach(GTK_TABLE(table), etree_filename_suffix_label,
            1, 2, 1, 2, GTK_FILL, 0, 5, 2);

    etree_filename_suffix_entry = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(etree_filename_suffix_entry), etree_filename_suffix);
    gtk_entry_set_width_chars(GTK_ENTRY(etree_filename_suffix_entry), 10);
    gtk_table_attach(GTK_TABLE(table), etree_filename_suffix_entry,
            2, 3, 1, 2, GTK_EXPAND | GTK_FILL, 0, 5, 2);

    radio2 = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(radio1),
            _("etree.org (d#t##)"));
    gtk_table_attach(GTK_TABLE(table), radio2, 0, 3, 3, 4, GTK_FILL, 0, 5, 2);

    etree_cd_length_label = gtk_label_new(_("CD Length:"));
    gtk_misc_set_alignment(GTK_MISC(etree_cd_length_label), 0, 0.5);
    gtk_table_attach(GTK_TABLE(table), etree_cd_length_label,
            1, 2, 5, 6, GTK_FILL, 0, 5, 2);

    etree_cd_length_entry = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(etree_cd_length_entry), etree_cd_length);
    gtk_entry_set_width_chars(GTK_ENTRY(etree_cd_length_entry), 10);
    gtk_table_attach(GTK_TABLE(table), etree_cd_length_entry,
            2, 3, 5, 6, GTK_EXPAND | GTK_FILL, 0, 5, 2);

    /* OK and Cancel Buttons */
    hbbox = gtk_hbutton_box_new();
    gtk_container_add(GTK_CONTAINER(vbox), hbbox);
    gtk_button_box_set_layout(GTK_BUTTON_BOX(hbbox), GTK_BUTTONBOX_END);
    gtk_box_set_spacing(GTK_BOX(hbbox), 10);

    ok_button = gtk_button_new_from_stock(GTK_STOCK_CLOSE);
    gtk_box_pack_end(GTK_BOX(hbbox), ok_button, FALSE, FALSE, 5);
    g_signal_connect(G_OBJECT(ok_button), "clicked",
        (GCallback)ok_button_clicked, window);

    g_signal_connect(GTK_OBJECT(radio2), "toggled",
        G_CALLBACK(use_etree_filename_suffix_toggled), NULL);

    loading_ui = TRUE;

    gboolean use_output_dir = appconfig_get_use_outputdir() ? TRUE : FALSE;
    gtk_widget_set_sensitive(outputdir_entry, use_output_dir);
    gtk_widget_set_sensitive(browse_button, use_output_dir);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(use_outputdir_toggle), use_output_dir);

    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(prepend_file_number_toggle),
            appconfig_get_prepend_file_number() ? TRUE : FALSE);

    gboolean use_etree = appconfig_get_use_etree_filename_suffix() ? TRUE : FALSE;
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio1), !use_etree);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio2), use_etree);

    loading_ui = FALSE;

    gtk_widget_show_all(window);
    set_audio_function_pointers();
    radio_buttons_toggled( NULL, NULL);
}

enum ConfigOptionType {
    INVALID = 0,
    STRING,
    INTEGER,
    BOOLEAN,
};

typedef struct ConfigOption_ ConfigOption;

struct ConfigOption_ {
    const char *key;
    enum ConfigOptionType type;
    void *setter;
    void *getter;
} config_options[] = {
#define OPTION(name, type) { #name, type, appconfig_set_ ## name, appconfig_get_ ## name }
    OPTION(config_file_version, INTEGER),

    OPTION(use_outputdir, BOOLEAN),
    OPTION(outputdir, STRING),

    OPTION(use_etree_filename_suffix, BOOLEAN),
    OPTION(etree_filename_suffix, STRING),
    OPTION(etree_cd_length, STRING),
    OPTION(prepend_file_number, BOOLEAN),

    OPTION(main_window_xpos, INTEGER),
    OPTION(main_window_ypos, INTEGER),
    OPTION(main_window_width, INTEGER),
    OPTION(main_window_height, INTEGER),

    OPTION(vpane1_position, INTEGER),
    OPTION(vpane2_position, INTEGER),

    OPTION(silence_percentage, INTEGER),
    OPTION(ask_really_quit, BOOLEAN),
    OPTION(show_toolbar, BOOLEAN),
    OPTION(show_moodbar, BOOLEAN),
#undef OPTION
    { NULL, INVALID, NULL, NULL },
};


void config_option_set_string(ConfigOption *option, gchar *value)
{
    ((void (*)(const char *))(option->setter))(value);
    g_free(value);
}

void config_option_set_integer(ConfigOption *option, int value)
{
    ((void (*)(int))(option->setter))(value);
}

const char *config_option_get_string(ConfigOption *option)
{
    return ((const char *(*)())(option->getter))();
}

int config_option_get_integer(ConfigOption *option)
{
    return ((int (*)())(option->getter))();
}

static int appconfig_read_file() {
    GKeyFile *keyfile = g_key_file_new();

    if (!g_key_file_load_from_file(keyfile, config_filename, G_KEY_FILE_NONE, NULL)) {
        g_key_file_free(keyfile);
        return 0;
    }

    ConfigOption *option = config_options;
    for (option=config_options; option->key; option++) {
        switch (option->type) {
            case INTEGER:
                config_option_set_integer(option,
                        g_key_file_get_integer(keyfile, "wavbreaker", option->key, NULL));
                break;
            case BOOLEAN:
                config_option_set_integer(option,
                        g_key_file_get_boolean(keyfile, "wavbreaker", option->key, NULL));
                break;
            case STRING:
                config_option_set_string(option,
                        g_key_file_get_string(keyfile, "wavbreaker", option->key, NULL));
                break;
            default:
                g_warning("Invalid option type: %d\n", option->type);
                break;
        }
    }

    g_key_file_free(keyfile);

    return 1;
}


void appconfig_write_file() {
    GKeyFile *keyfile = g_key_file_new();

    g_key_file_load_from_file(keyfile, config_filename, G_KEY_FILE_KEEP_COMMENTS, NULL);

    ConfigOption *option = config_options;
    for (option=config_options; option->key; option++) {
        switch (option->type) {
            case INTEGER:
                g_key_file_set_integer(keyfile, "wavbreaker", option->key,
                        config_option_get_integer(option));
                break;
            case BOOLEAN:
                g_key_file_set_boolean(keyfile, "wavbreaker", option->key,
                        config_option_get_integer(option));
                break;
            case STRING:
                g_key_file_set_string(keyfile, "wavbreaker", option->key,
                        config_option_get_string(option));
                break;
            default:
                g_warning("Invalid option type: %d\n", option->type);
                break;
        }
    }

    if (!g_key_file_save_to_file(keyfile, config_filename, NULL)) {
        g_warning("Could not save settings");
    }

    g_key_file_free(keyfile);
}

void appconfig_init()
{
    gchar *config_filename = g_build_path("/", g_get_user_config_dir(),
            "wavbreaker", "wavbreaker.conf", NULL);
    set_config_filename(config_filename);

    gchar *config_dir = g_path_get_dirname(config_filename);
    if (g_mkdir_with_parents(config_dir, 0700) != 0) {
        g_warning("Could not create configuration directory: %s", config_dir);
    }
    g_free(config_dir);
    g_free(config_filename);

    if (!appconfig_read_file()) {
        default_all_strings();
        appconfig_write_file();
    } else {
        default_all_strings();
    }
}

void default_all_strings() {
    /* default any values that where not in the config file */
    if (appconfig_get_outputdir() == NULL) {
        outputdir = g_strdup(getenv("PWD"));
    }
    if (appconfig_get_etree_filename_suffix() == NULL) {
        etree_filename_suffix = g_strdup("-");
    }
    if (appconfig_get_etree_cd_length() == NULL) {
        etree_cd_length = g_strdup("80");
    }

    set_audio_function_pointers();
}
