/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 * Version 0.6b
 *
 * Copyright (c) 2013-2014 Inviwo Foundation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Main file author: Rickard Englund
 *
 *********************************************************************************/

#include <modules/pythonqt/pythoneditorwidget.h>
#include <inviwo/core/common/inviwoapplication.h>
#include <inviwo/core/network/processornetwork.h>
#include <inviwo/core/util/logcentral.h>
#include <inviwo/core/util/urlparser.h>
#include <modules/python/pythonmodule.h>
#include <QCommandLinkButton>
#include <QSplitter>
#include <QFileDialog>
#include <QMessageBox>
#include <QPushButton>
#include <QButtonGroup>
#include <QToolButton>
#include <QSpacerItem>
#include <QHBoxLayout>
#include <QPlainTextEdit>
#include <QFrame>
#include <inviwo/core/util/clock.h>
#include "inviwo/qt/widgets/properties/syntaxhighlighter.h"

#include <inviwo/qt/widgets/inviwofiledialog.h>

namespace inviwo {

const static std::string defaultSource =
    "# Inviwo Python script \nimport inviwo \nimport inviwoqt \n\ninviwo.info() \ninviwoqt.info() "
    "\n";
PythonEditorWidget* PythonEditorWidget::instance_ = 0;
PythonEditorWidget* PythonEditorWidget::getPtr() {
    ivwAssert(instance_ != 0, "Singleton not yet created");
    return instance_;
}

PythonEditorWidget::PythonEditorWidget(QWidget* parent)
    : InviwoDockWidget(tr("Python Editor"), parent)
    , ProcessorNetworkObserver()
    , settings_("Inviwo", "Inviwo")
    , infoTextColor_(153, 153, 153)
    , errorTextColor_(255, 107, 107)
    , script_()
    , unsavedChanges_(false) {
    ivwAssert(instance_ == 0, "This is a Singelton, constructor may only be called once")
        instance_ = this;
    setObjectName("PythonEditor");
    settings_.beginGroup("PythonEditor");
    QString lastFile = settings_.value("lastScript", "").toString();
    settings_.endGroup();
    setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    setFloating(true);
    buildWidget();
    resize(500, 700);
    setVisible(false);
    ProcessorNetwork* processorNetwork = InviwoApplication::getPtr()->getProcessorNetwork();
    addObservation(processorNetwork);
    processorNetwork->addObserver(this);
    InviwoApplication::getRef().registerFileObserver(this);
    unsavedChanges_ = false;

    if (lastFile.size() != 0) loadFile(lastFile.toLocal8Bit().constData(), false);
}

void PythonEditorWidget::onProcessorNetworkChange() {
    if (script_.getScriptRecorder()->isRecording()) {
        script_.getScriptRecorder()->recordNetworkChanges();
        std::string currentScriptInEditor = pythonCode_->toPlainText().toLocal8Bit().constData();

        if (script_.getSource() != currentScriptInEditor) {
            // set new script source from recorder
            clearOutput();
            pythonCode_->setPlainText(script_.getSource().c_str());
            pythonCode_->setPlainText(script_.getSource().c_str());
            unsavedChanges_ = true;
        }
    }
}

void PythonEditorWidget::buildWidget() {
    setWindowIcon(QIcon(":/icons/python.png"));
    QWidget* content = new QWidget(this);
    QVBoxLayout* verticalLayout = new QVBoxLayout(content);
    ////Create Buttons
    QHBoxLayout* horizontalLayout = new QHBoxLayout();
    QToolButton* runButton = new QToolButton(content);
    QToolButton* newButton = new QToolButton(content);
    QToolButton* openButton = new QToolButton(content);
    QToolButton* saveButton = new QToolButton(content);
    QToolButton* saveAsButton = new QToolButton(content);
    QPushButton* clearOutputButton = new QPushButton(content);
    startRecordScriptButton_ = new QToolButton(content);
    endRecordScriptButton_ = new QToolButton(content);
    runButton->setShortcut(QKeySequence(tr("F5")));
    runButton->setIcon(QIcon(":/icons/python.png"));
    runButton->setToolTip("Compile and run (F5)");
    newButton->setIcon(QIcon(":/icons/new.png"));
    newButton->setToolTip("New file (Ctrl+N)");
    openButton->setIcon(QIcon(":/icons/open.png"));
    openButton->setToolTip("Open Python script");
    saveButton->setIcon(QIcon(":/icons/save.png"));
    saveButton->setToolTip("Save (Ctrl+S)");
    saveAsButton->setIcon(QIcon(":/icons/saveas.png"));
    saveAsButton->setToolTip("Save as");
    clearOutputButton->setText("Clear Output");
    startRecordScriptButton_->setIcon(QIcon(":/icons/player_record.png"));
    startRecordScriptButton_->setToolTip("Start Recording");
    endRecordScriptButton_->setIcon(QIcon(":/icons/player_stop.png"));
    endRecordScriptButton_->setToolTip("Stop Recording");
    endRecordScriptButton_->setDisabled(true);
    QFrame* line1 = new QFrame(content);
    line1->setFrameShape(QFrame::VLine);
    line1->setFrameShadow(QFrame::Sunken);
    QFrame* line2 = new QFrame(content);
    line2->setFrameShape(QFrame::VLine);
    line2->setFrameShadow(QFrame::Sunken);
    QFrame* line3 = new QFrame(content);
    line3->setFrameShape(QFrame::VLine);
    line3->setFrameShadow(QFrame::Sunken);
    horizontalLayout->addWidget(runButton);
    horizontalLayout->addWidget(line1);
    horizontalLayout->addWidget(newButton);
    horizontalLayout->addWidget(openButton);
    horizontalLayout->addWidget(saveButton);
    horizontalLayout->addWidget(saveAsButton);
    horizontalLayout->addWidget(line2);
    horizontalLayout->addWidget(clearOutputButton);
    horizontalLayout->addWidget(line3);
    horizontalLayout->addWidget(startRecordScriptButton_);
    horizontalLayout->addWidget(endRecordScriptButton_);
    QSpacerItem* horizontalSpacer =
        new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    horizontalLayout->addItem(horizontalSpacer);
    // Done creating buttons
    QSplitter* splitter = new QSplitter(content);
    splitter->setOrientation(Qt::Vertical);
    pythonCode_ = new QPlainTextEdit(content);
    pythonCode_->setObjectName("pythonEditor");
    setDefaultText();
    pythonOutput_ = new QTextEdit(content);
    pythonOutput_->setObjectName("pythonConsole");
    pythonOutput_->setReadOnly(true);
    pythonOutput_->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    syntaxHighligther_ =
        SyntaxHighligther::createSyntaxHighligther<Python>(pythonCode_->document());
    verticalLayout->addLayout(horizontalLayout);
    splitter->addWidget(pythonCode_);
    splitter->addWidget(pythonOutput_);
    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 0);
    verticalLayout->addWidget(splitter);
    setWidget(content);
    connect(pythonCode_, SIGNAL(textChanged()), this, SLOT(onTextChange()));
    connect(runButton, SIGNAL(clicked()), this, SLOT(run()));
    connect(newButton, SIGNAL(clicked()), this, SLOT(setDefaultText()));
    connect(openButton, SIGNAL(clicked()), this, SLOT(open()));
    connect(saveButton, SIGNAL(clicked()), this, SLOT(save()));
    connect(saveAsButton, SIGNAL(clicked()), this, SLOT(saveAs()));
    connect(clearOutputButton, SIGNAL(clicked()), this, SLOT(clearOutput()));
    connect(startRecordScriptButton_, SIGNAL(clicked()), this, SLOT(startRecordingScript()));
    connect(endRecordScriptButton_, SIGNAL(clicked()), this, SLOT(endRecordingScript()));
}

PythonEditorWidget::~PythonEditorWidget() {}

void PythonEditorWidget::appendToOutput(const std::string& msg, bool error) {
    pythonOutput_->setTextColor(error ? errorTextColor_ : infoTextColor_);
    pythonOutput_->append(msg.c_str());
}

void PythonEditorWidget::fileChanged(std::string fileName) {
    std::string msg = "The file " + URLParser::getFileNameWithExtension(scriptFileName_) +
                      " has been modified outside of Inwivo, do you want to reload its contents";
    int ret = QMessageBox::question(this, "Python Editor", msg.c_str(), "Yes", "No");

    if (ret == 0) {  // yes
        readFile();
    } else {
        unsavedChanges_ = true;
        // set code change to true so that we can quick save without making more changes
    }
}

void PythonEditorWidget::loadFile(std::string fileName, bool askForSave) {
    if (askForSave && unsavedChanges_) {
        int ret =
            QMessageBox::information(this, "Python Editor", "Do you want to save unsaved changes?",
                                     "Save", "Discard", "Cancel");

        if (ret == 0) save();

        if (ret == 2)  // Cancel
            return;
    }
    scriptFileName_ = fileName;
    settings_.beginGroup("PythonEditor");
    settings_.setValue("lastScript", scriptFileName_.c_str());
    settings_.endGroup();
    readFile();
}

void PythonEditorWidget::onPyhonExecutionOutput(std::string msg, OutputType outputType) {
    appendToOutput(msg, outputType != PythonExecutionOutputObeserver::standard);
    LogInfo(msg);
}

void PythonEditorWidget::save() {
    if (script_.getSource() == defaultSource) return;  // nothig to be saved

    if (scriptFileName_.size() == 0) {
        saveAs();
    } else if (unsavedChanges_) {
        stopFileObservation(scriptFileName_);
        std::ofstream file(scriptFileName_.c_str());
        file << pythonCode_->toPlainText().toLocal8Bit().constData();
        file.close();
        startFileObservation(scriptFileName_);
        LogInfo("Python Script saved(" << scriptFileName_ << ")");
        settings_.beginGroup("PythonEditor");
        settings_.setValue("lastScript", scriptFileName_.c_str());
        settings_.endGroup();
        unsavedChanges_ = false;
    }
}

void PythonEditorWidget::readFile() {
    std::ifstream file(scriptFileName_.c_str());
    std::string text((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    replaceInString(text, "\t", "    ");
    pythonCode_->setPlainText(text.c_str());
    script_.setSource(text);
    unsavedChanges_ = false;
}

bool PythonEditorWidget::hasFocus() const {
    if (InviwoDockWidget::hasFocus()) return true;

    if (pythonCode_->hasFocus()) return true;

    return false;
}

void PythonEditorWidget::saveAs() {
    if (script_.getSource() == defaultSource) return;  // nothig to be saved

    InviwoFileDialog saveFileDialog(this, "Save Python Script ...", "script");

    saveFileDialog.setDirectory(QDir(
        (InviwoApplication::getPtr()->getPath(InviwoApplication::PATH_DATA) + "scripts").c_str()));
    saveFileDialog.setFileMode(QFileDialog::AnyFile);
    saveFileDialog.setAcceptMode(QFileDialog::AcceptSave);
    saveFileDialog.setConfirmOverwrite(true);

    saveFileDialog.addSidebarPath(InviwoApplication::PATH_SCRIPTS);
    saveFileDialog.addSidebarPath(InviwoApplication::PATH_SCRIPTS);

    saveFileDialog.addExtension("py", "Python Script");

    if (saveFileDialog.exec()) {
        stopFileObservation(scriptFileName_);
        QString path = saveFileDialog.selectedFiles().at(0);

        if (!path.endsWith(".py")) path.append(".py");

        scriptFileName_ = path.toLocal8Bit().constData();
        save();
    }
}

void PythonEditorWidget::open() {
    if (unsavedChanges_) {
        int ret =
            QMessageBox::information(this, "Python Editor", "Do you want to save unsaved changes?",
                                     "Save", "Discard", "Cancel");

        if (ret == 0) save();

        if (ret == 2)  // Cancel
            return;
    }

    InviwoFileDialog openFileDialog(this, "Open Python Script ...", "script");
    openFileDialog.setDirectory(
        QDir(InviwoApplication::getPtr()->getPath(InviwoApplication::PATH_SCRIPTS).c_str()));
    openFileDialog.setFileMode(QFileDialog::AnyFile);

    openFileDialog.addSidebarPath(InviwoApplication::PATH_SCRIPTS);
    openFileDialog.addSidebarPath(InviwoApplication::PATH_SCRIPTS);

    openFileDialog.addExtension("py", "Python Script");

    if (openFileDialog.exec()) {
        stopFileObservation(scriptFileName_);
        scriptFileName_ = openFileDialog.selectedFiles().at(0).toLocal8Bit().constData();
        settings_.beginGroup("PythonEditor");
        settings_.setValue("lastScript", scriptFileName_.c_str());
        settings_.endGroup();
        startFileObservation(scriptFileName_);
        readFile();
    }
}

void PythonEditorWidget::run() {
    if (unsavedChanges_ && scriptFileName_.size() != 0)  // save if needed
        save();

    clearOutput();
    Clock c;
    c.start();
    bool ok = script_.run();
    c.stop();

    if (ok) {
        LogInfo("Python Script Executed succesfully");
    }

    LogInfo("Execution time: " << c.getElapsedMiliseconds() << " ms");
}

void PythonEditorWidget::show() {
    // setFloating(true);
    setVisible(true);
}

void PythonEditorWidget::setDefaultText() {
    if (unsavedChanges_) {
        int ret =
            QMessageBox::information(this, "Python Editor", "Do you want to save unsaved changes?",
                                     "Save", "Discard Changes", "Cancel");

        if (ret == 0)
            save();
        else if (ret == 2)  // cancel
            return;
    }

    pythonCode_->setPlainText(defaultSource.c_str());
    script_.setSource(defaultSource);
    stopFileObservation(scriptFileName_);
    scriptFileName_ = "";
    settings_.beginGroup("PythonEditor");
    settings_.setValue("lastScript", scriptFileName_.c_str());
    settings_.endGroup();
    unsavedChanges_ = false;
}

void PythonEditorWidget::clearOutput() { pythonOutput_->setText(""); }

void PythonEditorWidget::onTextChange() {
    std::string source = pythonCode_->toPlainText().toLocal8Bit().constData();
    int size = static_cast<int>(source.length());

    replaceInString(source, "\t", "    ");

    int prevPos = pythonCode_->textCursor().position();
    if (size + 3 == source.size()) {  // a tab was added
        prevPos += 3;
    }

    pythonCode_->blockSignals(true);
    pythonCode_->setPlainText(source.c_str());
    pythonCode_->blockSignals(false);

    QTextCursor cursor = pythonCode_->textCursor();
    cursor.setPosition(prevPos);
    pythonCode_->setTextCursor(cursor);

    script_.setSource(source);
    unsavedChanges_ = true;
}

void PythonEditorWidget::startRecordingScript() {
    script_.getScriptRecorder()->startRecording();
    startRecordScriptButton_->setDisabled(true);
    endRecordScriptButton_->setDisabled(false);
}

void PythonEditorWidget::endRecordingScript() {
    script_.getScriptRecorder()->endRecording();
    startRecordScriptButton_->setDisabled(false);
    endRecordScriptButton_->setDisabled(true);
}

}  // namespace
