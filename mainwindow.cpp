#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "history.h"
#include "xtrafunctions.h"
#include <QShortcut>
#include <QPropertyAnimation>
#include <QKeyEvent>
#include <QDesktopServices>
#include <QGraphicsBlurEffect>
#include <initializer_list>

/* TO-DO/FIX LIST
 *      LEGEND:
 *      [X] = DONE
 *      [~] = ALMOST DONE
 *      [...] = SOMEDAY
 *      [O] = SCRAPPED
 *
 *  DONE:
 * [X] Fix main screen output when doing the math. (since the first operation leads to "0" printed).
 * [X] Operation symbol problem #1. Changing between multiple consequetively fix-sets output to 0.
 * [X] Operation symbol problem #2. first operation messes up eqation when putting 2 consequitive operations.
 * [X] Operation symbol problem #3. using "*" or "/" after other operation causes problems.
 * [X] Don't input/remove excessive/hanging operation from equation when pressing <Enter>.
 * [X] Don't allow operation to be inserted first.
 * [X] Fix problem: <Enter> causes program crash @ line 197. "HISTORY.insert(EQUATION);"
 *  ^  Fixed through implementing deep copy, unsure if that was the cause or not.
 * [X] Figure out what to do with "=" in equation.
 * [X] Fix <BACKSPACE> equation problem.
 * [X] Don't allow equation preview (and anything else) to delete more than it should [only up to latest operation symbol].
 * [X] Fix crash due to <Backspace> <Delete> (in either order) being inputted.
 * [X] Not allow consequitive operations to be inputted "+--+/". Show last operation symbol replaced by new.
 * [X] Add history.
 *  ^  I believe history is being saved.
 * [X] <ESCAPE> opens options.
 * [X] Parse HISTORY data into history screen.
 *
 * IN PROGRESS:
 * [~] Add [+/-] button.
 * [ ] Fix scrollbarwidget in History.
 * [ ] Single # entry causes program crash.
 * [ ] Set continuation after pressing <ENTER>.
 * [ ] Don't allow negation to final answer (unless I do ^)
 * [ ] Negation after symbol input causes problem. -> Don't allow negation after symbol.
 * [ ] Make system where you can erase symbols and get to numbers where you can edit and replace their values on the equation dLL.
 * [ ] Add ability to change visuals (colors, mainly).
 * [ ] Add history memory management (how much to store into history, basically).
 * [ ] Edit size or scroll on output screen if number too long.
 * [ ] Implement quick solving.
 * [ ] Dynamically create new QPushButton in scrollarea for History when inserting into it.
 * [~] Fix History not enabling when o/c.
 * [ ] Save * location of EQUATION to recall it.
 * [ ] Enable and fix scaling during resizing window.
 *
 *  SCRAPPED:
 *
 */

// GLOBAL VARS.
maths::dLL EQUATION;
datastore::dLL HISTORY;
QString recentNumInput;
bool solved{false}, prevOp{false};

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Shortcuts
    ui->num0->setShortcut(Qt::Key_0);
    ui->num1->setShortcut(Qt::Key_1);
    ui->num2->setShortcut(Qt::Key_2);
    ui->num3->setShortcut(Qt::Key_3);
    ui->num4->setShortcut(Qt::Key_4);
    ui->num5->setShortcut(Qt::Key_5);
    ui->num6->setShortcut(Qt::Key_6);
    ui->num7->setShortcut(Qt::Key_7);
    ui->num8->setShortcut(Qt::Key_8);
    ui->num9->setShortcut(Qt::Key_9);
    ui->actClear->setShortcut(Qt::Key_Delete);
    ui->actBack->setShortcut(Qt::Key_Backspace);
    ui->signAdd->setShortcut(Qt::Key_Plus);
    ui->signSub->setShortcut(Qt::Key_Minus);
    ui->signDiv->setShortcut(Qt::Key_Slash);
    ui->signMult->setShortcut(Qt::Key_Asterisk);
    ui->signEquals->setShortcut(Qt::Key_Enter);
    ui->actHistory->setShortcut(Qt::Key_H);

    // Signal Connectors
    connect(ui->num0, SIGNAL(released()), this, SLOT(digitPressed()));
    connect(ui->num1, SIGNAL(released()), this, SLOT(digitPressed()));
    connect(ui->num2, SIGNAL(released()), this, SLOT(digitPressed()));
    connect(ui->num3, SIGNAL(released()), this, SLOT(digitPressed()));
    connect(ui->num4, SIGNAL(released()), this, SLOT(digitPressed()));
    connect(ui->num5, SIGNAL(released()), this, SLOT(digitPressed()));
    connect(ui->num6, SIGNAL(released()), this, SLOT(digitPressed()));
    connect(ui->num7, SIGNAL(released()), this, SLOT(digitPressed()));
    connect(ui->num8, SIGNAL(released()), this, SLOT(digitPressed()));
    connect(ui->num9, SIGNAL(released()), this, SLOT(digitPressed()));
    connect(ui->actClear, SIGNAL(released()), this, SLOT(clear()));
    connect(ui->actBack, SIGNAL(released()), this, SLOT(backspace()));
    connect(ui->signAdd, &QPushButton::released, this, [this]{arithmetics(0);});
    connect(ui->signSub, &QPushButton::released, this, [this]{arithmetics(1);});
    connect(ui->signMult, &QPushButton::released, this, [this]{arithmetics(2);});
    connect(ui->signDiv, &QPushButton::released, this, [this]{arithmetics(3);});
    connect(ui->signEquals, &QPushButton::released, this, [this]{arithmetics(4);});
    connect(ui->openSettings, &QPushButton::released, this, [this]{ocSettings(false);});
    connect(ui->closeSettings, &QPushButton::released, this, [this]{ocSettings(true);});
    connect(ui->actHistory, SIGNAL(released()), this, SLOT(ocHistory()));
    connect(ui->s_github, &QPushButton::released, this, [this]{openSocial(QUrl("https://www.github.com/infocus7/"));});
    connect(ui->s_linkedin, &QPushButton::released, this, [this]{openSocial(QUrl("https://www.linkedin.com/in/fabiangonz98/"));});
    connect(ui->s_twitter, &QPushButton::released, this, [this]{openSocial(QUrl("https://www.twitter.com/fabiangonz98/"));});
    connect(ui->b_plusminus, SIGNAL(released()), this, SLOT(negateNum()));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateScreen(double s_num)
{
    QString s_text{QString::number(s_num, 'g', 15)};
    ui->screenOutput->setText(s_text);
}

void MainWindow::updateEquation(QString string)
{
    ui->equationScreen->setText(string);
}

void MainWindow::digitPressed()
{
    if(solved == true)
    {
        clearScreen();
        solved = false;
    }

    QPushButton *button = (QPushButton*)sender();
    updateScreen((ui->screenOutput->text().append(button->text())).toDouble());
    updateEquation((ui->equationScreen->text().append(button->text())));
    recentNumInput.append(button->text());
}

void MainWindow::backspace()
{
    if(recentNumInput == ui->screenOutput->text())
    {
        QString number{ui->screenOutput->text()};
        number.chop(1);
        updateScreen(number.toDouble());

        QString eq{ui->equationScreen->text()};
        eq.chop(1);
        updateEquation(eq);

        recentNumInput.chop(1);
    }
}

void MainWindow::clear()
{
    QString number{ui->screenOutput->text()};
    number.clear();
    EQUATION.clear();
    updateScreen(number.toDouble());
    updateEquation(number);
}

void MainWindow::clearScreen()
{
    QString number{ui->screenOutput->text()};
    number.clear();
    recentNumInput.clear();
    updateScreen(number.toDouble());
}

void MainWindow::arithmetics(unsigned int operation)
{
    QLabel * eqScreen{ui->equationScreen};
    if(eqScreen->text().isEmpty())
        return;
    bool alteredOperation{false};
    QPushButton * button = (QPushButton*)sender();

    if(!recentNumInput.isEmpty())
        EQUATION.insert(recentNumInput);

    if(operation == 4) // "="
    {
        if(EQUATION.ready2math())
        {
            updateScreen(EQUATION.solve());
            ui->answerOutput->setText(ui->screenOutput->text());
        }
        else // Removes hanging operation.
            EQUATION.remove();

        updateEquation("");

        // Make a new qpushbutton in hisotry storing equation data. Get scrollbar to work. If doesn't work, manually add the buttons in form.
        QPushButton * historyButton = new QPushButton(HISTORY.insert(maths::dLL(EQUATION)), ui->HISTORY);
        historyButton->show();
        historyButton->setGeometry(0, 51 * (HISTORY.returnNumItems() - 1), 404, 51); // set y 51*(amt of items in history)
        historyButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
        // historyButton->setStyleSheet("");
        // ui->b_history1->setText(HISTORY.insert(maths::dLL(EQUATION)));

        EQUATION.clear();
    }
    else
    {
        if(eqScreen->text() != NULL && (eqScreen->text().back() == "+" || eqScreen->text().back() == "-" || eqScreen->text().back() == "*" || eqScreen->text().back() == "/"))
        {
            QString eq{eqScreen->text()};
            eq.chop(1);
            updateEquation(eq);
            alteredOperation = true;
        }

        eqScreen->setText(eqScreen->text().append(button->text()));

        if(alteredOperation == true)
        {
            EQUATION.replaceOperator(button->text());
        }

        if(EQUATION.ready2math())
        {
            updateScreen(EQUATION.solve());
            ui->answerOutput->setText(ui->screenOutput->text());
        }

        if(alteredOperation == false)
        {
            EQUATION.insert(button->text());
        }
    }

    recentNumInput.clear();
    solved = true;
}

void MainWindow::negateNum() // Not working as of 7.25.18 11.30AM
{
    QString num{ui->screenOutput->text()};
    int lastIndex{ui->equationScreen->text().length() - 1};
    if(lastIndex >= 0 && !(xtra::is_in(ui->equationScreen->text().at(lastIndex), {'+', '-', '/', '*'})))
    {
        if(ui->screenOutput->text().at(0) != '-') // If positive
        {
            QString newNum{num.prepend('-')};

            // edit screenoutput
            updateScreen(newNum.toDouble());
            // edit equationscreen

            // edit number inputted variable
            recentNumInput = newNum;
        }
        else // If negative
        {
            QString newNum{num.remove(0, 1)};

            // edit screenoutput
            updateScreen(newNum.toDouble());
            // edit equationscreen

            // edit number inputted variable
            recentNumInput = newNum;
        }
    }
}

void MainWindow::openSocial(QUrl website)
{
    QDesktopServices::openUrl(website);
}

void MainWindow::ocSettings(bool isOpen)
{
    QPropertyAnimation *animation = new QPropertyAnimation(ui->settingsScreen, "geometry");
    animation->setDuration(100);
    QGraphicsBlurEffect * blur = new QGraphicsBlurEffect;
    blur->setBlurHints(QGraphicsBlurEffect::QualityHint);

    if(isOpen == false)
    {
        animation->setKeyValueAt(0, QRect(-211, 0, 211, 701));
        animation->setKeyValueAt(1, QRect(0, 0, 211, 701));
    }
    else
    {
        blur->setBlurRadius(0);
        animation->setKeyValueAt(0, QRect(0, 0, 211, 701));
        animation->setKeyValueAt(1, QRect(-211, 0, 211, 701));
    }

    ui->closeSettings->setEnabled(!(ui->closeSettings->isEnabled()));
    ui->openSettings->setEnabled(!(ui->openSettings->isEnabled()));
    ui->mainScreen->setGraphicsEffect(blur);
    animation->start();
}

void MainWindow::ocHistory()
{
    QPropertyAnimation *animation = new QPropertyAnimation(ui->historyScreen, "geometry");
    animation->setDuration(100);

    bool isOpen{false};
    if(ui->historyScreen->y() == 239)
        isOpen = true;

    if(isOpen)
    {
        animation->setKeyValueAt(0, QRect(0, 239, 421, 816));
        animation->setKeyValueAt(1, QRect(0, 700, 421, 816));
    }
    else
    {
        animation->setKeyValueAt(0, QRect(0, 700, 421, 816));
        animation->setKeyValueAt(1, QRect(0, 239, 421, 816));
    }

    // Set to always enabled until I get this fixed.
    //ui->historyScreen->setEnabled(!(ui->historyScreen->isEnabled()));

    animation->start();

}

void MainWindow::keyPressEvent(QKeyEvent * event)
{
    if(event->key() == Qt::Key_Escape)
    {
       if(ui->openSettings->isEnabled())
       {
            ocSettings(false);
       }
       else
       {
            ocSettings(true);
       }
    }
}