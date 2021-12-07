/****************************************************************************
**
** Copyright (C) 2012 Denis Shienkov <denis.shienkov@gmail.com>
** Copyright (C) 2012 Laszlo Papp <lpapp@kde.org>
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtSerialPort module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include <QFileDialog>
#include <QIntValidator>
#include <QLineEdit>
#include <QSerialPortInfo>
#include <QColorDialog>
#include <QTranslator>

static const char blankString[] = QT_TRANSLATE_NOOP("SettingsDialog", "N/A");

SettingsDialog::SettingsDialog(QWidget *parent) :
        QDialog(parent), m_translator(new QTranslator()),
        m_ui(new Ui::SettingsDialog),
        m_intValidator(new QIntValidator(0, 4000000, this)) {
    bool rs = m_translator->load(":/translation/RobotCommander_zh.qm");
    if (rs) qApp->installTranslator(m_translator);
    m_ui->setupUi(this);

    m_ui->baudRateBox->setInsertPolicy(QComboBox::NoInsert);

    connect(m_ui->applyButton, &QPushButton::clicked, this, &SettingsDialog::apply);
    connect(m_ui->cancelButton, &QPushButton::clicked, this, &SettingsDialog::cancel);
    connect(m_ui->serialPortInfoListBox, SIGNAL(currentIndexChanged(int)), this, SLOT(showPortInfo(int)));
    connect(m_ui->baudRateBox, SIGNAL(currentIndexChanged(int)), this, SLOT(checkCustomBaudRatePolicy(int)));
    connect(m_ui->serialPortInfoListBox, SIGNAL(currentIndexChanged(int)), this,
            SLOT(checkCustomDevicePathPolicy(int)));

    fillPortsParameters();
    fillPortsInfo();

    //*****************************
    auto &s = m_settings;
    s.mapPic = ":/resource/map.jpg";
    s.RCP_picThanShape = true;
    s.RCP_robotPic = ":/icon/robot.png";
    s.RCP_picSize = 64;
    s.RCP_shape = Settings::Circular;
    s.RCP_shapeSize = 20;
    s.RCP_color = QColor(255, 255, 0);
    s.RTP_picThanShape = false;
    s.RTP_robotPic = "";
    s.RTP_picSize = 64;
    s.RTP_shape = Settings::Square;
    s.RTP_shapeSize = 20;
    s.RTP_color = QColor(255, 0, 0);
    s.language = Settings::Chinese;


    connect(m_ui->radioButton_RCP_Pic, &QRadioButton::toggled, this, &SettingsDialog::mod_RCP_widget_change);
    connect(m_ui->radioButton_RTP_Pic, &QRadioButton::toggled, this, &SettingsDialog::mod_RTP_widget_change);
    m_ui->radioButton_RTP_Shape->setChecked(true);
    m_ui->widget_RTP_Pic->setVisible(false);

    connect(m_ui->pushButton_MapPic, &QPushButton::clicked, this, &SettingsDialog::chooseFile_Map);
    connect(m_ui->pushButton_RCP_Pic, &QPushButton::clicked, this, &SettingsDialog::chooseFile_RCP);
    connect(m_ui->pushButton_RTP_Pic, &QPushButton::clicked, this, &SettingsDialog::chooseFile_RTP);
    connect(m_ui->pushButton_RCP_Color, &QPushButton::clicked, this, &SettingsDialog::chooseColor_RCP);
    connect(m_ui->pushButton_RTP_Color, &QPushButton::clicked, this, &SettingsDialog::chooseColor_RTP);

    //*****************************


    m_settings.name = m_ui->serialPortInfoListBox->currentText();

    if (m_ui->baudRateBox->currentIndex() == 4) {
        m_settings.baudRate = m_ui->baudRateBox->currentText().toInt();
    } else {
        m_settings.baudRate = static_cast<QSerialPort::BaudRate>(
                m_ui->baudRateBox->itemData(m_ui->baudRateBox->currentIndex()).toInt());
    }
    m_settings.stringBaudRate = QString::number(m_settings.baudRate);

    m_settings.dataBits = static_cast<QSerialPort::DataBits>(
            m_ui->dataBitsBox->itemData(m_ui->dataBitsBox->currentIndex()).toInt());
    m_settings.stringDataBits = m_ui->dataBitsBox->currentText();

    m_settings.parity = static_cast<QSerialPort::Parity>(
            m_ui->parityBox->itemData(m_ui->parityBox->currentIndex()).toInt());
    m_settings.stringParity = m_ui->parityBox->currentText();

    m_settings.stopBits = static_cast<QSerialPort::StopBits>(
            m_ui->stopBitsBox->itemData(m_ui->stopBitsBox->currentIndex()).toInt());
    m_settings.stringStopBits = m_ui->stopBitsBox->currentText();

    m_settings.flowControl = static_cast<QSerialPort::FlowControl>(
            m_ui->flowControlBox->itemData(m_ui->flowControlBox->currentIndex()).toInt());
    m_settings.stringFlowControl = m_ui->flowControlBox->currentText();


    recoverSettings();
}

SettingsDialog::~SettingsDialog() {
    delete m_ui;
    delete m_translator;
}

const Settings *SettingsDialog::settings() const {
    return &m_settings;
}

void SettingsDialog::showPortInfo(int idx) {
    if (idx == -1)
        return;

    const QStringList list = m_ui->serialPortInfoListBox->itemData(idx).toStringList();
    m_ui->descriptionLabel->setText(tr("Description: %1").arg(list.count() > 1 ? list.at(1) : tr(blankString)));
    m_ui->manufacturerLabel->setText(tr("Manufacturer: %1").arg(list.count() > 2 ? list.at(2) : tr(blankString)));
    m_ui->serialNumberLabel->setText(tr("Serial number: %1").arg(list.count() > 3 ? list.at(3) : tr(blankString)));
    m_ui->locationLabel->setText(tr("Location: %1").arg(list.count() > 4 ? list.at(4) : tr(blankString)));
    m_ui->vidLabel->setText(tr("Vendor Identifier: %1").arg(list.count() > 5 ? list.at(5) : tr(blankString)));
    m_ui->pidLabel->setText(tr("Product Identifier: %1").arg(list.count() > 6 ? list.at(6) : tr(blankString)));
}

void SettingsDialog::apply() {
    updateSettings();
    hide();
    emit needUpdateSettings();

    switch (m_settings.language) {
        case Settings::Chinese: {
            bool rs = m_translator->load(":/translation/RobotCommander_zh.qm");
            if (rs) qApp->installTranslator(m_translator);
            break;
        }
        case Settings::English: {
            bool rs = m_translator->load(":/translation/RobotCommander_en.qm");
            if (rs) qApp->installTranslator(m_translator);
            qApp->installTranslator(m_translator);
            break;
        }
    }
    m_ui->retranslateUi(this);
}

void SettingsDialog::cancel() {
    recoverSettings();
    hide();
}

void SettingsDialog::checkCustomBaudRatePolicy(int idx) {
    const bool isCustomBaudRate = !m_ui->baudRateBox->itemData(idx).isValid();
    m_ui->baudRateBox->setEditable(isCustomBaudRate);
    if (isCustomBaudRate) {
        m_ui->baudRateBox->clearEditText();
        QLineEdit *edit = m_ui->baudRateBox->lineEdit();
        edit->setValidator(m_intValidator);
    }
}

void SettingsDialog::checkCustomDevicePathPolicy(int idx) {
    const bool isCustomPath = !m_ui->serialPortInfoListBox->itemData(idx).isValid();
    m_ui->serialPortInfoListBox->setEditable(isCustomPath);
    if (isCustomPath)
        m_ui->serialPortInfoListBox->clearEditText();
}

void SettingsDialog::fillPortsParameters() {
    m_ui->baudRateBox->addItem(QStringLiteral("9600"), QSerialPort::Baud9600);
    m_ui->baudRateBox->addItem(QStringLiteral("19200"), QSerialPort::Baud19200);
    m_ui->baudRateBox->addItem(QStringLiteral("38400"), QSerialPort::Baud38400);
    m_ui->baudRateBox->addItem(QStringLiteral("115200"), QSerialPort::Baud115200);
    m_ui->baudRateBox->addItem(tr("Custom"));

    m_ui->dataBitsBox->addItem(QStringLiteral("5"), QSerialPort::Data5);
    m_ui->dataBitsBox->addItem(QStringLiteral("6"), QSerialPort::Data6);
    m_ui->dataBitsBox->addItem(QStringLiteral("7"), QSerialPort::Data7);
    m_ui->dataBitsBox->addItem(QStringLiteral("8"), QSerialPort::Data8);
    m_ui->dataBitsBox->setCurrentIndex(3);

    m_ui->parityBox->addItem(tr("None"), QSerialPort::NoParity);
    m_ui->parityBox->addItem(tr("Even"), QSerialPort::EvenParity);
    m_ui->parityBox->addItem(tr("Odd"), QSerialPort::OddParity);
    m_ui->parityBox->addItem(tr("Mark"), QSerialPort::MarkParity);
    m_ui->parityBox->addItem(tr("Space"), QSerialPort::SpaceParity);

    m_ui->stopBitsBox->addItem(QStringLiteral("1"), QSerialPort::OneStop);
#ifdef Q_OS_WIN
    m_ui->stopBitsBox->addItem(tr("1.5"), QSerialPort::OneAndHalfStop);
#endif
    m_ui->stopBitsBox->addItem(QStringLiteral("2"), QSerialPort::TwoStop);

    m_ui->flowControlBox->addItem(tr("None"), QSerialPort::NoFlowControl);
    m_ui->flowControlBox->addItem(tr("RTS/CTS"), QSerialPort::HardwareControl);
    m_ui->flowControlBox->addItem(tr("XON/XOFF"), QSerialPort::SoftwareControl);
}

void SettingsDialog::fillPortsInfo() {
    m_ui->serialPortInfoListBox->clear();
    QString description;
    QString manufacturer;
    QString serialNumber;
    const auto infos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info: infos) {
        QStringList list;
        description = info.description();
        manufacturer = info.manufacturer();
        serialNumber = info.serialNumber();
        list << info.portName()
             << (!description.isEmpty() ? description : blankString)
             << (!manufacturer.isEmpty() ? manufacturer : blankString)
             << (!serialNumber.isEmpty() ? serialNumber : blankString)
             << info.systemLocation()
             << (info.vendorIdentifier() ? QString::number(info.vendorIdentifier(), 16) : blankString)
             << (info.productIdentifier() ? QString::number(info.productIdentifier(), 16) : blankString);

        m_ui->serialPortInfoListBox->addItem(list.first(), list);
    }

    m_ui->serialPortInfoListBox->addItem(tr("Custom"));
}

void SettingsDialog::updateSettings() {
    m_settings.name = m_ui->serialPortInfoListBox->currentText();

    if (m_ui->baudRateBox->currentIndex() == 4) {
        m_settings.baudRate = m_ui->baudRateBox->currentText().toInt();
    } else {
        m_settings.baudRate = static_cast<QSerialPort::BaudRate>(
                m_ui->baudRateBox->itemData(m_ui->baudRateBox->currentIndex()).toInt());
    }
    m_settings.stringBaudRate = QString::number(m_settings.baudRate);

    m_settings.dataBits = static_cast<QSerialPort::DataBits>(
            m_ui->dataBitsBox->itemData(m_ui->dataBitsBox->currentIndex()).toInt());
    m_settings.stringDataBits = m_ui->dataBitsBox->currentText();

    m_settings.parity = static_cast<QSerialPort::Parity>(
            m_ui->parityBox->itemData(m_ui->parityBox->currentIndex()).toInt());
    m_settings.stringParity = m_ui->parityBox->currentText();

    m_settings.stopBits = static_cast<QSerialPort::StopBits>(
            m_ui->stopBitsBox->itemData(m_ui->stopBitsBox->currentIndex()).toInt());
    m_settings.stringStopBits = m_ui->stopBitsBox->currentText();

    m_settings.flowControl = static_cast<QSerialPort::FlowControl>(
            m_ui->flowControlBox->itemData(m_ui->flowControlBox->currentIndex()).toInt());
    m_settings.stringFlowControl = m_ui->flowControlBox->currentText();


    //*****************************
    auto &s = m_settings;
    s.mapPic = m_ui->lineEdit_MapPic->text();
    s.RCP_picThanShape = m_ui->radioButton_RCP_Pic->isChecked();
    s.RCP_robotPic = m_ui->lineEdit_RCP_Pic->text();
    const auto &text0 = m_ui->comboBox_RCP_iconSize->currentText();
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    s.RCP_picSize = text0.first(text0.size() / 2).toInt();
#else
    s.RCP_picSize = text0.midRef(0, text0.size() / 2).toInt();
#endif
    s.RCP_shape = static_cast<Settings::Shape>(m_ui->comboBox_RCP_Shape->currentIndex());
    s.RCP_shapeSize = m_ui->comboBox_RCP_Size->currentText().toInt();
    const auto &color0 = m_ui->pushButton_RCP_Color->toolTip().split(',');
    s.RCP_color = QColor(color0[0].toInt(), color0[1].toInt(), color0[2].toInt());
    s.RTP_picThanShape = m_ui->radioButton_RTP_Pic->isChecked();
    s.RTP_robotPic = m_ui->lineEdit_RTP_Pic->text();
    const auto &text1 = m_ui->comboBox_RTP_iconSize->currentText();
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    s.RTP_picSize = text1.first(text1.size() / 2).toInt();
#else
    s.RTP_picSize = text1.midRef(0, text1.size() / 2).toInt();
#endif
    s.RTP_shape = static_cast<Settings::Shape>(m_ui->comboBox_RTP_Shape->currentIndex());
    s.RTP_shapeSize = m_ui->comboBox_RTP_Size->currentText().toInt();
    const auto &color1 = m_ui->pushButton_RTP_Color->toolTip().split(',');
    s.RTP_color = QColor(color1[0].toInt(), color1[1].toInt(), color1[2].toInt());
    s.language = static_cast<Settings::Language>(m_ui->comboBox->currentIndex());
    //*****************************
}

void SettingsDialog::mod_RCP_widget_change(bool state) {
    m_ui->widget_RCP_Pic->setVisible(state);
    m_ui->widget_RCP_Shape->setVisible(!state);
}

void SettingsDialog::mod_RTP_widget_change(bool state) {
    m_ui->widget_RTP_Pic->setVisible(state);
    m_ui->widget_RTP_Shape->setVisible(!state);
}

void SettingsDialog::recoverSettings() {
    auto &s = m_settings;
    m_ui->lineEdit_MapPic->setText(s.mapPic);
    m_ui->radioButton_RCP_Pic->setChecked(s.RCP_picThanShape);
    m_ui->lineEdit_RCP_Pic->setText(s.RCP_robotPic);
    m_ui->comboBox_RCP_iconSize->setCurrentText(QString("%1×%1").arg(s.RCP_picSize));
    m_ui->comboBox_RCP_Shape->setCurrentIndex((int) s.RCP_shape);
    m_ui->comboBox_RCP_Size->setCurrentText(QString::number(s.RCP_shapeSize));
    m_ui->pushButton_RCP_Color->setStyleSheet(
            QString("background-color:rgb(%1, %2, %3)").arg(s.RCP_color.red()).arg(s.RCP_color.green()).arg(
                    s.RCP_color.blue()));
    m_ui->pushButton_RCP_Color->setToolTip(QString("%1,%2,%3").arg(s.RCP_color.red()).arg(s.RCP_color.green()).arg(
            s.RCP_color.blue()));
    m_ui->radioButton_RTP_Pic->setChecked(s.RTP_picThanShape);
    m_ui->lineEdit_RTP_Pic->setText(s.RTP_robotPic);
    m_ui->comboBox_RTP_iconSize->setCurrentText(QString("%1×%1").arg(s.RTP_picSize));
    m_ui->comboBox_RTP_Shape->setCurrentIndex((int) s.RTP_shape);
    m_ui->comboBox_RTP_Size->setCurrentText(QString::number(s.RTP_shapeSize));
    m_ui->pushButton_RTP_Color->setStyleSheet(
            QString("background-color:rgb(%1, %2, %3)").arg(s.RTP_color.red()).arg(s.RTP_color.green()).arg(
                    s.RTP_color.blue()));
    m_ui->pushButton_RTP_Color->setToolTip(QString("%1,%2,%3").arg(s.RTP_color.red()).arg(s.RTP_color.green()).arg(
            s.RTP_color.blue()));
    m_ui->comboBox->setCurrentIndex((int) s.language);
}

void SettingsDialog::chooseFile_Map() {
    QString file = QFileDialog::getOpenFileName(this, tr("Choose Picture"), ".");
    if (!file.isEmpty()) m_ui->lineEdit_MapPic->setText(file);
}

void SettingsDialog::chooseFile_RCP() {
    QString file = QFileDialog::getOpenFileName(this, tr("Choose Picture"), ".");
    if (!file.isEmpty()) m_ui->lineEdit_RCP_Pic->setText(file);
}

void SettingsDialog::chooseFile_RTP() {
    QString file = QFileDialog::getOpenFileName(this, tr("Choose Picture"), ".");
    if (!file.isEmpty()) m_ui->lineEdit_RTP_Pic->setText(file);
}

void SettingsDialog::chooseColor_RCP() {
    QColor color = QColorDialog::getColor();
    m_ui->pushButton_RCP_Color->setStyleSheet(
            QString("background-color:rgb(%1, %2, %3)").arg(color.red()).arg(color.green()).arg(color.blue()));
    m_ui->pushButton_RCP_Color->setToolTip(QString("%1,%2,%3").arg(color.red()).arg(color.green()).arg(color.blue()));
}

void SettingsDialog::chooseColor_RTP() {
    QColor color = QColorDialog::getColor();
    m_ui->pushButton_RTP_Color->setStyleSheet(
            QString("background-color:rgb(%1, %2, %3)").arg(color.red()).arg(color.green()).arg(color.blue()));
    m_ui->pushButton_RTP_Color->setToolTip(QString("%1,%2,%3").arg(color.red()).arg(color.green()).arg(color.blue()));
}

void SettingsDialog::setPage(int index) {
    m_ui->tabWidget->setCurrentIndex(index);
}
