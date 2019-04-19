/* vokoscreen - A desktop recorder
 * Copyright (C) 2017-2019 Volker Kohaupt
 * 
 * Author:
 *      Volker Kohaupt <vkohaupt@freenet.de>
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of version 2 of the GNU General Public License
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * --End_License--
 */

#include "QvkAudioWindows.h"

#include <QAudioDeviceInfo>
#include <QDebug>

QvkAudioWindows::QvkAudioWindows(Ui_formMainWindow *ui_mainwindow )
{
    ui = ui_mainwindow;
    timer = new QTimer( this );
    timer->setTimerType( Qt::PreciseTimer );
    timer->setInterval( 1000 );
    connect( timer, SIGNAL( timeout() ), this, SLOT( slot_update() ) );
}


QvkAudioWindows::~QvkAudioWindows()
{
}


void QvkAudioWindows::slot_getWindowsDevices()
{
    foreach (const QAudioDeviceInfo &deviceInfo, QAudioDeviceInfo::availableDevices( QAudio::AudioInput ) )
    {
        QCheckBox *checkboxAudioDevice = new QCheckBox();
        checkboxAudioDevice->setText( deviceInfo.deviceName() );
        checkboxAudioDevice->setAccessibleName( deviceInfo.deviceName() );
        checkboxAudioDevice->setObjectName( "checkboxAudioDevice" + deviceInfo.deviceName() );
        ui->verticalLayoutAudioDevices->addWidget( checkboxAudioDevice );
        checkboxAudioDevice->setAutoExclusive( true );
    }

    // The first audiodevice is the standard audiodevice. Tested under Windows 10
    QList<QCheckBox *> listQCheckBox = ui->scrollAreaWidgetContentsAudioDevices->findChildren<QCheckBox *>();
    listQCheckBox.at(0)->click();

    QSpacerItem *verticalSpacerAudioDevices = new QSpacerItem( 20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding );
    ui->verticalLayoutAudioDevices->addSpacerItem( verticalSpacerAudioDevices );
}


void QvkAudioWindows::slot_clearVerticalLayoutAudioDevices()
{
    QList<QCheckBox *> listQCheckBox = ui->scrollAreaWidgetContentsAudioDevices->findChildren<QCheckBox *>();
    for ( int i = 0; i < listQCheckBox.count(); i++ )
    {
       ui->verticalLayoutAudioDevices->removeWidget( listQCheckBox.at(i) );
       delete listQCheckBox.at(i);
    }

    for ( int i = 0; i < ui->verticalLayoutAudioDevices->count(); ++i )
    {
        QLayoutItem *layoutItem = ui->verticalLayoutAudioDevices->itemAt(i);
        if ( layoutItem->spacerItem() )
        {
            ui->verticalLayoutAudioDevices->removeItem( layoutItem );
            delete layoutItem;
        }
    }
}


void QvkAudioWindows::slot_start( bool value )
{
    if ( value == true )
    {
        counter = 0;
        slot_clearVerticalLayoutAudioDevices();
        slot_getWindowsDevices();
        timer->start();
    }
    else
    {
        timer->stop();
    }
}


void QvkAudioWindows::slot_update()
{
    int count = QAudioDeviceInfo::availableDevices( QAudio::AudioInput ).count();
    if ( count != counter )
    {
        counter = count;
        slot_clearVerticalLayoutAudioDevices();
        slot_getWindowsDevices();
    }
}
