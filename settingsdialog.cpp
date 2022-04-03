#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include <QColorDialog>
#include <QFileDialog>
#include <QIntValidator>
#include <QLineEdit>
#include <QSerialPortInfo>
#include <QTranslator>
#include "util/autorefreshcombobox.h"

QMap<SettingsDialog::Page, QString> SettingsDialog::PAGE_NAME{};

static const char blankString[] = QT_TRANSLATE_NOOP("SettingsDialog", "N/A");

SettingsDialog::SettingsDialog(QWidget *parent) :
        QDialog(parent), m_translator(new QTranslator()), ui(new Ui::SettingsDialog),
        m_intValidator(new QIntValidator(0, 4000000, this)) {
    ui->setupUi(this);
    this->resize(m_settings.settingsDialog_size);

    ui->baudRateBox->setInsertPolicy(QComboBox::NoInsert);

    connect(ui->listWidget, SIGNAL(currentRowChanged(int)), this, SLOT(showPage(int)));
    connect(ui->buttonBox, &QDialogButtonBox::clicked, this, &SettingsDialog::buttonBoxClicked);
    connect(ui->serialPortInfoListBox, SIGNAL(currentIndexChanged(int)), this, SLOT(showPortInfo(int)));
    connect(ui->baudRateBox, SIGNAL(currentIndexChanged(int)), this, SLOT(checkCustomBaudRatePolicy(int)));
    connect(ui->serialPortInfoListBox, SIGNAL(currentIndexChanged(int)), this,
            SLOT(checkCustomDevicePathPolicy(int)));

    fillPortsParameters();
    fillPortsInfo();

    //*****************************
    connect(ui->radioButton_RCP_Pic, &QRadioButton::toggled, this, &SettingsDialog::mod_RCP_widget_change);
    connect(ui->radioButton_RTP_Pic, &QRadioButton::toggled, this, &SettingsDialog::mod_RTP_widget_change);
    ui->radioButton_RTP_Shape->setChecked(true);
    ui->widget_RTP_Pic->setVisible(false);

    connect(ui->pushButton_MapPic, &QPushButton::clicked, this, &SettingsDialog::chooseFile_Map);
    connect(ui->pushButton_RCP_Pic, &QPushButton::clicked, this, &SettingsDialog::chooseFile_RCP);
    connect(ui->pushButton_RTP_Pic, &QPushButton::clicked, this, &SettingsDialog::chooseFile_RTP);
    connect(ui->pushButton_RCP_Color, &QPushButton::clicked, this, &SettingsDialog::chooseColor_RCP);
    connect(ui->pushButton_RTP_Color, &QPushButton::clicked, this, &SettingsDialog::chooseColor_RTP);

    connect(ui->serialPortInfoListBox, &AutoRefreshComboBox::needRefresh, this, &SettingsDialog::fillPortsInfo);
    connect(ui->lineEdit_MapPic, &QLineEdit::returnPressed, this, &SettingsDialog::adjustPreviewPic);
    connect(ui->listWidget_language, &QListWidget::currentRowChanged, this, &SettingsDialog::showLanguageSelected);
    connect(ui->pushButton_check_update, &QPushButton::clicked, this, &SettingsDialog::checkForUpdate);
    //*****************************

    auto &s = m_settings.serial;
    s.name = ui->serialPortInfoListBox->currentText();

    if (ui->baudRateBox->currentIndex() == 4) {
        s.baudRate = ui->baudRateBox->currentText().toInt();
    } else {
        s.baudRate = static_cast<QSerialPort::BaudRate>(ui->baudRateBox->itemData(
                ui->baudRateBox->currentIndex()).toInt());
    }
    s.stringBaudRate = QString::number(s.baudRate);

    s.dataBits = static_cast<QSerialPort::DataBits>(
            ui->dataBitsBox->itemData(ui->dataBitsBox->currentIndex()).toInt());
    s.stringDataBits = ui->dataBitsBox->currentText();

    s.parity = static_cast<QSerialPort::Parity>(
            ui->parityBox->itemData(ui->parityBox->currentIndex()).toInt());
    s.stringParity = ui->parityBox->currentText();

    s.stopBits = static_cast<QSerialPort::StopBits>(
            ui->stopBitsBox->itemData(ui->stopBitsBox->currentIndex()).toInt());
    s.stringStopBits = ui->stopBitsBox->currentText();

    s.flowControl = static_cast<QSerialPort::FlowControl>(
            ui->flowControlBox->itemData(ui->flowControlBox->currentIndex()).toInt());
    s.stringFlowControl = ui->flowControlBox->currentText();

    switchLanguage();
    recoverSettings();
}

SettingsDialog::~SettingsDialog() {
    m_settings.settingsDialog_size = this->size();
    delete ui;
    delete m_translator;
}

const Settings *SettingsDialog::settings() const {
    return &m_settings;
}

void SettingsDialog::showPortInfo(int idx) {
    if (idx == -1)
        return;

    const QStringList list = ui->serialPortInfoListBox->itemData(idx).toStringList();
    ui->descriptionLabel->setText(tr("Description: %1").arg(list.count() > 1 ? list.at(1) : tr(blankString)));
    ui->manufacturerLabel->setText(tr("Manufacturer: %1").arg(list.count() > 2 ? list.at(2) : tr(blankString)));
    ui->serialNumberLabel->setText(tr("Serial number: %1").arg(list.count() > 3 ? list.at(3) : tr(blankString)));
    ui->locationLabel->setText(tr("Location: %1").arg(list.count() > 4 ? list.at(4) : tr(blankString)));
    ui->vidLabel->setText(tr("Vendor Identifier: %1").arg(list.count() > 5 ? list.at(5) : tr(blankString)));
    ui->pidLabel->setText(tr("Product Identifier: %1").arg(list.count() > 6 ? list.at(6) : tr(blankString)));
}

void SettingsDialog::apply() {
    updateSettings();
    switchLanguage();
    emit needUpdateSettings(&m_settings);
}

void SettingsDialog::cancel() {
    recoverSettings();
}

void SettingsDialog::checkCustomBaudRatePolicy(int idx) {
    const bool isCustomBaudRate = !ui->baudRateBox->itemData(idx).isValid();
    ui->baudRateBox->setEditable(isCustomBaudRate);
    if (isCustomBaudRate) {
        ui->baudRateBox->clearEditText();
        QLineEdit *edit = ui->baudRateBox->lineEdit();
        edit->setValidator(m_intValidator);
    }
}

void SettingsDialog::checkCustomDevicePathPolicy(int idx) {
    const bool isCustomPath = !ui->serialPortInfoListBox->itemData(idx).isValid();
    ui->serialPortInfoListBox->setEditable(isCustomPath);
    if (isCustomPath)
        ui->serialPortInfoListBox->clearEditText();
}

void SettingsDialog::fillPortsParameters() {
    ui->baudRateBox->addItem(QStringLiteral("9600"), QSerialPort::Baud9600);
    ui->baudRateBox->addItem(QStringLiteral("19200"), QSerialPort::Baud19200);
    ui->baudRateBox->addItem(QStringLiteral("38400"), QSerialPort::Baud38400);
    ui->baudRateBox->addItem(QStringLiteral("115200"), QSerialPort::Baud115200);
    ui->baudRateBox->addItem(tr("Custom"));
    ui->baudRateBox->setCurrentIndex(3);

    ui->dataBitsBox->addItem(QStringLiteral("5"), QSerialPort::Data5);
    ui->dataBitsBox->addItem(QStringLiteral("6"), QSerialPort::Data6);
    ui->dataBitsBox->addItem(QStringLiteral("7"), QSerialPort::Data7);
    ui->dataBitsBox->addItem(QStringLiteral("8"), QSerialPort::Data8);
    ui->dataBitsBox->setCurrentIndex(3);

    ui->parityBox->addItem(tr("None"), QSerialPort::NoParity);
    ui->parityBox->addItem(tr("Even"), QSerialPort::EvenParity);
    ui->parityBox->addItem(tr("Odd"), QSerialPort::OddParity);
    ui->parityBox->addItem(tr("Mark"), QSerialPort::MarkParity);
    ui->parityBox->addItem(tr("Space"), QSerialPort::SpaceParity);

    ui->stopBitsBox->addItem(QStringLiteral("1"), QSerialPort::OneStop);
#ifdef Q_OS_WIN
    ui->stopBitsBox->addItem(tr("1.5"), QSerialPort::OneAndHalfStop);
#endif
    ui->stopBitsBox->addItem(QStringLiteral("2"), QSerialPort::TwoStop);

    ui->flowControlBox->addItem(tr("None"), QSerialPort::NoFlowControl);
    ui->flowControlBox->addItem(tr("RTS/CTS"), QSerialPort::HardwareControl);
    ui->flowControlBox->addItem(tr("XON/XOFF"), QSerialPort::SoftwareControl);
}

void SettingsDialog::fillPortsInfo() {
    ui->serialPortInfoListBox->clear();
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

        ui->serialPortInfoListBox->addItem(list.first(), list);
    }

    ui->serialPortInfoListBox->addItem(tr("Custom"));
}

void SettingsDialog::updateSettings() {
    auto &ss = m_settings.serial;
    ss.name = ui->serialPortInfoListBox->currentText();

    if (ui->baudRateBox->currentIndex() == 4) {
        ss.baudRate = ui->baudRateBox->currentText().toInt();
    } else {
        ss.baudRate = static_cast<QSerialPort::BaudRate>(
                ui->baudRateBox->itemData(ui->baudRateBox->currentIndex()).toInt());
    }
    ss.stringBaudRate = QString::number(ss.baudRate);

    ss.dataBits = static_cast<QSerialPort::DataBits>(
            ui->dataBitsBox->itemData(ui->dataBitsBox->currentIndex()).toInt());
    ss.stringDataBits = ui->dataBitsBox->currentText();

    ss.parity = static_cast<QSerialPort::Parity>(
            ui->parityBox->itemData(ui->parityBox->currentIndex()).toInt());
    ss.stringParity = ui->parityBox->currentText();

    ss.stopBits = static_cast<QSerialPort::StopBits>(
            ui->stopBitsBox->itemData(ui->stopBitsBox->currentIndex()).toInt());
    ss.stringStopBits = ui->stopBitsBox->currentText();

    ss.flowControl = static_cast<QSerialPort::FlowControl>(
            ui->flowControlBox->itemData(ui->flowControlBox->currentIndex()).toInt());
    ss.stringFlowControl = ui->flowControlBox->currentText();


    //*****************************
    auto &s = m_settings;
    s.map_pic = ui->lineEdit_MapPic->text();
    s.mark_cur.pic_or_shape = ui->radioButton_RCP_Pic->isChecked();
    s.mark_cur.pic = ui->lineEdit_RCP_Pic->text();
    const auto &text0 = ui->comboBox_RCP_iconSize->currentText();
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    s.mark_cur.pic_size = text0.first(text0.size() / 2).toInt();
#else
    s.mark_cur.pic_size = text0.midRef(0, text0.shape_size() / 2).toInt();
#endif
    s.mark_cur.shape = static_cast<Settings::Mark::Shape>(ui->comboBox_RCP_Shape->currentIndex());
    s.mark_cur.shape_size = ui->comboBox_RCP_Size->currentText().toInt();
    const auto &color0 = ui->pushButton_RCP_Color->toolTip().split(',');
    s.mark_cur.color = QColor(color0[0].toInt(), color0[1].toInt(), color0[2].toInt());
    s.mark_tar.pic_or_shape = ui->radioButton_RTP_Pic->isChecked();
    s.mark_tar.pic = ui->lineEdit_RTP_Pic->text();
    const auto &text1 = ui->comboBox_RTP_iconSize->currentText();
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    s.mark_tar.pic_size = text1.first(text1.size() / 2).toInt();
#else
    s.mark_tar.pic_size = text1.midRef(0, text1.shape_size() / 2).toInt();
#endif
    s.mark_tar.shape = static_cast<Settings::Mark::Shape>(ui->comboBox_RTP_Shape->currentIndex());
    s.mark_tar.shape_size = ui->comboBox_RTP_Size->currentText().toInt();
    const auto &color1 = ui->pushButton_RTP_Color->toolTip().split(',');
    s.mark_tar.color = QColor(color1[0].toInt(), color1[1].toInt(), color1[2].toInt());
    s.language = static_cast<Settings::Language>(ui->listWidget_language->currentRow());
    s.channel = static_cast<Settings::ReleaseChannel>(ui->comboBox_channel->currentIndex());
    s.auto_check_update = ui->checkBox_autoUpdate->isChecked();
    //*****************************
}

void SettingsDialog::mod_RCP_widget_change(bool state) {
    ui->widget_RCP_Pic->setVisible(state);
    ui->widget_RCP_Shape->setVisible(!state);
}

void SettingsDialog::mod_RTP_widget_change(bool state) {
    ui->widget_RTP_Pic->setVisible(state);
    ui->widget_RTP_Shape->setVisible(!state);
}

void SettingsDialog::recoverSettings() {
    const auto &s = m_settings;
    ui->lineEdit_MapPic->setText(s.map_pic);
    ui->radioButton_RCP_Pic->setChecked(s.mark_cur.pic_or_shape);
    ui->lineEdit_RCP_Pic->setText(s.mark_cur.pic);
    ui->comboBox_RCP_iconSize->setCurrentText(QString("%1×%1").arg(s.mark_cur.pic_size));
    ui->comboBox_RCP_Shape->setCurrentIndex(static_cast<int>(s.mark_cur.shape));
    ui->comboBox_RCP_Size->setCurrentText(QString::number(s.mark_cur.shape_size));
    QString color_tip = QString("%1,%2,%3").arg(s.mark_cur.color.red()).arg(s.mark_cur.color.green())
            .arg(s.mark_cur.color.blue());
    ui->pushButton_RCP_Color->setStyleSheet(QString("background-color:rgb(%1)").arg(color_tip));
    ui->pushButton_RCP_Color->setToolTip(color_tip);
    ui->radioButton_RTP_Pic->setChecked(s.mark_tar.pic_or_shape);
    ui->lineEdit_RTP_Pic->setText(s.mark_tar.pic);
    ui->comboBox_RTP_iconSize->setCurrentText(QString("%1×%1").arg(s.mark_tar.pic_size));
    ui->comboBox_RTP_Shape->setCurrentIndex(static_cast<int>(s.mark_tar.shape));
    ui->comboBox_RTP_Size->setCurrentText(QString::number(s.mark_tar.shape_size));
    color_tip = QString("%1,%2,%3").arg(s.mark_tar.color.red()).arg(s.mark_tar.color.green())
            .arg(s.mark_tar.color.blue());
    ui->pushButton_RTP_Color->setStyleSheet(QString("background-color:rgb(%1)").arg(color_tip));
    ui->pushButton_RTP_Color->setToolTip(color_tip);
    ui->listWidget_language->setCurrentRow(static_cast<int>(s.language));
    ui->comboBox_channel->setCurrentIndex(static_cast<int>(s.channel));
    ui->checkBox_autoUpdate->setChecked(s.auto_check_update);
}

void SettingsDialog::chooseFile_Map() {
    QString file = QFileDialog::getOpenFileName(this, tr("Choose Picture"), ".");
    if (!file.isEmpty()) {
        ui->lineEdit_MapPic->setText(file);
        adjustPreviewPic();
    }
}

void SettingsDialog::chooseFile_RCP() {
    QString file = QFileDialog::getOpenFileName(this, tr("Choose Picture"), ".");
    if (!file.isEmpty()) ui->lineEdit_RCP_Pic->setText(file);
}

void SettingsDialog::chooseFile_RTP() {
    QString file = QFileDialog::getOpenFileName(this, tr("Choose Picture"), ".");
    if (!file.isEmpty()) ui->lineEdit_RTP_Pic->setText(file);
}

void SettingsDialog::chooseColor_RCP() {
    QColor color = QColorDialog::getColor();
    ui->pushButton_RCP_Color->setStyleSheet(
            QString("background-color:rgb(%1, %2, %3)").arg(color.red()).arg(color.green()).arg(color.blue()));
    ui->pushButton_RCP_Color->setToolTip(QString("%1,%2,%3").arg(color.red()).arg(color.green()).arg(color.blue()));
}

void SettingsDialog::chooseColor_RTP() {
    QColor color = QColorDialog::getColor();
    ui->pushButton_RTP_Color->setStyleSheet(
            QString("background-color:rgb(%1, %2, %3)").arg(color.red()).arg(color.green()).arg(color.blue()));
    ui->pushButton_RTP_Color->setToolTip(QString("%1,%2,%3").arg(color.red()).arg(color.green()).arg(color.blue()));
}

void SettingsDialog::showPage(int index) {
    Page page = static_cast<Page>(index);
    ui->label_name->setText(PAGE_NAME[page]);
    ui->stackedWidget->setCurrentIndex(index);
    if (page == Page::General) adjustPreviewPic();
}

void SettingsDialog::showPage(SettingsDialog::Page page) {
    this->show();
    ui->listWidget->setCurrentRow(static_cast<int>(page));
}

void SettingsDialog::adjustPreviewPic() {
    QString file = ui->lineEdit_MapPic->text();
    if (!file.isEmpty()) {
        QPixmap pixmap{file};
        if (pixmap.width() * ui->widget->height() > pixmap.height() * ui->widget->width()) {
            pixmap = pixmap.scaledToWidth(ui->widget->width());
        } else {
            pixmap = pixmap.scaledToHeight(ui->widget->height());
        }
        ui->label_map_preview->setPixmap(pixmap);
    }
}

void SettingsDialog::showLanguageSelected(int index) {
    ui->label_language->setText(tr("The language selected is %1.").arg(ui->listWidget_language->item(index)->text()));
}

void SettingsDialog::checkForUpdate() {
    m_settings.channel = static_cast<Settings::ReleaseChannel>(ui->comboBox_channel->currentIndex());
    emit needCheckForUpdate();
}

void SettingsDialog::checkUpdateResult(Updater::Result result) {
    QString text;
    switch (result) {
        case Updater::isLatest:
            text = tr("This is the latest version.");
            break;
        case Updater::isNotLatest:
            text = tr("There is a newer version of this program.");
            break;
        case Updater::networkError:
            text = tr("The network has an unknown problem.");
            break;
    }
    ui->label_isLatest->setText(text);
}

void SettingsDialog::buttonBoxClicked(QAbstractButton *button) {
    switch (ui->buttonBox->standardButton(button)) {
        case QDialogButtonBox::Ok: {
            apply();
            close();
            break;
        }
        case QDialogButtonBox::Cancel: {
            cancel();
            close();
            break;
        }
        case QDialogButtonBox::Apply:
            apply();
            break;
        default:
            break;
    }
}

void SettingsDialog::switchLanguage() {
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
    PAGE_NAME[Page::General] = tr("General");
    PAGE_NAME[Page::SerialPort] = tr("Serial Port");
    PAGE_NAME[Page::Appearance] = tr("Appearance");
    PAGE_NAME[Page::Update] = tr("Update");
    ui->retranslateUi(this);
}
