//
// Created by hamlet on 2022/3/18.
//

// You may need to build the project (run Qt uic code generator) to get "ui_commandpanel.h" resolved

#include "commandpanel.h"
#include "ui_commandpanel.h"


CommandPanel::CommandPanel(QWidget *parent) :
        PanelBase(parent), ui(new Ui::CommandPanel), m_commandModel(new CommandModel(this)) {
    ui->setupUi(this);
    ui->listView->setModel(m_commandModel);
}

CommandPanel::~CommandPanel() {
    delete ui;
}

void CommandPanel::retranslateUi() {
    ui->retranslateUi(this);
}

