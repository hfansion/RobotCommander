uic mainwindow.ui -o ui_mainwindow.h
uic settingsdialog.ui -o ui_settingsdialog.ui
lupdate *.h *.cpp *.ui ./command/*.* ./data/*.* ./info/*.* -ts ./translation/RobotCommander_en.ts ./translation/RobotCommander_zh.ts