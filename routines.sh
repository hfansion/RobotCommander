uic mainwindow.ui -o ui_mainwindow.h
uic settingsdialog.ui -o ui_settingsdialog.h
uic panel/senderpanel.ui -o panel/ui_senderpanel.h
uic panel/consolepanel.ui -o panel/ui_consolepanel.h
lupdate *.h *.cpp *.ui ./command/*.* ./data/*.* ./info/*.* ./panel/*.* -ts ./translation/RobotCommander_en.ts ./translation/RobotCommander_zh.ts
lrelease ./translation/RobotCommander_*.ts