#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <gst/gst.h>

//static GMainLoop *loop;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    qDebug().noquote() << VK_GStreamer_Version();

    qDebug() << gst_version_string ();

    connect( ui->pushButtonStart, SIGNAL(clicked(bool)), this, SLOT(VK_Start() ) );
    connect( ui->pushButtonStop, SIGNAL(clicked(bool)), this, SLOT(VK_Stop() ) );
}

MainWindow::~MainWindow()
{
    delete ui;
}

QString MainWindow::VK_GStreamer_Version()
{
    uint major, minor, micro, nano;
    QString nano_str;

    gst_version(&major, &minor, &micro, &nano);

    if (nano == 1)
      nano_str = "(CVS)";
    else if (nano == 2)
      nano_str = "(Prerelease)";
    else
      nano_str = "";

    QString gstrVersion = QString::number( major ).append( "." )
                 .append( QString::number( minor)).append( "." )
                 .append( QString::number( micro))
                 .append( nano_str );
    return gstrVersion;
}


// gstreamer-plugins-bad-orig-addon
// gstreamer-plugins-good-extra
// libgstinsertbin-1_0-0
void MainWindow::VK_Start()
{
    //loop = g_main_loop_new (NULL, FALSE);

    GstElementFactory *factory = gst_element_factory_find( "ximagesrc" );
    if ( !factory )
    {
      qDebug() << "Failed to find factory of type ximagesrc";
      return;
    }
    gst_object_unref (GST_OBJECT (factory));

    // gst_parse_launch --> https://gstreamer.freedesktop.org/data/doc/gstreamer/head/gstreamer/html/gstreamer-GstParse.html#gst-parse-launch-full
    // ximagesrc        --> https://gstreamer.freedesktop.org/data/doc/gstreamer/head/gst-plugins-good-plugins/html/gst-plugins-good-plugins-ximagesrc.html
    // ximagesrc        --> Element Pads --> https://gstreamer.freedesktop.org/data/doc/gstreamer/head/gst-plugins-good-plugins/html/gst-plugins-good-plugins-ximagesrc.html
    // videoconvert     --> https://gstreamer.freedesktop.org/data/doc/gstreamer/head/gst-plugins-base-plugins/html/gst-plugins-base-plugins-videoconvert.html
    // x264enc          --> https://gstreamer.freedesktop.org/data/doc/gstreamer/head/gst-plugins-ugly-plugins/html/gst-plugins-ugly-plugins-x264enc.html
    // matroskamux      --> https://gstreamer.freedesktop.org/data/doc/gstreamer/head/gst-plugins-good/html/gst-plugins-good-plugins-matroskamux.html
    // filesink         --> https://gstreamer.freedesktop.org/data/doc/gstreamer/head/gstreamer-plugins/html/gstreamer-plugins-filesink.html

    // Option -e beendet die Aufnahme richtig
    // Da kommt die Frage auf, welcher Befehl ist dafür notwendig.
    // gst-launch-1.0 -e ximagesrc ! video/x-raw, framerate=25/1 ! videoconvert ! x264enc ! matroskamux ! filesink location=/home/vk/Videos/desktop.mkv

    pipeline = gst_parse_launch( "ximagesrc ! video/x-raw, framerate=50/1 ! videoconvert ! x264enc ! matroskamux ! filesink location=/home/vk/Videos/desktop.mkv", &error );



/*
    // Create the elements
    source = gst_element_factory_make ("ximagesrc", "source");
    g_object_set (G_OBJECT (source), "show-pointer", true, NULL);

    //https://gstreamer.freedesktop.org/data/doc/gstreamer/head/gstreamer/html/GstCaps.html
    //http://comments.gmane.org/gmane.comp.video.gstreamer.devel/53765
    GstCaps *caps = gst_caps_new_simple ("video/x-raw",
                                         "format", G_TYPE_STRING, "I420",
                                         "framerate", GST_TYPE_FRACTION, 25, 1,
                                         "pixel-aspect-ratio", GST_TYPE_FRACTION, 1, 1,
                                         "width", G_TYPE_INT, 320,
                                         "height", G_TYPE_INT, 240,
                                         NULL);

    videoconvert = gst_element_factory_make ("videoconvert", "videoconvert");

    // Preset found with, gst-inspect-1.0 x264enc
    enum x264preset { None, ultrafast, superfast, veryfast, faster, medium, slow, slower, veryslow, placebo };
    VK_VideoEncoder = gst_element_factory_make ("x264enc", "x264enc");
    g_object_set (G_OBJECT (VK_VideoEncoder), "speed-preset", x264preset::medium , NULL);

    VK_VideoQueue = gst_element_factory_make ("queue2", "videoq");

    matroskamux = gst_element_factory_make ("matroskamux", "matroskamux");

    filesink = gst_element_factory_make ("filesink",  "filesink");
    g_object_set (G_OBJECT (filesink), "location", "/home/vk/Videos/vokoscreen.mkv", NULL);

    // Create the empty pipeline
    pipeline = gst_pipeline_new ("test-pipeline");

    if (!pipeline || !source || !caps || !videoconvert || !VK_VideoEncoder || !VK_VideoQueue || !matroskamux || !filesink)
    {
      g_printerr ("Not all elements could be created.\n");
      return;
    }

    // Build the pipeline
    gst_bin_add_many (GST_BIN (pipeline), source, videoconvert, VK_VideoEncoder, VK_VideoQueue, matroskamux, filesink, NULL);

    if (gst_element_link_many (source, videoconvert, VK_VideoEncoder, VK_VideoQueue, matroskamux, filesink, NULL ) != TRUE)
    {
      g_printerr ("Elements could not be linked.\n");
      gst_object_unref (pipeline);
      return;
    }
*/

    // https://gstreamer.freedesktop.org/documentation/application-development/basics/helloworld.html

    // Start playing
    GstStateChangeReturn ret;
    ret = gst_element_set_state(pipeline, GST_STATE_PLAYING);
    qDebug() << ret;
    if (ret == GST_STATE_CHANGE_FAILURE) {
      g_printerr ("Unable to set the pipeline to the playing state.\n");
      gst_object_unref (pipeline);
      return;
    }

    /* Iterate */
    //g_print ("Running...\n");
    //g_main_loop_run (loop);
}


void MainWindow::VK_Stop()
{
    // http://gstreamer-devel.narkive.com/AMLXdRKP/how-can-i-tell-if-all-elements-received-the-eos
    // wait for EOS
    bool a = gst_element_send_event (pipeline, gst_event_new_eos());
    qDebug() << a;
    GstClockTime timeout = 10 * GST_SECOND;
    GstMessage *msg;
    msg = gst_bus_timed_pop_filtered (GST_ELEMENT_BUS (pipeline), timeout, GST_MESSAGE_EOS );


    GstStateChangeReturn ret ;
    ret = gst_element_set_state (pipeline, GST_STATE_PAUSED);
    qDebug() << ret;
    ret = gst_element_set_state (pipeline, GST_STATE_READY);
    qDebug() << ret;
    ret = gst_element_set_state (pipeline, GST_STATE_NULL);
    qDebug() << ret;
    gst_object_unref (pipeline);
}
